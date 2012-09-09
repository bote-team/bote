#include "StdAfx.h"
#include "Packet.h"

#include "LZMA_BotE.h"

#pragma warning (disable: 4244 4267 4018)

namespace network
{

	CPacket::CPacket() : m_nFlags(0), m_nID(MSGID_NONE), m_nRequestId(-1), m_nSize(0), m_pData(NULL)
	{
	}

	CPacket::CPacket(CMessage *pMessage, int nRequestId, BOOL bRequest) : m_nFlags(0), m_nRequestId(nRequestId)
	{
		ASSERT(pMessage);
		ASSERT(nRequestId >= -1);

		// Info aus Nachricht übernehmen
		m_nID = pMessage->GetID();

		// evtl. Flag für Request setzen
		if (nRequestId >= 0 && bRequest) m_nFlags |= PCK_REQUEST;

		// Nachricht serialisieren, Daten in Puffer aufnehmen
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pMessage->Serialize(ar);
		ar.Close();

		// Daten komprimieren, wenn sie länger sind als PACKET_COMPRESS
		BOOL bCompress = (memFile.GetLength() > PACKET_COMPRESS);

		if (!bCompress)
		{
			// Puffer von memFile übernehmen
			m_nSize = memFile.GetLength();
			m_pData = memFile.Detach();
		}
		else
		{
			// Flag setzen
			m_nFlags |= PCK_COMPRESSED;

			// serialisierte Daten komprimieren
			UINT nSize = memFile.GetLength();
			BYTE *pIn = memFile.Detach();
			CMemFile out;
#ifdef _DEBUG
			TRACE("compressing: %u -> ", nSize);
			clock_t time = clock();
#endif
			VERIFY(Compress(pIn, nSize, out));
			m_nSize = out.GetLength();
#ifdef _DEBUG
			time = clock() - time;
			TRACE("%u (%u ms)\n", m_nSize, 1000 * time / CLOCKS_PER_SEC);
#endif
			free(pIn);

			// Puffer von out übernehmen
			m_pData = out.Detach();
		}
	}

	CPacket::~CPacket()
	{
		if (m_pData) delete[] m_pData;
	}

	BOOL CPacket::Send(CAsyncSocket &socket, int &nError)
	{
		nError = 0;
		if (!m_pData) return TRUE;

		// Header zusammenstellen
		UINT nHeaderSize = 2 * sizeof(BYTE) + sizeof(UINT) + sizeof(int) + PACKET_CAPTURE_LEN;
		BYTE pHeader[2 * sizeof(BYTE) + sizeof(UINT) + sizeof(int) + PACKET_CAPTURE_LEN];

		BYTE *p = pHeader;
		memcpy(p, PACKET_CAPTURE_PATTERN, PACKET_CAPTURE_LEN);
		p += PACKET_CAPTURE_LEN;
		memcpy(p, &m_nFlags, sizeof(m_nFlags));
		p += sizeof(m_nFlags);
		memcpy(p, &m_nID, sizeof(m_nID));
		p += sizeof(m_nID);
		memcpy(p, &m_nRequestId, sizeof(m_nRequestId));
		p += sizeof(m_nRequestId);
		memcpy(p, &m_nSize, sizeof(m_nSize));
		p += sizeof(m_nSize);

#ifdef _DEBUG
		clock_t time = clock();
#endif

		// Header versenden
		clock_t deadline = clock() + SEND_TIMEOUT * CLOCKS_PER_SEC;
		while (!SyncSend(socket, pHeader, nHeaderSize, nError))
		{
			ASSERT(nError);
			if (nError != WSAEWOULDBLOCK) return FALSE;
			Sleep(50);
			if (clock() > deadline)
			{
				TRACE("send timeout\n");
				return FALSE;
			}
		}

		// Timer zurücksetzen, Daten versenden
		UINT nSize = m_nSize;
		p = m_pData;
		deadline = clock() + SEND_TIMEOUT * CLOCKS_PER_SEC;
		while (nSize)
		{
			int nDone = socket.Send(p, nSize);
			if (nDone == SOCKET_ERROR)
			{
				nError = socket.GetLastError();
				if (nError != WSAEWOULDBLOCK)
					return FALSE;
				else
				{
					Sleep(50);
					if (clock() > deadline)
					{
						TRACE("send timeout\n");
						return FALSE;
					}
				}
			}
			else if (nDone > 0)
			{
				p += nDone;
				nSize -= nDone;
				deadline = clock() + SEND_TIMEOUT * CLOCKS_PER_SEC;
			}
			else
			{
				ASSERT(nDone == 0);
			}
		}

#ifdef _DEBUG
		time = clock() - time;
		TRACE("packet with %u byte(s) sent (%u ms)\n", sizeof(m_nFlags) + sizeof(m_nID)
			+ sizeof(m_nSize) + m_nSize, 1000 * time / CLOCKS_PER_SEC);
#endif

		nError = 0;
		return TRUE;
	}

