/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include "BuildingInfo.h"
#include "Ships\ShipInfo.h"
#include "Troops\TroopInfo.h"
#include "Galaxy\ResourceRoute.h"

class CSystem;
class CAssemblyList : public CObject
{
public:
	DECLARE_SERIAL (CAssemblyList)
	
	// Standardkonstruktor
	CAssemblyList(void);

	// Destruktor
	virtual ~CAssemblyList(void);

	// Kopierkonstruktor
	CAssemblyList(const CAssemblyList & rhs);
	
	// Zuweisungsoperatur
	CAssemblyList & operator=(const CAssemblyList &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	// Funktion gibt den Bauauftrag an der Stelle "pos" in der Bauliste zurück
	short GetAssemblyListEntry(USHORT pos) const {return m_iEntry[pos];}
	
	// Nachfolgende Funktionen geben die noch verbleibenden Kosten des Auftrages in der Bauliste zurück
	int GetNeededIndustryInAssemblyList(USHORT entry) const {return m_iNeededIndustryInAssemblyList[entry];}
	UINT GetNeededTitanInAssemblyList(USHORT entry) const {return m_iNeededTitanInAssemblyList[entry];}
	UINT GetNeededDeuteriumInAssemblyList(USHORT entry) const {return m_iNeededDeuteriumInAssemblyList[entry];}
	UINT GetNeededDuraniumInAssemblyList(USHORT entry) const {return m_iNeededDuraniumInAssemblyList[entry];}
	UINT GetNeededCrystalInAssemblyList(USHORT entry) const {return m_iNeededCrystalInAssemblyList[entry];}
	UINT GetNeededIridiumInAssemblyList(USHORT entry) const {return m_iNeededIridiumInAssemblyList[entry];}
	UINT GetNeededDilithiumInAssemblyList(USHORT entry) const {return m_iNeededDilithiumInAssemblyList[entry];}

	/// Funktion gibt die jeweiligen verbleibenden Kosten für einen Bauauftrag zurück.
	UINT GetNeededResourceInAssemblyList(USHORT entry, BYTE res) const;

	// Nachfolgende Funktionen geben die Kosten für einen Bauauftrag zurück
	UINT GetNeededIndustryForBuild() const {return m_iNeededIndustryForBuild;}
	UINT GetNeededTitanForBuild() const {return m_iNeededTitanForBuild;}
	UINT GetNeededDeuteriumForBuild() const {return m_iNeededDeuteriumForBuild;}
	UINT GetNeededDuraniumForBuild() const {return m_iNeededDuraniumForBuild;}
	UINT GetNeededCrystalForBuild() const {return m_iNeededCrystalForBuild;}
	UINT GetNeededIridiumForBuild() const {return m_iNeededIridiumForBuild;}
	UINT GetNeededDilithiumForBuild() const {return m_iNeededDilithiumForBuild;}

	/// Funktion gibt die jeweiligen Kosten für einen Bauauftrag zurück.
	UINT GetNeededResourceForBuild(BYTE res) const;

	// Funktion gibt zurück, ob das Gebäude in der Bauliste gekauft wurde
	BOOLEAN GetWasBuildingBought() const {return m_bWasBuildingBought;}
	
	// Funktion gibt die kompletten Kaufkosten des Bauauftrages zurück
	int GetBuildCosts() const {return m_iBuildCosts;}
	
	// zum Schreiben der Membervariablen
	// Setzt ob ein Bauauftrag gekauft wurde auf wahr oder falsch
	void SetWasBuildingBought(BOOLEAN TrueOrFalse) {m_bWasBuildingBought = TrueOrFalse;}

// sonstige Funktionen
	// Funktion berechnet die benötigten Rohstoffe. Übergeben wird die Information des gewünschten Gebäudes bzw. des
	// gewünschten Schiffes oder der gewünschten Truppe, der RunningNumber des Bauauftrages, einen Zeiger auf alle
	// vorhandenen Gebäude in dem System und ein Zeiger auf die Forschung
	void CalculateNeededRessources(CBuildingInfo* buildingInfo, CShipInfo* shipInfo, CTroopInfo* troopInfo,
		BuildingArray* m_Buildings, int RunningNumber, CResearchInfo* ResearchInfo, float modifier = 1.0f);

	// Funktion berechnet die Kosten für jedes Update in der Bauliste. Wird in der NextRound() Funktion aufgerufen.
	// Weil in der Zwischenzeit hätte der Spieler ein Gebäude bauen können, das die Kosten mancher Updates verändern
	// könnte. Übergeben wird ein Zeiger auf das Feld aller Gebäudeinformationen, ein Zeiger auf alle stehenden
	// Gebäude im System und ein Zeiger auf die Forschungsinformation.
	void CalculateNeededRessourcesForUpdate(BuildingInfoArray* follower, BuildingArray* m_Buildings, CResearchInfo* ResearchInfo);

	// Funktion	macht einen Eintrag in der Bauliste, dafür wird die RunningNumber des Auftrages übergeben.
	// z.B. 1 = primitive Farm, -2 = Upgrade primitive Farm zu Typ 1 Automatikfarm.
	// Nachdem wir diese Funktion aufgerufen haben und ein TRUE zurückbekommen haben müssen wir die Funktion 
	// CalculateVariables() aufgerufen! (Weil wir wenn die Handelsgüter aus der Bauliste verschwinden, sofort das
	// neue Latinum angezeigt werden soll.)
	BOOLEAN MakeEntry(int runningNumber, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT], bool bOnlyTest = false);

