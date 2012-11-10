// botf2Doc.h : Schnittstelle der Klasse CBotf2Doc
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
//#include "vld.h"

#include "Ships\GenShipName.h"
#include "General\Statistics.h"
#include "Races\VictoryObserver.h"
#include "Remanager.h"
#include "System\GlobalBuildings.h"
#include "PeerData.h"
#include "SoundManager.h"
#include "IOData.h"
#include <vector>
#include "Ships/ShipArray.h"

// forward declaration
class CMainFrame;
class CNetworkHandler;
class CSector;
class CSystem;
class CGraphicPool;
class CRaceController;
class CMajor;
class CSectorAI;
class CAIPrios;

class CBotf2Doc : public CDocument, public network::CPeerData
{
protected: // Nur aus Serialisierung erzeugen
	DECLARE_DYNCREATE(CBotf2Doc)

	// Attribute
	CGraphicPool *m_pGraphicPool;		///< Objekt verwaltet Grafiken f�r BotE
	USHORT m_iRound;					///< aktuelle Rundenanzahl
	float m_fDifficultyLevel;			///< der Schwierigkeitsgrad eines Spiels
	float m_fStardate;					///< Startrek Sternzeit
	CPoint m_ptKO;						///< Koordinaten des aktuell angeklickten Sektors

	BYTE m_iShowWhichTechInView3;		///< Welche Tech soll in View3 angezeigt werden?
	short m_iShowWhichShipInfoInView3;	///< Welche Schiffsinfo soll in View 3 angezeigt werden

	std::vector<CSector> m_Sectors;
	std::vector<CSystem> m_Systems;

	//CSector** m_Sector/*[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT]*/;	///< Matrix von Sektoren der Klasse CSector anlegen
	//CSystem** m_System/*[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT]*/;	///< auf jeden Sektor ein potentielles System anlegen

	CGenShipName m_GenShipName;			///< Variable, die alle m�glichen Schiffsnamen beinhaltet

	CArray<CTroopInfo> m_TroopInfo;		// In diesem Feld werden alle Informationen zu den Truppen gespeichert
	ShipInfoArray m_ShipInfoArray;		// dynamisches Feld, in dem die ganzen Informationen zu den Schiffen gespeichert sind
	CShipArray m_ShipArray;				// dynamisches Feld, in das die ganzen Schiffe gespeichert werden
	BuildingInfoArray BuildingInfo;		// alle Geb�udeinfos zu allen Geb�uden im Spiel
	CGlobalBuildings m_GlobalBuildings;	// alle gebauten Geb�ude aller Rassen im Spiel
	short m_iNumberOfFleetShip;			// Das Schiff welches sozusagen die Flotte anf�hrt
	short m_iNumberOfTheShipInFleet;	// Nummber des Schiffes in der Flotte, wenn wir ein Flotte haben
	short m_NumberOfTheShipInArray;		// Hilfsvariable, mit der auf ein spezielles Schiff im Array zugekriffen werden kann

	CStatistics m_Statistics;			///< Statistikobjekt, in dem Statistiken des Spiels gespeichert sind

	CNetworkHandler *m_pNetworkHandler;
	bool m_bDataReceived;				///< hat der Client die Daten komplett vom Server erhalten
	bool m_bRoundEndPressed;			///< Wurde der Rundenendebutton gedr�ckt
	bool m_bDontExit;					///< hartes Exit verhindern, wenn Spiel beginnt
	bool m_bGameLoaded;					///< wurde im Dialog ein zu ladendes Spiel ausgew�hlt

	CArray<SNDMGR_MESSAGEENTRY> m_SoundMessages[7];	///< Die einzelnen Sprachmitteilungen zur neuen Runde
	USHORT m_iSelectedView[7];						///< Welche View soll in der MainView angezeigt werden? z.B. Galaxie oder System

	CRaceController* m_pRaceCtrl;		///< Rassencontroller f�r alle Rassen des Spiels
	CSectorAI*		 m_pSectorAI;		///< Informationen zu allen Sektoren, welche die KI ben�tigt.
	CAIPrios*		 m_pAIPrios;		///< zus�tzliche Priotit�ten, welche f�r die System-KI-Berechnung ben�tigt werden

