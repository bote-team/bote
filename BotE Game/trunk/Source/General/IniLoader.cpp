#include "stdafx.h"
#include "IniLoader.h"
#include "Options.h"
#include "IOData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIniLoader::CIniLoader(void)
{
	m_sIniPath = CIOData::GetInstance()->GetIniPath();

	// Prüfen ob die Ini-Datei mit der aktuellen Version übereinstimmt
	CString sVersion;
	ReadValue("Special", "VERSION", sVersion);
	
	CFileFind fileFind;
	// wenn es die Datei noch nicht gibt oder die Versionsnummer stimmt nicht überein
	if (sVersion != VERSION_INFORMATION || !fileFind.FindFile(m_sIniPath))
	{
		// Werte schreiben
		WriteValue("General", "AUTOSAVE", "ON");
		WriteValue("General", "USERNAME", "");
		WriteValue("General", "DIFFICULTY", "EASY");
				
		WriteValue("Audio", "HARDWARESOUND", "ON");
		WriteValue("Audio", "SOUND", "ON");
		WriteValue("Audio", "MUSIC", "ON");
		WriteValue("Audio", "MUSICVOLUME", "0.3");

		WriteValue("Video", "SHOWTRADEROUTES", "ON");
		WriteValue("Video", "ANIMATEDICON", "ON");
		WriteValue("Video", "TOOLTIPDELAY", "750");

		WriteValue("Control", "SHOWSCROLLBARS", "OFF");
		WriteValue("Control", "INVERTMOUSE", "OFF");
		WriteValue("Control", "HIDEMENUBAR", "ON");
		
		WriteValue("Special", "VERSION", VERSION_INFORMATION);
		WriteValue("Special", "RANDOMSEED", "-1");
		WriteValue("Special", "STARDENSITY", "35");
		WriteValue("Special", "MINORDENSITY", "30");
		WriteValue("Special", "ANOMALYDENSITY", "9");
		
		WriteValue("Victory_Conditions", "Elimination", "ON");
		WriteValue("Victory_Conditions", "Diplomacy", "ON");
		WriteValue("Victory_Conditions", "Conquest", "ON");
		WriteValue("Victory_Conditions", "Research", "ON");
		WriteValue("Victory_Conditions", "Combat", "ON");
		WriteValue("Victory_Conditions", "Sabotage", "ON");
	}
}

CIniLoader::~CIniLoader(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CIniLoader* CIniLoader::GetInstance(void)
{
	static CIniLoader instance; 
    return &instance;
}

/// Diese Funktion gibt den zum <code>sKey</code> gehörenden Eintrag aus der ini-Datei zurück.
bool CIniLoader::ReadValue(const CString& sSection, const CString& sKey, bool& bValue) const
{
	CString sValue;
	if (!ReadIniValue(sSection, sKey, sValue))
		return false;
	
	if (StringToInt(sValue) == 1)
		bValue = true;
	else
		bValue = false;
	
	return true;
}

/// Diese Funktion gibt den zum <code>sKey</code> gehörenden Eintrag aus der ini-Datei zurück.
bool CIniLoader::ReadValue(const CString& sSection, const CString& sKey, int& nValue) const
{
	CString sValue;
	if (!ReadIniValue(sSection, sKey, sValue))
		return false;
	
	nValue = atoi(sValue);
	return true;
}

/// Diese Funktion gibt den zum <code>sKey</code> gehörenden Eintrag aus der ini-Datei zurück.
bool CIniLoader::ReadValue(const CString& sSection, const CString& sKey, float& fValue) const
{
	CString sValue;
	if (!ReadIniValue(sSection, sKey, sValue))
		return false;
	
	fValue = (float)atof(sValue);
	return true;
}

/// Diese Funktion gibt den zum <code>sKey</code> gehörenden Eintrag aus der ini-Datei zurück.
bool CIniLoader::ReadValue(const CString& sSection, const CString& sKey, CString& sValue) const
{
	if (!ReadIniValue(sSection, sKey, sValue))
		return false;
	return true;
}

/// Diese Funktion schreibt den zum <code>sKey</code> gehörenden Eintrag <code>value</code> in die ini-Datei.
bool CIniLoader::WriteValue(const CString& sSection, const CString& sKey, const CString& sValue)
{	
	if (m_sIniPath.IsEmpty())
		return false;
	if (WritePrivateProfileString(sSection, sKey, sValue, m_sIniPath) == 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
///Diese Funktion wandelt ein Wort, wie z.B. On oder True in einen Wahrheitswert um.
int CIniLoader::StringToInt(const CString& sValue) const
{
	CString sTemp = sValue;
	sTemp.MakeUpper();
	if (sTemp == "ON" || sTemp == "1" || sTemp == "TRUE")
		return 1;
	else
		return 0;
}

/**
 * Diese Funktion gibt den zum <code>sKey</code> gehörenden Eintrag aus der ini-Datei zurück.
 */
bool CIniLoader::ReadIniValue(const CString& sSection, const CString& sKey, CString& sReturnValue) const
{
	if (m_sIniPath.IsEmpty())
		return false;

	char read[_MAX_PATH];
	if (GetPrivateProfileString(sSection, sKey, "", read, _MAX_PATH, m_sIniPath) == 0)
		return false;
	
	sReturnValue = read;
	return true;
}