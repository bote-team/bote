// FileReader.cpp: Implementierung der Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "FileReader.h"
#include "array_sort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFileReader::CFileReader()
{
}

CFileReader::~CFileReader()
{
}

void CFileReader::ReadDataFromFile(CArray<CBuildingInfo,CBuildingInfo>* m_BuildingInfos)
{
	if (m_BuildingInfos != NULL)
		m_BuildingInfos->RemoveAll();
	CBuildingInfo info;
	USHORT i = 0;
	CString csInput;
	
	CString data[140];
	CString fileName = "Buildings.data";
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))		// Datei wird ge�ffnet
	{
		// ALPHA4 -> 133
		// ALPHA5 -> 140
		while (file.ReadString(csInput))
		{
			// Daten lesen
			data[i++] = csInput;
			if (i == 140)
			{
				i = 0;
				info.SetRunningNumber(atoi(data[0]));
				info.SetOwnerOfBuilding(atoi(data[1]));
				info.SetBuildingName(data[2],0);
				info.SetBuildingName(data[3],1);
				info.SetBuildingDescription(data[4],0);
				info.SetBuildingDescription(data[5],1);
				info.SetUpgradeable(atoi(data[6]));
				info.SetGraphikFileName(data[7]);
				info.SetMaxInSystem(atoi(data[8]),atoi(data[9]));
				info.SetMaxInEmpire(atoi(data[10]),atoi(data[11]));
				info.SetOnlyHomePlanet(atoi(data[12]));
				info.SetOnlyOwnColony(atoi(data[13]));
				info.SetOnlyMinorRace(atoi(data[14]));
				info.SetOnlyTakenSystem(atoi(data[15]));
				info.SetOnlyInSystemWithName(data[16]);
				info.SetMinHabitants(atoi(data[17]));
				info.SetMinInSystem(atoi(data[18]),atoi(data[19]));
				info.SetMinInEmpire(atoi(data[20]),atoi(data[21]));
				info.SetOnlyRace(atoi(data[22]));
				info.SetPlanetTypes(A,atoi(data[23]));
				info.SetPlanetTypes(B,atoi(data[24]));
				info.SetPlanetTypes(C,atoi(data[25]));
				info.SetPlanetTypes(E,atoi(data[26]));
				info.SetPlanetTypes(F,atoi(data[27]));
				info.SetPlanetTypes(G,atoi(data[28]));
				info.SetPlanetTypes(H,atoi(data[29]));
				info.SetPlanetTypes(I,atoi(data[30]));
				info.SetPlanetTypes(J,atoi(data[31]));
				info.SetPlanetTypes(K,atoi(data[32]));
				info.SetPlanetTypes(L,atoi(data[33]));
				info.SetPlanetTypes(M,atoi(data[34]));
				info.SetPlanetTypes(N,atoi(data[35]));
				info.SetPlanetTypes(O,atoi(data[36]));
				info.SetPlanetTypes(P,atoi(data[37]));
				info.SetPlanetTypes(Q,atoi(data[38]));
				info.SetPlanetTypes(R,atoi(data[39]));
				info.SetPlanetTypes(S,atoi(data[40]));
				info.SetPlanetTypes(T,atoi(data[41]));
				info.SetPlanetTypes(Y,atoi(data[42]));
				info.SetBioTech(atoi(data[43]));
				info.SetEnergyTech(atoi(data[44]));
				info.SetCompTech(atoi(data[45]));
				info.SetPropulsionTech(atoi(data[46]));
				info.SetConstructionTech(atoi(data[47]));
				info.SetWeaponTech(atoi(data[48]));
				info.SetNeededIndustry(atoi(data[49]));
				info.SetNeededEnergy(atoi(data[50]));
				info.SetNeededTitan(atoi(data[51]));
				info.SetNeededDeuterium(atoi(data[52]));
				info.SetNeededDuranium(atoi(data[53]));
				info.SetNeededCrystal(atoi(data[54]));
				info.SetNeededIridium(atoi(data[55]));
				info.SetNeededDilithium(atoi(data[56]));
				info.SetFoodProd(atoi(data[57]));
				info.SetIPProd(atoi(data[58]));
				info.SetEnergyProd(atoi(data[59]));
				info.SetSPProd(atoi(data[60]));
				info.SetFPProd(atoi(data[61]));
				info.SetTitanProd(atoi(data[62]));
				info.SetDeuteriumProd(atoi(data[63]));
				info.SetDuraniumProd(atoi(data[64]));
				info.SetCrystalProd(atoi(data[65]));
				info.SetIridiumProd(atoi(data[66]));
				info.SetDilithiumProd(atoi(data[67]));
				info.SetLatinumProd(atoi(data[68]));
				info.SetMoralProd(atoi(data[69]));
				info.SetMoralProdEmpire(atoi(data[70]));
				info.SetFoodBoni(atoi(data[71]));
				info.SetIndustryBoni(atoi(data[72]));
				info.SetEnergyBoni(atoi(data[73]));
				info.SetSecurityBoni(atoi(data[74]));
				info.SetResearchBoni(atoi(data[75]));
				info.SetTitanBoni(atoi(data[76]));
				info.SetDeuteriumBoni(atoi(data[77]));
				info.SetDuraniumBoni(atoi(data[78]));
				info.SetCrystalBoni(atoi(data[79]));
				info.SetIridiumBoni(atoi(data[80]));
				info.SetDilithiumBoni(atoi(data[81]));
				info.SetAllRessourceBoni(atoi(data[82]));
				info.SetLatinumBoni(atoi(data[83]));
				info.SetBioTechBoni(atoi(data[84]));
				info.SetEnergyTechBoni(atoi(data[85]));
				info.SetCompTechBoni(atoi(data[86]));
				info.SetPropulsionTechBoni(atoi(data[87]));
				info.SetConstructionTechBoni(atoi(data[88]));
				info.SetWeaponTechBoni(atoi(data[89]));
				info.SetInnerSecurityBoni(atoi(data[90]));
				info.SetEconomySpyBoni(atoi(data[91]));
				info.SetEconomySabotageBoni(atoi(data[92]));
				info.SetResearchSpyBoni(atoi(data[93]));
				info.SetResearchSabotageBoni(atoi(data[94]));
				info.SetMilitarySpyBoni(atoi(data[95]));
				info.SetMilitarySabotageBoni(atoi(data[96]));
				info.SetShipYard(atoi(data[97]));
				info.SetBuildableShipTypes(atoi(data[98]));
				info.SetShipYardSpeed(atoi(data[99]));
				info.SetBarrack(atoi(data[100]));
				info.SetBarrackSpeed(atoi(data[101]));
				info.SetHitpoints(atoi(data[102]));
				info.SetShieldPower(atoi(data[103]));
				info.SetShieldPowerBoni(atoi(data[104]));
				info.SetShipDefend(atoi(data[105]));
				info.SetShipDefendBoni(atoi(data[106]));
				info.SetGroundDefend(atoi(data[107]));
				info.SetGroundDefendBoni(atoi(data[108]));
				info.SetScanPower(atoi(data[109]));
				info.SetScanPowerBoni(atoi(data[110]));
				info.SetScanRange(atoi(data[111]));
				info.SetScanRangeBoni(atoi(data[112]));
				info.SetShipTraining(atoi(data[113]));
				info.SetTroopTraining(atoi(data[114]));
				info.SetResistance(atoi(data[115]));
				info.SetAddedTradeRoutes(atoi(data[116]));
				info.SetIncomeOnTradeRoutes(atoi(data[117]));
				info.SetShipRecycling(atoi(data[118]));
				info.SetBuildingBuildSpeed(atoi(data[119]));
				info.SetUpdateBuildSpeed(atoi(data[120]));
				info.SetShipBuildSpeed(atoi(data[121]));
				info.SetTroopBuildSpeed(atoi(data[122]));
				info.SetPredecessor(atoi(data[123]));
				info.SetIsBuildingOnline(atoi(data[124]));
				info.SetWorker(atoi(data[125]));
				info.SetNeverReady(atoi(data[126]));
				info.SetEquivalent(NOBODY,0);		// niemand-index immer auf NULL setzen
				for (int p = HUMAN; p <= DOMINION; p++)
					info.SetEquivalent(p,atoi(data[126+p]));
				// neu in Alpha5
				for (int res = TITAN; res <= DILITHIUM; res++)
					info.SetResourceDistributor(res, atoi(data[133+res]));
				info.SetNeededSystems(atoi(data[139]));
				
				// Information in das Geb�udeinfofeld schreiben
				m_BuildingInfos->Add(info);			
				
			}
		}

	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Buildings.data\" kann nicht ge�ffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

void CFileReader::WriteDataToFile(CArray<CBuildingInfo,CBuildingInfo>* buildingInfos, BOOLEAN language)
{
// Safety copies
		CopyFile(
					"Buildings-Pre-Pre.data",
					"Buildings-Pre-Pre-Pre.data",
					FALSE);
		CopyFile(
					"Buildings-Pre.data",
					"Buildings-Pre-Pre.data",
					FALSE);

		CopyFile(
					"Buildings.data",
					"Buildings-Pre.data",
					FALSE);


	int i = 0;
	CString s;
	CBuildingInfo buildingInfo;
	CString fileName="Buildings.data";		// Name des zu �ffnenden Files 
	CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird ge�ffnet
	{
		for (int i = 0; i < buildingInfos->GetSize(); i++)
		{
			if (buildingInfos->GetAt(i).GetPredecessorID() == buildingInfos->GetAt(i).GetRunningNumber())
			{
				CString s;
				s.Format("Error ... the predecessor ID is the same like the ID of the building!\n\nAll your changes for the building: %s (ID: %d) were canceled", buildingInfos->GetAt(i).GetBuildingName(language), buildingInfos->GetAt(i).GetRunningNumber());
				AfxMessageBox(s);				
			}

			s.Format("%d\n",buildingInfos->GetAt(i).GetRunningNumber());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOwnerOfBuilding());
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetBuildingName(language));
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetBuildingName(!language));
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetBuildingDescription(language));
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetBuildingDescription(!language));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetUpgradeable());
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetGraphikFileName());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMaxInSystem().Number);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMaxInSystem().RunningNumber);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMaxInEmpire().Number);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMaxInEmpire().RunningNumber);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOnlyHomePlanet());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOnlyOwnColony());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOnlyMinorRace());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOnlyTakenSystem());
			file.WriteString(s);
			s.Format("%s\n",buildingInfos->GetAt(i).GetOnlyInSystemWithName());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMinHabitants());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMinInSystem().Number);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMinInSystem().RunningNumber);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMinInEmpire().Number);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMinInEmpire().RunningNumber);
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetOnlyRace());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(A));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(B));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(C));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(E));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(F));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(G));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(H));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(I));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(J));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(K));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(L));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(M));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(N));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(O));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(P));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(Q));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(R));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(S));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(T));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPlanetTypes(Y));
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetBioTech());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEnergyTech());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetCompTech());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPropulsionTech());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetConstructionTech());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetWeaponTech());	
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededIndustry());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededEnergy());	
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededTitan());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededDeuterium());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededDuranium());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededCrystal());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededIridium());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeededDilithium());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetFoodProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIPProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEnergyProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetSPProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetFPProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetTitanProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDeuteriumProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDuraniumProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetCrystalProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIridiumProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDilithiumProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetLatinum());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMoralProd());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMoralProdEmpire());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetFoodBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIndustryBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEnergyBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetSecurityBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetResearchBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetTitanBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDeuteriumBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDuraniumBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetCrystalBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIridiumBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetDilithiumBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetAllRessourcesBoni());	
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetLatinumBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetBioTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEnergyTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetCompTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPropulsionTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetConstructionTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetWeaponTechBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetInnerSecurityBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEconomySpyBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetEconomySabotageBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetResearchSpyBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetResearchSabotageBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMilitarySpyBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMilitarySabotageBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipYard());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetMaxBuildableShipSize());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipYardSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetBarrack());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetBarrackSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetHitPoints());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShieldPower());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShieldPowerBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipDefend());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipDefendBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetGroundDefend());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetGroundDefendBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetScanPower());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetScanPowerBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetScanRange());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetScanRangeBoni());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipTraining());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetTroopTraining());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetResistance());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetAddedTradeRoutes());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIncomeOnTradeRoutes());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipRecycling());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetBuildingBuildSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetUpdateBuildSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetShipBuildSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetTroopBuildSpeed());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetPredecessorID());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetIsBuildingOnline());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetWorker());
			file.WriteString(s);
			s.Format("%d\n",buildingInfos->GetAt(i).GetNeverReady());
			file.WriteString(s);
			for (int p = HUMAN; p <= DOMINION; p++)
			{
				s.Format("%d\n",buildingInfos->GetAt(i).GetEquivalent(p));
				file.WriteString(s);
			}
			// neu in Alpha5	
			for (int res = TITAN; res <= DILITHIUM; res++)
			{
				s.Format("%d\n", buildingInfos->GetAt(i).GetResourceDistributor(res));
				file.WriteString(s);
			}
			s.Format("%d\n", buildingInfos->GetAt(i).GetNeededSystems());
			file.WriteString(s);

		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Buildings.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen


	// Export into csv.file
	i = 0;
	//CString s;
	//CBuildingInfo buildingInfo;
	CString fileNameExport="BuildingsExport.csv";		// Name des zu �ffnenden Files 
	//CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileNameExport, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird ge�ffnet
	{
			//headline
			s.Format("Nr;Race;Name;Name2;Desc;Desc2;Upg;bop;maxS;maxSysID;maxE;maxEmpID;oHP;ooC;oMR;oTS;o_inS;minHab;minS;minSysID;minE;minEmpID;oR;A;B;C;E;F;G;H;I;J;K;L;M;N;O;P;Q;R;S;T;Y;Bio;En;Comp;Prop;Constr;Weap;nInd;nEn;nTit;nDeu;nDur;nCry;nIri;nDil;FProd;IndP;EnP;IntP;ResP;TitP;DeuP;DurP;CryP;IriP;DerP;CreP;MorP;M_Emp;FBon;IndB;EnB;IntB;ResB;TitB;DeuB;DurB;CryB;IriB;DerB;AllResB;CreB;BioTB;EnTB;CoTB;PrTB;CoTB;WTB;InSecB;EcoSpyB;EcoSabB;RSpyB;RSabB;MilSpyB;MilSabB;SY;maxBSS;SY_Sp;Bar;BarSp;Hit;Sh;ShB;ShDef;ShDB;GrDef;GDB;Sc;ScB;ScR;ScRB;ShT;TrT;Resist;TrR;IncTR;ShRecy;BBSp;UpBSp;ShBSp;TrBSp;PreID;Onl;Worker;n_r;Eq1;Eq2;Eq3;Eq4;Eq5;Eq6;TitD;DeuD;DurD;CryD;IriD;DerD;nSys\n");
			file.WriteString(s);

		for (int i = 0; i < buildingInfos->GetSize(); i++)
		{
			if (buildingInfos->GetAt(i).GetPredecessorID() == buildingInfos->GetAt(i).GetRunningNumber())
			{
				CString s;
				s.Format("Error ... the predecessor ID is the same like the ID of the building!\n\nAll your changes for the building: %s (ID: %d) were canceled", buildingInfos->GetAt(i).GetBuildingName(language), buildingInfos->GetAt(i).GetRunningNumber());
				AfxMessageBox(s);				
			}

			s.Format("%d;",buildingInfos->GetAt(i).GetRunningNumber());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOwnerOfBuilding());
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetBuildingName(language));
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetBuildingName(!language));
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetBuildingDescription(language));
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetBuildingDescription(!language));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetUpgradeable());
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetGraphikFileName());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMaxInSystem().Number);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMaxInSystem().RunningNumber);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMaxInEmpire().Number);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMaxInEmpire().RunningNumber);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOnlyHomePlanet());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOnlyOwnColony());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOnlyMinorRace());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOnlyTakenSystem());
			file.WriteString(s);
			s.Format("%s;",buildingInfos->GetAt(i).GetOnlyInSystemWithName());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMinHabitants());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMinInSystem().Number);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMinInSystem().RunningNumber);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMinInEmpire().Number);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMinInEmpire().RunningNumber);
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetOnlyRace());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(A));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(B));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(C));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(E));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(F));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(G));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(H));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(I));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(J));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(K));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(L));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(M));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(N));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(O));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(P));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(Q));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(R));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(S));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(T));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPlanetTypes(Y));
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetBioTech());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEnergyTech());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetCompTech());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPropulsionTech());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetConstructionTech());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetWeaponTech());	
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededIndustry());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededEnergy());	
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededTitan());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededDeuterium());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededDuranium());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededCrystal());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededIridium());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeededDilithium());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetFoodProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIPProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEnergyProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetSPProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetFPProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetTitanProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDeuteriumProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDuraniumProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetCrystalProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIridiumProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDilithiumProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetLatinum());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMoralProd());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMoralProdEmpire());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetFoodBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIndustryBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEnergyBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetSecurityBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetResearchBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetTitanBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDeuteriumBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDuraniumBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetCrystalBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIridiumBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetDilithiumBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetAllRessourcesBoni());	
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetLatinumBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetBioTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEnergyTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetCompTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPropulsionTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetConstructionTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetWeaponTechBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetInnerSecurityBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEconomySpyBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetEconomySabotageBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetResearchSpyBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetResearchSabotageBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMilitarySpyBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMilitarySabotageBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipYard());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetMaxBuildableShipSize());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipYardSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetBarrack());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetBarrackSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetHitPoints());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShieldPower());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShieldPowerBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipDefend());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipDefendBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetGroundDefend());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetGroundDefendBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetScanPower());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetScanPowerBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetScanRange());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetScanRangeBoni());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipTraining());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetTroopTraining());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetResistance());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetAddedTradeRoutes());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIncomeOnTradeRoutes());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipRecycling());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetBuildingBuildSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetUpdateBuildSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetShipBuildSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetTroopBuildSpeed());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetPredecessorID());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetIsBuildingOnline());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetWorker());
			file.WriteString(s);
			s.Format("%d;",buildingInfos->GetAt(i).GetNeverReady());
			file.WriteString(s);
			for (int p = HUMAN; p <= DOMINION; p++)
			{
				s.Format("%d;",buildingInfos->GetAt(i).GetEquivalent(p));
				file.WriteString(s);
			}
			// neu in Alpha5	
			for (int res = TITAN; res <= DILITHIUM; res++)
			{
				s.Format("%d;", buildingInfos->GetAt(i).GetResourceDistributor(res));
				file.WriteString(s);
			}
			s.Format("%d;", buildingInfos->GetAt(i).GetNeededSystems());
			file.WriteString(s);
			s.Format("\n");
			file.WriteString(s);

		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"BuildingsExport.csv\" konnte nicht geschrieben werden...");
	}
	file.Close();							// Datei wird geschlossen
}