	// Funktion berechnet die Kosten des Bauauftrags, wenn man dieses sofort kaufen will. Die Kosten des Sofortkaufes
	// sind von den Marktpreisen abhängig, daher müssen diese übergeben werden.
	void CalculateBuildCosts(USHORT resPrices[5]);

	// Funktion setzt die noch restlichen Baukosten auf 1 und sagt, dass wir jetzt was gekauft haben. Wenn wir kaufen
	// können bestimmt die Fkt "CalculateBuildCosts()". Diese Fkt. immer vorher aufrufen. Die Latinumkosten werden
	// zurückgegeben. Übergeben wird das aktuelle Latinum des Imperiums.
	int BuyBuilding(int EmpiresLatinum);

	// Die Funktion berechnet was nach dem Tick noch an verbleibender Industrieleistung aufzubringen ist.
	// Ist der Bauauftrag fertig gibt die Funktion ein TRUE zurück. Wenn wir ein TRUE zurückbekommen, müssen
	// wir direkt danach die Funktion ClearAssemblyList() aufrufen!
	BOOLEAN CalculateBuildInAssemblyList(USHORT m_iIndustryProd);

	// Funktion löscht einen Eintrag aus der Bauliste, wenn das Gebäude fertig wurde oder wir den ersten
	// Eintrag manuell löschen möchten. Nach Aufruf dieser Funktion muß unbedingt die Funktion
	// CalculateVariables() aufgerufen werden.
	void ClearAssemblyList(const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT]);

	// Ordnet die Bauliste so, dass keine leeren Einträge in der Mitte vorkommen können. Wird z.B. aufgerufen,
	// nachdem wir einen Auftrag aus der Bauliste entfernt haben. Darf aber nicht aufgerufen werden, wenn wir
	// den 0. Eintrag entfernen, dann müssen wir ClearAssemblyList() aufrufen, weil diese Funktion die nötigen
	// Rohstoffe gleich mit abzieht.
	void AdjustAssemblyList(short entry);

	// Resetfunktion für die Klasse CAssemblyList
	void Reset();

private:
// private Funktionen
	/// Diese Funktion entfernt die benötigten Ressourcen aus dem lokalen Lager des Systems und falls Ressourcenrouten
	/// bestehen auch die Ressourcen in den Startsystemen der Route. Aber nur falls dies auch notwendig sein sollte.
	void RemoveResourceFromStorage(BYTE res, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT], CArray<CPoint>* routesFrom);

// Attribute
	// Der Auftrag in der Bauliste.
	short m_iEntry[ALE];
	
	// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
	int	  m_iNeededIndustryInAssemblyList[ALE];	// benötigte IP die wir noch benötigen um das Projekt fertig zu stellen
	UINT m_iNeededTitanInAssemblyList[ALE];	// benötigtes Titan, das wir benötigen um das Projekt fertig zu stellen
	UINT m_iNeededDeuteriumInAssemblyList[ALE];// benötigtes Deuterium, das wir benötigen um das Projekt fertig zu stellen
	UINT m_iNeededDuraniumInAssemblyList[ALE];	// benötigtes Duranium, das wir benötigen um das Projekt fertig zu stellen
	UINT m_iNeededCrystalInAssemblyList[ALE];	// benötigtes Crystal, das wir benötigen um das Projekt fertig zu stellen
	UINT m_iNeededIridiumInAssemblyList[ALE];	// benötigtes Iridium, das wir benötigen um das Projekt fertig zu stellen
	UINT m_iNeededDilithiumInAssemblyList[ALE];// benötigtes Dilithium, das wir benötigen um das Projekt fertig zu stellen

	// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau benötigt werden
	UINT m_iNeededIndustryForBuild;			// benötigte IP zum Bauen des Gebäudes/Updates
	UINT m_iNeededTitanForBuild;				// benötigtes Titan zum Bauen des Gebäudes/Updates
	UINT m_iNeededDeuteriumForBuild;			// benötigtes Deuterium zum Bauen des Gebäudes/Updates
	UINT m_iNeededDuraniumForBuild;			// benötigtes Duranium zum Bauen des Gebäudes/Updates
	UINT m_iNeededCrystalForBuild;				// benötigtes Crystal zum Bauen des Gebäudes/Updates
	UINT m_iNeededIridiumForBuild;				// benötigtes Iridium zum Bauen des Gebäudes/Updates
	UINT m_iNeededDilithiumForBuild;			// benötigtes Dilithium zum Bauen des Gebäudes/Updates

	// Wurde das Gebäude gekauft in dieser Runde gekauft
	BOOLEAN   m_bWasBuildingBought;			
	
	// Die Baukosten eines Auftrages
	int    m_iBuildCosts;	
};
