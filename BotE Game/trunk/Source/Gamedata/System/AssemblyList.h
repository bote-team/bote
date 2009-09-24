/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include "BuildingInfo.h"
#include "ShipInfo.h"
#include "TroopInfo.h"
#include "ResourceRoute.h"

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
	// Funktion gibt den Bauauftrag an der Stelle "pos" in der Bauliste zur�ck
	short GetAssemblyListEntry(USHORT pos) const {return m_iEntry[pos];}
	
	// Nachfolgende Funktionen geben die noch verbleibenden Kosten des Auftrages in der Bauliste zur�ck
	int GetNeededIndustryInAssemblyList(USHORT entry) const {return m_iNeededIndustryInAssemblyList[entry];}
	UINT GetNeededTitanInAssemblyList(USHORT entry) const {return m_iNeededTitanInAssemblyList[entry];}
	UINT GetNeededDeuteriumInAssemblyList(USHORT entry) const {return m_iNeededDeuteriumInAssemblyList[entry];}
	UINT GetNeededDuraniumInAssemblyList(USHORT entry) const {return m_iNeededDuraniumInAssemblyList[entry];}
	UINT GetNeededCrystalInAssemblyList(USHORT entry) const {return m_iNeededCrystalInAssemblyList[entry];}
	UINT GetNeededIridiumInAssemblyList(USHORT entry) const {return m_iNeededIridiumInAssemblyList[entry];}
	UINT GetNeededDilithiumInAssemblyList(USHORT entry) const {return m_iNeededDilithiumInAssemblyList[entry];}

	/// Funktion gibt die jeweiligen verbleibenden Kosten f�r einen Bauauftrag zur�ck.
	UINT GetNeededResourceInAssemblyList(USHORT entry, BYTE res) const;

	// Nachfolgende Funktionen geben die Kosten f�r einen Bauauftrag zur�ck
	UINT GetNeededIndustryForBuild() const {return m_iNeededIndustryForBuild;}
	UINT GetNeededTitanForBuild() const {return m_iNeededTitanForBuild;}
	UINT GetNeededDeuteriumForBuild() const {return m_iNeededDeuteriumForBuild;}
	UINT GetNeededDuraniumForBuild() const {return m_iNeededDuraniumForBuild;}
	UINT GetNeededCrystalForBuild() const {return m_iNeededCrystalForBuild;}
	UINT GetNeededIridiumForBuild() const {return m_iNeededIridiumForBuild;}
	UINT GetNeededDilithiumForBuild() const {return m_iNeededDilithiumForBuild;}

	/// Funktion gibt die jeweiligen Kosten f�r einen Bauauftrag zur�ck.
	UINT GetNeededResourceForBuild(BYTE res) const;

	// Funktion gibt zur�ck, ob das Geb�ude in der Bauliste gekauft wurde
	BOOLEAN GetWasBuildingBought() const {return m_bWasBuildingBought;}
	
	// Funktion gibt die kompletten Kaufkosten des Bauauftrages zur�ck
	int GetBuildCosts() const {return m_iBuildCosts;}
	
	// zum Schreiben der Membervariablen
	// Setzt ob ein Bauauftrag gekauft wurde auf wahr oder falsch
	void SetWasBuildingBought(BOOLEAN TrueOrFalse) {m_bWasBuildingBought = TrueOrFalse;}