	BOOL CPacket::SyncSend(CAsyncSocket &socket, const void *lpBuf, int nBufLen, int &nError, int nFlags)
	{
		ASSERT(lpBuf != NULL && nBufLen >= 0);

		const BYTE *p = (BYTE *)lpBuf;
		while (nBufLen)
		{
			int nDone = socket.Send(p, nBufLen, nFlags);
			if (nDone == SOCKET_ERROR)
			{
				nError = socket.GetLastError();
				return FALSE;
			}
			p += nDone;
			nBufLen -= nDone;
		}

		return TRUE;
	}

	CPacket *CPacket::Receive(CAsyncSocket &socket, int &nError, BOOL &bClosed)
	{
		nError = 0;
		bClosed = FALSE;

		CPacket *pPacket = NULL;

		BYTE pHeader[2 * sizeof(BYTE) + sizeof(UINT) + sizeof(int) + PACKET_CAPTURE_LEN];
		UINT nHeaderSize = 2 * sizeof(BYTE) + sizeof(UINT) + sizeof(int) + PACKET_CAPTURE_LEN;

		// warten, bis Header vollständig empfangen
		int nDone = 0;
		while (true)
		{
			// warten, bis der Empfangspuffer genügend Daten enthält
			do
			{
				nDone = socket.Receive(pHeader, nHeaderSize, MSG_PEEK);
				if (nDone == 0)
				{
					// die Verbindung wurde getrennt
					bClosed = TRUE;
					goto cancel;
				}
				else if (nDone == SOCKET_ERROR)
					// liefert den Fehlercode WSAEWOULDBLOCK, wenn keine Daten empfangen wurden
					goto error;
			}
			while (nDone < nHeaderSize);

			// prüfen, ob Header mit Capture Pattern beginnt
			if (memcmp(pHeader, PACKET_CAPTURE_PATTERN, PACKET_CAPTURE_LEN) != 0)
			{
				// im Fehlerfall nach dem ersten Buchstaben des Capture Patterns suchen
				int idx = 1;
				ASSERT(PACKET_CAPTURE_LEN >= 1);
				for (; idx < nHeaderSize; idx++)
					if (memcmp(pHeader + idx, PACKET_CAPTURE_PATTERN, 1) == 0)
						break;
				// Daten vor dem ersten Buchstaben aus Empfangspuffer entfernen
				socket.Receive(pHeader, idx);
				nDone -= idx;
				// mit dem Warten auf genügend Daten fortsetzen
				continue;
			}
			else
				break;
		}

		// Header lesen (hinter dem Capture Pattern beginnen)
#ifdef _DEBUG
		clock_t time = clock();
#endif
		nDone = socket.Receive(pHeader, nHeaderSize);
		ASSERT(nDone == nHeaderSize);

		pPacket = new CPacket();
		ASSERT(sizeof(pPacket->m_nFlags) + sizeof(pPacket->m_nID) + sizeof(pPacket->m_nRequestId)
			+ sizeof(pPacket->m_nSize) == nHeaderSize - PACKET_CAPTURE_LEN);

		BYTE *p = pHeader + PACKET_CAPTURE_LEN;
		memcpy(&pPacket->m_nFlags, p, sizeof(pPacket->m_nFlags));
		p += sizeof(pPacket->m_nFlags);
		memcpy(&pPacket->m_nID, p, sizeof(pPacket->m_nID));
		p += sizeof(pPacket->m_nID);
		memcpy(&pPacket->m_nRequestId, p, sizeof(pPacket->m_nRequestId));
		p += sizeof(pPacket->m_nRequestId);
		memcpy(&pPacket->m_nSize, p, sizeof(pPacket->m_nSize));
		p += sizeof(pPacket->m_nSize);

		// Daten lesen; warten, bis Daten vollständig sind
		pPacket->m_pData = new BYTE[pPacket->m_nSize];
		p = pPacket->m_pData;

		int nDataSize = pPacket->m_nSize;
		clock_t deadline = clock() + RECEIVE_TIMEOUT * CLOCKS_PER_SEC;
		while (nDataSize)
		{
			nDone = socket.Receive(p, nDataSize);
			if (nDone > 0)
			{
				// haben weitere Daten empfangen
				p += nDone;
				nDataSize -= nDone;
				// Timeout neu beginnen (für langsame Verbindungen)
				deadline = clock() + RECEIVE_TIMEOUT * CLOCKS_PER_SEC;
			}
			else if (nDone == 0)
			{
				// Verbindung wurde von anderer Seite getrennt
				bClosed = TRUE;
				goto cancel;
			}
			else
			{
				ASSERT(nDone == SOCKET_ERROR);
				if (socket.GetLastError() == WSAEWOULDBLOCK)
				{
					// momentan keine weiteren Daten verfügbar
					Sleep(50);
					// bei Timeout empfangene Daten verwerfen, abbrechen
					if (clock() > deadline)
					{
						TRACE("receive timeout\n");
						goto cancel;
					}
				}
				else
					// ein Fehler ist aufgetreten
					goto error;
			}
		}

		// Paket vollständig empfangen
#ifdef _DEBUG
		time = clock() - time;
		TRACE("packet with %u byte(s) received (%u ms)\n", sizeof(pPacket->m_nFlags) + sizeof(pPacket->m_nID)
			+ sizeof(pPacket->m_nSize) + pPacket->m_nSize, 1000 * time / CLOCKS_PER_SEC);
#endif

		return pPacket;

	error:
		nError = socket.GetLastError();
	cancel:
		if (pPacket) delete pPacket;
		return NULL;
	}

