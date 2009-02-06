#include "stdafx.h"
#include "MilitaryIntelObj.h"
#include "Botf2Doc.h"

IMPLEMENT_SERIAL (CMilitaryIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMilitaryIntelObj::CMilitaryIntelObj(void)
{
	m_byType = 2;
	m_KO = CPoint(-1,-1);
}

CMilitaryIntelObj::CMilitaryIntelObj(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, USHORT number,
		BOOLEAN building, BOOLEAN ship, BOOLEAN troop)
	: CIntelObject(owner, enemy, round, isSpy, 2), m_KO(ko), m_nID(id), m_nNumber(number), m_bBuilding(building), m_bShip(ship), m_bTroop(troop)
{
}

CMilitaryIntelObj::~CMilitaryIntelObj(void)
{
}

/// Kopierkonstruktor
CMilitaryIntelObj::CMilitaryIntelObj(const CMilitaryIntelObj & rhs)
{
	m_byOwner = rhs.m_byOwner;
	m_byEnemy = rhs.m_byEnemy;
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
void CMilitaryIntelObj::CreateText(CBotf2Doc* pDoc, BYTE n, BYTE param)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	if (this->GetIsSpy())
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSpyOff.data";		// Name des zu Öffnenden Files 
	else
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSabOff.data";		// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			// Rasse bestimmen
			if (atoi(s) == m_byOwner)
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
						switch (m_byEnemy)
						{
						case HUMAN:		{s = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
						case FERENGI:	{s = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
						case KLINGON:	{s = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
						case ROMULAN:	{s = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
						case CARDASSIAN:{s = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
						case DOMINION:	{s = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
						}
						csInput.Replace("$race$", s);
						if (m_KO != CPoint(-1,-1))
						{
							s = pDoc->m_Sector[m_KO.x][m_KO.y].GetName(TRUE);
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
								if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == OUTPOST)
									s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
										CResourceManager::GetString("CLASS"), CResourceManager::GetString("OUTPOST"));
								else if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == STARBASE)
									s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
										CResourceManager::GetString("CLASS"), CResourceManager::GetString("STARBASE"));
								else
								{
									if (m_nNumber == 1)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(), CResourceManager::GetString("CLASS"), CResourceManager::GetString("SHIP"));
									else
										s = CResourceManager::GetString("SHIPS");
								}
								csInput.Replace("$ship$", s);
							}
							else if (m_bTroop)
							{
								if (m_nNumber == 1)
									s = pDoc->m_TroopInfo.GetAt(m_nID - 20000).GetName();
								else
									s = CResourceManager::GetString("TROOPS");
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
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSabDef.data";			// Name des zu Öffnenden Files 
		if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
		{
			while (file.ReadString(csInput))
			{
				int pos = 0;
				CString s = csInput.Tokenize(":", pos);
				// Rasse bestimmen
				if (atoi(s) == m_byEnemy)
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
								s = pDoc->m_Sector[m_KO.x][m_KO.y].GetName(TRUE);
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
									if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == OUTPOST)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
											CResourceManager::GetString("CLASS"), CResourceManager::GetString("OUTPOST"));
									else if (pDoc->m_ShipInfoArray[m_nID - 10000].GetShipType() == STARBASE)
										s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(),
											CResourceManager::GetString("CLASS"), CResourceManager::GetString("STARBASE"));
									else
									{
										if (m_nNumber == 1)
											s.Format("%s-%s %s", pDoc->m_ShipInfoArray.GetAt(m_nID - 10000).GetShipClass(), CResourceManager::GetString("CLASS"), CResourceManager::GetString("SHIP"));
										else
											s = CResourceManager::GetString("SHIPS");
									}
									csInput.Replace("$ship$", s);
								}
								else if (m_bTroop)
								{
									if (m_nNumber == 1)
										s = pDoc->m_TroopInfo.GetAt(m_nID - 20000).GetName();
									else
										s = CResourceManager::GetString("TROOPS");
									csInput.Replace("$troop$", s);
								}							
							}
							if (m_nNumber != NULL)
							{
								s.Format("%d", m_nNumber);
								csInput.Replace("$number$", s);
							}						
							m_strEnemyDesc = csInput;
							if (param != NOBODY)
							{
								switch (param)
								{
								case HUMAN:		{s = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
								case FERENGI:	{s = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
								case KLINGON:	{s = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
								case ROMULAN:	{s = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
								case CARDASSIAN:{s = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
								case DOMINION:	{s = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
								}
								csInput = CResourceManager::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
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