	CVictoryObserver m_VictoryObserver;	///< �berwachung der Siegbedingungen

	CReManager m_RandomEventManager;    ///< Berechnet Random Events

public:
	// Operationen
public:
	// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CBotf2Doc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	/// Serialisiert die Daten, welche am Anfang des Spiels einmal gesendet werden m�ssen.
	void SerializeBeginGameData(CArchive& ar);

	/// Serialisiert die Daten f�r die <code>CNextRound</code>-Nachricht.
	void SerializeNextRoundData(CArchive &ar);

	/// Serialisiert die Daten f�r die <code>CEndOfRound</code>-Nachricht �ber das angegebene Volk.
	/// <code>race</code> wird beim Schreiben ignoriert.
	void SerializeEndOfRoundData(CArchive &ar, network::RACE race);
	//}}AFX_VIRTUAL

	/**
	 *  L�dt die angegebene Datei, deserialisiert die in der Datei enthaltenen Daten ins Dokument.
	 *  Ruft unmittelbar vor dem Deserialisieren <code>Reset()</code> auf. <code>Reset()</code> wird
	 *  nicht aufgerufen, wenn zuvor ein Fehler aufgetreten ist.
	 *  Aufbau eines Savegames:
	 *  <pre>
	 *  BYTE[]		"BotE"			Magic Number
	 *  UINT		version			Versionsnummer des Spielstandes
	 * 	UINT		size			L�nge von data
	 *  BYTE[]		data			size Bytes, komprimierte serialisierte Daten des Dokuments
	 *  </pre>
	 */
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	/**
	 * Speichert die serialisierten Daten des Dokuments komprimiert in einer Datei. Das Format des
	 * Savegames ist bei <code>OnOpenDocument()</code> angegeben. Existierende Dateien werden �berschrieben.
	 */
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	// Implementierung
	/// Standardkonstruktor
	CBotf2Doc(void);

	/// Standarddestruktor
	virtual ~CBotf2Doc(void);

	/// Funktion liefert einen Zeiger auf den GraphicPool f�r BotE.
	CGraphicPool* GetGraphicPool(void) {return m_pGraphicPool;}

	/// Funktion gibt einen Zeiger auf das Rahmenfenster (CMainFrame) zur�ck.
	/// @return Rahmenfenster MainFrame
	CMainFrame* GetMainFrame(void) const;

	/// Funktion liest die Ini-Datei neu ein und legt die Werte neu fest.
	void ResetIniSettings(void);

	//Loads the random seed from ini file and seeds rand() with it In case that OnlyIfDifferentThan != NULL,
	//sets the new seed only if the new one is different than the passed value.
	static void RandomSeed(const int* OnlyIfDifferentThan = NULL);

	/// Funktion gibt den Schwierigkeitsgrad des Spiels zur�ck.
	float GetDifficultyLevel(void) const {return m_fDifficultyLevel;}

	const CShip& GetShip(int number) const {
		return m_ShipArray.GetAt(number);
	}
	CShip& GetShip(int number) {
		return m_ShipArray.GetAt(number);
	}
	//returns the ship at m_NumberOfTheShipInArray
	const CShip& CurrentShip() const {
		return GetShip(m_NumberOfTheShipInArray);
	}
	//returns the ship at m_NumberOfTheShipInArray
	CShip& CurrentShip() {
		return GetShip(m_NumberOfTheShipInArray);
	}
	//return ship at m_iNumberOfFleetShip
	const CShip& FleetShip() const {
		return GetShip(m_iNumberOfFleetShip);
	}
	//return ship at m_iNumberOfFleetShip
	CShip& FleetShip() {
		return GetShip(m_iNumberOfFleetShip);
	}

	const CPoint& GetKO(void) const {return m_ptKO;}
	void SetKO(int x, int y);

	/// Funktion gibt die Koordinate des Hauptsystems einer Majorrace zur�ck.
	/// @param sMajor Rassen-ID
	/// @return Koordinate auf der Galaxiemap
	CPoint GetRaceKO(const CString& sMajorID) const;

