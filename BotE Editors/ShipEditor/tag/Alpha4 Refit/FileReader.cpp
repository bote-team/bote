// FileReader.cpp: Implementierung der Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "FileReader.h"
//#include "array_sort.h"
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Sortierfunktion für Schiffsklassen
bool cmp(const CShipInfo* a, const CShipInfo* b)
{
	return a->GetShipClass() < b->GetShipClass();
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFileReader::CFileReader()
{
}

CFileReader::~CFileReader()
{
}

void CFileReader::ReadDataFromFile(CArray<CShipInfo*>* shipInfos)
{
	if (shipInfos != NULL)
	{
		for (int i = 0; i < shipInfos->GetSize(); i++)
		{
			delete shipInfos->ElementAt(i);
			shipInfos->ElementAt(i) = NULL;
		}
		shipInfos->RemoveAll();
	}

	CShipInfo* ShipInfo = new CShipInfo;

	int i = 0;
	USHORT j = 0;
	int weapons = 0;	// Zähler um Waffen hinzuzufügen
	BOOL torpedo = FALSE;
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString data[40];
	CString torpedoData[7];
	CString beamData[10];
	CString fileName="Shiplist.data";				// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput != "$END_OF_SHIPDATA$")
			{
				if (csInput == "$Torpedo$")
				{
					weapons = 7;	// weil wir 7 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 10;	// weil wir 10 Informationen für einen Beam brauchen
					torpedo = FALSE;
				}
				else if (torpedo == TRUE && weapons > 0)
				{
					torpedoData[7-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Torpedodaten hinzufügen
						CTorpedoWeapons torpedoWeapon;
						torpedoWeapon.ModifyTorpedoWeapon(atoi(torpedoData[0]),atoi(torpedoData[1]),
							atoi(torpedoData[2]),atoi(torpedoData[3]),torpedoData[4],atoi(torpedoData[5]),atoi(torpedoData[6]));
						ShipInfo->GetTorpedoWeapons()->Add(torpedoWeapon);
					}
				}
				else if (torpedo == FALSE && weapons > 0)
				{
					beamData[10-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Beamdaten hinzufügen
						CBeamWeapons beamWeapon;
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),beamData[3],
							atoi(beamData[4]),atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
						ShipInfo->GetBeamWeapons()->Add(beamWeapon);
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
				ShipInfo->SetRace(atoi(data[0]));
				ShipInfo->SetID(j);
				ShipInfo->SetShipClass(data[1]);
				ShipInfo->SetShipDescription(data[2]);
				ShipInfo->SetShipType(atoi(data[3]));
				
				ShipInfo->SetShipSize(atoi(data[4]));
				ShipInfo->SetManeuverability(atoi(data[5]));
				
				ShipInfo->SetBioTech(atoi(data[6]));
				ShipInfo->SetEnergyTech(atoi(data[7]));
				ShipInfo->SetComputerTech(atoi(data[8]));
				ShipInfo->SetPropulsionTech(atoi(data[9]));
				ShipInfo->SetConstructionTech(atoi(data[10]));
				ShipInfo->SetWeaponTech(atoi(data[11]));
				ShipInfo->SetNeededIndustry(atoi(data[12]));
				ShipInfo->SetNeededTitan(atoi(data[13]));
				ShipInfo->SetNeededDeuterium(atoi(data[14]));
				ShipInfo->SetNeededDuranium(atoi(data[15]));
				ShipInfo->SetNeededCrystal(atoi(data[16]));
				ShipInfo->SetNeededIridium(atoi(data[17]));
				ShipInfo->SetNeededDilithium(atoi(data[18]));
				
				ShipInfo->SetOnlyInSystem(data[19]);

				ShipInfo->GetHull()->ModifyHull(atoi(data[22]),atoi(data[20]),atoi(data[21]),atoi(data[23]),atoi(data[24]));
				ShipInfo->GetShield()->ModifyShield(atoi(data[25]),atoi(data[26]),atoi(data[27]));
				ShipInfo->SetSpeed(atoi(data[28]));
				ShipInfo->SetRange(atoi(data[29]));
				ShipInfo->SetScanPower(atoi(data[30]));
				ShipInfo->SetScanRange(atoi(data[31]));
				ShipInfo->SetStealthPower(atoi(data[32]));
				ShipInfo->SetStorageRoom(atoi(data[33]));
				ShipInfo->SetColonizePoints(atoi(data[34]));
				ShipInfo->SetStationBuildPoints(atoi(data[35]));
				ShipInfo->SetMaintenanceCosts(atoi(data[36]));
				
				ShipInfo->SetSpecial(0, atoi(data[37]));
				ShipInfo->SetSpecial(1, atoi(data[38]));

				ShipInfo->SetObsoleteShipClass(data[39]);
							
				shipInfos->Add(ShipInfo);
				ShipInfo = new CShipInfo();
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

	if (ShipInfo)
	{
		delete ShipInfo;
		ShipInfo = NULL;
	}
}

void CFileReader::WriteDataToFile(CArray<CShipInfo*>* shipInfos)
{
	this->CheckBeforeWrite(shipInfos);

	int i = 0;
	USHORT j = 0;
	int weapons = 0;	// Zähler um Waffen hinzuzufügen
	BOOL torpedo = FALSE;
	CString s;										// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName="Shiplist.data";		// Name des zu Öffnenden Files 
	CStdioFile file;												// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		for (i = 0; i < shipInfos->GetSize(); i++)
		{
			s.Format("%d\n",shipInfos->GetAt(i)->GetRace());
			//USHORT race = shipInfos->GetAt(i)->GetRace();
			//file.Write(&race, sizeof(race));
			file.WriteString(s);
			s.Format("%s\n",shipInfos->GetAt(i)->GetShipClass());
			file.WriteString(s);
			s.Format("%s\n",shipInfos->GetAt(i)->GetShipDescription());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetShipType());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetShipSize());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetManeuverability());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetBioTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetEnergyTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetComputerTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetPropulsionTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetConstructionTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetWeaponTech());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededIndustry());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededTitan());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededDeuterium());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededDuranium());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededCrystal());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededIridium());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetNeededDilithium());
			file.WriteString(s);
			s.Format("%s\n",shipInfos->GetAt(i)->GetOnlyInSystem());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetSpeed());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetRange());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetScanPower());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetScanRange());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetStealthPower());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetStorageRoom());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetColonizePoints());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetStationBuildPoints());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetSpecial(0));
			file.WriteString(s);
			s.Format("%d\n",shipInfos->GetAt(i)->GetSpecial(1));
			file.WriteString(s);
			s.Format("%s\n",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file.WriteString(s);
			for (int j = 0; j < shipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
			{
				s.Format("$Torpedo$\n");
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoType());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeFirerate());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes());
				file.WriteString(s);
				s.Format("%s\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeName());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetOnlyMicroPhoton());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetAccuracy());
				file.WriteString(s);
			}
			for (int j = 0; j < shipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
			{
				s.Format("$Beam$\n");
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamType());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamPower());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamNumber());
				file.WriteString(s);
				s.Format("%s\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamName());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetModulating());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetPiercing());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBonus());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamLenght());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetRechargeTime());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetShootNumber());
				file.WriteString(s);
			}
			// Beim letzten keine Freizeile machen
			if (i < shipInfos->GetUpperBound())
				s.Format("$END_OF_SHIPDATA$\n");
			else
				s.Format("$END_OF_SHIPDATA$");
			file.WriteString(s);
			
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Shiplist.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

