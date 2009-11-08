/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// BuildingInfo.h: Schnittstelle für die Klasse CBuildingInfo.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "BuildingProd.h"

struct NumberOfID
{
	USHORT RunningNumber;
	USHORT Number;
};

class CBuildingInfo : public CBuildingProd
{

public:
	DECLARE_SERIAL (CBuildingInfo)
	// Standardkonstruktor
	CBuildingInfo(void);
	
	// Destruktor
	virtual ~CBuildingInfo(void);

	// Kopierkonstruktor
	CBuildingInfo(const CBuildingInfo & rhs);

	// Zuweisungsoperatur
	CBuildingInfo & operator=(const CBuildingInfo &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	BYTE GetOwnerOfBuilding() const {return m_iOwnerOfBuilding;}

	const CString& GetBuildingName(BOOLEAN language = 0) const {return m_strBuildingName[language];}
	
	const CString& GetBuildingDescription(BOOLEAN language = 0) const {return m_strDescription[language];}

	BOOLEAN GetUpgradeable() const {return m_bUpgradeable;}

	const CString& GetGraphikFileName() const {return m_strGraphikFileName;}
	
	NumberOfID GetMaxInSystem() const {return m_MaxInSystem;}

	NumberOfID GetMaxInEmpire() const {return m_MaxInEmpire;}
	
	BOOLEAN GetOnlyHomePlanet() const {return m_bOnlyHomePlanet;}

	BOOLEAN GetOnlyOwnColony() const {return m_bOnlyOwnColony;}

	BOOLEAN GetOnlyMinorRace() const {return m_bOnlyMinorRace;}

	BOOLEAN GetOnlyTakenSystem() const {return m_bOnlyTakenSystem;}

	const CString& GetOnlyInSystemWithName() const {return m_strOnlyInSystemWithName;}

	USHORT GetMinHabitants() const {return m_iMinHabitants;}

	NumberOfID GetMinInSystem() const {return m_MinInSystem;}

	NumberOfID GetMinInEmpire() const {return m_MinInEmpire;}

	BOOLEAN GetOnlyRace() const {return m_bOnlyRace;}

	BOOLEAN GetPlanetTypes(USHORT i) const {return m_bPlanetTypes[i];}

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
	USHORT GetNeededDilithium() const {return m_iNeededDilithium;}
	USHORT GetNeededResource(BYTE res) const;
	
	USHORT GetPredecessorID() const {return m_iPredecessorID;}

	USHORT GetEquivalent(USHORT race) const {return m_iBuildingEquivalent[race];}

	// zum Schreiben der Membervariablen
	void SetOwnerOfBuilding(BYTE owner) {m_iOwnerOfBuilding = owner;}
	
	void SetBuildingName(CString BuildingName, BOOLEAN language = 0) {m_strBuildingName[language] = BuildingName;}

	void SetBuildingDescription(CString Description, BOOLEAN language = 0) {m_strDescription[language] = Description;}
	
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

	void SetOnlyInSystemWithName(CString Name) {m_strOnlyInSystemWithName = Name;}

	void SetMinHabitants(USHORT min) {m_iMinHabitants = min;}

	void SetMinInSystem(USHORT number, USHORT runningNumber) {m_MinInSystem.Number = number; 
		m_MinInSystem.RunningNumber = runningNumber;}

	void SetMinInEmpire(USHORT number, USHORT runningNumber) {m_MinInEmpire.Number = number; 
		m_MinInEmpire.RunningNumber = runningNumber;}

	void SetOnlyRace(BOOLEAN TrueOrFalse) {m_bOnlyRace = TrueOrFalse;}

	void SetPlanetTypes(USHORT i, BOOLEAN TrueOrFalse) {m_bPlanetTypes[i] = TrueOrFalse;}

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
	void SetNeededDilithium(USHORT NeededDilithium) {m_iNeededDilithium = NeededDilithium;}

	void SetPredecessor(USHORT RunningNumber) {m_iPredecessorID = RunningNumber;}

	void SetEquivalent(BYTE race, USHORT equivalentID) {m_iBuildingEquivalent[race] = equivalentID;}
	
private:
	// Rasse wer Gebäude bauen kann
	BYTE m_iOwnerOfBuilding;
	
	// Der Name des Gebäudes
	CString m_strBuildingName[2];
	
	// Die Gebäudebeschreibung
	CString m_strDescription[2];

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
	BOOLEAN m_bPlanetTypes[20];

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
	USHORT m_iNeededDilithium;
	
	// Die RunningNumber (ID) des Vorgängers des Gebäudes. Wird für alle Updates benötigt.
	USHORT m_iPredecessorID;
	
	// Gebäudeäquivalent zu den anderen Rassen. Darin wird die RunningNumber des Gebäudeäquivalents gespeichert.
	USHORT m_iBuildingEquivalent[7];
};

typedef CArray<CBuildingInfo, CBuildingInfo> BuildingInfoArray;	// Das dynamische Feld, welches alle Gebäude speichern soll