	CSector& GetSector(int x, int y) {
		return m_Sectors.at(x+y*STARMAP_SECTORS_HCOUNT);
	}
	const CSector& GetSector(int x, int y) const {
		return m_Sectors.at(x+y*STARMAP_SECTORS_HCOUNT);
	}

	//returns sector at m_ptKO
	const CSector& CurrentSector() const {
		return GetSector(m_ptKO.x, m_ptKO.y);
	}
	//returns sector at m_ptKO
	CSector& CurrentSector() {
		return GetSector(m_ptKO.x, m_ptKO.y);
	}

	CSystem& GetSystem(int x, int y) {
		return m_Systems.at(x+y*STARMAP_SECTORS_HCOUNT);
	}
	const CSystem& GetSystem(int x, int y) const {
		return m_Systems.at(x+y*STARMAP_SECTORS_HCOUNT);
	}

	//returns system at m_ptKO
	const CSystem& CurrentSystem() const {
		return GetSystem(m_ptKO.x, m_ptKO.y);
	}
	//returns system at m_ptKO
	CSystem& CurrentSystem() {
		return GetSystem(m_ptKO.x, m_ptKO.y);
	}

	CSystem& GetSystemForSector(const CSector& s) {
		return GetSystem(s.GetKO().x, s.GetKO().y);
	}
	const CSystem& GetSystemForSector(const CSector& s) const {
		return GetSystem(s.GetKO().x, s.GetKO().y);
	}

	CBuildingInfo& GetBuildingInfo(int id) {ASSERT(id > 0); return BuildingInfo[id-1];}
	const CString& GetBuildingName(int id) const {ASSERT(id > 0); return BuildingInfo[id-1].GetBuildingName();}
	const CString& GetBuildingDescription(int id) const {ASSERT(id > 0); return BuildingInfo[id-1].GetBuildingDescription();}

	/// Funktion l�dt f�r die ausgew�hlte Spielerrasse alle Grafiken f�r die Views.
	void LoadViewGraphics(void);

	/// Funktion veranlasst die Views zu jeder neuen Runde ihr Aufgaben zu erledigen, welche zu jeder neuen Runde ausgef�hrt
	/// werden m�ssen. Es werden zum Beispiel Variablen wieder zur�ckgesetzt.
	void DoViewWorkOnNewRound(void);

	void PrepareData(void);						// generiert ein neues Spiel
	void NextRound(void);						// zur N�chsten Runde voranschreiten
	void ApplyShipsAtStartup(void);				// Die Schiffe zum Start anlegen, �bersichtshalber nicht alles in NewDocument
	void ApplyBuildingsAtStartup(void);			// Die Geb�ude zum Start in den Hauptsystemen anlegen
	void ApplyTroopsAtStartup(void);			// Die Truppen zu Beginn in den Hauptsystemen anlegen
	void ReadBuildingInfosFromFile(void);		// Die Infos zu den Geb�uden aus den Datein einlesen
	void ReadShipInfosFromFile(void);			// Die Infos zu den Schiffen aus der Datei einlesen
	void ReadTroopInfosFromFile(void);			// Die Infos zu den Truppen aus der Datei einlesen
	void BuildBuilding(USHORT id, const CPoint& KO);	// Das jeweilige Geb�ude bauen

	/// Funktion zum bauen des jeweiligen Schiffes in einem System.
	/// @param ID ID des Schiffes
	/// @param KO Sektorkoordinate des Schiffes
	/// @param sOwnerID Besitzer des Schiffes
	void BuildShip(int nID, const CPoint& KO, const CString& sOwnerID);

	/// Funktion zum L�schen des Schiffes aus dem Schiffsarray.
	/// @param nIndex Index des Schiffes im Array
	void RemoveShip(int nIndex);

	/// Funktion beachtet die erforschten Spezialforschungen einer Rasse und verbessert die
	/// Eigenschaften der �bergebenen Schiffes.
	/// @param pShip Schiff welches durch Spezialforschungen eventuell verbessert wird
	/// @param pShipOwner Zeiger auf den Besitzer des Schiffes
	void AddSpecialResearchBoniToShip(CShip* pShip, CMajor* pShipOwner) const;

