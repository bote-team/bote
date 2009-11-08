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

/*
struct building_struct
{
	USHORT m_iRunningNumber;
	BYTE m_iOwnerOfBuilding;
	CString m_strBuildingName;
	CString m_strDescription;
	BOOLEAN m_bUpgradeable;
	CString m_strGraphikFileName;
	NumberOfID m_MaxInSystem;
	NumberOfID m_MaxInEmpire;
	BOOLEAN m_bOnlyHomePlanet;
	BOOLEAN m_bOnlyOwnColony;
	BOOLEAN m_bOnlyMinorRace;
	BOOLEAN m_bOnlyTakenSystem;
	CString m_strOnlyInSystemWithName;
	USHORT m_iMinHabitants;
	NumberOfID m_MinInSystem;
	NumberOfID m_MinInEmpire;
	BOOLEAN m_bOnlyRace;
	BOOLEAN m_bPlanetTypes[20];
	BYTE m_iBioTech;
	BYTE m_iEnergyTech;
	BYTE m_iCompTech;
	BYTE m_iPropulsionTech;
	BYTE m_iConstructionTech;
	BYTE m_iWeaponTech;	
	USHORT m_iNeededIndustry;
	USHORT m_iNeededEnergy;	
	USHORT m_iNeededTitan;
	USHORT m_iNeededDeuterium;
	USHORT m_iNeededDuranium;
	USHORT m_iNeededCrystal;
	USHORT m_iNeededIridium;
	USHORT m_iNeededDilithium;
	short m_iFood;
	USHORT m_iIP;
	USHORT m_iEnergy;
	USHORT m_iSP;
	USHORT m_iFP;
	USHORT m_iTitan;
	USHORT m_iDeuterium;
	USHORT m_iDuranium;
	USHORT m_iCrystal;
	USHORT m_iIridium;
	USHORT m_iDilithium;
	short m_iLatinum;
	BYTE m_iMoral;
	BYTE m_iMoralEmpire;
	short m_iFoodBoni;
	short m_iIndustryBoni;
	short m_iEnergyBoni;
	short m_iSecurityBoni;
	short m_iResearchBoni;
	short m_iTitanBoni;
	short m_iDeuteriumBoni;
	short m_iDuraniumBoni;
	short m_iCrystalBoni;
	short m_iIridiumBoni;
	short m_iDilithiumBoni;
	short m_iAllRessourceBoni;	
	short m_iLatinumBoni;
	short m_iBioTechBoni;
	short m_iEnergyTechBoni;
	short m_iCompTechBoni;
	short m_iPropulsionTechBoni;
	short m_iConstructionTechBoni;
	short m_iWeaponTechBoni;
	short m_iInnerSecurityBoni;
	short m_iEconomySpyBoni;
	short m_iEconomySabotageBoni;
	short m_iResearchSpyBoni;
	short m_iResearchSabotageBoni;
	short m_iMilitarySpyBoni;
	short m_iMilitarySabotageBoni;
	BOOLEAN m_bShipYard;
	BYTE m_iBuildableShipTypes;
	short m_iShipYardSpeed;
	BOOLEAN m_bBarrack;
	short m_iBarrackSpeed;
	USHORT m_iHitPoints;
	USHORT m_iShieldPower;
	short m_iShieldPowerBoni;
	USHORT m_iShipDefend;
	short m_iShipDefendBoni;
	USHORT m_iGroundDefend;
	short m_iGroundDefendBoni;
	USHORT m_iScanPower;
	short m_iScanPowerBoni;
	BYTE m_iScanRange;
	short m_iScanRangeBoni;
	short m_iShipTraining;
	short m_iTroopTraining;
	short m_iResistance;				// Bestechungsresistenz
	BYTE m_iAddedTradeRoutes;
	short m_iIncomeOnTradeRoutes;
	short m_iShipRecycling;
	short m_iBuildingBuildSpeed;
	short m_iUpdateBuildSpeed;
	short m_iShipBuildSpeed;
	short m_iTroopBuildSpeed;
	USHORT m_iPredecessorID;
	BOOLEAN m_bOnline;
	BOOLEAN m_bWorker;
	BOOLEAN m_bNeverReady;
	USHORT m_iBuildingEquivalent[7];
};
*/

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
	CString data[133];
	CString fileName = "Buildings.data";
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))		// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			// Daten lesen
			data[i++] = csInput;
			if (i == 133)
			{
				i = 0;
			/*	info.SetRunningNumber(atoi(data[0]));
				info.SetOwnerOfBuilding(atoi(data[1]));
				info.SetBuildingName(data[2]);
				info.SetBuildingDescription(data[3]);
				info.SetUpgradeable(atoi(data[4]));
				info.SetGraphikFileName(data[5]);
				info.SetMaxInSystem(atoi(data[6]),atoi(data[7]));
				info.SetMaxInEmpire(atoi(data[8]),atoi(data[9]));
				info.SetOnlyHomePlanet(atoi(data[10]));
				info.SetOnlyOwnColony(atoi(data[11]));
				info.SetOnlyMinorRace(atoi(data[12]));
				info.SetOnlyTakenSystem(atoi(data[13]));
				info.SetOnlyInSystemWithName(data[14]);
				info.SetMinHabitants(atoi(data[15]));
				info.SetMinInSystem(atoi(data[16]),atoi(data[17]));
				info.SetMinInEmpire(atoi(data[18]),atoi(data[19]));
				info.SetOnlyRace(atoi(data[20]));
				info.SetPlanetTypes(A,atoi(data[21]));
				info.SetPlanetTypes(B,atoi(data[22]));
				info.SetPlanetTypes(C,atoi(data[23]));
				info.SetPlanetTypes(E,atoi(data[24]));
				info.SetPlanetTypes(F,atoi(data[25]));
				info.SetPlanetTypes(G,atoi(data[26]));
				info.SetPlanetTypes(H,atoi(data[27]));
				info.SetPlanetTypes(I,atoi(data[28]));
				info.SetPlanetTypes(J,atoi(data[29]));
				info.SetPlanetTypes(K,atoi(data[30]));
				info.SetPlanetTypes(L,atoi(data[31]));
				info.SetPlanetTypes(M,atoi(data[32]));
				info.SetPlanetTypes(N,atoi(data[33]));
				info.SetPlanetTypes(O,atoi(data[34]));
				info.SetPlanetTypes(P,atoi(data[35]));
				info.SetPlanetTypes(Q,atoi(data[36]));
				info.SetPlanetTypes(R,atoi(data[37]));
				info.SetPlanetTypes(S,atoi(data[38]));
				info.SetPlanetTypes(T,atoi(data[39]));
				info.SetPlanetTypes(Y,atoi(data[40]));
				info.SetBioTech(atoi(data[41]));
				info.SetEnergyTech(atoi(data[42]));
				info.SetCompTech(atoi(data[43]));
				info.SetPropulsionTech(atoi(data[44]));
				info.SetConstructionTech(atoi(data[45]));
				info.SetWeaponTech(atoi(data[46]));
				info.SetNeededIndustry(atoi(data[47]));
				info.SetNeededEnergy(atoi(data[48]));
				info.SetNeededTitan(atoi(data[49]));
				info.SetNeededDeuterium(atoi(data[50]));
				info.SetNeededDuranium(atoi(data[51]));
				info.SetNeededCrystal(atoi(data[52]));
				info.SetNeededIridium(atoi(data[53]));
				info.SetNeededDilithium(atoi(data[54]));
				info.SetFoodProd(atoi(data[55]));
				info.SetIPProd(atoi(data[56]));
				info.SetEnergyProd(atoi(data[57]));
				info.SetSPProd(atoi(data[58]));
				info.SetFPProd(atoi(data[59]));
				info.SetTitanProd(atoi(data[60]));
				info.SetDeuteriumProd(atoi(data[61]));
				info.SetDuraniumProd(atoi(data[62]));
				info.SetCrystalProd(atoi(data[63]));
				info.SetIridiumProd(atoi(data[64]));
				info.SetDilithiumProd(atoi(data[65]));
				info.SetLatinumProd(atoi(data[66]));
				info.SetMoralProd(atoi(data[67]));
				info.SetMoralProdEmpire(atoi(data[68]));
				info.SetFoodBoni(atoi(data[69]));
				info.SetIndustryBoni(atoi(data[70]));
				info.SetEnergyBoni(atoi(data[71]));
				info.SetSecurityBoni(atoi(data[72]));
				info.SetResearchBoni(atoi(data[73]));
				info.SetTitanBoni(atoi(data[74]));
				info.SetDeuteriumBoni(atoi(data[75]));
				info.SetDuraniumBoni(atoi(data[76]));
				info.SetCrystalBoni(atoi(data[77]));
				info.SetIridiumBoni(atoi(data[78]));
				info.SetDilithiumBoni(atoi(data[79]));
				info.SetAllRessourceBoni(atoi(data[80]));
				info.SetLatinumBoni(atoi(data[81]));
				info.SetBioTechBoni(atoi(data[82]));
				info.SetEnergyTechBoni(atoi(data[83]));
				info.SetCompTechBoni(atoi(data[84]));
				info.SetPropulsionTechBoni(atoi(data[85]));
				info.SetConstructionTechBoni(atoi(data[86]));
				info.SetWeaponTechBoni(atoi(data[87]));
				info.SetInnerSecurityBoni(atoi(data[88]));
				info.SetEconomySpyBoni(atoi(data[89]));
				info.SetEconomySabotageBoni(atoi(data[90]));
				info.SetResearchSpyBoni(atoi(data[91]));
				info.SetResearchSabotageBoni(atoi(data[92]));
				info.SetMilitarySpyBoni(atoi(data[93]));
				info.SetMilitarySabotageBoni(atoi(data[94]));
				info.SetShipYard(atoi(data[95]));
				info.SetBuildableShipTypes(atoi(data[96]));
				info.SetShipYardSpeed(atoi(data[97]));
				info.SetBarrack(atoi(data[98]));
				info.SetBarrackSpeed(atoi(data[99]));
				info.SetHitpoints(atoi(data[100]));
				info.SetShieldPower(atoi(data[101]));
				info.SetShieldPowerBoni(atoi(data[102]));
				info.SetShipDefend(atoi(data[103]));
				info.SetShipDefendBoni(atoi(data[104]));
				info.SetGroundDefend(atoi(data[105]));
				info.SetGroundDefendBoni(atoi(data[106]));
				info.SetScanPower(atoi(data[107]));
				info.SetScanPowerBoni(atoi(data[108]));
				info.SetScanRange(atoi(data[109]));
				info.SetScanRangeBoni(atoi(data[110]));
				info.SetShipTraining(atoi(data[111]));
				info.SetTroopTraining(atoi(data[112]));
				info.SetResistance(atoi(data[113]));
				info.SetAddedTradeRoutes(atoi(data[114]));
				info.SetIncomeOnTradeRoutes(atoi(data[115]));
				info.SetShipRecycling(atoi(data[116]));
				info.SetBuildingBuildSpeed(atoi(data[117]));
				info.SetUpdateBuildSpeed(atoi(data[118]));
				info.SetShipBuildSpeed(atoi(data[119]));
				info.SetTroopBuildSpeed(atoi(data[120]));
				info.SetPredecessor(atoi(data[121]));
				info.SetIsBuildingOnline(atoi(data[122]));
				info.SetWorker(atoi(data[123]));
				info.SetNeverReady(atoi(data[124]));
				info.SetEquivalent(NOBODY,0);		// niemand-index immer auf NULL setzen
				for (int p = HUMAN; p <= DOMINION; p++)
					info.SetEquivalent(p,atoi(data[124+p]));
		*/		
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
				// Information in das Gebäudeinfofeld schreiben
				m_BuildingInfos->Add(info);
			}
		}

	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Buildings.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

