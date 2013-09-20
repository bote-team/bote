/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// BuildingInfo.h: Schnittstelle für die Klasse CBuildingInfo.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Building.h"

struct NumberOfID
{
public:
	USHORT RunningNumber;
	USHORT Number;

	NumberOfID() :
		RunningNumber(0),
			Number(0)
	{}
};

class CBuildingInfo : public CObject
{
	enum PlanetTypes
	{
		PLANETTYPE_A	= 1,
		PLANETTYPE_B	= 2,
		PLANETTYPE_C	= 4,
		PLANETTYPE_E	= 8,
		PLANETTYPE_F	= 16,
		PLANETTYPE_G	= 32,
		PLANETTYPE_H	= 64,
		PLANETTYPE_I	= 128,
		PLANETTYPE_J	= 256,
		PLANETTYPE_K	= 512,
		PLANETTYPE_L	= 1024,
		PLANETTYPE_M	= 2048,
		PLANETTYPE_N	= 4096,
		PLANETTYPE_O	= 8192,
		PLANETTYPE_P	= 16384,
		PLANETTYPE_Q	= 32768,
		PLANETTYPE_R	= 65536,
		PLANETTYPE_S	= 131072,
		PLANETTYPE_T	= 262144,
		PLANETTYPE_Y	= 524288
	};
public:
	DECLARE_SERIAL (CBuildingInfo)							// Klasse serialisierbar machen
	// Standardkonstruktor
	CBuildingInfo(void);

	// Destruktor
	~CBuildingInfo(void);

	// Kopierkonstruktor
	CBuildingInfo(const CBuildingInfo & rhs);

