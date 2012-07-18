#include "stdafx.h"
#include "EcoIntelObj.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CEcoIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CEcoIntelObj::CEcoIntelObj(void) :
	m_byNumber(0),
	m_nID(0),
	m_iCredits(0)
{
	m_byType = 0;
	m_KO = CPoint(-1,-1);
}

CEcoIntelObj::CEcoIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 0), m_KO(ko), m_nID(id), m_byNumber(number)
{
	m_iCredits = NULL;
}

CEcoIntelObj::CEcoIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, int credits)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 0), m_iCredits(credits)
{
	m_KO = CPoint(-1,-1);
	m_nID = 0;
	m_byNumber = 0;
}

CEcoIntelObj::~CEcoIntelObj(void)
{
}

/// Kopierkonstruktor
CEcoIntelObj::CEcoIntelObj(const CEcoIntelObj & rhs)
{
	m_sOwner = rhs.m_sOwner;
	m_sEnemy = rhs.m_sEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;

	m_KO = rhs.m_KO;
	m_nID = rhs.m_nID;
	m_byNumber = rhs.m_byNumber;
	m_iCredits = rhs.m_iCredits;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEcoIntelObj::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_KO;
		ar << m_nID;
		ar << m_byNumber;
		ar << m_iCredits;
	}
	else if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_nID;
		ar >> m_byNumber;
		ar >> m_iCredits;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CEcoIntelObj::CreateText(CBotf2Doc* pDoc, BYTE n, const CString& param)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	if (this->GetIsSpy())
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSpyOff.data";		// Name des zu Öffnenden Files
	else
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSabOff.data";		// Name des zu Öffnenden Files
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			// Rasse bestimmen
			if (s == m_sOwner)
			{
				s = csInput.Tokenize(":", pos);
				// Typ (Wirtschaft, Militär...) bestimmen
				if (atoi(s) == m_byType)
				{
					s = csInput.Tokenize(":", pos);
					// Nummer der Nachricht bestimmen
					if (atoi(s) == n)
					{
						csInput.Delete(0, pos);
						// in csInput steht nun die Beschreibung für den Aggressor
						// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
						CMajor* pEnemey = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sEnemy));
						if (pEnemey)
						{
							s = pEnemey->GetEmpireNameWithArticle();
							csInput.Replace("$race$", s);
						}

						if (m_KO != CPoint(-1,-1))
						{
							s = pDoc->m_Sectors.at(m_KO.x+(m_KO.y)*STARMAP_SECTORS_HCOUNT).GetName();
							csInput.Replace("$system$", s);
						}
						if (m_nID != NULL)
						{
							s = pDoc->GetBuildingName(m_nID);
							csInput.Replace("$building$", s);
						}
						if (m_byNumber != NULL)
						{
							s.Format("%d", m_byNumber);
							csInput.Replace("$number$", s);
						}
						s.Format("%d", m_iCredits);
						csInput.Replace("$credits$", s);
						m_strOwnerDesc = csInput;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (this->GetIsSpy())
			AfxMessageBox("Error! Could not open file \"MajorIntelSpyOff.data\"...");
		else
			AfxMessageBox("Error! Could not open file \"MajorIntelSabOff.data\"...");
		exit(1);
	}
	file.Close();

	// Nachricht für das Opfer erstellen
	if (this->GetIsSabotage())
	{
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSabDef.data";			// Name des zu Öffnenden Files
		if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
		{
			while (file.ReadString(csInput))
			{
				int pos = 0;
				CString s = csInput.Tokenize(":", pos);
				// Rasse bestimmen
				if (s == m_sEnemy)
				{
					s = csInput.Tokenize(":", pos);
					// Typ (Wirtschaft, Militär...) bestimmen
					if (atoi(s) == m_byType)
					{
						s = csInput.Tokenize(":", pos);
						// Nummer der Nachricht bestimmen
						if (atoi(s) == n)
						{
							csInput.Delete(0, pos);
							// in csInput steht nun die Beschreibung für das Opfer
							// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
							if (m_KO != CPoint(-1,-1))
							{
								s = pDoc->m_Sectors.at(m_KO.x+(m_KO.y)*STARMAP_SECTORS_HCOUNT).GetName();
								csInput.Replace("$system$", s);
							}
							if (m_nID != NULL)
							{
								s = pDoc->GetBuildingName(m_nID);
								csInput.Replace("$building$", s);
							}
							if (m_byNumber != NULL)
							{
								s.Format("%d", m_byNumber);
								csInput.Replace("$number$", s);
							}
							s.Format("%d", m_iCredits);
							csInput.Replace("$credits$", s);
							m_strEnemyDesc = csInput;
							if (param != "")
							{
								CMajor* pParam = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(param));
								if (pParam)
								{
									s = pParam->GetEmpireNameWithArticle();
									csInput = CResourceManager::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
								}
							}
							else
								csInput = CResourceManager::GetString("DO_NOT_KNOW_RESPONSIBLE_RACE");
							m_strEnemyDesc += " "+csInput;
							break;
						}
					}
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open file \"MajorIntelSabDef.data\"...");
			exit(1);
		}
		file.Close();
	}
}
