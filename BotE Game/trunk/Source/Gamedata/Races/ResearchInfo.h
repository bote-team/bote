/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// ResearchInfo.h: Schnittstelle für die Klasse CResearchInfo.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESEARCHINFO_H__187C2D02_4967_4B6C_8079_8723C8473B26__INCLUDED_)
#define AFX_RESEARCHINFO_H__187C2D02_4967_4B6C_8079_8723C8473B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ResearchComplex.h"
#include <math.h>

class CResearchInfo : public CObject 
{
	friend class CResearch; // damit wir in der Klasse CResearch (und nur dieser) auf die private Attribute zugreifen können
public:
	DECLARE_SERIAL (CResearchInfo)
	/// Konstruktor
	CResearchInfo(void);

	/// Destruktor
	~CResearchInfo();
	
	/// Kopierkonstruktor
	CResearchInfo(const CResearchInfo & rhs);
	/// Zuweisungsoperator
	CResearchInfo & operator=(const CResearchInfo &);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/**
	 * Diese Funktionen geben die benötigten FP für ein jeweiliges Techgebiet zurück. Diese werden hier dynamisch
	 * berechnet. Der "Startwert" für die einzelnen Gebiete ist der Multiplikator (z.B. 125 oder 150). An die einzelnen
	 * Funktionen wird das Techlevel <code>techLevel</code> übergeben, für welches man die benötigten FP haben will.
	 */
	ULONG GetBio(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 150.0);}
	ULONG GetEnergy(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 125.0);}
	ULONG GetComp(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 150.0);}
	ULONG GetPropulsion(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 150.0);}
	ULONG GetConstruction(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 175.0);}
	ULONG GetWeapon(USHORT nTechLevel) const {return (ULONG)(pow(2.25f, nTechLevel) * 175.0);}
	
	/**
	 * Diese Funktion gibt einen Wahrheitswert zurück, der angibt, ob der Spieler die Wahl einer der 3 Möglichkeiten
	 * bei der Spezialforschung schon getroffen hat.
	 */
	BOOLEAN GetChoiceTaken(void) const {return m_bChoiceTaken;}

	/**
	 * Diese Funktion gibt einen Zeiger auf den aktuellen Komplex der Spezialforschung zurück.
	 */
	CResearchComplex* GetCurrentResearchComplex() {return &m_ResearchComplex[m_iCurrentComplex];}
	
	/**
	 * Diese Funktion gibt einen Zeiger auf einen bestimmten Komplex der Spezialforschung zurück.
	 * Welcher Komplex zurückgegeben wird, wird durch den Parameter <code>complex</code> bestimmt.
	 */
	CResearchComplex* GetResearchComplex(BYTE complex) {return &m_ResearchComplex[complex];}
	
	/**
	 * Funktion gibt den Namen einer gewünschten Technologie zurück. Als Parameter muss eine Nummer <code>tech</code>
	 * übergeben werden, die die Technologie identifiziert.
	 */
	const CString& GetTechName(BYTE tech) const {return m_strTechName[tech];}

	/**
	 * Funktion gibt die Beschreibung einer bestimmten Technologie zurück. Als Parameter muss eine Nummer
	 * <code>tech</code> übergeben werden, die die Technologie identifiziert.
	 */
	const CString& GetTechDescription(BYTE tech) const {return m_strTechDescription[tech];}

	// sonstige Funktionen
	/**
	 * Diese Funktion wählt zufällig ein Unique-Themengebiet aus den noch nicht erforschten Komplexen aus.
	 * Vor Aufruf der Funktion sollte überprüft werden, dass nicht schon alle Komplexe erforscht wurden, da
	 * es sonst zum Absturz des Programms kommen könnte.
	 */
	void ChooseUniqueResearch(void);
	
	/**
	 * Diese Funktion ändert den Status des aktuellen Komplexes. Dabei ändert sie gleichzeitig auch den Status
	 * der zuvor gewählten Wahlmöglichkeit. Als Parameter wird dabei ein neuer Status <code>newstatus</code>
	 * übergeben.
	 */
	void ChangeStatusOfComplex(BYTE newstatus);
	
	/**
	 * Diese Funktion wählt eine der drei Möglichkeiten der Uniqueforschung aus. Dafür muss man das Gebiet, welches
	 * erforscht werden soll mit dem Parameter <code>possibility</code> übergeben. Genaueres steht in der Definition
	 * dieser Funktion.
	 */
	void SetUniqueResearchChoosePossibility(BYTE possibility);

	/**
	 * Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie, an der gerade geforscht
	 * wird. Dies wird in den Attributen <code>m_strTechName</code> und <code>m_strTechDescription</code> gespeichert.
	 * Als Parameter müssen dafür die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>, die
	 * aktuell erforscht wird übergeben werden.
	 */
	void SetTechInfos(BYTE tech, BYTE level);


	/**
	 * Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie
	 * Dies wird in den Parametern <code>m_sTechName</code> und <code>m_sTechDesc</code> gespeichert.
	 * Als Parameter müssen dafür die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>
	 * übergeben werden.
	 */
	static void GetTechInfos(BYTE tech, BYTE level, CString& sTechName, CString& sTechDesc);
	
private:
	/// Die derzeit 12 Objekte für die einzelnen Komplexe der Spezialforschung
	CResearchComplex m_ResearchComplex[NoUC];
	
	/// Der aktuell gewählter Komplex
	short m_iCurrentComplex;
	
	/// Wurde eine der drei Wahlmöglichkeiten getroffen
	BOOLEAN m_bChoiceTaken;

	/// Der Name der Technologie, an der gerade geforscht wird.
	CString m_strTechName[6];

	/// Die Beschreibung der Technologie, an der gerade geforscht wird.
	CString m_strTechDescription[6];
};

#endif // !defined(AFX_RESEARCHINFO_H__187C2D02_4967_4B6C_8079_8723C8473B26__INCLUDED_)
