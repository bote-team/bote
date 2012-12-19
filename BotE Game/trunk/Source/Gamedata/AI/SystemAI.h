/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Options.h"

// forward declaration
class CBotf2Doc;
class CMajor;
class CBuildingInfo;
class CSystemAI
{
public:
	/// Konstruktor
	CSystemAI(CBotf2Doc* pDoc);

	/// Destruktor
	~CSystemAI(void);

	/// Diese Funktion führt die Berechnungen für die künstliche Intelligenz in einem System aus und macht
	/// alle möglichen Einträge und Veränderungen. Als Paramter wird dafür die Koordinate <code>ko</code>
	/// des Systems übergeben.
	void ExecuteSystemAI(CPoint ko);

private:
	//AI does not know how to use troops. Most of the time they are only harmfull (reduce moral, maintenance costs).
	//So just remove any start troops.
	//A human would pack them into a transporter and use it up to build an outpost.
	void KillTroops();

	/// Diese Funktion kauft unter Umständen den aktuellen Bauauftrag. Somit kommt der Ausbaue schneller voran.
	void PerhapsBuy();

	/// Diese Funktion legt die Prioritäten an, mit welcher ein bestimmtes Arbeitergebäude gebaut werden soll.
	void CalcPriorities();


	bool CheckMoral(const CBuildingInfo& bi, bool build) const;

	/// Diese Funktion wählt ein zu bauendes Gebäude aus der Liste der baubaren Gebäude. Es werden nur Gebäude
	/// ausgewählt, welche in die Prioritätenliste passen. Der Rückgabewert ist die ID des Bauauftrages.
	/// Wird <code>0</code> zurückgegeben, so wurde kein Gebäude gefunden, welches in das Anforderungsprofil passt.
	int ChooseBuilding();

	/// Diese Funktion wählt ein zu bauendes Schiff aus der Liste der baubaren Schiffe. Der Rückgabewert ist die ID
	/// des Schiffes (also größer 10000!). Wird <code>0</code> zurückgegeben, so wurde kein baubares Schiffe gefunden.
	/// Übergeben wird dafür die Priorität <code>prio</code> des zu bauenden Schiffes (mittels Funktion <function>
	/// GetShipBuildPrios</function> ermitteln) und Wahrheitswertr, in denen steht welcher Schiffstyp gebaut werden soll.
	int ChooseShip(int prio, BOOLEAN chooseCombatship, BOOLEAN chooseColoship, BOOLEAN chooseTransport);

	/// Funktion überprüft ob das Gebäude mit der ID <code>id</code> in die Bauliste gesetzt werden kann.
	/// Wenn es in die Bauliste gesetzt werden konnte gibt die Funktion ein <code>TRUE</code> zurück.
	BOOLEAN MakeEntryInAssemblyList(short id);

	/// Diese Funktion besetzt die Gebäude mit Arbeitern.
	void AssignWorkers();

	/// Diese Funktion versucht Arbeiter aus Bereichen abzuziehen. Dabei werden aber keine Arbeiter aus dem Nahrungs-
	/// und Energiebereich abgezogen. Die Funktion berechnet die neue Arbeiterverteilung, auch die freien Arbeiter, und
	/// gibt ein <code>TRUE</code> zurück, wenn Arbeiter abgezogen werden konnten. Ansonsten gibt sie <code>FALSE</code>
	/// zurück.
	BOOLEAN DumpWorker();

	/// Funktion reißt überflüssige Nahrungs- und Energiegebäude im System ab.
	void ScrapBuildings();

	/// Diese Funktion berechnet die neue Nahrungs-, Industrie- und Energieproduktion im System. Sie sollte aufgerufen
	/// werden, wenn Arbeiter aus diesen Gebieten verändert wurden. Dabei werden aber alle anderen Produktionen
	/// gelöscht.
	void CalcProd();

	/// Diese Funktion legt möglicherweise eine Handelsroute zu einem anderen System an. Dadurch wird halt auch die
	/// Beziehung zu Minorraces verbessert.
	void ApplyTradeRoutes();

	/// Diese Funktion berechnet die Priorität, mit welcher ein gewisser Schiffstyp gebaut werden soll. Übergeben werden
	/// dabei drei Referenzen auf Booleanvariablen, in welcher dann ein <code>TRUE</code> steht wurde ausgewählt.
	/// @return ist die Priorität, mit welcher der Schiffstyp gebaut werden soll.
	int GetShipBuildPrios(BOOLEAN &chooseCombatship, BOOLEAN &chooseColoship, BOOLEAN &chooseTransport);

	/// Funktion überprüft, ob für einen bestimmten Arbeitertyp auch ein Gebäude in der Liste der baubaren
	/// Gebäude exisitiert. Wenn dies nicht der Fall ist, so ist auch die Priorität solch ein Gebäude zu bauen
	/// gleich Null.
	/// @param nWorker Arbeitertyp (FOOD_WORKER, INDUSTRY_WORKER usw.)
	/// @return <code>TRUE</code> wenn ein Gebäude baubar ist, ansonsten <code>FALSE</code>
	bool CheckBuilding(WORKER::Typ nWorker) const;

	/// Funktion berechnet die Priorität der Nahrungsmittelproduktion im System
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Nahrungsmittelproduktion
	int GetFoodPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorität der Industrieproduktion im System
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Industrieproduktion
	int GetIndustryPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorität der Industrieproduktion im System
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Industrieproduktion
	int GetEnergyPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorität der Geheimdienstproduktion im System
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Geheimdienstproduktion
	int GetIntelPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorität der Forschungsproduktion im System
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Forschungsproduktion
	int GetResearchPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorität einer bestimmten Ressourcenproduktion im System
	/// @param nWorker Arbeiter für eine bestimmte Ressource
	/// @param dMaxHab maximale Bevölkerungszahl im System
	/// @return Priotität der Forschungsproduktion
	int GetResourcePrio(WORKER::Typ nWorker, double dMaxHab) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// Ein Zeiger auf das Document.
	CBotf2Doc* m_pDoc;

	/// Die Koordinate des Systems, in welchem gerade die KI-Berechnungen durchgeführt werden sollen
	CPoint m_KO;

	/// Die Prioritäten der einzelnen Arbeitergebäude
	int m_iPriorities[10];

	/// Was sollte am ehesten gebaut werden? 0 - Gebäude, 1 - Upgrade, 2 - Schiff, 3 - Truppen
	BYTE m_bBuildWhat;

	/// Wurden die Prioritäten berechnet, so nimmt diese Variable den Wert <code>TRUE</code> an. Ansonsten ist
	/// ihr Wert <code>FALSE</code>.
	BOOLEAN m_bCalcedPrio;

	/// auf diese Variable wird während der Berechnung die Majorrace gespeichert, der das System gehört.
	CMajor* m_pMajor;
};
