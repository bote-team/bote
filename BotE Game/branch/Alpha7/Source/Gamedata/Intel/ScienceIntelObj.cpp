#include "stdafx.h"
#include "ScienceIntelObj.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "IOData.h"
#include "General/Loc.h"

IMPLEMENT_SERIAL (CScienceIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CScienceIntelObj::CScienceIntelObj(void) :
m_iFP(0),
m_byNumber(0),
m_nID(0),
m_nTechlevel(-1),
m_nTechType(-1),
m_nChoosenSpecialTech(-1)
{
	m_byType = 1;
	m_KO = CPoint(-1,-1);
}

CScienceIntelObj::CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 1), m_KO(ko), m_nID(id), m_byNumber(number)
{
	m_iFP = NULL;
	m_nTechlevel = -1;
	m_nTechType = -1;
	m_nSpecialTechComplex = RESEARCH_COMPLEX::NONE;
	m_nChoosenSpecialTech = -1;
}

CScienceIntelObj::CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, UINT FP)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 1), m_iFP(FP)
{
	m_KO = CPoint(-1,-1);
	m_nID = NULL;
	m_byNumber = 0;
	m_nTechlevel = -1;
	m_nTechType = -1;
	m_nSpecialTechComplex = RESEARCH_COMPLEX::NONE;
	m_nChoosenSpecialTech = -1;
}

CScienceIntelObj::CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, short techLevel, short techType, RESEARCH_COMPLEX::Typ specialTechComplex, short choosenSpecialTech)
: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 1), m_nTechlevel(techLevel), m_nTechType(techType), m_nSpecialTechComplex(specialTechComplex), m_nChoosenSpecialTech(choosenSpecialTech)
{
	m_iFP = NULL;
	m_KO = CPoint(-1,-1);
	m_nID = NULL;
	m_byNumber = 0;
}

CScienceIntelObj::~CScienceIntelObj(void)
{
}

/// Kopierkonstruktor
CScienceIntelObj::CScienceIntelObj(const CScienceIntelObj & rhs)
{
	m_sOwner = rhs.m_sOwner;
	m_sEnemy = rhs.m_sEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;

	m_byNumber = rhs.m_byNumber;
	m_iFP = rhs.m_iFP;
	m_KO = rhs.m_KO;
	m_nChoosenSpecialTech = rhs.m_nChoosenSpecialTech;
	m_nID = rhs.m_nID;
	m_nSpecialTechComplex = rhs.m_nSpecialTechComplex;
	m_nTechlevel = rhs.m_nTechlevel;
	m_nTechType = rhs.m_nTechType;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CScienceIntelObj::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_byNumber;
		ar << m_iFP;
		ar << m_KO;
		ar << m_nChoosenSpecialTech;
		ar << m_nID;
		ar << m_nSpecialTechComplex;
		ar << m_nTechlevel;
		ar << m_nTechType;
	}
	else if (ar.IsLoading())
	{
		ar >> m_byNumber;
		ar >> m_iFP;
		ar >> m_KO;
		ar >> m_nChoosenSpecialTech;
		ar >> m_nID;
		int nComplex;
		ar >> nComplex;
		m_nSpecialTechComplex = (RESEARCH_COMPLEX::Typ)nComplex;
		ar >> m_nTechlevel;
		ar >> m_nTechType;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CScienceIntelObj::CreateText(CBotEDoc* pDoc, BYTE n, const CString& param)
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
						CMajor* pEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sEnemy));
						if (pEnemy)
						{
							s = pEnemy->GetEmpireNameWithArticle();
							csInput.Replace("$race$", s);
						}
						if (m_KO != CPoint(-1,-1))
						{
							s = pDoc->GetSector(m_KO.x, m_KO.y).GetName();
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
						if (m_nTechlevel != -1)
						{
							switch (m_nTechType)
							{
							case 0: s = CLoc::GetString("BIOTECH"); break;
							case 1: s = CLoc::GetString("ENERGYTECH"); break;
							case 2: s = CLoc::GetString("COMPUTERTECH"); break;
							case 3: s = CLoc::GetString("CONSTRUCTIONTECH"); break;
							case 4: s = CLoc::GetString("PROPULSIONTECH"); break;
							case 5: s = CLoc::GetString("WEAPONTECH"); break;
							}
							csInput.Replace("$techtype$", s);
							s.Format("%d", m_nTechlevel);
							csInput.Replace("$techlevel$", s);
						}
						if (m_nSpecialTechComplex != RESEARCH_COMPLEX::NONE)
						{
							s = pEnemy->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(m_nSpecialTechComplex)->GetComplexName();
							csInput.Replace("$specialtech$", s);
							s = pEnemy->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(m_nSpecialTechComplex)->GetFieldName((BYTE)m_nChoosenSpecialTech);
							csInput.Replace("$choosenspecial$", s);
						}
						s.Format("%d", m_iFP);
						csInput.Replace("$FP$", s);
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
								s = pDoc->GetSector(m_KO.x, m_KO.y).GetName();
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
							if (m_nTechlevel != -1)
							{
								switch (m_nTechType)
								{
								case 0: s = CLoc::GetString("BIOTECH"); break;
								case 1: s = CLoc::GetString("ENERGYTECH"); break;
								case 2: s = CLoc::GetString("COMPUTERTECH"); break;
								case 3: s = CLoc::GetString("CONSTRUCTIONTECH"); break;
								case 4: s = CLoc::GetString("PROPULSIONTECH"); break;
								case 5: s = CLoc::GetString("WEAPONTECH"); break;
								}
								csInput.Replace("$techtype$", s);
								s.Format("%d", m_nTechlevel);
								csInput.Replace("$techlevel$", s);
							}
							if (m_nSpecialTechComplex != RESEARCH_COMPLEX::NONE)
							{
								CMajor* pEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sEnemy));
								if (pEnemy)
								{
									s = pEnemy->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(m_nSpecialTechComplex)->GetComplexName();
									csInput.Replace("$specialtech$", s);
									s = pEnemy->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(m_nSpecialTechComplex)->GetFieldName((BYTE)m_nChoosenSpecialTech);
									csInput.Replace("$choosenspecial$", s);
								}
							}
							s.Format("%d", m_iFP);
							csInput.Replace("$FP$", s);
							m_strEnemyDesc = csInput;
							if (param != "")
							{
								CMajor* pParam = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(param));
								if (pParam)
								{
									s = pParam->GetEmpireNameWithArticle();
									csInput = CLoc::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
								}
							}
							else
								csInput = CLoc::GetString("DO_NOT_KNOW_RESPONSIBLE_RACE");
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
