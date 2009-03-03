#pragma once
#include "afx.h"

class CResourceManager : public CObject
{
public:

	/// Kostruktor
	CResourceManager(void);

	/// Destruktor
	~CResourceManager(void);

	/// Funktion initiert die Klasse CResourceManager. Sie muss zu Beginn aufgerufen werden
	static void Init(void);

	/// Funktion gibt einen String zurück, der in einer StringTable steht.
	static CString GetString(CString key, BOOLEAN forceBigStarting = FALSE, CString subString1 = "", CString subString2 = "");

private:
	static CMapStringToString m_Strings;
};
