// FileReader.h: Schnittstelle für die Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
#define AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TroopInfo.h"

class CFileReader : public CObject  
{
public:
	CFileReader();
	virtual ~CFileReader();

	/**
	  * Funktion ließt Daten aus einer Datei und speichert diese in das
	  * dynamsiche Array <code>m_TroopInfos<code>.
	  */
	void ReadDataFromFile(CArray<CTroopInfo,CTroopInfo>* m_TroopInfos);
	
	/**
	  * Funktion schreibt Daten in eine Datei
	  */
	void WriteDataToFile(CArray<CTroopInfo,CTroopInfo>* m_TroopInfos);

	/**
	  * Funktion gibt String aus Stringtable zurück
	  */
	CString GetString(CString);
};

#endif // !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