// sonstige Funktionen
	// Funktion berechnet die ben�tigten Rohstoffe. �bergeben wird die Information des gew�nschten Geb�udes bzw. des
	// gew�nschten Schiffes oder der gew�nschten Truppe, der RunningNumber des Bauauftrages, einen Zeiger auf alle
	// vorhandenen Geb�ude in dem System und ein Zeiger auf die Forschung
	void CalculateNeededRessources(CBuildingInfo* buildingInfo, CShipInfo* shipInfo, CTroopInfo* troopInfo,
		BuildingArray* m_Buildings, int RunningNumber, CResearchInfo* ResearchInfo, float modifier = 1.0f);

	// Funktion berechnet die Kosten f�r jedes Update in der Bauliste. Wird in der NextRound() Funktion aufgerufen.
	// Weil in der Zwischenzeit h�tte der Spieler ein Geb�ude bauen k�nnen, das die Kosten mancher Updates ver�ndern
	// k�nnte. �bergeben wird ein Zeiger auf das Feld aller Geb�udeinformationen, ein Zeiger auf alle stehenden
	// Geb�ude im System und ein Zeiger auf die Forschungsinformation.
	void CalculateNeededRessourcesForUpdate(BuildingInfoArray* follower, BuildingArray* m_Buildings, CResearchInfo* ResearchInfo);

	// Funktion	macht einen Eintrag in der Bauliste, daf�r wird die RunningNumber des Auftrages �bergeben.
	// z.B. 1 = primitive Farm, -2 = Upgrade primitive Farm zu Typ 1 Automatikfarm. Wenn ein Update gemacht werden
	// soll mu� auch die RunningNumber des Vorg�ngers �bergeben werden.
	// Nachdem wir diese Funktion aufgerufen haben und ein TRUE zur�ckbekommen haben m�ssen wir die Funktion 
	// CalculateVariables() aufgerufen! (Weil wir wenn die Handelsg�ter aus der Bauliste verschwinden, sofort das
	// neue Latinum angezeigt werden soll.)
	BOOLEAN MakeEntry(int runningNumber, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT]);

	// Funktion berechnet die Kosten des Bauauftrags, wenn man dieses sofort kaufen will. Die Kosten des Sofortkaufes
	// sind von den Marktpreisen abh�ngig, daher m�ssen diese �bergeben werden.
	void CalculateBuildCosts(USHORT resPrices[5]);

	// Funktion setzt die noch restlichen Baukosten auf 1 und sagt, dass wir jetzt was gekauft haben. Wenn wir kaufen
	// k�nnen bestimmt die Fkt "CalculateBuildCosts()". Diese Fkt. immer vorher aufrufen. Die Latinumkosten werden
	// zur�ckgegeben. �bergeben wird das aktuelle Latinum des Imperiums.
	int BuyBuilding(int EmpiresLatinum);

	// Die Funktion berechnet was nach dem Tick noch an verbleibender Industrieleistung aufzubringen ist.
	// Ist der Bauauftrag fertig gibt die Funktion ein TRUE zur�ck. Wenn wir ein TRUE zur�ckbekommen, m�ssen
	// wir direkt danach die Funktion ClearAssemblyList() aufrufen!
	BOOLEAN CalculateBuildInAssemblyList(USHORT m_iIndustryProd);

	// Funktion l�scht einen Eintrag aus der Bauliste, wenn das Geb�ude fertig wurde oder wir den ersten
	// Eintrag manuell l�schen m�chten. Nach Aufruf dieser Funktion mu� unbedingt die Funktion
	// CalculateVariables() aufgerufen werden.
	void ClearAssemblyList(const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT]);

	// Ordnet die Bauliste so, dass keine leeren Eintr�ge in der Mitte vorkommen k�nnen. Wird z.B. aufgerufen,
	// nachdem wir einen Auftrag aus der Bauliste entfernt haben. Darf aber nicht aufgerufen werden, wenn wir
	// den 0. Eintrag entfernen, dann m�ssen wir ClearAssemblyList() aufrufen, weil diese Funktion die n�tigen
	// Rohstoffe gleich mit abzieht.
	void AdjustAssemblyList(short entry);

	// Resetfunktion f�r die Klasse CAssemblyList
	void Reset();

private:
// private Funktionen
	/// Diese Funktion entfernt die ben�tigten Ressourcen aus dem lokalen Lager des Systems und falls Ressourcenrouten
	/// bestehen auch die Ressourcen in den Startsystemen der Route. Aber nur falls dies auch notwendig sein sollte.
	void RemoveResourcesFromStorages(USHORT entry, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT], CArray<CPoint>* routesFrom);

// Attribute
	// Der Auftrag in der Bauliste.
	short m_iEntry[ALE];
	
	// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
	int	  m_iNeededIndustryInAssemblyList[ALE];	// ben�tigte IP die wir noch ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededTitanInAssemblyList[ALE];	// ben�tigtes Titan, das wir ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededDeuteriumInAssemblyList[ALE];// ben�tigtes Deuterium, das wir ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededDuraniumInAssemblyList[ALE];	// ben�tigtes Duranium, das wir ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededCrystalInAssemblyList[ALE];	// ben�tigtes Crystal, das wir ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededIridiumInAssemblyList[ALE];	// ben�tigtes Iridium, das wir ben�tigen um das Projekt fertig zu stellen
	UINT m_iNeededDilithiumInAssemblyList[ALE];// ben�tigtes Dilithium, das wir ben�tigen um das Projekt fertig zu stellen

	// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau ben�tigt werden
	UINT m_iNeededIndustryForBuild;			// ben�tigte IP zum Bauen des Geb�udes/Updates
	UINT m_iNeededTitanForBuild;				// ben�tigtes Titan zum Bauen des Geb�udes/Updates
	UINT m_iNeededDeuteriumForBuild;			// ben�tigtes Deuterium zum Bauen des Geb�udes/Updates
	UINT m_iNeededDuraniumForBuild;			// ben�tigtes Duranium zum Bauen des Geb�udes/Updates
	UINT m_iNeededCrystalForBuild;				// ben�tigtes Crystal zum Bauen des Geb�udes/Updates
	UINT m_iNeededIridiumForBuild;				// ben�tigtes Iridium zum Bauen des Geb�udes/Updates
	UINT m_iNeededDilithiumForBuild;			// ben�tigtes Dilithium zum Bauen des Geb�udes/Updates

	// Wurde das Geb�ude gekauft in dieser Runde gekauft
	BOOLEAN   m_bWasBuildingBought;			
	
	// Die Baukosten eines Auftrages
	int    m_iBuildCosts;	
};