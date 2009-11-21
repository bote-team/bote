#include "stdafx.h"
#include "IOData.h"

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
	m_sAppPath = "";

	// absoluten Pfad ermitteln
	char fullPath[_MAX_PATH];
	_fullpath(fullPath, "", _MAX_PATH);
	
	m_sAppPath = fullPath;
	//m_sAppPath = "F:\\C++\\Projekte\\BotE Projekt\\BotE Install\\BotE complete";
	m_sAppPath += "\\";
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