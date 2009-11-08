// CombatSimulatorDoc.cpp : Implementierung der Klasse CCombatSimulatorDoc
//

#include "stdafx.h"
#include "CombatSimulator.h"

#include "CombatSimulatorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCombatSimulatorDoc

IMPLEMENT_DYNCREATE(CCombatSimulatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CCombatSimulatorDoc, CDocument)
END_MESSAGE_MAP()


// CCombatSimulatorDoc-Erstellung/Zerstörung

CCombatSimulatorDoc::CCombatSimulatorDoc()
{
	// TODO: Hier Code für One-Time-Konstruktion einfügen

}

CCombatSimulatorDoc::~CCombatSimulatorDoc()
{
	m_ShipArray.RemoveAll();
	m_ShipInfoArray.RemoveAll();
	combat.Reset();
}

BOOL CCombatSimulatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)
	srand((unsigned)time(NULL));
	
	combat.Reset();
	m_ShipArray.RemoveAll();
	m_ShipInfoArray.RemoveAll();

	this->ReadShipInfosFromFile();
	this->ApplyShipsAtStartup();
	short r = repeat;
	for (int i = 0; i <= DOMINION; i++)
		wins[i] = 0;
	
	do 
	{
		for (int y = 0; y < m_ShipArray.GetSize(); y++)
		{
			CArray<CShip*,CShip*> pShipArray;
			pShipArray.RemoveAll();
			for (int i = 0; i < m_ShipArray.GetSize(); i++)
			{
				// Wenn das Schiff nicht unserer Rasse gehört
				if (m_ShipArray.GetAt(i).GetOwnerOfShip() != m_ShipArray.GetAt(y).GetOwnerOfShip())
				{
					// Dann können wir diese Schiffe an einem Kampf teilnehmen lassen
					pShipArray.Add(&m_ShipArray.ElementAt(i));
					// Wenn das Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
					if (m_ShipArray.GetAt(i).GetFleet() != 0)
						for (int j = 0; j < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); j++)
							pShipArray.Add(m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j));
				}
			}
			// Jetzt haben wir das Feld mit den Zeigern auf die Schiffe gefüllt
			// Nun müssen wir noch die Schiffe unserer Rasse mit eventueller Flotte hinzufügen
			if (!pShipArray.IsEmpty())
			{
				for (int i = 0; i < m_ShipArray.GetSize(); i++)
					if (m_ShipArray.GetAt(i).GetOwnerOfShip() == m_ShipArray.GetAt(y).GetOwnerOfShip() &&
						m_ShipArray.GetAt(i).GetKO() == m_ShipArray.GetAt(y).GetKO())
					{
						// Wenn unser Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
						if (m_ShipArray.GetAt(i).GetFleet() != 0)
							for (int j = 0; j < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); j++)
								pShipArray.Add(m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j));
						pShipArray.Add(&m_ShipArray.ElementAt(i));
					}
				// Jetzt können wir einen Kampf stattfinden lassen
				combat.SetInvolvedShips(&pShipArray);
				combat.PreCombatCalculation();
				break;
			}
		}
		if (r > 0)
		{
			BYTE winner[7];
			memset(winner, 2, sizeof(BYTE) * 7);
			do
			{
				combat.CalculateCombat(winner);
			} while (combat.m_bReady);
			// Nach einem Kampf kann geschaut werden, wer noch Schiffe besitzt. Diese Rassen haben den Kampf danach gewonnen
			// Erstmal wird für alle beteiligten Rassen der Kampf auf verloren gesetzt. Danach wird geschaut, wer noch
			// Schiffe besitzt. Für diese Rassen wird der Kampf dann auf gewonnen gesetzt. Alle anderen Rassen gelten als
			// nicht kampfbeteiligt.
			/*for (int i = HUMAN; i <= DOMINION; i++)
				if (combat.m_bInvolvedRaces[i])
					winner[i] = FALSE;
			for (int i = 0; i < combat.m_CS.GetSize(); i++)
				winner[combat.m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = TRUE;
			for (int i = 0; i <= DOMINION; i++)
				if (winner[i] == 1)
					wins[i]++;
			*/
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			for (int i = HUMAN; i <= DOMINION; i++)
			{
				// gewonnen
				if (winner[i] == 1)
				{
					wins[i]++;
				}
				// verloren
				else if (winner[i] == 0)
				{
				}
				// unentschieden
				else if (winner[i] == 3)
				{
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			combat.Reset();
			m_ShipArray.RemoveAll();
			m_ShipInfoArray.RemoveAll();
			this->ReadShipInfosFromFile();
			this->ApplyShipsAtStartup();
			r--;
		}
	} while (r > 0);
	return TRUE;
}

