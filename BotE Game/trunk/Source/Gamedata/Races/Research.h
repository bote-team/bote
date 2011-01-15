/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Research.h: Schnittstelle für die Klasse CResearch.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESEARCH_H__E0439F02_20AD_47EF_9CA1_FE1DB8BCAFC1__INCLUDED_)
#define AFX_RESEARCH_H__E0439F02_20AD_47EF_9CA1_FE1DB8BCAFC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ResearchInfo.h"

class CResearch : public CObject  
{
public:
	DECLARE_SERIAL (CResearch)
// Konstruktor & Destruktor	
	CResearch();
	virtual ~CResearch();
// Kopierkonstruktor
	CResearch(const CResearch & rhs);
// Zuweisungsoperator
	CResearch & operator=(const CResearch &);
// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	BYTE GetBioTech() const {return m_iBioTech;}
	BYTE GetEnergyTech() const {return m_iEnergyTech;}
	BYTE GetCompTech() const {return m_iCompTech;}
	BYTE GetPropulsionTech() const {return m_iPropulsionTech;}
	BYTE GetConstructionTech() const {return m_iConstructionTech;}
	BYTE GetWeaponTech() const {return m_iWeaponTech;}
	
	BYTE GetBioPercentage() const {return m_iBioPercentage;}
	BYTE GetEnergyPercentage() const {return m_iEnergyPercentage;}
	BYTE GetCompPercentage() const {return m_iComputerPercentage;}
	BYTE GetPropulsionPercentage() const {return m_iPropulsionPercentage;}
	BYTE GetConstructionPercentage() const {return m_iConstructionPercentage;}
	BYTE GetWeaponPercentage() const {return m_iWeaponPercentage;}
	BYTE GetUniquePercentage() const {return m_iUniquePercentage;}

	ULONG GetBioFP() const {return (ULONG)m_lBioFP;}
	ULONG GetEnergyFP() const {return (ULONG)m_lEnergyFP;}
	ULONG GetComputerFP() const {return (ULONG)m_lComputerFP;}
	ULONG GetPropulsionFP() const {return (ULONG)m_lPropulsionFP;}
	ULONG GetConstructionFP() const {return (ULONG)m_lConstructionFP;}
	ULONG GetWeaponFP() const {return (ULONG)m_lWeaponFP;}
	ULONG GetUniqueFP() const {return (ULONG)m_lUniqueFP;}

	short GetBioTechBoni() const {return m_iBioTechBoni;}
	short GetEnergyTechBoni() const {return m_iEnergyTechBoni;}
	short GetCompTechBoni() const {return m_iCompTechBoni;}
	short GetPropulsionTechBoni() const {return m_iPropulsionTechBoni;}
	short GetConstructionTechBoni() const {return m_iConstructionTechBoni;}
	short GetWeaponTechBoni() const {return m_iWeaponTechBoni;}

	BOOLEAN GetLockStatus(BYTE tech) const ;
	BOOLEAN GetUniqueReady() const {return m_bUniqueReady;}
	BYTE GetNumberOfUniqueResearch() const {return m_iNumberOfUnique;}

	CResearchInfo* GetResearchInfo() {return &ResearchInfo;}
	// zum Schreiben der Membervariablen
	void SetBioTech(BYTE BioTech) {m_iBioTech = BioTech;}
	void SetEnergyTech(BYTE EnergyTech) {m_iEnergyTech = EnergyTech;}
	void SetCompTech(BYTE CompTech) {m_iCompTech = CompTech;}
	void SetPropulsionTech(BYTE PropulsionTech) {m_iPropulsionTech = PropulsionTech;}
	void SetConstructionTech(BYTE ConstructionTech) {m_iConstructionTech = ConstructionTech;}
	void SetWeaponTech(BYTE WeaponTech) {m_iWeaponTech = WeaponTech;}

	void SetPercentage(BYTE tech, BYTE percentage);	// BIO->tech == 0, ENERGY->tech == 1, BAU->tech == 3 usw.
	void SetLock(BYTE tech, BOOLEAN locked);