void CFileReader::CheckBeforeWrite(CArray<CShipInfo*>* shipInfos)
{
	// Sortierung
	std::vector<CShipInfo*> infos[UNKNOWN + 1];
	for (int x = 0; x < shipInfos->GetSize(); x++)
	{
		BYTE nRace = shipInfos->GetAt(x)->GetRace();
		infos[nRace].push_back(shipInfos->GetAt(x));
	}
	shipInfos->RemoveAll();
	for (int x = HUMAN; x <= UNKNOWN; x++)
	{
		// einzelne Felder sortieren
		std::sort(infos[x].begin(), infos[x].end(), cmp);
		// nun in sortierter Reihenfolge wieder einfügen
		for (size_t y = 0; y < infos[x].size(); y++)
			shipInfos->Add(infos[x][y]);
	}

	// Sicherheitshalber jedes Waffe auf jedem Schiff mit jeder Waffe auf jedem anderen Schiff checken und
	// sicherstellen, das gleiche Werte auch überall gleich sind
	
	// nun jedes Schiff, welches diese Waffe drauf hat anpassen, da sich Werte geändert haben könnten
	for (int i = 0; i < shipInfos->GetSize(); i++)
	{
		// Torpedowaffen checken
		for (int j = 0; j < shipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
		{
			CTorpedoWeapons* weapon1 = &(shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j));
			for (int m = 0; m < shipInfos->GetSize(); m++)
			{
				if (m != i)
				{
					for (int n = 0; n < shipInfos->GetAt(m)->GetTorpedoWeapons()->GetSize(); n++)
					{
						CTorpedoWeapons* weapon2 = &(shipInfos->GetAt(m)->GetTorpedoWeapons()->GetAt(n));
						if (weapon1->GetTupeName() == weapon2->GetTupeName())
						{
							weapon2->ModifyTorpedoWeapon(
								weapon2->GetTorpedoType(),		// bleibt gleich
								weapon1->GetNumber(),			// anpassen
								weapon1->GetTupeFirerate(),		// anpassen
								weapon2->GetNumberOfTupes(),	// bleibt gleich
								weapon2->GetTupeName(),			// bleibt gleich
								weapon1->GetOnlyMicroPhoton(),	// anpassen
								weapon2->GetAccuracy());		// bleibt gleich
						}
					}
				}
			}
		}

		// Beamwaffen checken
		for (int j = 0; j < shipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
		{
			CBeamWeapons* weapon1 = &(shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j));
			for (int m = 0; m < shipInfos->GetSize(); m++)
			{
				if (m != i)
				{
					for (int n = 0; n < shipInfos->GetAt(m)->GetBeamWeapons()->GetSize(); n++)
					{
						CBeamWeapons* weapon2 = &(shipInfos->GetAt(m)->GetBeamWeapons()->GetAt(n));
						if (weapon1->GetBeamName() == weapon2->GetBeamName())
						{
							weapon2->ModifyBeamWeapon(
								weapon2->GetBeamType(),		// bleibt gleich
								weapon1->GetBeamPower(),	// anpassen
								weapon2->GetBeamNumber(),	// bleibt gleich
								weapon2->GetBeamName(),		// bleibt gleich
								weapon1->GetModulating(),	// anpassen
								weapon1->GetPiercing(),		// anpassen
								weapon1->GetBonus(),		// anpassen
								weapon1->GetBeamLenght(),	// anpassen
								weapon1->GetRechargeTime(),	// anpassen
								weapon1->GetShootNumber());	// anpassen
						}
					}
				}
			}
		}
	}
}