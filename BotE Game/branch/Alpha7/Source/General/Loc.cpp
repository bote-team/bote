#include "stdafx.h"
#include "Loc.h"
#include "IOData.h"


// statische Variable initialisieren
CMapStringToString CLoc::m_StringTable;

CLoc::CLoc(void)
{
}

CLoc::~CLoc(void)
{
}

/// Funktion initiert die Klasse CLoc. Sie muss zu Beginn aufgerufen werden
void CLoc::Init(void)
{
	// Name des zu öffnenden Files
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Strings\\StringTable.txt";
	CStdioFile file;

	// Datei wird geöffnet
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
		BOOLEAN foundKey = FALSE;
		CString key;
		while (file.ReadString(csInput))
		{
			// Haben einen key gefunden
			if (csInput.Left(1) == '[' && csInput.Right(1) == ']')
			{
				csInput.Remove('[');
				csInput.Remove(']');
				key = csInput;
				foundKey = TRUE;
			}
			// Hatten wir einen Key gefunden, so ist die darauffolgende Zeile der Wert, der übernommen wird
			else if (foundKey)
			{
				foundKey = FALSE;
				m_StringTable.SetAt(key, csInput);
			}
		}
		file.Close();
	}
	else
	{
		AfxMessageBox("Error! Could not open file \"StringTable.txt\" ...");
	}
}

/// Funktion gibt einen String zurück, der in einer StringTable steht.
CString CLoc::GetString(const CString& key, BOOLEAN forceBigStarting, const CString& subString1, const CString& subString2)
{
	CString returnString;
	if (!m_StringTable.Lookup(key, returnString))
		return key + " is missing";

	// Haben wir subStrings übergeben, so müssen wir die § Zeichen ersetzen
	if (subString1 != "")
	{
		// hier ein bisl umständlich, aber sonst würde er alle "§" Zeichen ersetzen
		int pos = returnString.FindOneOf("§");
		if (pos != -1)
		{
			returnString.Delete(pos);
			returnString.Insert(pos,subString1);
			if (subString2 != "")
				returnString.Replace("§",subString2);
		}
	}
	if (forceBigStarting)
	{
		CString upper = (CString)returnString.GetAt(0);
		returnString.SetAt(0, upper.MakeUpper().GetAt(0));
	}
	return returnString;
}

/*
CString CLoc::Text(LPCTSTR lpszKeyFormat, ...)
{
	CString value;
	if (!m_StringTable.Lookup(lpszKeyFormat, value))
		return CString(lpszKeyFormat) + " is missing";

	// Format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, lpszKeyFormat);
	LPTSTR lpszTemp;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER, value, 0, 0, (LPTSTR)&lpszTemp, 0, &argList) == 0 || lpszTemp == NULL)
	{
		AfxThrowMemoryException();
	}

	// Copy lpszTemp into the result string
	value = CString(lpszTemp); // Compatible with VC6 and VC7.

	// Clean-up
	LocalFree(lpszTemp);
	va_end(argList);

	return value;
}
*/
