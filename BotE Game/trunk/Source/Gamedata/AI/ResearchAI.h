/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#include "Options.h"

// forward declaration
class CBotEDoc;

class CResearchAI
{
public:
	/// Standardkonstruktor
	CResearchAI(void);

	/// Standarddestruktor
	~CResearchAI(void);

	/// Diese Funktion führt die Berechnungen für die künstliche Intelligenz bei der Zuteilung der
	/// Forschungspunkte aus.
	/// @param pDoc Zeiger auf das Dokument
	static void Calc(CBotEDoc* pDoc);
};