	/// Funktion f�gt ein Schiff zur Liste der verlorenen Schiffe in der Schiffshistory ein.
	/// @param sEvent Ereignis warum Schiff weg/zerst�rt/verschwunden ist
	/// @param sStatus Status des Schiffes (meist zerst�rt)
	void AddToLostShipHistory(const CShip* pShip, const CString& sEvent, const CString& sStatus);

	/// Funktion generiert die Starmaps, so wie sie nach Rundenberechnung auch angezeigt werden k�nnen.
	/// @param sOnlyForRaceID wenn dieser Wert gesetzt, wird die Starmap nur f�r diese Rasse neu berechnet
	void GenerateStarmap(const CString& sOnlyForRaceID = "");

	/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
	void BuildTroop(BYTE ID, CPoint ko);

	USHORT GetCurrentRound() const {return m_iRound;}

	USHORT GetCurrentShipIndex(void) const {return m_NumberOfTheShipInArray;}
	USHORT GetNumberOfFleetShip(void) const {return m_iNumberOfFleetShip;}
	USHORT GetNumberOfTheShipInFleet(void) const {return m_iNumberOfTheShipInFleet;}
	void SetCurrentShipIndex(int NumberOfTheShipInArray);
	void SetNumberOfFleetShip(int NumberOfFleetShip);
	void SetNumberOfTheShipInFleet(int NumberOfTheShipInFleet);

	// new in ALPHA5

	/// Funktion gibt den Controller f�r die Rassen w�hrend eines BotE Spiels zur�ck.
	/// @return Zeiger auf Rassencontroller
	CRaceController* GetRaceCtrl(void) const {return m_pRaceCtrl;}

	/// Funktion gibt Zeiger auf das Statistikobjekt f�r BotE zur�ck.
	/// @return Zeiger auf Statistiken
	CStatistics* GetStatistics(void) {return &m_Statistics;}

	/// Funktion gibt einen Zeiger auf die Sektoren-KI zur�ck.
	CSectorAI* GetSectorAI(void) const {return m_pSectorAI;}

	/// Startkoordinaten der Hauptsysteme aller Majorraces.
	map<CString, pair<int, int> > m_mRaceKO;

	/// Funktion gibt das Feld mit den Schiffsinformationen zur�ck.
	/// @return Zeiger auf das Schiffsinformationsfeld
	ShipInfoArray* GetShipInfos(void) {return &m_ShipInfoArray;}

	/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zur�ck.
	/// @return Zeiger auf Majorrace-Rassenobjekt
	CString GetPlayersRaceID(void) const;

	bool m_bGameOver;	///< ist das Spiel

	// neu f�r Kampf
	set<CString> m_sCombatSectors;		///< Sektoren in denen diese Runde schon ein Kampf stattgefunden hat

	CPoint m_ptCurrentCombatSector;		///< aktueller Kampfsektor

	bool m_bCombatCalc;					///< es werden gerade die Kampfrunden berechnet

	COMBAT_ORDER::Typ m_nCombatOrder;	///< im Kampfmen� eingestellter Kampfbefehl

	bool m_bNewGame;					///< wird ein neues Spiel gestartet (bzw. geladen)

private:
	map<CString, COMBAT_ORDER::Typ> m_mCombatOrders; ///< alle Kampfbefehle der Clients (Autokampf, R�ckzug, Gru�...)

	map<CString, map<pair<int, int>, CPoint> >  m_mShipRetreatSectors;	///< R�ckzugsesktoren aller Schiffe nach allen K�mpfen

protected:
	// Private Funktionen die bei der NextRound Berechnung aufgerufen werden. Dadurch wird die NextRound Funktion
	// um einiges verkleinert

	/// Funktion gibt einen Zeiger auf die lokale Spielerrasse zur�ck.
	/// @return Zeiger auf Majorrace-Rassenobjekt
	CMajor* GetPlayersRace(void) const;

	/// Diese Funktion f�hrt allgemeine Berechnung durch, die immer zu Beginn der NextRound-Calculation stattfinden
	/// m�ssen. So werden z.B. alte Nachrichten gel�scht, die Statistiken berechnet usw..
	void CalcPreDataForNextRound();