	/// Funktion setzt die schon erforschten Forschungspunkte in einem Gebiet auf einen übergebenen Wert.
	/// @param tech Forschungsbereich (Bio == 0, Energy == 1 usw.)
	/// @param fp neue erforschte Forschungspunkte
	void SetFP(BYTE tech, UINT fp);

	/// Funktion setzt die einzelnen Forschungsboni.
	/// @param researchBoni Feld mit den einzelnen Forschungsboni
	void SetResearchBoni(const short researchBoni[6]);

	// sonstige Funktionen
	
	/// Diese Funktion berechnet alles, was mit Forschung in einem Imperium zu tun hat. Diese Funktion wird in der 
	/// NextRound() Funktion aufgerufen. Als Parameter werden dafür die aktuellen <code>FP</code> des Imperiums
	/// Der Rückgabewert dieser Funktion ist ein Zeiger auf das Attribut <code>m_strMessage</code> und vom Typ CString*.
	CString* CalculateResearch(ULONG FP);
	
	/// Diese Funktion setzt alle Variablen des CResearch-Objektes wieder auf ihre Ausgangswerte
	void Reset(void);


private:
	// aktuelle Forschungsstufen
	BYTE m_iBioTech;				// aktuelle Biotechstufe des Imperiums
	BYTE m_iEnergyTech;				// aktuelle Energietechstufe des Imperiums
	BYTE m_iCompTech;				// aktuelle Computertechstufe des Imperiums
	BYTE m_iPropulsionTech;			// aktuelle Antriebstechstufe des Imperiums
	BYTE m_iConstructionTech;		// aktuelle Bautechstufe des Imperiums
	BYTE m_iWeaponTech;				// aktuelle Waffentechstufe des Imperiums
	BYTE m_iUniqueTech;				// aktuelle UniqueTechstufe des Imperiums
	// prozentuale Anteile auf die einzelnen Forschungen
	BYTE m_iBioPercentage;
	BYTE m_iEnergyPercentage;
	BYTE m_iComputerPercentage;
	BYTE m_iPropulsionPercentage;
	BYTE m_iConstructionPercentage;
	BYTE m_iWeaponPercentage;
	BYTE m_iUniquePercentage;
	// aktuell erforschte FP´s in den einzelnen Forschungen (nach außen hin nur ULONG Werte, wegen Runden aber intern als float gespeichert
	float m_lBioFP;
	float m_lEnergyFP;
	float m_lComputerFP;
	float m_lPropulsionFP;
	float m_lConstructionFP;
	float m_lWeaponFP;
	float m_lUniqueFP;
	// ist die Forschung gelockt, d.h. wir können den prozentualen Anteil nicht ändern
	BOOLEAN m_bBioLocked;
	BOOLEAN m_bEnergyLocked;
	BOOLEAN m_bComputerLocked;
	BOOLEAN m_bPropulsionLocked;
	BOOLEAN m_bConstructionLocked;
	BOOLEAN m_bWeaponLocked;
	BOOLEAN m_bUniqueLocked;
	// Boni auf die einzelnen Forschungsgebiete
	short m_iBioTechBoni;
	short m_iEnergyTechBoni;
	short m_iCompTechBoni;
	short m_iPropulsionTechBoni;
	short m_iConstructionTechBoni;
	short m_iWeaponTechBoni;
	// Informationen, wieviele FP´s wir pro Stufe in den einzelnen Techs brauchen und unsere Unique-Forschung
	CResearchInfo ResearchInfo;
	BOOLEAN m_bUniqueReady;	// Ist eine Uniqueforschung beendet und die nächste könnte gestartet werden
	BYTE m_iNumberOfUnique;	// Die Wievielte Uniqueforschung haben wir jetzt
	// Rückgabewert für CalculateResearch Funktion
	CString m_strMessage[8];	// sagt mir, was ich erforscht habe -> Message erstellen in Doc, aktuell 8 Messages mgl.


};

#endif // !defined(AFX_RESEARCH_H__E0439F02_20AD_47EF_9CA1_FE1DB8BCAFC1__INCLUDED_)
