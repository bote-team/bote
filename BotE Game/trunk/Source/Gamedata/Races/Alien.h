/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#include "Minor.h"

#pragma once

class CAlien : public CMinor
{

public:
	/// Standardkonstruktur
	CAlien(void);
	/// Standarddestruktor
	virtual ~CAlien(void);

	/// Funktion zum erstellen von Weltraummonstern
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	virtual void Create(const CStringArray& saInfo, int& nPos);

};
