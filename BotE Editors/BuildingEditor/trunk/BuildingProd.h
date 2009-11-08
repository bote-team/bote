/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// BuildingProd.h: Schnittstelle für die Klasse CBuildingProd.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "building.h"

class CBuildingProd : public CBuilding
{
public:
	DECLARE_SERIAL (CBuildingProd)

	/// Standardkonstruktor
	CBuildingProd(void);
	
	/// Standarddestruktor
	~CBuildingProd(void);

	/// Kopierkonstruktor
	CBuildingProd(const CBuildingProd & rhs);

	/// Zuweisungsoperatur
	CBuildingProd & operator=(const CBuildingProd &);

	/// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
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
	USHORT GetDilithiumProd() const {return m_iDilithium;}
	USHORT GetResourceProd(BYTE res) const;
	short GetLatinum() const {return m_iLatinum;}
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
	short GetDilithiumBoni() const {return m_iDilithiumBoni;}
	short GetAllRessourcesBoni() const {return m_iAllRessourceBoni;}
	short GetLatinumBoni() const {return m_iLatinumBoni;}

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
	BYTE GetMaxBuildableShipSize() const {return m_iBuildableShipSizes;}
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

	// neu für Alpha5
	BOOLEAN GetResourceDistributor(BYTE res) const { return m_bResourceDistributor[res]; }	
	
	/*
	USHORT GetScannerInterferer() const {return m_iInterfereScanner;}
	USHORT* GetNeededResBoni() {return m_iNeededResBoni;}
	short* GetStorageModificator() {return m_iStorageModificator;}
	BYTE GetHabitat() const {return m_byHabitat;}
	BYTE GetCamouflageLvl() const {return m_byCamouflageLvl;}
	BOOLEAN* GetTechLink() {return m_bTechLink;}
	USHORT GetNeededCredits() const {return m_iNeededCredits;}
	*/

	// zum Schreiben der Membervariablen
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
	void SetDilithiumProd(USHORT Dilithium) {m_iDilithium = Dilithium;}
	void SetLatinumProd(short Latinum) {m_iLatinum = Latinum;}
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
	void SetDilithiumBoni(short DilithiumBoni) {m_iDilithiumBoni = DilithiumBoni;}
	void SetAllRessourceBoni(short AllRessourceBoni) {m_iAllRessourceBoni = AllRessourceBoni;}
	void SetLatinumBoni(short LatinumBoni) {m_iLatinumBoni = LatinumBoni;}
	
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
	void SetBuildableShipTypes(BYTE type) {m_iBuildableShipSizes = type;}
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

	// neu für Alpha5
	void SetResourceDistributor(BYTE res, BOOLEAN bIs) { m_bResourceDistributor[res] = bIs; }	
	/*
	void SetScannerInterferer(USHORT interfereScanner) {m_iInterfereScanner = interfereScanner;}
	void SetHabitat(BYTE habitat) {m_byHabitat = habitat;}
	void SetCamouflageLvl(BYTE camouflage) {m_byCamouflageLvl = camouflage;}
	void SetNeededCredits(USHORT neededCredits) {m_iNeededCredits = neededCredits;}
	*/	

protected:
	// Die nötige Energie zum Betrieb des Gebäudes
	USHORT m_iNeededEnergy;	

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
	USHORT m_iDilithium;
	short m_iLatinum;
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
	short m_iDilithiumBoni;
	short m_iAllRessourceBoni;	
	short m_iLatinumBoni;

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
	BYTE m_iBuildableShipSizes;
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

	// neu für Alpha5
	BOOLEAN m_bResourceDistributor[DILITHIUM + 1];	///< Ressourcenverteiler (Nahrung, Titan, Deuterium, Duranium, Kristalle, Iridium, Deritium)
		
	/*
	// Aufgehoben für die Zukunft
	USHORT m_iInterfereScanner;					///< Störsender
	USHORT m_iNeededResBoni[8];					///< benötigter Ressourcenbonus im System (Titan bis Energie)
	short m_iStorageModificator[FOOD + 1];		///< Lagermodifiktator (Titan bis Nahrung)
	BYTE m_byHabitat;							///< Habitatmultiplikator (0 kein Habitat, 1 == normal, 2 doppelte Größe usw.))
	BYTE m_byCamouflageLvl;						///< Tarneigenschaft des Gebäudes (wichtig für Geheimdienst und Bombardements)
	BOOLEAN m_bTechLink[6];						///< Verbindung zur Techstufe, welche die Produktion des Gebäudes um die Tech multipliziert
	USHORT m_iNeededCredits;					///< benötige Credits zum Bau des Gebäudes
	*/
};
