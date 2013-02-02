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

	/// Diese Funktion generiert einen einmaligen Schiffsnamen. Als Parameter werden dafür die Rasse <code>sRaceID</code>
	/// und ein Parameter, welcher angibt ob es sich um eine Station handelt <code>station</code> übergeben.
	CString GenerateShipName(const CString& sRaceID, BOOLEAN station);

private:
	/// Feld aller Schiffsnamen
	map<CString, vector<CString> > m_mShipNames;

	/// Feld aller benutzten Schiffsnamen
	map<CString, vector<CString> > m_mStillAvailableNames;

	/// Zählvariable, welche die Buchstabenbennung am Ende vom Schiffsnamen vornimmt. Zu Beginn auf '' gesetzt.
	map<CString, unsigned> m_mCounter;
};
