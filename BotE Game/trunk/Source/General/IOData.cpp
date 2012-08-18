#include "stdafx.h"
#include "IOData.h"
#include "botf2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIOData::CIOData(void)
{
	// Pfad zur BotE.exe Datei ermitteln ermitteln
	m_sAppPath = GetBotEAppPath();

	// Pfad zu den eigenen Dateien ermitteln
	m_sUserDataPath = GetUserDataPath();

	// konnte aus irgendeinem Grund nicht auf den Ordner der eigenen Dateien zugegriffen werden,
	// so wird dieser Pfad gleich dem Anwendungspfad gesetzt
	if (m_sUserDataPath == "")
		m_sUserDataPath = m_sAppPath;
}

CIOData::~CIOData(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CIOData* CIOData::GetInstance(void)
{
	static CIOData instance;
    return &instance;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion ermittelt den Pfad zur BotE.exe Datei.
/// @return Pfad zur BotE.exe Datei
CString CIOData::GetBotEAppPath(void) const
{
	CString sPath = "";

	// absoluten Pfad ermitteln
	char fullPath[_MAX_PATH];
	_fullpath(fullPath, "", _MAX_PATH);

	sPath = fullPath;
#ifdef DEVELOPMENT_VERSION
	sPath += "\\game";
#endif
	sPath += "\\";

	return sPath;
}

/// Funktion ermittelt den Pfad zum Ordner "Eigene Dateien".
/// @return Pfad zum Ordner "Eigene Dateien"
CString CIOData::GetUserDataPath(void) const
{
	CString sPath = "";

	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, szPath)))
	{
		CString sUserDataPath = szPath;
		sPath = sUserDataPath + "\\Birth of the Empires";
		if (!CreateDirectory(sPath, NULL))
		{
			// Error creating directory
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				AfxMessageBox("Error:\nCould not create \"Birth of the Empires\" folder in\n" + sUserDataPath);
				return "";
			}
		}
		sPath += "\\";
	}

	return sPath;
}

CString CIOData::GetLogPath() const {
	const CCommandLineParameters* const clp = dynamic_cast<CBotf2App*>(AfxGetApp())->GetCommandLineParameters();
	const std::string& log_path = clp->LogDir();
	if(log_path.empty())
		return m_sUserDataPath + BOTE_LOG_FILE;
	return CString(log_path.c_str()) + BOTE_LOG_FILE;
}
