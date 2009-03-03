// FileReader.h: Schnittstelle f¸r die Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
#define AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MinorRace.h"

class CFileReader : public CObject  
{
public:
	CFileReader();
	virtual ~CFileReader();

	/**
	  * Funktion lieﬂt Daten aus einer Datei und speichert diese in das
	  * dynamsiche Array <code>m_MinorInfos<code>.
	  */
	void ReadDataFromFile(CArray<CMinorRace,CMinorRace>* m_MinorInfos);
	
	/**
	  * Funktion schreibt Daten in eine Datei
	  */
	void WriteDataToFile(CArray<CMinorRace,CMinorRace>* m_MinorInfos);
};

#endif // !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
