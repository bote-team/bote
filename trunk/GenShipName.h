/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// GenShipName.h: Schnittstelle für die Klasse CGenShipName.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENSHIPNAME_H__0751A0E0_6DF3_42DC_A224_8CFA37A29AC2__INCLUDED_)
#define AFX_GENSHIPNAME_H__0751A0E0_6DF3_42DC_A224_8CFA37A29AC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Options.h"

class CGenShipName : public CObject  
{
public:
	DECLARE_SERIAL (CGenShipName)
	
	/// ctor
	CGenShipName();
	
	/// dtor
	virtual ~CGenShipName();
	
	/// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	
	/// Diese Funktion generiert einen einmaligen Schiffsnamen. Als Parameter werden dafür die Rasse <code>race</code>
	/// und ein Parameter, welcher angibt ob es sich um eine Station handelt <code>station</code> übergeben.
	CString GenerateShipName(BYTE race, BOOLEAN station);

private:
	/// Feld aller Schiffsnamen
	CStringArray m_ShipNames[7];	
	
	/// Feld aller benutzten Schiffsnamen
	CStringArray m_UsedNames[7];
	
	/// Zählvariable, welche die Buchstabenbennung am Ende vom Schiffsnamen vornimmt. Zu Beginn auf 'A' gesetzt.
	USHORT m_iCounter[7];
};

#endif // !defined(AFX_GENSHIPNAME_H__0751A0E0_6DF3_42DC_A224_8CFA37A29AC2__INCLUDED_)
