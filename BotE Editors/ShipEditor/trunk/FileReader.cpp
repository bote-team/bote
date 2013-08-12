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
	// auf csInput wird die jeweilige Zeile gespeichert
	CString csInput;
	CString data[40];  //Alpha7  CString data[40]; 
	
	// ALPHA4 REFIT
	//CString torpedoData[7];
	//CString beamData[10];
	
	// ALPHA5
	CString torpedoData[9];
	CString beamData[12];
	
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
					weapons = 9;	// weil wir 9 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 12;	// weil wir 12 Informationen für einen Beam brauchen
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
						
						ShipInfo->GetTorpedoWeapons()->Add(torpedoWeapon);
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
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),beamData[3],
							atoi(beamData[4]),atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
						// folgende Zeile neu in Alpha5
						beamWeapon.GetFirearc()->SetValues(atoi(beamData[10]), atoi(beamData[11]));

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

//AfxMessageBox("Fehler0");

	// reading Shiplist-LanguageFile 

	CString fileName2="Shiplist-LanguageFile.data";				// Name des zu Öffnenden Files 
	CStdioFile file2;													// Varibale vom Typ CStdioFile
	//j = 0;
	if (file2.Open(fileName2, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		//AfxMessageBox("Fehler1");
		while (file2.ReadString(csInput))
		{
			//AfxMessageBox("Fehler2");
			if (csInput != "$END_OF_SHIPDATA$")
			{
				//AfxMessageBox("Fehler3");
				/*
				if (csInput == "$Torpedo$")
				{
					weapons = 9;	// weil wir 9 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 12;	// weil wir 12 Informationen für einen Beam brauchen
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
						
						ShipInfo->GetTorpedoWeapons()->Add(torpedoWeapon);
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
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),beamData[3],
							atoi(beamData[4]),atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
						// folgende Zeile neu in Alpha5
						beamWeapon.GetFirearc()->SetValues(atoi(beamData[10]), atoi(beamData[11]));

						ShipInfo->GetBeamWeapons()->Add(beamWeapon);
					}
					//
				}
				else
				{
					data[i] = csInput;
					i++;
				}
				
			}
			else
			{
				*/
				// sonstige Informationen an das Objekt übergeben
				/*
				ShipInfo->SetRace(atoi(data[0]));*/
				ShipInfo->SetID(j);
				ShipInfo->SetShipClass(data[0]);
				ShipInfo->SetShipDescription(data[1]);
				/*ShipInfo->SetShipType(atoi(data[3]));
				
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
				*/
				ShipInfo->SetOnlyInSystem(data[2]);
				/*
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
				*/

				ShipInfo->SetObsoleteShipClass(data[3]);
							
				//shipInfos->Add(ShipInfo);
				ShipInfo = new CShipInfo();

				//i = 0;
				j++;
			}
		}
	}
	else
	{	
		AfxMessageBox("Error! File \"Shiplist-LanguageFile.data\" could not be read...open already?");
	}
	file2.Close();							// Datei wird geschlossen


	if (ShipInfo)
	{
		delete ShipInfo;
		ShipInfo = NULL;
	}
}

void CFileReader::WriteDataToFile(CArray<CShipInfo*>* shipInfos)
{
	this->CheckBeforeWrite(shipInfos);

	// Part 1: safety copies

	// Part 2: write Shiplist.data

	// Part 3: write Shiplist-LanguageFile.data

	// Part 4: write Shiplist-Export.csv

	// Part 5: write Shiplist-Export-Beam.csv

	// Part 6: write Shiplist-Export-Torpedo.csv


	// Safety copies
	CopyFile(
				"Shiplist-Pre-Pre.data",
				"Shiplist-Pre-Pre-Pre.data",
				FALSE);

	CopyFile(
				"Shiplist-Pre.data",
				"Shiplist-Pre-Pre.data",
				FALSE);

	CopyFile(
				"Shiplist.data",
				"Shiplist-Pre.data",
				FALSE);

	// Safety copies of Shiplist-LanguageFile
	CopyFile(
				"Shiplist-LanguageFile-Pre-Pre.data",
				"Shiplist-LanguageFile-Pre-Pre-Pre.data",
				FALSE);
	CopyFile(
				"Shiplist-LanguageFile-Pre.data",
				"Shiplist-LanguageFile-Pre-Pre.data",
				FALSE);

	CopyFile(
				"Shiplist-LanguageFile.data",
				"Shiplist-LanguageFile-Pre.data",
				FALSE);

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
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle());
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
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetAngle());
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
		AfxMessageBox("Error! File \"Shiplist-Language.data\" could not be saved...open already?");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen

