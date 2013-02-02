/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// GenDiploMessage.h: Schnittstelle für die Klasse CGenDiploMessage.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "AI\DiplomacyAI.h"

class CGenDiploMessage
{
public:
	// Konstruktion & Destruktion
	CGenDiploMessage(void);
	virtual ~CGenDiploMessage(void);

	/// Statische Funktion zum Erzeugen eines Angebotstextes für ein diplomatisches Angebot
	/// einer Majorrace.
	/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
	/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
	static bool GenerateMajorOffer(CDiplomacyInfo& info);

	/// Statische Funktion zum Erzeugen eines Antworttextes auf ein diplomatisches Angebot.
	/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
	/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
	static bool GenerateMajorAnswer(CDiplomacyInfo& info);

	/// Statische Funktion zum Erzeugen eines Angebotstextes für ein diplomatisches Angebot
	/// einer Minorrace.
	/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
	/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
	static bool GenerateMinorOffer(CDiplomacyInfo& info);

	/// Statische Funktion zum Erzeugen eines Antworttextes auf ein diplomatisches Angebot.
	/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
	/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
	static bool GenerateMinorAnswer(CDiplomacyInfo& info);
};
