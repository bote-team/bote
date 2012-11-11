#include "stdafx.h"

#include "LZMA_BotE.h"

#include "7zip/Compress/LZMA_C/LzmaDecode.h"
#include "7zip/Compress/LZMA/LZMAEncoder.h"

extern "C"
{
#include "7zip/Compress/Branch/BranchX86.h"
}

#pragma warning (disable: 4267 4244)


#define LZMA_PROPS_SIZE			14
#define LZMA_SIZE_OFFSET		6

#define SZE_FAIL				(1)
#define SZE_DATA_ERROR			(1)
#define SZE_OUTOFMEMORY			(2)
#define SZE_OUT_OVERFLOW		(3)

BOOL BotE_LzmaGetUncompressedSize(const BYTE *inBuffer, size_t inSize, size_t *outSize)
{
	if (inSize < LZMA_PROPS_SIZE) return FALSE;
	if (outSize) *outSize = 0;

	size_t i;
	for (i = 0; i < sizeof(size_t); i++)
		*outSize += ((size_t)inBuffer[LZMA_SIZE_OFFSET + i]) << (8 * i);
	for (; i < 8; i++)
		if (inBuffer[LZMA_SIZE_OFFSET + i] != 0) return FALSE;

	return TRUE;
}

BOOL BotE_LzmaDecompress(const BYTE *inBuffer, size_t inSize, CFile &out,
	void *(*allocFunc)(size_t size), void (*freeFunc)(void *))
{
	CLzmaDecoderState state;  /* it's about 24 bytes structure, if int is 32-bit */
	int result;
	SizeT outSizeProcessedLoc;
	SizeT inProcessed;
	int useFilter;

	if (inSize < LZMA_PROPS_SIZE) return FALSE;
	useFilter = inBuffer[0];

	if (useFilter > 1) return FALSE;

	if (LzmaDecodeProperties(&state.Properties, inBuffer + 1, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK)
		return FALSE;
	state.Probs = (CProb *)allocFunc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (state.Probs == 0) return FALSE;

	size_t outSize;
	if (!BotE_LzmaGetUncompressedSize(inBuffer, inSize, &outSize)) return FALSE;

	BYTE *outBuffer = (BYTE *)allocFunc(outSize);
	result = LzmaDecode(&state,
		inBuffer + LZMA_PROPS_SIZE, (SizeT)inSize - LZMA_PROPS_SIZE, &inProcessed,
		outBuffer, (SizeT)outSize, &outSizeProcessedLoc);
	freeFunc(state.Probs);
	if (result != LZMA_RESULT_OK)
	{
		freeFunc(outBuffer);
		return FALSE;
	}
	if (useFilter == 1)
	{
		UInt32 _prevMask;
		UInt32 _prevPos;
		x86_Convert_Init(_prevMask, _prevPos);
		x86_Convert(outBuffer, (UInt32)outSizeProcessedLoc, 0, &_prevMask, &_prevPos, 0);
	}

	out.Write(outBuffer, outSize);
	freeFunc(outBuffer);

	return TRUE;
}

class CInStreamRam : public ISequentialInStream, public CMyUnknownImp
{
public:
	CInStreamRam() :
		Data(NULL),
		Size(0),
		Pos(0)
	{}

private:
	const Byte *Data;
	size_t Size;
	size_t Pos;

public:
	MY_UNKNOWN_IMP void Init(const Byte *data, size_t size)
	{
		Data = data;
		Size = size;
		Pos = 0;
	}

	STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP CInStreamRam::Read(void *data, UInt32 size, UInt32 *processedSize)
{
	UInt32 remain = Size - Pos;
	if (size > remain) size = remain;
	for (UInt32 i = 0; i < size; i++) ((Byte *)data)[i] = Data[Pos + i];
	Pos += size;
	if(processedSize != NULL) *processedSize = size;
	return S_OK;
}

class COutStreamRam : public ISequentialOutStream, public CMyUnknownImp
{
private:
	CFile *m_pOut;
	size_t m_nInitialPosition;

public:
	COutStreamRam() :
		m_pOut(NULL),
		m_nInitialPosition(0),
		Overflow(false),
		Pos(0)
	{}

	bool Overflow;
	size_t Pos;

	void Init(CFile *pOut)
	{
		m_pOut = pOut;
		m_nInitialPosition = pOut->GetPosition();
		Overflow = false;
		Pos = 0;
	}

	void SetPos(size_t pos)
	{
		Overflow = false;
		Pos = m_pOut->Seek(m_nInitialPosition + pos, CFile::begin) - m_nInitialPosition;
	}

	MY_UNKNOWN_IMP HRESULT WriteByte(Byte b)
	{
		m_pOut->Write(&b, sizeof(Byte));
		Pos++;
		return S_OK;
	}

	STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
};

STDMETHODIMP COutStreamRam::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
	m_pOut->Write(data, size);
	if (processedSize != NULL) *processedSize = size;
	Pos += size;
	return S_OK;
}

BOOL BotE_LzmaCompress(const BYTE *inBuffer, size_t inSize, CFile &out,
	UINT dictionarySize, ESZ_FILTER_MODE filterMode)
{
#ifndef _NO_EXCEPTIONS
	try { 
#endif
		const size_t kIdSize = 1;
		const size_t kLzmaPropsSize = 5;
		const size_t kMinDestSize = kIdSize + kLzmaPropsSize + 8;
		NCompress::NLZMA::CEncoder *encoderSpec = new NCompress::NLZMA::CEncoder;
		CMyComPtr<ICompressCoder> encoder = encoderSpec;

		PROPID propIDs[] = 
		{ 
			NCoderPropID::kAlgorithm,
			NCoderPropID::kDictionarySize,  
			NCoderPropID::kNumFastBytes,
		};
		const int kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);
		PROPVARIANT properties[kNumProps];
		properties[0].vt = VT_UI4;
		properties[1].vt = VT_UI4;
		properties[2].vt = VT_UI4;
		properties[0].ulVal = (UInt32)2;
		properties[1].ulVal = (UInt32)dictionarySize;
		properties[2].ulVal = (UInt32)64;

		if (encoderSpec->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
			return FALSE;

		COutStreamRam *outStreamSpec = new COutStreamRam;
		if (outStreamSpec == 0) return FALSE;
		CMyComPtr<ISequentialOutStream> outStream = outStreamSpec;
		CInStreamRam *inStreamSpec = new CInStreamRam;
		if (inStreamSpec == 0) return FALSE;
		CMyComPtr<ISequentialInStream> inStream = inStreamSpec;

		outStreamSpec->Init(&out);
		if (outStreamSpec->WriteByte(0) != S_OK)
			return FALSE;

		if (encoderSpec->WriteCoderProperties(outStream) != S_OK)
			return FALSE;
		if (outStreamSpec->Pos != kIdSize + kLzmaPropsSize)
			return FALSE;

		int i;
		for (i = 0; i < 8; i++)
		{
			UInt64 t = (UInt64)(inSize);
			if (outStreamSpec->WriteByte((Byte)((t) >> (8 * i))) != S_OK)
				return FALSE;
		}

		Byte *filteredStream = 0;

		bool useFilter = (filterMode != SZ_FILTER_NO);
		if (useFilter)
		{
			if (inSize != 0)
			{
				filteredStream = (Byte *)MyAlloc(inSize);
				if (filteredStream == 0) return FALSE;
				memmove(filteredStream, inBuffer, inSize);
			}
			UInt32 _prevMask;
			UInt32 _prevPos;
			x86_Convert_Init(_prevMask, _prevPos);
			x86_Convert(filteredStream, (UInt32)inSize, 0, &_prevMask, &_prevPos, 1);
		}

		UInt32 minSize = 0;
		int numPasses = (filterMode == SZ_FILTER_AUTO) ? 3 : 1;
		bool bestIsFiltered = false;
		int mainResult = TRUE;
		size_t startPos = outStreamSpec->Pos;
		for (i = 0; i < numPasses; i++)
		{
			if (numPasses > 1 && i == numPasses - 1 && !bestIsFiltered)
				break;
			outStreamSpec->SetPos(startPos);
			bool curModeIsFiltered = false;
			if (useFilter && i == 0)
				curModeIsFiltered = true;
			if (numPasses > 1 && i == numPasses - 1)
				curModeIsFiltered = true;

			inStreamSpec->Init(curModeIsFiltered ? filteredStream : inBuffer, inSize);

			HRESULT lzmaResult = encoder->Code(inStream, outStream, 0, 0, 0);

			mainResult = TRUE;
			if (lzmaResult == E_OUTOFMEMORY)
			{
				mainResult = FALSE;
				break;
			} 
			if (i == 0 || outStreamSpec->Pos <= minSize)
			{
				minSize = outStreamSpec->Pos;
				bestIsFiltered = curModeIsFiltered;
			}
			if (outStreamSpec->Overflow)
				mainResult = FALSE;
			else if (lzmaResult != S_OK)
			{
				mainResult = FALSE;
				break;
			} 
		}
		if (bestIsFiltered)
		{
			out.Seek(0, CFile::begin);
			Byte b = 1;
			out.Write(&b, sizeof(Byte));
		}
		if (useFilter) MyFree(filteredStream);
		return mainResult;

#ifndef _NO_EXCEPTIONS
	} catch(...) { return FALSE; }
#endif
}
