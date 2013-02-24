/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// GenShipName.h: Schnittstelle für die Klasse CGenShipName.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <map>
#include <vector>

using namespace std;

// forwar declaration
class CBotEDoc;

class CGenShipName : public CObject
{
public:
	DECLARE_SERIAL (CGenShipName)

	/// ctor
	CGenShipName();
	CGenShipName(const CGenShipName& o);

	/// dtor
	virtual ~CGenShipName();

	/// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/// Funktion zum einmaligen Einlesen der ganzen Schiffsnamen.
	void Init(CBotEDoc* pDoc);

	/// Diese Funktion generiert einen einmaligen Schiffsnamen.
	/// @param sRaceID ID der Rasse, um nach einem Schiffsnamen aus einer ...ShipNames.data Datei zu suchen
	/// @param sRaceName Rassenname mit Postfix (wird verwendet, wenn kein zugehörige ShipNames.data Datei vorhanden ist)
	/// @param bIsStation Gibt an, ob es sich um eine Station handelt
	/// return Schiffs- bzw. Staionsname (sollte einmalig sein!)
	CString GenerateShipName(const CString& sRaceID, const CString &sRaceName, bool bIsStation);

private:
	/// Feld aller Schiffsnamen
	map<CString, vector<CString> > m_mShipNames;

	/// Feld aller benutzten Schiffsnamen
	map<CString, vector<CString> > m_mStillAvailableNames;

	/// Zählvariable, welche die Buchstabenbennung am Ende vom Schiffsnamen vornimmt. Zu Beginn auf '' gesetzt.
	map<CString, unsigned> m_mCounter;
};
