#pragma once

class CShipMap;
class CMajor;
class CMinor;

enum GLOBALEVENTTYPE
{
	GLOBALEVENTSYSTEM = 0,
	GLOBALEVENTRESEARCH = 1,
	GLOBALEVENTMINOR = 2
};

enum SYSTEMEVENTTYPE
{
	SYSTEMEVENTMORALBOOST=0,// Moral bonus
	SYSTEMEVENTMORALMALUS=1,// Moral malus
	SYSTEMEVENTPLANETMOVEMENT=2,
	SYSTEMEVENTDEMOGRAPHIC=3
};
enum EXPLOREEVENTTYPE
{
	ALIENTEC=0,
	EVENTSHIPXP=1
};
//#define GLOBALEVENTTYPES 2
//#define SYSTEMEVENTTYPES 3
//#define EXPLOREEVENTTYPES 2
//Klasse zum berchnen der Random Events(Random Event Manager)
class CReManager : public CObject
{
	public:
	//DECLARE_SERIAL (CReManager)
	//Konstruktor/Destruktor
	CReManager(void);
	~CReManager(void);

	///Berechnet Randomevents für die gegebene Majorrace
	void CalcEvents(CMajor* pRace);

	///Berechnet Events wenn ein Sektor erkundet wird
	void CalcExploreEvent(const CPoint& ko, CMajor* pRace, CShipMap* ships);

private:

	//Für Random Events die nur ein System betreffen
	bool SystemEvent(const CPoint &ko, CMajor* pRace);

	//Für globale Events
	void GlobalEventResearch(CMajor* pRace);
	void GlobalEventMinor(CMajor* pRace, CMinor* pMinor);

	const unsigned m_uiGlobalProb;//0-100
	const unsigned m_uiProbPerSystem;//0-1000
	const unsigned m_uiProbPerMinor;//0-1000


};
