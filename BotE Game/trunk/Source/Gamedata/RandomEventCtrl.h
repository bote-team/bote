#pragma once

class CShipMap;
class CMajor;
class CMinor;

/// Singleton-Klasse zum Berechnen der Random Events (Random Event Manager)
class CRandomEventCtrl
{
private:
	/// Konstruktor private, damit man sich keine Instanzen holen kann.
	CRandomEventCtrl(void);

	/// Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
	CRandomEventCtrl(const CRandomEventCtrl& cc);

	enum GLOBALEVENTTYPE
	{
		GLOBALEVENTSYSTEM,
		GLOBALEVENTRESEARCH,
		GLOBALEVENTMINOR
	};

	enum SYSTEMEVENTTYPE
	{
		SYSTEMEVENTMORALBOOST,// Moral bonus
		SYSTEMEVENTMORALMALUS,// Moral malus
		SYSTEMEVENTPLANETMOVEMENT,
		SYSTEMEVENTDEMOGRAPHIC
	};
	enum EXPLOREEVENTTYPE
	{
		ALIENTEC,
		EVENTSHIPXP
	};

public:
	/// Standarddestruktor
	~CRandomEventCtrl(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CRandomEventCtrl* GetInstance(void);

	/// Liest aus der Ini-Datei, ob Zufallsereignisse aktiviert sind
	/// @return <code>true</code> wenn Zufallsereignisse aktiviert, sonst <code>false</code>
	bool IsActivated() const { return m_bIsActivated; }

	///Berechnet Randomevents für die gegebene Majorrace
	void CalcEvents(CMajor* pRace);

	///Berechnet Events wenn ein Sektor erkundet wird
	void CalcExploreEvent(const CPoint& ko, CMajor* pRace, CShipMap* ships);

	/// Funktion berechnet Schiffsevents
	void CalcShipEvents() const;

private:
	// Für Random Events die nur ein System betreffen
	// @return true in case an event happened
	bool SystemEvent(const CPoint &ko, CMajor* pRace);

	//Für globale Events
	void GlobalEventResearch(CMajor* pRace);
	void GlobalEventMinor(CMajor* pRace, CMinor* pMinor);

	// Attribute
	const unsigned m_uiGlobalProb;//0-100
	const unsigned m_uiProbPerSystem;//0-1000
	const unsigned m_uiProbPerMinor;//0-1000

	bool m_bIsActivated;
};