	CMessage *CPacket::GetMessage() const
	{
		CMemFile memFile;

		if ((m_nFlags & PCK_COMPRESSED) != 0)
		{
			// Daten liegen komprimiert vor, erst dekomprimieren
#ifdef _DEBUG
			TRACE("decompressing: %u -> ", m_nSize);
			clock_t time = clock();
#endif
			VERIFY(Decompress(m_pData, m_nSize, memFile));
#ifdef _DEBUG
			UINT nSize = memFile.GetLength();
			time = clock() - time;
			TRACE("%u (%u ms)\n", nSize, 1000 * time / CLOCKS_PER_SEC);
#endif
			// Position im memFile zurück auf Anfang setzen
			memFile.Seek(0, CFile::begin);
		}
		else
		{
			// Daten liegen unkomprimiert vor
			memFile.Attach(m_pData, m_nSize);
		}

		// passendes Nachrichten-Objekt erzeugen
		CMessage *pMessage = CMessage::CreateMessage(m_nID);
		if (!pMessage) return NULL;

		// Nachricht aus Puffer deserialisieren
		CArchive ar(&memFile, CArchive::load);
		pMessage->Serialize(ar);
		ar.Close();

		if ((m_nFlags & PCK_COMPRESSED) == 0)
		{
			// verhindern, dass m_pData mit Zerstörung von memFile freigegeben wird
			memFile.Detach();
		}

		return pMessage;
	}

	BOOL CPacket::Compress(BYTE *lpBuf, UINT nBufLen, CFile &out)
	{
		return BotE_LzmaCompress(lpBuf, nBufLen, out);
//		out.Write(lpBuf, nBufLen);
//		return TRUE;
	}

	BOOL CPacket::Decompress(BYTE *lpBuf, UINT nBufLen, CFile &out)
	{
		return BotE_LzmaDecompress(lpBuf, nBufLen, out);
//		out.Write(lpBuf, nBufLen);
//		return TRUE;
	}

};