	/// Diese Funktion berechnet den kompletten Systemangriff.
	void CalcSystemAttack();

	/// Diese Funktion berechnet alles im Zusammenhang mit dem Geheimdienst.
	void CalcIntelligence();

	/// Diese Funktion berechnet die Forschung eines Imperiums
	void CalcResearch();

	/// Diese Funktion berechnet die Auswirkungen von diplomatischen Angeboten und ob Minorraces Angebote an
	/// Majorraces abgeben.
	void CalcDiplomacy();

	/// Diese Funktion berechnet das Planetenwachstum, die Auftr�ge in der Bauliste und sonstige Einstellungen aus der
	/// alten Runde.

private:
	//helpers for CalcOldRoundData
	void UpdateGlobalBuildings(const CSystem& system);
protected:
	void CalcOldRoundData();

	/// Diese Funktion berechnet die Produktion der Systeme, was in den Baulisten gebaut werden soll und sonstige
	/// Daten f�r die neue Runde.
	void CalcNewRoundData();

public:
	/// Function puts a scanned square over the given sector in the middle, if range == 1, 9 sectors are affected
	/// for instance. Function calculates and sets the scan power values for each of these sectors.
	/// @param sector: The middle sector where the scan power affecting object is. This can be a ship,
	/// a scanning deteriorating anomaly, or a continuum scanner (or continuum scanner upgrade).
	/// @param range: range of the object
	/// @param power: scan power of the object
	/// @param race: race who gets these scan powers (can be a minor race)
	/// @param bBetterScanner: are we on a scanning improving anomaly (implies the scan power affecting object is a ship or base)
	/// @param patrolship: is this a patrolship (implies the scan power affecting object is a ship or base)
	/// @param anomaly: is the scan power affecting object a scanning deteriorating anomaly
	void PutScannedSquareOverSector(const CSector& sector, unsigned range, const int power,
		const CRace& race, bool bBetterScanner = false, bool patrolship = false, bool anomaly = false);
protected:
	/// Diese Funktion berechnet die kompletten Handelsaktivit�ten.
	void CalcTrade();

	/// Diese Funktion berechnet die Schiffsbefehle. Der Systemangriffsbefehl ist davon ausgenommen.
	void CalcShipOrders();

	/// Diese Funktion berechnet die Schiffsbewegung und noch weitere kleine Sachen im Zusammenhang mit Schiffen.
	void CalcShipMovement();

	bool IsShipCombat();

	/// Diese Funktion berechnet einen m�glichen Weltraumkampf und dessen Auswirkungen.
	/// @return <code>true</code> wenn ein Kampf stattgefunden hat, sonst <code>false</code>
	void CalcShipCombat();

	/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
	/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
	void CalcShipEffects();
private:
	//helper functions for CalcShipEffects()
	void CalcShipRetreat();
protected:
	/// Diese Funktion f�hrt allgemeine Berechnung durch, die immer zum Ende der NextRound-Calculation stattfinden m�ssen.
	void CalcEndDataForNextRound();
private:
	//helper functions for CalcContactNewRaces()
	void CalcContactClientWork(CMajor& Major, const CRace& ContactedRace);
	void CalcContactCommutative(CMajor& Major, CRace& ContactedRace, const CPoint& p);
	void CalcContactShipToMajorShip(CRace& Race, const CSector& sector, const CPoint& p);
	void CalcContactMinor(CMajor& Major, const CSector& sector, const CPoint& p);
protected:
	/// Diese Funktion �berpr�ft, ob neue Rassen kennengelernt wurden.
	void CalcContactNewRaces();

	/// Funktion berechnet, ob zuf�llig Alienschiffe ins Spiel kommen.
	void CalcRandomAlienEntities();

	/// Funktion berechnet Auswirkungen von Alienschiffe auf Systeme, �ber denen sie sich befinden.
	void CalcAlienShipEffects();

	/// Funktion generiert die Galaxiemap inkl. der ganzen Systeme und Planeten zu Beginn eines neuen Spiels.
	void GenerateGalaxy();

private:
	void AllocateSectorsAndSystems();
	void SerializeSectorsAndSystems(CArchive& ar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CBotf2Doc)
	// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
	//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