// write language file 
// with language file values stay and names are exchanged. At least complete shiplist.data with exchanged language can be saved.

	i = 0;
	j = 0;
	weapons = 0;	// Zähler um Waffen hinzuzufügen
	torpedo = FALSE;
								// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName2="Shiplist-LanguageFile.data";		// Name des zu Öffnenden Files 
	CStdioFile file2;												// Varibale vom Typ CStdioFile
	if (file2.Open(fileName2, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		for (i = 0; i < shipInfos->GetSize(); i++)
		{
			// number -> s.Format("%d\n",shipInfos->GetAt(i)->GetRace());   // new ones change also to file"2"
			//file.WriteString("s");
			s.Format("%s\n",shipInfos->GetAt(i)->GetShipClass());
			file2.WriteString(s);
			s.Format("%s\n",shipInfos->GetAt(i)->GetShipDescription());
			file2.WriteString(s);
			/*s.Format("%d\n",shipInfos->GetAt(i)->GetShipType());
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
			*/
			s.Format("%s\n",shipInfos->GetAt(i)->GetOnlyInSystem());
			file2.WriteString(s);
			/*
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
			*/
			s.Format("%s\n",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file2.WriteString(s);
			/*
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
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle());
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
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file.WriteString(s);
				s.Format("%d\n",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file.WriteString(s);
			}
				
			*/
			// Beim letzten keine Freizeile machen
			if (i < shipInfos->GetUpperBound())
				s.Format("$END_OF_SHIPDATA$\n");
			else
				s.Format("$END_OF_SHIPDATA$");
			file2.WriteString(s);
			
			
		}
	}
	else
	{	
		AfxMessageBox("Error! File \"Shiplist-Language.data\" could not be saved...open already?");
	}
	file2.Close();	// file2 = Shiplist-Language.data


	// File 3 = Shiplist-Export.csv
	i = 0;
	j = 0;
	weapons = 0;	// Zähler um Waffen hinzuzufügen
	torpedo = FALSE;
	//CString s;										// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName3="Shiplist-Export.csv";		// Name des zu Öffnenden Files 
	CStdioFile file3;												// Varibale vom Typ CStdioFile
	if (file3.Open(fileName3, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		file3.WriteString("Race;ShipClass;Shipdiscription;Typ;Size;Man;Bio;En;Comp;Prop;Constr;Weap;Ind;Tit;Deu;Dur;Cry;Iri;Der;buildonlyin;BHull;HMat;Dhul;Abla;RegSh;maxSh;SType;Comma;Spee;Rang;ScP;ScR;Cam;Stor;Col-P;Sta-P;Mcosts;Ass1;Ass2;b_obsolete;Weapon1;W2;W3;W4;W5;W6;W7;W8;W9;W10;W11;W12;W13;W14;W15;W16;W17;W18;W19;W20;W21;W22;W23;W24;W25;W26;W27;W28;W29;W30;W31;W32;W33;W34;W35;W36;W37;W38;W39;W40;W41;W42;W43;W44;W45;W46;W47;W48;W49;W50;W51;W52;W53;W54;W55;W56;W57;W58;W59;W60;W61;W62;W63;W64;W65;W66;W67;W68;W69;W70;W71;W72;W73;W74;W75;W76;W77;W78;W79;W80,W81;W82;W83;W84;W85;W86;W87;W88;W89\n");

		for (i = 0; i < shipInfos->GetSize(); i++)
		{
			s.Format("%d;",shipInfos->GetAt(i)->GetRace());
			file3.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipClass());
			file3.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipType());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipSize());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetManeuverability());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetBioTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetEnergyTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetComputerTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetPropulsionTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetConstructionTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetWeaponTech());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIndustry());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededTitan());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDeuterium());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDuranium());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededCrystal());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIridium());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDilithium());
			file3.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetOnlyInSystem());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpeed());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetRange());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanPower());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanRange());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStealthPower());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStorageRoom());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetColonizePoints());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStationBuildPoints());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(0));
			file3.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(1));
			file3.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file3.WriteString(s);
			for (int j = 0; j < shipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
			{
				s.Format("$Torpedo$;");
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoType());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeFirerate());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes());
				file3.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeName());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetOnlyMicroPhoton());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetAccuracy());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file3.WriteString(s);
			}
			for (int j = 0; j < shipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
			{
				s.Format("$Beam$;");
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamType());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamPower());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamNumber());
				file3.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamName());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetModulating());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetPiercing());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBonus());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamLenght());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetRechargeTime());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetShootNumber());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file3.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file3.WriteString(s);
			}
			// Beim letzten keine Freizeile machen
			if (i < shipInfos->GetUpperBound())
				s.Format("$END_OF_SHIPDATA$\n");
			else
				s.Format("$END_OF_SHIPDATA$\n");
			file3.WriteString(s);
			
		}
	}
	else
	{	
		AfxMessageBox("Error! File \"Shiplist-Export.csv\" could not be saved...open already?");
	}
	file3.Close();							// File3 = Shiplist-Export.csv


	// File 4 = Shiplist-Export-BEAM.csv
	i = 0;
	j = 0;
	weapons = 0;	// Zähler um Waffen hinzuzufügen
	torpedo = FALSE;
	//CString s;										// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName4="Shiplist-Export-Beam.csv";		// Name des zu Öffnenden Files 
	CStdioFile file4;												// Varibale vom Typ CStdioFile
	if (file4.Open(fileName4, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		file4.WriteString("Beam;Type;Dam;#B;Name;Modu;Pierc;BonSys;Length;Recharge;ShNr;Pos;Ang;Race;ShipClass;Typ;Size;Man;Bio;En;Comp;Prop;Constr;Weap;Ind;Tit;Deu;Dur;Cry;Iri;Der;buildonlyin;BHull;HMat;Dhul;Abla;RegSh;maxSh;SType;Comma;Spee;Rang;ScP;ScR;Cam;Stor;Col-P;Sta-P;Mcosts;Ass1;Ass2;b_obsolete;Shipdiscription\n");
		for (i = 0; i < shipInfos->GetSize(); i++)
		{
			/*
			s.Format("%d;",shipInfos->GetAt(i)->GetRace());
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipClass());
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipType());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipSize());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetManeuverability());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetBioTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetEnergyTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetComputerTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetPropulsionTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetConstructionTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetWeaponTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIndustry());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededTitan());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDeuterium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDuranium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededCrystal());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIridium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDilithium());
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetOnlyInSystem());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpeed());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetRange());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanPower());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanRange());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStealthPower());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStorageRoom());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetColonizePoints());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStationBuildPoints());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(0));
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(1));
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file4.WriteString(s);
			*/
			for (int j = 0; j < shipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
			{
				/*
				s.Format("$Torpedo$;");
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoType());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeFirerate());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes());
				file4.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeName());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetOnlyMicroPhoton());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetAccuracy());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file4.WriteString(s);
				*/
			}
			for (int j = 0; j < shipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
			{
				s.Format("$Beam$;");
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamType());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamPower());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamNumber());
				file4.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamName());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetModulating());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetPiercing());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBonus());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamLenght());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetRechargeTime());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetShootNumber());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file4.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file4.WriteString(s);

			s.Format("%d;",shipInfos->GetAt(i)->GetRace());
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipClass());
			file4.WriteString(s);
			//s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			//file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipType());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipSize());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetManeuverability());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetBioTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetEnergyTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetComputerTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetPropulsionTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetConstructionTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetWeaponTech());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIndustry());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededTitan());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDeuterium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDuranium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededCrystal());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIridium());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDilithium());
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetOnlyInSystem());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpeed());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetRange());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanPower());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanRange());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStealthPower());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStorageRoom());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetColonizePoints());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStationBuildPoints());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(0));
			file4.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(1));
			file4.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file4.WriteString(s);

			s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			file4.WriteString(s);

			s.Format("$END_OF_BEAM$\n");
			file4.WriteString(s);
			}
			//s.Format("$END_OF_SHIPDATA$\n");
			//file4.WriteString(s);
		}
	}
	else
	{	
		AfxMessageBox("Error! File \"Shiplist-Export-Beam.csv\" could not be saved...open already?");
	}
	file4.Close();							// File4 = Shiplist-Export-Beam.csv


	// File 5 = Shiplist-Export-Torpedo.csv
	i = 0;
	j = 0;
	weapons = 0;	// Zähler um Waffen hinzuzufügen
	torpedo = FALSE;
	//CString s;										// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName5="Shiplist-Export-Torpedo.csv";		// Name des zu Öffnenden Files 
	CStdioFile file5;												// Varibale vom Typ CStdioFile
	if (file5.Open(fileName5, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{

		file5.WriteString("Torpedo;ToType;#To/S;Rate;#Tu;Name;Micr;Acc;MouP;Ang;Race;ShipClass;Typ;Size;Man;Bio;En;Comp;Prop;Constr;Weap;Ind;Tit;Deu;Dur;Cry;Iri;Der;buildonlyin;BHull;HMat;Dhul;Abla;RegSh;maxSh;SType;Comma;Spee;Rang;ScP;ScR;Cam;Stor;Col-P;Sta-P;Mcosts;Ass1;Ass2;b_obsolete;Shipdiscription\n");
		for (i = 0; i < shipInfos->GetSize(); i++)
		{
			/*
			s.Format("%d;",shipInfos->GetAt(i)->GetRace());
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipClass());
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipType());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipSize());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetManeuverability());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetBioTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetEnergyTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetComputerTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetPropulsionTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetConstructionTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetWeaponTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIndustry());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededTitan());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDeuterium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDuranium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededCrystal());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIridium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDilithium());
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetOnlyInSystem());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpeed());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetRange());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanPower());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanRange());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStealthPower());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStorageRoom());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetColonizePoints());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStationBuildPoints());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(0));
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(1));
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file5.WriteString(s);
			*/
			for (int j = 0; j < shipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
			{
				
				s.Format("$Torpedo$;");
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoType());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeFirerate());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes());
				file5.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeName());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetOnlyMicroPhoton());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetAccuracy());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file5.WriteString(s);

			s.Format("%d;",shipInfos->GetAt(i)->GetRace());
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetShipClass());
			file5.WriteString(s);
			//s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			//file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipType());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShipSize());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetManeuverability());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetBioTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetEnergyTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetComputerTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetPropulsionTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetConstructionTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetWeaponTech());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIndustry());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededTitan());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDeuterium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDuranium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededCrystal());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededIridium());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetNeededDilithium());
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetOnlyInSystem());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetBaseHull());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetHullMaterial());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetDoubleHull());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetAblative());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetHull()->GetPolarisation());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetMaxShield());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetShieldType());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetShield()->GetRegenerative());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpeed());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetRange());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanPower());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetScanRange());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStealthPower());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStorageRoom());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetColonizePoints());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetStationBuildPoints());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetMaintenanceCosts());
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(0));
			file5.WriteString(s);
			s.Format("%d;",shipInfos->GetAt(i)->GetSpecial(1));
			file5.WriteString(s);
			s.Format("%s;",shipInfos->GetAt(i)->GetObsoleteShipClass());
			file5.WriteString(s);
			
			s.Format("%s;",shipInfos->GetAt(i)->GetShipDescription());
			file5.WriteString(s);
				
				
			s.Format("$END_OF_TORPEDO$\n");
			file5.WriteString(s);
			}
			for (int j = 0; j < shipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
			{
				/*
				s.Format("$Beam$;");
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamType());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamPower());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamNumber());
				file5.WriteString(s);
				s.Format("%s;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamName());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetModulating());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetPiercing());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBonus());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamLenght());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetRechargeTime());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetShootNumber());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetPosition());
				file5.WriteString(s);
				s.Format("%d;",shipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetFirearc()->GetAngle());
				file5.WriteString(s);
				*/
			}
			//s.Format("$END_OF_SHIPDATA$\n");
			//file5.WriteString(s);
		}
	}
	else
	{	
		AfxMessageBox("Error! File \"Shiplist-Export-Torpedo.csv\" could not be saved...open already?");
	}
	file5.Close();							// File5 = Shiplist-Export-Torpedo.csv
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