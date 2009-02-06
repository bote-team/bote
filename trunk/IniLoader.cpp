#include "stdafx.h"
#include "IniLoader.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIniLoader::CIniLoader(void)
{
	CreateIniFile();
}

CIniLoader::~CIniLoader(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
int CIniLoader::GetValue(CString key)
{
	if (key.IsEmpty())
		return 0;
	key.MakeUpper();
	CString returnString;
	if (m_Strings.Lookup(key, returnString))
		return StringToInt(returnString);
	else
		return 0;
}

/// Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
float CIniLoader::GetFloatValue(CString key)
{
	if (key.IsEmpty())
		return 1.0f;
	key.MakeUpper();
	CString returnString;
	if (m_Strings.Lookup(key, returnString))
		return (float)(atof(returnString));
	else
		return 1.0f;
}

/// Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
CString CIniLoader::GetStringValue(CString key)
{
	if (key.IsEmpty())
		return "";
	key.MakeUpper();
	CString returnString;
	if (m_Strings.Lookup(key, returnString))
		return returnString;
	else
		return "";
}

/// Diese Funktion schreibt den zum <code>key</code> gehörenden Eintrag <code>value</code> in die ini-Datei.
BOOL CIniLoader::WriteIniString(CString key, CString value, CString filename)
{
	CStdioFile file;
	CString csInput;
	// File lesen
	if (file.Open(filename, CFile::modeRead | CFile::typeText))
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString tmpKey = csInput.Tokenize("=", pos);
			if (tmpKey == key)
			{
				tmpKey += "=";
				tmpKey += value;
				CString returnValue;
				// Stringmap aktualisieren
				if (m_Strings.Lookup(key, returnValue))
					m_Strings.SetAt(key, value);
				break;
			}
		}
		file.Close();
	}
	else
		return FALSE;
	
	// jetzt neue Daten in das File schreiben
	if (file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		POSITION pos = m_Strings.GetStartPosition();
		while (pos != NULL)
		{
			CString key;
			CString value;
			CString s;
			m_Strings.GetNextAssoc(pos, key, value);
			s.Format("%s=%s",key,value);
			file.WriteString(s+"\n");
		}			
		file.Close();
	}
	else
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion liest das ini-File ein und speichert die Daten in der CStringToStringMap. Konnte die ini-
/// Datei aus irgendeinem Grund nicht geöffnet werden liefert die Funktion <code>FALSE</code> zurück, ansonsten
/// wird <code>TRUE</code> zurückgegeben.
BOOL CIniLoader::ReadIniFile(CString filename)
{
	CStdioFile file;
	CString csInput;
	if (file.Open("BotE.ini", CFile::modeRead | CFile::typeText))
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString key = csInput.Tokenize("=", pos);
			key.MakeUpper();
			CString value = csInput.Tokenize("=", pos);
			if (!key.IsEmpty())
				m_Strings.SetAt(key,value);
		}
		file.Close();
	}
	else
		return FALSE;
	return TRUE;
}

/// Funktion erstellt die ini-Datei und schreibt einige Standartdaten in die Ini-Datei.
void CIniLoader::CreateIniFile(CString filename)
{
	if (!ReadIniFile(filename))
	{
		CStdioFile file;	
		if (file.Open(filename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			file.WriteString("AUTOSAVE=ON\n");
			file.WriteString("HARDWARESOUND=ON\n");
			file.WriteString("MUSIC=ON\n");
			file.WriteString("MUSICVOLUME=0.3\n");
			file.WriteString("SOUND=ON\n");
			file.WriteString("SHOWTRADEROUTES=ON\n");
			file.WriteString("USERNAME=\n");
			file.WriteString("DIFFICULTY=NORMAL\n");
			file.WriteString("SMOOTHGALAXY=OFF\n");
			file.WriteString("SMOOTHSCALING=ON\n");
		}
		file.Close();
		ReadIniFile(filename);
	}
}

///Diese Funktion wandelt ein Wort, wie z.B. On oder True in einen Wahrheitswert um.
int CIniLoader::StringToInt(CString string)
{
	string.MakeUpper();
	if (string == "ON" || string == "1" || string == "TRUE")
		return 1;
	else
		return 0;
}