// MinorRace.h: Schnittstelle für die Klasse CMinorRace.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINORRACE_H__EC73F601_FB3E_4558_B093_AC1645FE476B__INCLUDED_)
#define AFX_MINORRACE_H__EC73F601_FB3E_4558_B093_AC1645FE476B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Options.h"
#include "afxtempl.h"

// Die verschiedenen Arten, welchen Status eine Minorrace gegenüber ein Majorrace haben kann
/*	WAR						-1
	NO_AGREEMENT			0
	NON_AGGRESSION_PACT		1
	TRADE_AGREEMENT			2
	FRIENDSHIP_AGREEMENT	3
	ALLIANCE				4
	AFFILIATION				5	// Wie Kooperation, nur wenn jemand uns Krieg erklärt, erklärt die Minorrace ihm auch Krieg und umgekehrt
	MEMBERSHIP				6
	PRESENT					10
	CORRUPTION				11
*/


typedef unsigned short USHORT;

class CMinorRace : public CObject  
{
public:
// Konstruktion & Destruktion
	CMinorRace();
	virtual ~CMinorRace();
// Kopierkonstruktor
	CMinorRace(const CMinorRace & rhs);
// Zuweisungsoperator
	CMinorRace & operator=(const CMinorRace &);

// Testweise mal den "<" und ">" Operator überschrieben -> es wird jetzt sortiert.
// hier wird aktuell nach dem Rassennamen sortiert
	bool operator< (const CMinorRace& elem2) const { return m_strRaceName < elem2.m_strRaceName;}
	bool operator> (const CMinorRace& elem2) const { return m_strRaceName > elem2.m_strRaceName;}

/// Zugriffsfunktionen
	/// zum Lesen der Membervariablen
	CString GetRaceName() const {return m_strRaceName;}
	CString GetHomeplanetName() const {return m_strHomePlanet;}
	CString GetRaceDescription() const {return m_strRaceDescription;}
	CString GetGraphicName() const {return m_strGraphicName;}
	BYTE GetRelationshipToMajorRace(USHORT race) const {return m_iRelationship[race];}
	BYTE GetTechnologicalProgress() const {return m_iTechnologicalProgress;}
	BOOL GetProperty(BYTE kind) const {return m_bProperty[kind];}
	BYTE GetCorruptibility() const {return m_iCorruptibility;}
	BOOLEAN GetSpaceflightNation() const {return m_bSpaceflightNation;}
	
	/// zum Schreiben der Membervariablen
	void SetHomePlanet(const CString& name) {m_strHomePlanet = name;}
	void SetRaceName(const CString& name) {m_strRaceName = name;}
	void SetRaceDescription(const CString& desc) {m_strRaceDescription = desc;}
	void SetGraphicName(const CString& name) {m_strGraphicName = name;}
	void SetRelationship(USHORT race, BYTE relation) {m_iRelationship[race] = relation;}
	void SetTechnologicalProgress(BYTE progress) {m_iTechnologicalProgress = progress;}
	void SetProperty(BYTE kind,BOOLEAN is) {m_bProperty[kind] = is;}
	void SetCorruptibility(BYTE cor) {m_iCorruptibility = cor;}
	void SetSpaceflightNation(BOOLEAN is) {m_bSpaceflightNation = is;}
	
	
/// sonstige Funktionen
	void Reset();

private:
	CString m_strRaceName;				///< Der Name der Minorrace
	CString m_strHomePlanet;			///< Der Name des Heimatplaneten der Minorrace
	CString m_strRaceDescription;		///< Die Beschreibung der Minorrace
	CString m_strGraphicName;			///< Name der zugehörigen Grafikdatei
	BYTE m_iRelationship[7];			///< Die einzelnen Beziehung der Minorrace zu den Majorraces
	BYTE m_iTechnologicalProgress;		///< wie fortschrittlich ist die Minorrace?
	BOOL m_bProperty[11];			///< Art der Minorrace (landwirtschaftlich, kriegerisch, industriell usw.)
	BYTE m_iCorruptibility;				///< wie stark ändert sich die Beziehung beim Geschenke geben?
	BOOLEAN m_bSpaceflightNation;		///< ist die Minorrace eine Raumfahrtnation?
};

#endif // !defined(AFX_MINORRACE_H__EC73F601_FB3E_4558_B093_AC1645FE476B__INCLUDED_)
