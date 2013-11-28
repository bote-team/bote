#include "stdafx.h"
#include "MilitaryIntelObj.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "IOData.h"
#include "General/Loc.h"

IMPLEMENT_SERIAL (CMilitaryIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMilitaryIntelObj::CMilitaryIntelObj(void) :
	m_KO(),
	m_nID(0),
	m_nNumber(0),
	m_bTroop(FALSE),
	m_bShip(FALSE),
	m_bBuilding(FALSE)
{
	m_byType = 2;
	m_KO = CPoint(-1,-1);
}

CMilitaryIntelObj::CMilitaryIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, USHORT number,
		BOOLEAN building, BOOLEAN ship, BOOLEAN troop)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 2), m_KO(ko), m_nID(id), m_nNumber(number), m_bTroop(troop), m_bShip(ship), m_bBuilding(building)
{
}

CMilitaryIntelObj::~CMilitaryIntelObj(void)
{
}

/// Kopierkonstruktor
CMilitaryIntelObj::CMilitaryIntelObj(const CMilitaryIntelObj & rhs)
{
	m_sOwner = rhs.m_sOwner;
	m_sEnemy = rhs.m_sEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;

	m_bBuilding = rhs.m_bBuilding;
	m_bShip = rhs.m_bShip;
	m_bTroop = rhs.m_bTroop;
	m_KO = rhs.m_KO;
	m_nID = rhs.m_nID;
	m_nNumber = rhs.m_nNumber;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMilitaryIntelObj::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_bBuilding;
		ar << m_bShip;
		ar << m_bTroop;
		ar << m_KO;
		ar << m_nID;
		ar << m_nNumber;
	}
	else if (ar.IsLoading())
	{
		ar >> m_bBuilding;
		ar >> m_bShip;
		ar >> m_bTroop;
		ar >> m_KO;
		ar >> m_nID;
		ar >> m_nNumber;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CMilitaryIntelObj::CreateText(CBotEDoc* pDoc, BYTE n, const CString& param)
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
							s = pEnemy->GetRaceNameWithArticle();
							csInput.Replace("$race$", s);
						}
						if (m_KO != CPoint(-1,-1))
						{
							s = pDoc->GetSystem(m_KO.x, m_KO.y).GetLongName();
							csInput.Replace("$system$", s);
						}
						if (m_nID != NULL)
						{
							if (m_bBuilding && m_nID < 10000)
							{
								s = pDoc->GetBuildingName(m_nID);
								csInput.Replace("$building$", s);
							}
							else if (m_bShip && m_nID < 20000)
							{
								// zwischen Schiff und Station unterscheiden
								if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == SHIP_TYPE::OUTPOST)
									s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
										CLoc::GetString("CLASS"), CLoc::GetString("OUTPOST"));
								else if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == SHIP_TYPE::STARBASE)
									s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
										CLoc::GetString("CLASS"), CLoc::GetString("STARBASE"));
								else
								{
									if (m_nNumber == 1)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(), CLoc::GetString("CLASS"), CLoc::GetString("SHIP"));
									else
										s = CLoc::GetString("SHIPS");
								}
								csInput.Replace("$ship$", s);
							}
							else if (m_bTroop)
							{
								if (m_nNumber == 1)
									s = pDoc->m_TroopInfo.GetAt(m_nID - 20000).GetName();
								else
									s = CLoc::GetString("TROOPS");
								csInput.Replace("$troop$", s);
							}
						}
						if (m_nNumber != NULL)
						{
							s.Format("%d", m_nNumber);
							csInput.Replace("$number$", s);
						}
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
								s = pDoc->GetSystem(m_KO.x, m_KO.y).GetLongName();
								csInput.Replace("$system$", s);
							}
							if (m_nID != NULL)
							{
								if (m_bBuilding && m_nID < 10000)
								{
									s = pDoc->GetBuildingName(m_nID);
									csInput.Replace("$building$", s);
								}
								else if (m_bShip && m_nID < 20000)
								{
									// zwischen Schiff und Station unterscheiden
									if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == SHIP_TYPE::OUTPOST)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
											CLoc::GetString("CLASS"), CLoc::GetString("OUTPOST"));
									else if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == SHIP_TYPE::STARBASE)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
											CLoc::GetString("CLASS"), CLoc::GetString("STARBASE"));
									else
									{
										if (m_nNumber == 1)
											s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(), CLoc::GetString("CLASS"), CLoc::GetString("SHIP"));
										else
											s = CLoc::GetString("SHIPS");
									}
									csInput.Replace("$ship$", s);
								}
								else if (m_bTroop)
								{
									if (m_nNumber == 1)
										s = pDoc->m_TroopInfo.GetAt(m_nID - 20000).GetName();
									else
										s = CLoc::GetString("TROOPS");
									csInput.Replace("$troop$", s);
								}
							}
							if (m_nNumber != NULL)
							{
								s.Format("%d", m_nNumber);
								csInput.Replace("$number$", s);
							}
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
