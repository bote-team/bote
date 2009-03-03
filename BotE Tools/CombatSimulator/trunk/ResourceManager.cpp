#include "stdafx.h"
#include "ResourceManager.h"

// statische Variable initialisieren
CMapStringToString CResourceManager::m_Strings;

CResourceManager::CResourceManager(void)
{
}

CResourceManager::~CResourceManager(void)
{
}

/// Funktion initiert die Klasse CResourceManager. Sie muss zu Beginn aufgerufen werden
void CResourceManager::Init(void)
{
	// Name des zu Öffnenden Files 
	CString fileName="C:\\Botf2\\Data\\Strings\\StringTable.txt";	
	CStdioFile file;						// Varibale vom Typ CStdioFile
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	BOOLEAN foundKey = FALSE;
	CString key;
	// Datei wird geöffnet
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		while (file.ReadString(csInput))
		{
			// Hatten wir einen Key gefunden, so ist die darauffolgende Zeile der Wert, der übernommen wird
			if (foundKey)
			{
				foundKey = FALSE;
				m_Strings.SetAt(key,csInput);
			}
			// Haben einen key gefunden
			else if (csInput.Left(1) == '[' && csInput.Right(1) == ']')
			{
				csInput.Remove('[');
				csInput.Remove(']');
				key = csInput;
				foundKey = TRUE;
			}			
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"StringTable.txt\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();
}

/// Funktion gibt einen String zurück, der in einer StringTable steht.
CString CResourceManager::GetString(CString key, BOOLEAN forceBigStarting, CString subString1, CString subString2)
{
	CString returnString;
	if (!m_Strings.Lookup(key, returnString))
	{
		key += "is missing";
		return key;
	}
	// Haben wir subStrings übergeben, so müssen wir die § Zeichen ersetzen
	if (subString1 != "")
	{
		// hier ein bisl umständlich, aber sonst würde er alle "§" Zeichen ersetzen
		int pos = -1;
		pos = returnString.FindOneOf("§");
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
		CString upper(returnString.GetAt(0));
		returnString.Remove(returnString.GetAt(0));
		returnString.Insert(0, upper.MakeUpper());
	}
	return returnString;
}