	// Zuweisungsoperatur
	CBuildingInfo & operator=(const CBuildingInfo &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	USHORT GetRunningNumber() const {return m_iRunningNumber;}

	BYTE GetOwnerOfBuilding() const {return m_iOwnerOfBuilding;}

	const CString& GetBuildingName() const {return m_strBuildingName;}

	const CString& GetBuildingDescription() const {return m_strDescription;}

	BOOLEAN GetUpgradeable() const {return m_bUpgradeable;}

	const CString& GetGraphikFileName() const {return m_strGraphikFileName;}

	NumberOfID GetMaxInSystem() const {return m_MaxInSystem;}

	USHORT GetMaxInEmpire() const {return m_MaxInEmpire.Number;}

	BOOLEAN GetOnlyHomePlanet() const {return m_bOnlyHomePlanet;}

	BOOLEAN GetOnlyOwnColony() const {return m_bOnlyOwnColony;}

	BOOLEAN GetOnlyMinorRace() const {return m_bOnlyMinorRace;}

	BOOLEAN GetOnlyTakenSystem() const {return m_bOnlyTakenSystem;}

	const CString& GetOnlyInSystemWithName() const {return m_strOnlyInSystemWithName;}

	USHORT GetMinHabitants() const {return m_iMinHabitants;}

	NumberOfID GetMinInSystem() const {return m_MinInSystem;}

	BOOLEAN GetOnlyRace() const {return m_bOnlyRace;}

	BOOLEAN GetPlanetTypes(int type) const
	{
		switch(type)
		{
		case PLANET_CLASSES::PLANETCLASS_A: return (m_PlanetTypes & PLANETTYPE_A) == PLANETTYPE_A;
		case PLANET_CLASSES::PLANETCLASS_B:	return (m_PlanetTypes & PLANETTYPE_B) == PLANETTYPE_B;
		case PLANET_CLASSES::PLANETCLASS_C:	return (m_PlanetTypes & PLANETTYPE_C) == PLANETTYPE_C;
		case PLANET_CLASSES::PLANETCLASS_E:	return (m_PlanetTypes & PLANETTYPE_E) == PLANETTYPE_E;
		case PLANET_CLASSES::PLANETCLASS_F:	return (m_PlanetTypes & PLANETTYPE_F) == PLANETTYPE_F;
		case PLANET_CLASSES::PLANETCLASS_G:	return (m_PlanetTypes & PLANETTYPE_G) == PLANETTYPE_G;
		case PLANET_CLASSES::PLANETCLASS_H:	return (m_PlanetTypes & PLANETTYPE_H) == PLANETTYPE_H;
		case PLANET_CLASSES::PLANETCLASS_I:	return (m_PlanetTypes & PLANETTYPE_I) == PLANETTYPE_I;
		case PLANET_CLASSES::PLANETCLASS_J:	return (m_PlanetTypes & PLANETTYPE_J) == PLANETTYPE_J;
		case PLANET_CLASSES::PLANETCLASS_K:	return (m_PlanetTypes & PLANETTYPE_K) == PLANETTYPE_K;
		case PLANET_CLASSES::PLANETCLASS_L:	return (m_PlanetTypes & PLANETTYPE_L) == PLANETTYPE_L;
		case PLANET_CLASSES::PLANETCLASS_M:	return (m_PlanetTypes & PLANETTYPE_M) == PLANETTYPE_M;
		case PLANET_CLASSES::PLANETCLASS_N:	return (m_PlanetTypes & PLANETTYPE_N) == PLANETTYPE_N;
		case PLANET_CLASSES::PLANETCLASS_O:	return (m_PlanetTypes & PLANETTYPE_O) == PLANETTYPE_O;
		case PLANET_CLASSES::PLANETCLASS_P:	return (m_PlanetTypes & PLANETTYPE_P) == PLANETTYPE_P;
		case PLANET_CLASSES::PLANETCLASS_Q:	return (m_PlanetTypes & PLANETTYPE_Q) == PLANETTYPE_Q;
		case PLANET_CLASSES::PLANETCLASS_R:	return (m_PlanetTypes & PLANETTYPE_R) == PLANETTYPE_R;
		case PLANET_CLASSES::PLANETCLASS_S:	return (m_PlanetTypes & PLANETTYPE_S) == PLANETTYPE_S;
		case PLANET_CLASSES::PLANETCLASS_T:	return (m_PlanetTypes & PLANETTYPE_T) == PLANETTYPE_T;
		case PLANET_CLASSES::PLANETCLASS_Y:	return (m_PlanetTypes & PLANETTYPE_Y) == PLANETTYPE_Y;
		default: return FALSE;
		}
	}

	BYTE GetBioTech() const {return m_iBioTech;}
	BYTE GetEnergyTech() const {return m_iEnergyTech;}
	BYTE GetCompTech() const {return m_iCompTech;}
	BYTE GetPropulsionTech() const {return m_iPropulsionTech;}
	BYTE GetConstructionTech() const {return m_iConstructionTech;}
	BYTE GetWeaponTech() const {return m_iWeaponTech;}

	USHORT GetNeededIndustry() const {return m_iNeededIndustry;}
	USHORT GetNeededTitan() const {return m_iNeededTitan;}
	USHORT GetNeededDeuterium() const {return m_iNeededDeuterium;}
	USHORT GetNeededDuranium() const {return m_iNeededDuranium;}
	USHORT GetNeededCrystal() const {return m_iNeededCrystal;}
	USHORT GetNeededIridium() const {return m_iNeededIridium;}
	USHORT GetNeededDeritium() const {return m_iNeededDeritium;}
	USHORT GetNeededResource(BYTE res) const;

	USHORT GetPredecessorID() const {return m_iPredecessorID;}

	USHORT GetEquivalent(USHORT race) const {return m_iBuildingEquivalent[race];}

	USHORT GetNeededEnergy() const {return m_iNeededEnergy;}

	short GetFoodProd() const {return m_iFood;}
	USHORT GetIPProd() const {return m_iIP;}
	USHORT GetEnergyProd() const {return m_iEnergy;}
	USHORT GetSPProd() const {return m_iSP;}
	USHORT GetFPProd() const {return m_iFP;}
	USHORT GetTitanProd() const {return m_iTitan;}
	USHORT GetDeuteriumProd() const {return m_iDeuterium;}
	USHORT GetDuraniumProd() const {return m_iDuranium;}
	USHORT GetCrystalProd() const {return m_iCrystal;}
	USHORT GetIridiumProd() const {return m_iIridium;}
	USHORT GetDeritiumProd() const {return m_iDeritium;}
	USHORT GetResourceProd(BYTE res) const;
	short GetCredits() const {return m_iCredits;}
	short GetMoralProd() const {return m_iMoral;}
	short GetMoralProdEmpire() const {return m_iMoralEmpire;}

	short GetFoodBoni() const {return m_iFoodBoni;}
	short GetIndustryBoni() const {return m_iIndustryBoni;}
	short GetEnergyBoni() const {return m_iEnergyBoni;}
	short GetSecurityBoni() const {return m_iSecurityBoni;}
	short GetResearchBoni() const {return m_iResearchBoni;}
	short GetTitanBoni() const {return m_iTitanBoni;}
	short GetDeuteriumBoni() const {return m_iDeuteriumBoni;}
	short GetDuraniumBoni() const {return m_iDuraniumBoni;}
	short GetCrystalBoni() const {return m_iCrystalBoni;}
	short GetIridiumBoni() const {return m_iIridiumBoni;}
	short GetDeritiumBoni() const {return m_iDeritiumBoni;}
	short GetAllRessourcesBoni() const {return m_iAllRessourceBoni;}
	short GetCreditsBoni() const {return m_iCreditsBoni;}

	short GetBioTechBoni() const {return m_iBioTechBoni;}
	short GetEnergyTechBoni() const {return m_iEnergyTechBoni;}
	short GetCompTechBoni() const {return m_iCompTechBoni;}
	short GetPropulsionTechBoni() const {return m_iPropulsionTechBoni;}
	short GetConstructionTechBoni() const {return m_iConstructionTechBoni;}
	short GetWeaponTechBoni() const {return m_iWeaponTechBoni;}

	short GetInnerSecurityBoni() const {return m_iInnerSecurityBoni;}
	short GetEconomySpyBoni() const {return m_iEconomySpyBoni;}
	short GetEconomySabotageBoni() const {return m_iEconomySabotageBoni;}
	short GetResearchSpyBoni() const {return m_iResearchSpyBoni;}
	short GetResearchSabotageBoni() const {return m_iResearchSabotageBoni;}
	short GetMilitarySpyBoni() const {return m_iMilitarySpyBoni;}
	short GetMilitarySabotageBoni() const {return m_iMilitarySabotageBoni;}

	// Hier die restlichen Zugriffsfunktionen
	BOOLEAN GetShipYard() const {return m_bShipYard;}
	SHIP_SIZE::Typ GetMaxBuildableShipSize() const {return m_iBuildableShipSizes;}
	short GetShipYardSpeed() const {return m_iShipYardSpeed;}
	BOOLEAN GetBarrack() const {return m_bBarrack;}
	short GetBarrackSpeed() const {return m_iBarrackSpeed;}
	USHORT GetHitPoints() const {return m_iHitPoints;}
	USHORT GetShieldPower() const {return m_iShieldPower;}
	short GetShieldPowerBoni() const {return m_iShieldPowerBoni;}
	USHORT GetShipDefend() const {return m_iShipDefend;}
	short GetShipDefendBoni() const {return m_iShipDefendBoni;}
	USHORT GetGroundDefend() const {return m_iGroundDefend;}
	short GetGroundDefendBoni() const {return m_iGroundDefendBoni;}
	USHORT GetScanPower() const {return m_iScanPower;}
	short GetScanPowerBoni() const {return m_iScanPowerBoni;}
	BYTE GetScanRange() const {return m_iScanRange;}
	short GetScanRangeBoni() const {return m_iScanRangeBoni;}
	short GetShipTraining() const {return m_iShipTraining;}
	short GetTroopTraining() const {return m_iTroopTraining;}
	short GetResistance() const {return m_iResistance;}
	BYTE GetAddedTradeRoutes() const {return m_iAddedTradeRoutes;}
	short GetIncomeOnTradeRoutes() const {return m_iIncomeOnTradeRoutes;}
	short GetShipRecycling() const {return m_iShipRecycling;}
	short GetBuildingBuildSpeed() const {return m_iBuildingBuildSpeed;}
	short GetUpdateBuildSpeed() const {return m_iUpdateBuildSpeed;}
	short GetShipBuildSpeed() const {return m_iShipBuildSpeed;}
	short GetTroopBuildSpeed() const {return m_iTroopBuildSpeed;}

	BOOLEAN GetWorker() const {return m_bWorker;}
	BOOLEAN GetNeverReady() const {return m_bNeverReady;}
	BOOLEAN GetAllwaysOnline() const {return m_bAllwaysOnline;}

	BOOLEAN GetResourceDistributor(BYTE res) const { return m_bResourceDistributor[res]; }
	short GetNeededSystems() const { return m_iNeededSystems; }

	// zum Schreiben der Membervariablen


	// zum Schreiben der Membervariablen
	void SetRunningNumber(USHORT RunningNumber) {m_iRunningNumber = RunningNumber;}

	void SetOwnerOfBuilding(BYTE owner) {m_iOwnerOfBuilding = owner;}

	void SetBuildingName(const CString& BuildingName) {m_strBuildingName = BuildingName;}

	void SetBuildingDescription(const CString& Description) {m_strDescription = Description;}

	void SetUpgradeable(BOOLEAN TrueOrFalse) {m_bUpgradeable = TrueOrFalse;}

	void SetGraphikFileName(const CString& FileName) {m_strGraphikFileName = FileName;}

	void SetMaxInSystem(USHORT number, USHORT runningNumber) {m_MaxInSystem.Number = number;
		m_MaxInSystem.RunningNumber = runningNumber;}

	void SetMaxInEmpire(USHORT number, USHORT runningNumber) {m_MaxInEmpire.Number = number;
		m_MaxInEmpire.RunningNumber = runningNumber;}

	void SetOnlyHomePlanet(BOOLEAN TrueOrFalse) {m_bOnlyHomePlanet = TrueOrFalse;}

	void SetOnlyOwnColony(BOOLEAN TrueOrFalse) {m_bOnlyOwnColony = TrueOrFalse;}

	void SetOnlyMinorRace(BOOLEAN TrueOrFalse) {m_bOnlyMinorRace = TrueOrFalse;}

	void SetOnlyTakenSystem(BOOLEAN TrueOrFalse) {m_bOnlyTakenSystem = TrueOrFalse;}

	void SetOnlyInSystemWithName(const CString& Name) {m_strOnlyInSystemWithName = Name;}

	void SetMinHabitants(USHORT min) {m_iMinHabitants = min;}

	void SetMinInSystem(USHORT number, USHORT runningNumber) {m_MinInSystem.Number = number;
		m_MinInSystem.RunningNumber = runningNumber;}

	void SetMinInEmpire(USHORT number, USHORT runningNumber) {m_MinInEmpire.Number = number;
		m_MinInEmpire.RunningNumber = runningNumber;}

	void SetOnlyRace(BOOLEAN TrueOrFalse) {m_bOnlyRace = TrueOrFalse;}

	void SetPlanetTypes(int type, BOOLEAN allow)
	{
		switch (type)
		{
		case PLANET_CLASSES::PLANETCLASS_A:	SetAttributes(allow, PLANETTYPE_A, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_B:	SetAttributes(allow, PLANETTYPE_B, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_C:	SetAttributes(allow, PLANETTYPE_C, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_E:	SetAttributes(allow, PLANETTYPE_E, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_F:	SetAttributes(allow, PLANETTYPE_F, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_G:	SetAttributes(allow, PLANETTYPE_G, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_H:	SetAttributes(allow, PLANETTYPE_H, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_I:	SetAttributes(allow, PLANETTYPE_I, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_J:	SetAttributes(allow, PLANETTYPE_J, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_K:	SetAttributes(allow, PLANETTYPE_K, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_L:	SetAttributes(allow, PLANETTYPE_L, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_M:	SetAttributes(allow, PLANETTYPE_M, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_N:	SetAttributes(allow, PLANETTYPE_N, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_O:	SetAttributes(allow, PLANETTYPE_O, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_P:	SetAttributes(allow, PLANETTYPE_P, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_Q:	SetAttributes(allow, PLANETTYPE_Q, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_R:	SetAttributes(allow, PLANETTYPE_R, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_S:	SetAttributes(allow, PLANETTYPE_S, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_T:	SetAttributes(allow, PLANETTYPE_T, m_PlanetTypes);	break;
		case PLANET_CLASSES::PLANETCLASS_Y:	SetAttributes(allow, PLANETTYPE_Y, m_PlanetTypes);	break;
		}
	}

	void SetBioTech(BYTE BioTech) {m_iBioTech = BioTech;}
	void SetEnergyTech(BYTE EnergyTech) {m_iEnergyTech = EnergyTech;}
	void SetCompTech(BYTE CompTech) {m_iCompTech = CompTech;}
	void SetPropulsionTech(BYTE PropulsionTech) {m_iPropulsionTech = PropulsionTech;}
	void SetConstructionTech(BYTE ConstructionTech) {m_iConstructionTech = ConstructionTech;}
	void SetWeaponTech(BYTE WeaponTech) {m_iWeaponTech = WeaponTech;}

	void SetNeededIndustry(USHORT NeededIndustry) {m_iNeededIndustry = NeededIndustry;}
	void SetNeededTitan(USHORT NeededTitan) {m_iNeededTitan = NeededTitan;}
	void SetNeededDeuterium(USHORT NeededDeuterium) {m_iNeededDeuterium = NeededDeuterium;}
	void SetNeededDuranium(USHORT NeededDuranium) {m_iNeededDuranium = NeededDuranium;}
	void SetNeededCrystal(USHORT NeededCrystal) {m_iNeededCrystal = NeededCrystal;}
	void SetNeededIridium(USHORT NeededIridium) {m_iNeededIridium = NeededIridium;}
	void SetNeededDeritium(USHORT NeededDeritium) {m_iNeededDeritium = NeededDeritium;}

	void SetPredecessor(USHORT RunningNumber) {m_iPredecessorID = RunningNumber;}

	void SetEquivalent(BYTE race, USHORT equivalentID) {m_iBuildingEquivalent[race] = equivalentID;}

	void SetNeededEnergy(USHORT NeededEnergy) {m_iNeededEnergy = NeededEnergy;}

	void SetFoodProd(short Food) {m_iFood = Food;}
	void SetIPProd(USHORT IP) {m_iIP = IP;}
	void SetEnergyProd(USHORT Energy) {m_iEnergy = Energy;}
	void SetSPProd(USHORT SP) {m_iSP = SP;}
	void SetFPProd(USHORT FP) {m_iFP = FP;}
	void SetTitanProd(USHORT Titan) {m_iTitan = Titan;}
	void SetDeuteriumProd(USHORT Deuterium) {m_iDeuterium = Deuterium;}
	void SetDuraniumProd(USHORT Duranium) {m_iDuranium = Duranium;}
	void SetCrystalProd(USHORT Crystal) {m_iCrystal = Crystal;}
	void SetIridiumProd(USHORT Iridium) {m_iIridium = Iridium;}
	void SetDeritiumProd(USHORT Deritium) {m_iDeritium = Deritium;}
	void SetCreditsProd(short Credits) {m_iCredits = Credits;}
	void SetMoralProd(short MoralProd) {m_iMoral = MoralProd;}
	void SetMoralProdEmpire(short MoralProdEmpire) {m_iMoralEmpire = MoralProdEmpire;}

	void SetFoodBoni(short FoodBoni) {m_iFoodBoni = FoodBoni;}
	void SetIndustryBoni(short IndustryBoni) {m_iIndustryBoni = IndustryBoni;}
	void SetEnergyBoni(short EnergyBoni) {m_iEnergyBoni = EnergyBoni;}
	void SetSecurityBoni(short SecurityBoni) {m_iSecurityBoni = SecurityBoni;}
	void SetResearchBoni(short ResearchBoni) {m_iResearchBoni = ResearchBoni;}
	void SetTitanBoni(short TitanBoni) {m_iTitanBoni = TitanBoni;}
	void SetDeuteriumBoni(short DeuteriumBoni) {m_iDeuteriumBoni = DeuteriumBoni;}
	void SetDuraniumBoni(short DuraniumBoni) {m_iDuraniumBoni = DuraniumBoni;}
	void SetCrystalBoni(short CrystalBoni) {m_iCrystalBoni = CrystalBoni;}
	void SetIridiumBoni(short IridiumBoni) {m_iIridiumBoni = IridiumBoni;}
	void SetDeritiumBoni(short DeritiumBoni) {m_iDeritiumBoni = DeritiumBoni;}
	void SetAllRessourceBoni(short AllRessourceBoni) {m_iAllRessourceBoni = AllRessourceBoni;}
	void SetCreditsBoni(short CreditsBoni) {m_iCreditsBoni = CreditsBoni;}

	void SetBioTechBoni(short BioTechBoni) {m_iBioTechBoni = BioTechBoni;}
	void SetEnergyTechBoni(short EnergyTechBoni) {m_iEnergyTechBoni = EnergyTechBoni;}
	void SetCompTechBoni(short CompTechBoni) {m_iCompTechBoni = CompTechBoni;}
	void SetPropulsionTechBoni(short PropulsionTechBoni) {m_iPropulsionTechBoni = PropulsionTechBoni;}
	void SetConstructionTechBoni(short ConstructionTechBoni) {m_iConstructionTechBoni = ConstructionTechBoni;}
	void SetWeaponTechBoni(short WeaponTechBoni) {m_iWeaponTechBoni = WeaponTechBoni;}

	void SetInnerSecurityBoni(short InnerSecurityBoni) {m_iInnerSecurityBoni = InnerSecurityBoni;}
	void SetEconomySpyBoni(short EconomySpyBoni) {m_iEconomySpyBoni = EconomySpyBoni;}
	void SetEconomySabotageBoni(short EconomySabotageBoni) {m_iEconomySabotageBoni = EconomySabotageBoni;}
	void SetResearchSpyBoni(short ResearchSpyBoni) {m_iResearchSpyBoni = ResearchSpyBoni;}
	void SetResearchSabotageBoni(short ResearchSabotageBoni) {m_iResearchSabotageBoni = ResearchSabotageBoni;}
	void SetMilitarySpyBoni(short MilitarySpyBoni) {m_iMilitarySpyBoni = MilitarySpyBoni;}
	void SetMilitarySabotageBoni(short MilitarySabotageBoni) {m_iMilitarySabotageBoni = MilitarySabotageBoni;}

	void SetShipYard(BOOLEAN TrueOrFalse) {m_bShipYard = TrueOrFalse;}
	void SetBuildableShipTypes(SHIP_SIZE::Typ nSize) {m_iBuildableShipSizes = nSize;}
	void SetShipYardSpeed(short speed) {m_iShipYardSpeed = speed;}
	void SetBarrack(BOOLEAN TrueOrFalse) {m_bBarrack = TrueOrFalse;}
	void SetBarrackSpeed(short speed) {m_iBarrackSpeed = speed;}
	void SetHitpoints(USHORT hitpoints) {m_iHitPoints = hitpoints;}
	void SetShieldPower(USHORT power) {m_iShieldPower = power;}
	void SetShieldPowerBoni(short powerboni) {m_iShieldPowerBoni = powerboni;}
	void SetShipDefend(USHORT defend) {m_iShipDefend = defend;}
	void SetShipDefendBoni(short defendboni) {m_iShipDefendBoni = defendboni;}
	void SetGroundDefend(USHORT defend) {m_iGroundDefend = defend;}
	void SetGroundDefendBoni(short defendboni) {m_iGroundDefendBoni = defendboni;}
	void SetScanPower(USHORT power) {m_iScanPower = power;}
	void SetScanPowerBoni(short powerboni) {m_iScanPowerBoni = powerboni;}
	void SetScanRange(BYTE Range) {m_iScanRange = Range;}
	void SetScanRangeBoni(short Rangeboni) {m_iScanRangeBoni = Rangeboni;}
	void SetShipTraining(short training) {m_iShipTraining = training;}
	void SetTroopTraining(short training) {m_iTroopTraining = training;}
	void SetResistance(short value) {m_iResistance = value;}
	void SetAddedTradeRoutes(BYTE addedRoutes) {m_iAddedTradeRoutes = addedRoutes;}
	void SetIncomeOnTradeRoutes(short income) {m_iIncomeOnTradeRoutes = income;}
	void SetShipRecycling(short recycling) {m_iShipRecycling = recycling;}
	void SetBuildingBuildSpeed(short speed) {m_iBuildingBuildSpeed = speed;}
	void SetUpdateBuildSpeed(short speed) {m_iUpdateBuildSpeed = speed;}
	void SetShipBuildSpeed(short speed) {m_iShipBuildSpeed = speed;}
	void SetTroopBuildSpeed(short speed) {m_iTroopBuildSpeed = speed;}

	void SetWorker(BOOLEAN TrueOrFalse) {m_bWorker = TrueOrFalse;}
	void SetNeverReady(BOOLEAN TrueOrFalse) {m_bNeverReady = TrueOrFalse;}
	void SetAllwaysOnline(BOOLEAN TrueOrFalse) {m_bAllwaysOnline = TrueOrFalse;}

	void SetResourceDistributor(BYTE res, BOOLEAN bIs) { m_bResourceDistributor[res] = bIs; }
	void SetNeededSystems(short nNumber) { m_iNeededSystems = nNumber; }

	// sonstige Funktionen
	/**
	 * Diese Funktion erstellt einen String über die komplette Produktion des Gebäudes. Übergeben wird dabei
	 * die Anzahl dieser Gebäude.
	 */
	CString GetProductionAsString(USHORT number = 1) const;

	/// Funktion gibt zurück, ob das Gebäude mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param pResearch Forschungsobjekt der Rasse
	/// @return Wahrheitswert
	bool IsBuildingBuildableNow(const BYTE reserachLevels[6]) const;

private:
	// Die laufende Nummer (ID) des Gebäudes
	USHORT m_iRunningNumber;

	// Rasse wer Gebäude bauen kann
	BYTE m_iOwnerOfBuilding;

	// Der Name des Gebäudes
	CString m_strBuildingName;

	// Die Gebäudebeschreibung
	CString m_strDescription;

	// Ist das Gebäude upgradebar
	BOOLEAN m_bUpgradeable;

	// Der Name der zum Gebäude gehörenden Grafikdatei
	CString m_strGraphikFileName;

	// maximale Anzahl des Gebäudes die wir gebaut haben dürfen im System
	NumberOfID m_MaxInSystem;

	// maximale Anzahl des Gebäudes die wir in allen Systemen gebaut haben müssen
	NumberOfID m_MaxInEmpire;

	// Nur baubar im Heimatsystem der jeweiligen Rasse
	BOOLEAN m_bOnlyHomePlanet;

	// Nur baubar auf eigenen Kolonien bzw. auch im Heimatsystem
	BOOLEAN m_bOnlyOwnColony;

	// Nur baubar in einem System einer Minorrace (Mitgliedschaft)
	BOOLEAN m_bOnlyMinorRace;

	// Nur baubar in einem gegnerischen eroberten System
	BOOLEAN m_bOnlyTakenSystem;

	// Nur baubar in System mit diesem Namen. Z.B. Spezialgebäude der Minorraces sollen ja nur in ihrem Heimat-
	// system baubar sein.
	CString m_strOnlyInSystemWithName;

	// minimale Bevölkerungsvoraussetzung im System in Milliarden
	USHORT m_iMinHabitants;

	// minimale Anzahl des Gebäudes die wir gebaut haben müssen im System
	NumberOfID m_MinInSystem;

	// minimale Anzahl des Gebäudes die wir in allen Systemen gebaut haben müssen
	NumberOfID m_MinInEmpire;

	// Wirklich nur die Rasse der das Gebäude gehört darf dieses bauen, z.B. wenn Planet erobert wurde und
	// Eroberer bestimmte Gebäude nicht bauen darf, obwohl diese schon stehen
	BOOLEAN m_bOnlyRace;

	// Die Planetentypen auf denen wir das Gebäude bauen können
	int m_PlanetTypes;

	// Die benötigten Forschungsstufen für den Bau des Gebäudes
	BYTE m_iBioTech;
	BYTE m_iEnergyTech;
	BYTE m_iCompTech;
	BYTE m_iPropulsionTech;
	BYTE m_iConstructionTech;
	BYTE m_iWeaponTech;

	// Die benötigten Rohstoffe für den Bau des Gebäudes
	USHORT m_iNeededIndustry;
	USHORT m_iNeededTitan;
	USHORT m_iNeededDeuterium;
	USHORT m_iNeededDuranium;
	USHORT m_iNeededCrystal;
	USHORT m_iNeededIridium;
	USHORT m_iNeededDeritium;

	// Die RunningNumber (ID) des Vorgängers des Gebäudes. Wird für alle Updates benötigt.
	USHORT m_iPredecessorID;

	// Gebäudeäquivalent zu den anderen Rassen. Darin wird die RunningNumber des Gebäudeäquivalents gespeichert.
	USHORT m_iBuildingEquivalent[7];

	// Die nötige Energie zum Betrieb des Gebäudes
	USHORT m_iNeededEnergy;

	// Ist das Gebäude online?
	BOOLEAN m_bAllwaysOnline;

	// Produktion -> überprüfen ob diese auch USHORT bleiben können?
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
	USHORT m_iDeritium;
	short m_iCredits;
	short m_iMoral;
	short m_iMoralEmpire;

	// Boni / Mali, das sind alles Prozentangaben zu irgendwas
	// Produktionsboni
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
	short m_iDeritiumBoni;
	short m_iAllRessourceBoni;
	short m_iCreditsBoni;

	// Forschungstechboni
	short m_iBioTechBoni;
	short m_iEnergyTechBoni;
	short m_iCompTechBoni;
	short m_iPropulsionTechBoni;
	short m_iConstructionTechBoni;
	short m_iWeaponTechBoni;

	// Sicherheitsboni
	short m_iInnerSecurityBoni;
	short m_iEconomySpyBoni;
	short m_iEconomySabotageBoni;
	short m_iResearchSpyBoni;
	short m_iResearchSabotageBoni;
	short m_iMilitarySpyBoni;
	short m_iMilitarySabotageBoni;

	// sonstige Informationen
	BOOLEAN m_bShipYard;
	SHIP_SIZE::Typ m_iBuildableShipSizes;
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

	// benötigt das Gebäude Arbeiter?
	BOOLEAN m_bWorker;

	// ist das Gebäude niemals fertig (z.B. ein Tribunal)
	BOOLEAN m_bNeverReady;

	// new in ALPHA5
	BOOLEAN m_bResourceDistributor[RESOURCES::DERITIUM + 1];	///< Ressourcenverteiler (Nahrung, Titan, Deuterium, Duranium, Kristalle, Iridium, Deritium)
	short m_iNeededSystems;							///< Mindestanzahl eigener Systeme um Gebäude bauen zu können
};

typedef CArray<CBuildingInfo,CBuildingInfo> BuildingInfoArray;	// Das dynamische Feld, welches alle Gebäude speichern soll
