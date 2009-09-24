/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// GenSectorName.h: Schnittstelle für die Klasse CGenSectorName.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENSECTORNAME_H__74957795_2BDD_4496_BF8D_0D3C43427DA5__INCLUDED_)
#define AFX_GENSECTORNAME_H__74957795_2BDD_4496_BF8D_0D3C43427DA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGenSectorName : public CObject
{
public:
	DECLARE_SERIAL (CGenSectorName)
	
	/// Standardkonstruktor
	CGenSectorName();
	
	/// Destruktor
	virtual ~CGenSectorName();
	
	/// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	
	/// Funktion liest die Systemnames aus den externen Datafiles ein
	void ReadSystemNames();

	/**
	 * Diese Funktion gibt uns einen einzigartigen Systemnamen zurück. Dieser Name wurde vorher noch nicht vergeben.
	 * Als Parameter wird nur ein Wert übergeben, der der Funktion sagt ob es sich um ein Minorracesystem handelt
	 * oder nicht.
	*/
	CString GenerateSectorName(BOOLEAN IsMinorRaceInSector);

private:
	CArray<CString> m_strName;		///< Feld aller Namen der Planetensysteme ohne Minor- und Majorracesysteme
	CArray<CString> m_strRaceName;	///< Feld aller Namen der Minorracesysteme
};

#endif // !defined(AFX_GENSECTORNAME_H__74957795_2BDD_4496_BF8D_0D3C43427DA5__INCLUDED_)
