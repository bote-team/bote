// CombatSimulatorDoc.h : Schnittstelle der Klasse CCombatSimulatorDoc
//
#pragma once
#include "Combat.h"
#include "Fleet.h"
#include "ShipInfo.h"

class CCombatSimulatorDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CCombatSimulatorDoc();
	DECLARE_DYNCREATE(CCombatSimulatorDoc)

// Attribute
public:
	CCombat combat;
	ShipArray m_ShipArray;
	ShipInfoArray m_ShipInfoArray;
	int SlowingSpeed;
	CString race;
	short repeat;
	map<CString, int> wins;
// Vorgänge
public:
	void ApplyShipsAtStartup();
	void ReadShipInfosFromFile();
	void BuildShip(int ID);
	
// Überschreibungen
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementierung
public:
	virtual ~CCombatSimulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	
protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};


