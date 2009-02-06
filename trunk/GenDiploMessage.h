/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// GenDiploMessage.h: Schnittstelle für die Klasse CGenDiploMessage.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENDIPLOMESSAGE_H__9883AB36_6E6C_4277_9E00_944834F5C484__INCLUDED_)
#define AFX_GENDIPLOMESSAGE_H__9883AB36_6E6C_4277_9E00_944834F5C484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MajorRace.h"

class CGenDiploMessage  
{
public:
	// Konstruktion & Destruktion
	CGenDiploMessage();
	virtual ~CGenDiploMessage();

	static CString GenerateMinorAnswer(const CString& minorRaceName, USHORT minorType, short type, BOOL accepted);
	static CString GenerateMajorAnswer(USHORT majorRaceNumber, short type, BOOL accepted);
	static CString GenerateMinorOffer(const CString& minorRaceName, USHORT minorType, short type);
	static CString GenerateMajorOffer(USHORT majorRaceNumber, short type, USHORT latinum, USHORT ressources[5], short duration = -1);
};

#endif // !defined(AFX_GENDIPLOMESSAGE_H__9883AB36_6E6C_4277_9E00_944834F5C484__INCLUDED_)
