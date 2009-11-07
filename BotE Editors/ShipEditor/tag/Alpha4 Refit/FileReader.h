// FileReader.h: Schnittstelle für die Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
#define AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ShipInfo.h"

class CFileReader : public CObject  
{
public:
	CFileReader();
	virtual ~CFileReader();
	void ReadDataFromFile(CArray<CShipInfo*>* shipInfos);
	void WriteDataToFile(CArray<CShipInfo*>* shipInfos);

private:
	void CheckBeforeWrite(CArray<CShipInfo*>* shipInfos);
};

#endif // !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