// CCombatSimulatorDoc-Serialisierung

void CCombatSimulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else
	{
		// TODO: Hier Code zum Laden einfügen
	}
}


// CCombatSimulatorDoc-Diagnose

#ifdef _DEBUG
void CCombatSimulatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCombatSimulatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCombatSimulatorDoc-Befehle
void CCombatSimulatorDoc::ApplyShipsAtStartup()
{
	int what = 0;	// 0 -> Speed, 1 -> Race, 2 -> Ships -> 3 Repeat
	race = NOBODY;
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	//CString fileName="C:\\Botf2\\Data\\Ships\\CombatScript.txt";		// Name des zu Öffnenden Files 
	CString fileName="CombatScript.txt";								// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput == "§SLOWING§")
				what = 0;
			else if (csInput == "§RACE§")
				what = 1;
			else if (csInput == "§SHIPS§")
				what = 2;
			else if (csInput == "§REPEAT§")
				what = 3;
			else
			{
				if (what == 0)
					this->SlowingSpeed = atoi(csInput);
				else if (what == 1)
				{
					race = atoi(csInput);
					if (race == 0 || race > 6)
					{
						AfxMessageBox("Scripting_Error: wrong race data!");
						exit(1);
					}
				}
				else if (what == 3)
					this->repeat = atoi(csInput);
				else
				{
					// ID der Schiffsklasse im Schiffsinfofeld suchen
					BOOLEAN found = FALSE;
					for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
						if (csInput == m_ShipInfoArray.GetAt(i).GetShipClass())
						{
							BuildShip(i);
							found = TRUE;
							break;
						}
					if (!found)
					{
						CString s;
						s.Format("can't find shipclass ""%s",csInput);
						AfxMessageBox(s);
					}
				}
			}
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"CombatScript.txt\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

// Funktion ließt aus der Datei die Informationen zu allen Schiffen ein und speichert diese im dynamischen Feld
// später können wir ingame diese Informationen ändern und uns unsere eigenen Schiffchen bauen
void CCombatSimulatorDoc::ReadShipInfosFromFile()
{
	CShipInfo ShipInfo;
	int i = 0;
	USHORT j = 0;
	int weapons = 0;	// Zähler um Waffen hinzuzufügen
	BOOL torpedo = FALSE;
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString data[40];
	CString torpedoData[9];
	CString beamData[12];
	
	CString fileName="Shiplist.data";				// Name des zu Öffnenden Files 
	CStdioFile file;														// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput != "$END_OF_SHIPDATA$")
			{
				if (csInput == "$Torpedo$")
				{
					weapons = 9;	// 9, weil wir 9 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 12;	// 12, weil wir 12 Informationen für einen Beam brauchen
					torpedo = FALSE;
				}
				else if (torpedo == TRUE && weapons > 0)
				{
					torpedoData[9-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Torpedodaten hinzufügen
						CTorpedoWeapons torpedoWeapon;
						torpedoWeapon.ModifyTorpedoWeapon(atoi(torpedoData[0]),atoi(torpedoData[1]),
							atoi(torpedoData[2]),atoi(torpedoData[3]),torpedoData[4],atoi(torpedoData[5]),atoi(torpedoData[6]));
						// folgende Zeile neu in Alpha5
						torpedoWeapon.GetFirearc()->SetValues(atoi(torpedoData[7]), atoi(torpedoData[8]));
						
						ShipInfo.GetTorpedoWeapons()->Add(torpedoWeapon);
					}
				}
				else if (torpedo == FALSE && weapons > 0)
				{
					beamData[12-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Beamdaten hinzufügen
						CBeamWeapons beamWeapon;
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),beamData[3],atoi(beamData[4])
							,atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
						// folgende Zeile neu in Alpha5
						beamWeapon.GetFirearc()->SetValues(atoi(beamData[10]), atoi(beamData[11]));

						ShipInfo.GetBeamWeapons()->Add(beamWeapon);
					}
				}
				else
				{
					data[i] = csInput;
					i++;
				}
			}
			else
			{
				// sonstige Informationen an das Objekt übergeben
				ShipInfo.SetRace(atoi(data[0]));
				ShipInfo.SetID(j);
				ShipInfo.SetShipClass(data[1]);
				ShipInfo.SetShipDescription(data[2]);
				ShipInfo.SetShipType(atoi(data[3]));
				ShipInfo.SetShipSize(atoi(data[4]));
				ShipInfo.SetManeuverability(atoi(data[5]));
				ShipInfo.SetBioTech(atoi(data[6]));
				ShipInfo.SetEnergyTech(atoi(data[7]));
				ShipInfo.SetComputerTech(atoi(data[8]));
				ShipInfo.SetPropulsionTech(atoi(data[9]));
				ShipInfo.SetConstructionTech(atoi(data[10]));
				ShipInfo.SetWeaponTech(atoi(data[11]));
				ShipInfo.SetNeededIndustry(atoi(data[12]));
				ShipInfo.SetNeededTitan(atoi(data[13]));
				ShipInfo.SetNeededDeuterium(atoi(data[14]));
				ShipInfo.SetNeededDuranium(atoi(data[15]));
				ShipInfo.SetNeededCrystal(atoi(data[16]));
				ShipInfo.SetNeededIridium(atoi(data[17]));
				ShipInfo.SetNeededDilithium(atoi(data[18]));
				ShipInfo.SetOnlyInSystem(data[19]);
				ShipInfo.GetHull()->ModifyHull(atoi(data[22]),atoi(data[20]),atoi(data[21]),atoi(data[23]),atoi(data[24]));
				ShipInfo.GetShield()->ModifyShield(atoi(data[25]),atoi(data[26]),atoi(data[27]));
				ShipInfo.SetSpeed(atoi(data[28]));
				ShipInfo.SetRange(atoi(data[29]));
				ShipInfo.SetScanPower(atoi(data[30]));
				ShipInfo.SetScanRange(atoi(data[31]));
				ShipInfo.SetStealthPower(atoi(data[32]));
				ShipInfo.SetStorageRoom(atoi(data[33]));
				ShipInfo.SetColonizePoints(atoi(data[34]));
				ShipInfo.SetStationBuildPoints(atoi(data[35]));
				ShipInfo.SetMaintenanceCosts(atoi(data[36]));
				ShipInfo.SetSpecial(0, atoi(data[37]));
				ShipInfo.SetSpecial(1, atoi(data[38]));
				ShipInfo.SetObsoleteShipClass(data[39]);
				ShipInfo.CalculateFinalCosts();
				ShipInfo.SetStartOrder();
				m_ShipInfoArray.Add(ShipInfo);
				ShipInfo.DeleteWeapons();
				i = 0;
				j++;
			}
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Shiplist.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

void CCombatSimulatorDoc::BuildShip(int ID)
{
	m_ShipArray.Add((CShip)m_ShipInfoArray.GetAt(ID));
	m_ShipArray.ElementAt(m_ShipArray.GetUpperBound()).SetOwnerOfShip(race);
	if (m_ShipInfoArray.GetAt(ID).GetStealthPower() > 3)
		m_ShipArray.ElementAt(m_ShipArray.GetUpperBound()).SetCloak();
}
