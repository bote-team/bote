#include "stdafx.h"
#include "Alien.h"
#include "AI\MinorAI.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAlien::CAlien(void) :
	CMinor(RACE_TYPE_ALIEN)
{

}

CAlien::~CAlien(void)
{
}

/// Funktion zum erstellen von Weltraummonstern
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CAlien::Create(const CStringArray& saInfo, int& nPos)
{
	AssertBotE(nPos >= 0);

	CMinor::Reset(true);

	// Minorrace nun anlegen
	m_sID				= saInfo[nPos++];				// Rassen-ID
	m_sID.Remove(':');
	m_sName				= saInfo[nPos++];				// Rassenname
	m_sDescription				= saInfo[nPos++];				// Rassenbeschreibung

	// grafische Attribute
	m_sGraphicFile		= saInfo[nPos++];				// Name der zugehörigen Grafikdatei
	m_iTechnologicalProgress = atoi(saInfo[nPos++]);

	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		AssertBotE(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}
	m_nSpecialAbility	= atoi(saInfo[nPos++]);

	m_bSpaceflight		= atoi(saInfo[nPos++]) == 0 ? false : true;
	m_iCorruptibility	= atoi(saInfo[nPos++]);

	// Schiffsnummer vergeben
	m_byShipNumber = PLAYER_RACES::MINORNUMBER;

	// Minorrace - KI anlegen
	m_pDiplomacyAI = boost::make_shared<CMinorAI>(shared_from_this());
}