void CFileReader::WriteDataToFile(CArray<CBuildingInfo,CBuildingInfo>* buildingInfos, BOOLEAN language)
{
	// Sortierung
	//c_arraysort<CArray<CShipInfo,CShipInfo>,CShipInfo>(m_ShipInfos,sort_asc);
	int i = 0;
	CString s;
	CBuildingInfo buildingInfo;
	CString fileName="Buildings.data";		// Name des zu Öffnenden Files 
	CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
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
			
		/*	building_struct bs;
			bs.m_iRunningNumber = buildingInfos->GetAt(i).GetRunningNumber();
			bs.m_iOwnerOfBuilding = buildingInfos->GetAt(i).GetOwnerOfBuilding();
			bs.m_strBuildingName = buildingInfos->GetAt(i).GetBuildingName();
			bs.m_strDescription = buildingInfos->GetAt(i).GetBuildingDescription();
			bs.m_bUpgradeable = buildingInfos->GetAt(i).GetUpgradeable();
			bs.m_strGraphikFileName = buildingInfos->GetAt(i).GetGraphikFileName();
			bs.m_MaxInSystem.Number = buildingInfos->GetAt(i).GetMaxInSystem().Number;
			bs.m_MaxInSystem.RunningNumber = buildingInfos->GetAt(i).GetMaxInSystem().RunningNumber;
			bs.m_MaxInEmpire.Number = buildingInfos->GetAt(i).GetMaxInEmpire().Number;
			bs.m_MaxInEmpire.RunningNumber = buildingInfos->GetAt(i).GetMaxInEmpire().RunningNumber;
			bs.m_bOnlyHomePlanet = buildingInfos->GetAt(i).GetOnlyHomePlanet();
			bs.m_bOnlyOwnColony = buildingInfos->GetAt(i).GetOnlyOwnColony();
			bs.m_bOnlyMinorRace = buildingInfos->GetAt(i).GetOnlyMinorRace();
			bs.m_bOnlyTakenSystem = buildingInfos->GetAt(i).GetOnlyTakenSystem();
			bs.m_strOnlyInSystemWithName = buildingInfos->GetAt(i).GetOnlyInSystemWithName();
			bs.m_iMinHabitants = buildingInfos->GetAt(i).GetMinHabitants();
			bs.m_MinInSystem.Number = buildingInfos->GetAt(i).GetMinInSystem().Number;
			bs.m_MinInSystem.RunningNumber = buildingInfos->GetAt(i).GetMinInSystem().RunningNumber;
			bs.m_MinInEmpire.Number = buildingInfos->GetAt(i).GetMinInEmpire().Number;
			bs.m_MinInEmpire.RunningNumber = buildingInfos->GetAt(i).GetMinInEmpire().RunningNumber;
			bs.m_bOnlyRace = buildingInfos->GetAt(i).GetOnlyRace();
			bs.m_bPlanetTypes[A] = buildingInfos->GetAt(i).GetPlanetTypes(A);
			bs.m_bPlanetTypes[B] = buildingInfos->GetAt(i).GetPlanetTypes(B);
			bs.m_bPlanetTypes[C] = buildingInfos->GetAt(i).GetPlanetTypes(C);
			bs.m_bPlanetTypes[E] = buildingInfos->GetAt(i).GetPlanetTypes(E);
			bs.m_bPlanetTypes[F] = buildingInfos->GetAt(i).GetPlanetTypes(F);
			bs.m_bPlanetTypes[G] = buildingInfos->GetAt(i).GetPlanetTypes(G);
			bs.m_bPlanetTypes[H] = buildingInfos->GetAt(i).GetPlanetTypes(H);
			bs.m_bPlanetTypes[I] = buildingInfos->GetAt(i).GetPlanetTypes(I);
			bs.m_bPlanetTypes[J] = buildingInfos->GetAt(i).GetPlanetTypes(J);
			bs.m_bPlanetTypes[K] = buildingInfos->GetAt(i).GetPlanetTypes(K);
			bs.m_bPlanetTypes[L] = buildingInfos->GetAt(i).GetPlanetTypes(L);
			bs.m_bPlanetTypes[M] = buildingInfos->GetAt(i).GetPlanetTypes(M);
			bs.m_bPlanetTypes[N] = buildingInfos->GetAt(i).GetPlanetTypes(N);
			bs.m_bPlanetTypes[O] = buildingInfos->GetAt(i).GetPlanetTypes(O);
			bs.m_bPlanetTypes[P] = buildingInfos->GetAt(i).GetPlanetTypes(P);
			bs.m_bPlanetTypes[Q] = buildingInfos->GetAt(i).GetPlanetTypes(Q);
			bs.m_bPlanetTypes[R] = buildingInfos->GetAt(i).GetPlanetTypes(R);
			bs.m_bPlanetTypes[S] = buildingInfos->GetAt(i).GetPlanetTypes(S);
			bs.m_bPlanetTypes[T] = buildingInfos->GetAt(i).GetPlanetTypes(T);
			bs.m_bPlanetTypes[Y] = buildingInfos->GetAt(i).GetPlanetTypes(Y);
			bs.m_iBioTech = buildingInfos->GetAt(i).GetBioTech();
			bs.m_iEnergyTech = buildingInfos->GetAt(i).GetEnergyTech();
			bs.m_iCompTech = buildingInfos->GetAt(i).GetCompTech();
			bs.m_iPropulsionTech = buildingInfos->GetAt(i).GetPropulsionTech();
			bs.m_iConstructionTech = buildingInfos->GetAt(i).GetConstructionTech();
			bs.m_iWeaponTech = buildingInfos->GetAt(i).GetWeaponTech();	
			bs.m_iNeededIndustry = buildingInfos->GetAt(i).GetNeededIndustry();
			bs.m_iNeededEnergy = buildingInfos->GetAt(i).GetNeededEnergy();	
			bs.m_iNeededTitan = buildingInfos->GetAt(i).GetNeededTitan();
			bs.m_iNeededDeuterium = buildingInfos->GetAt(i).GetNeededDeuterium();
			bs.m_iNeededDuranium = buildingInfos->GetAt(i).GetNeededDuranium();
			bs.m_iNeededCrystal = buildingInfos->GetAt(i).GetNeededCrystal();
			bs.m_iNeededIridium = buildingInfos->GetAt(i).GetNeededIridium();
			bs.m_iNeededDilithium = buildingInfos->GetAt(i).GetNeededDilithium();
			bs.m_iFood = buildingInfos->GetAt(i).GetFoodProd();
			bs.m_iIP = buildingInfos->GetAt(i).GetIPProd();
			bs.m_iEnergy = buildingInfos->GetAt(i).GetEnergyProd();
			bs.m_iSP = buildingInfos->GetAt(i).GetSPProd();
			bs.m_iFP = buildingInfos->GetAt(i).GetFPProd();
			bs.m_iTitan = buildingInfos->GetAt(i).GetTitanProd();
			bs.m_iDeuterium = buildingInfos->GetAt(i).GetDeuteriumProd();
			bs.m_iDuranium = buildingInfos->GetAt(i).GetDuraniumProd();
			bs.m_iCrystal = buildingInfos->GetAt(i).GetCrystalProd();
			bs.m_iIridium = buildingInfos->GetAt(i).GetIridiumProd();
			bs.m_iDilithium = buildingInfos->GetAt(i).GetDilithiumProd();
			bs.m_iLatinum = buildingInfos->GetAt(i).GetLatinum();
			bs.m_iMoral = buildingInfos->GetAt(i).GetMoralProd();
			bs.m_iMoralEmpire = buildingInfos->GetAt(i).GetMoralProdEmpire();
			bs.m_iFoodBoni = buildingInfos->GetAt(i).GetFoodBoni();
			bs.m_iIndustryBoni = buildingInfos->GetAt(i).GetIndustryBoni();
			bs.m_iEnergyBoni = buildingInfos->GetAt(i).GetEnergyBoni();
			bs.m_iSecurityBoni = buildingInfos->GetAt(i).GetSecurityBoni();
			bs.m_iResearchBoni = buildingInfos->GetAt(i).GetResearchBoni();
			bs.m_iTitanBoni = buildingInfos->GetAt(i).GetTitanBoni();
			bs.m_iDeuteriumBoni = buildingInfos->GetAt(i).GetDeuteriumBoni();
			bs.m_iDuraniumBoni = buildingInfos->GetAt(i).GetDuraniumBoni();
			bs.m_iCrystalBoni = buildingInfos->GetAt(i).GetCrystalBoni();
			bs.m_iIridiumBoni = buildingInfos->GetAt(i).GetIridiumBoni();
			bs.m_iDilithiumBoni = buildingInfos->GetAt(i).GetDilithiumBoni();
			bs.m_iAllRessourceBoni = buildingInfos->GetAt(i).GetAllRessourcesBoni();	
			bs.m_iLatinumBoni = buildingInfos->GetAt(i).GetLatinumBoni();
			bs.m_iBioTechBoni = buildingInfos->GetAt(i).GetBioTechBoni();
			bs.m_iEnergyTechBoni = buildingInfos->GetAt(i).GetEnergyTechBoni();
			bs.m_iCompTechBoni = buildingInfos->GetAt(i).GetCompTechBoni();
			bs.m_iPropulsionTechBoni = buildingInfos->GetAt(i).GetPropulsionTechBoni();
			bs.m_iConstructionTechBoni = buildingInfos->GetAt(i).GetConstructionTechBoni();
			bs.m_iWeaponTechBoni = buildingInfos->GetAt(i).GetWeaponTechBoni();
			bs.m_iInnerSecurityBoni = buildingInfos->GetAt(i).GetInnerSecurityBoni();
			bs.m_iEconomySpyBoni = buildingInfos->GetAt(i).GetEconomySpyBoni();
			bs.m_iEconomySabotageBoni = buildingInfos->GetAt(i).GetEconomySabotageBoni();
			bs.m_iResearchSpyBoni = buildingInfos->GetAt(i).GetResearchSpyBoni();
			bs.m_iResearchSabotageBoni = buildingInfos->GetAt(i).GetResearchSabotageBoni();
			bs.m_iMilitarySpyBoni = buildingInfos->GetAt(i).GetMilitarySpyBoni();
			bs.m_iMilitarySabotageBoni = buildingInfos->GetAt(i).GetMilitarySabotageBoni();
			bs.m_bShipYard = buildingInfos->GetAt(i).GetShipYard();
			bs.m_iBuildableShipTypes = buildingInfos->GetAt(i).GetBuildableShipTypes();
			bs.m_iShipYardSpeed = buildingInfos->GetAt(i).GetShipYardSpeed();
			bs.m_bBarrack = buildingInfos->GetAt(i).GetBarrack();
			bs.m_iBarrackSpeed = buildingInfos->GetAt(i).GetBarrackSpeed();
			bs.m_iHitPoints = buildingInfos->GetAt(i).GetHitPoints();
			bs.m_iShieldPower = buildingInfos->GetAt(i).GetShieldPower();
			bs.m_iShieldPowerBoni = buildingInfos->GetAt(i).GetShieldPowerBoni();
			bs.m_iShipDefend = buildingInfos->GetAt(i).GetShipDefend();
			bs.m_iShipDefendBoni = buildingInfos->GetAt(i).GetShipDefendBoni();
			bs.m_iGroundDefend = buildingInfos->GetAt(i).GetGroundDefend();
			bs.m_iGroundDefendBoni = buildingInfos->GetAt(i).GetGroundDefendBoni();
			bs.m_iScanPower = buildingInfos->GetAt(i).GetScanPower();
			bs.m_iScanPowerBoni = buildingInfos->GetAt(i).GetScanPowerBoni();
			bs.m_iScanRange = buildingInfos->GetAt(i).GetScanRange();
			bs.m_iScanRangeBoni = buildingInfos->GetAt(i).GetScanRangeBoni();
			bs.m_iShipTraining = buildingInfos->GetAt(i).GetShipTraining();
			bs.m_iTroopTraining = buildingInfos->GetAt(i).GetTroopTraining();
			bs.m_iResistance = buildingInfos->GetAt(i).GetResistance();
			bs.m_iAddedTradeRoutes = buildingInfos->GetAt(i).GetAddedTradeRoutes();
			bs.m_iIncomeOnTradeRoutes = buildingInfos->GetAt(i).GetIncomeOnTradeRoutes();
			bs.m_iShipRecycling = buildingInfos->GetAt(i).GetShipRecycling();
			bs.m_iBuildingBuildSpeed = buildingInfos->GetAt(i).GetBuildingBuildSpeed();
			bs.m_iUpdateBuildSpeed = buildingInfos->GetAt(i).GetUpdateBuildSpeed();
			bs.m_iShipBuildSpeed = buildingInfos->GetAt(i).GetShipBuildSpeed();
			bs.m_iTroopBuildSpeed = buildingInfos->GetAt(i).GetTroopBuildSpeed();
			bs.m_iPredecessorID = buildingInfos->GetAt(i).GetPredecessorID();
			bs.m_bOnline = buildingInfos->GetAt(i).GetIsBuildingOnline();
			bs.m_bWorker = buildingInfos->GetAt(i).GetWorker();
			bs.m_bNeverReady = buildingInfos->GetAt(i).GetNeverReady();
			for (int p = HUMAN; p <= DOMINION; p++)
				bs.m_iBuildingEquivalent[p] = buildingInfos->GetAt(i).GetEquivalent(p);
			file.Write(&bs, sizeof(bs));*/
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Buildings.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}