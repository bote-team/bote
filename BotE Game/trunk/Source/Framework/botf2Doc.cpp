// botf2Doc.cpp : Implementierung der Klasse CBotf2Doc
//

#include "stdafx.h"
#include "resources.h"
#include "botf2.h"
#include "botf2Doc.h"
#include "LZMA_BotE.h"
#include "GalaxyMenuView.h"
#include "SmallInfoView.h"
#include "MenuChooseView.h"
#include "MainBaseView.h"
#include "PlanetBottomView.h"
#include "NetworkHandler.h"
#include "MainFrm.h"
#include "IniLoader.h"
#include "ImageStone/ImageStone.h"
#include "IOData.h"
#include "General/ResourceManager.h"

#include "Races\RaceController.h"
#include "Races\DiplomacyController.h"
#include "Ships\Combat.h"
#include "System\AttackSystem.h"
#include "Intel\IntelCalc.h"
#include "Remanager.h"
#include "Sanity.h"
#include "Test.h"

#include "AI\AIPrios.h"
#include "AI\SectorAI.h"
#include "AI\SystemAI.h"
#include "AI\ShipAI.h"
#include "AI\CombatAI.h"
#include "AI\ResearchAI.h"

#include "Galaxy\Anomaly.h"
#include "Ships/Ships.h"
#include "NewRoundDataCalculator.h"
#include "OldRoundDataCalculator.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc

IMPLEMENT_DYNCREATE(CBotf2Doc, CDocument)

BEGIN_MESSAGE_MAP(CBotf2Doc, CDocument)
	//{{AFX_MSG_MAP(CBotf2Doc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CBotf2Doc::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CBotf2Doc::OnUpdateFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc Konstruktion/Destruktion

#pragma warning(push)
#pragma warning (disable:4351)
CBotf2Doc::CBotf2Doc() :
	m_ptKO(0, 0),
	m_ptCurrentCombatSector(-1, -1),
	m_bCombatCalc(false),
	m_bDataReceived(false),
	m_bDontExit(false),
	m_bGameLoaded(false),
	m_bGameOver(false),
	m_bNewGame(true),
	m_bRoundEndPressed(false),
	m_fDifficultyLevel(1.0f),
	m_fStardate(121000.0f),
	m_iRound(1),
	m_iSelectedView(),
	m_iShowWhichShipInfoInView3(0),
	m_iShowWhichTechInView3(0),
	m_nCombatOrder(COMBAT_ORDER::NONE)
{
	resources::pDoc = this;

	//Init MT with single log file
	CString sLogPath = CIOData::GetInstance()->GetLogPath();
	const CCommandLineParameters* const clp = resources::pClp;
	const std::set<const std::string>& domains = clp->LogDomains();
	MT::CMyTrace::Init(sLogPath,
		domains.empty() ? std::set<const std::string>(MT::DEFAULT_LOG_DOMAINS,
			MT::DEFAULT_LOG_DOMAINS + sizeof(MT::DEFAULT_LOG_DOMAINS) / sizeof(*MT::DEFAULT_LOG_DOMAINS))
			: domains, clp->ActiveDomains());
	MT::CMyTrace::SetLevel(clp->LogLevel());

	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	CResourceManager::Init();

	m_pGraphicPool = new CGraphicPool(CIOData::GetInstance()->GetAppPath() + "Graphics\\");

	m_pRaceCtrl = new CRaceController();

	m_pAIPrios = new CAIPrios(this);
	m_pSectorAI= new CSectorAI(this);

	m_pNetworkHandler = new CNetworkHandler(this);
	server.AddServerListener(m_pNetworkHandler);
	client.AddClientListener(m_pNetworkHandler);
}
#pragma warning(pop)

CBotf2Doc::~CBotf2Doc()
{
	resources::pDoc = NULL;

	if (m_pGraphicPool)
		delete m_pGraphicPool;
	if (m_pRaceCtrl)
		delete m_pRaceCtrl;
	if (m_pAIPrios)
		delete m_pAIPrios;
	if (m_pSectorAI)
		delete m_pSectorAI;

	m_pGraphicPool	= NULL;
	m_pRaceCtrl		= NULL;
	m_pAIPrios		= NULL;
	m_pSectorAI		= NULL;

	m_ShipInfoArray.RemoveAll();

	if (m_pNetworkHandler)
	{
		server.RemoveServerListener(m_pNetworkHandler);
		client.RemoveClientListener(m_pNetworkHandler);
		delete m_pNetworkHandler;
		m_pNetworkHandler = NULL;
	}

	for(std::vector<CSector>::iterator sector = m_Sectors.begin();
		sector != m_Sectors.end(); ++sector) {
		sector->Reset();
	}
	for(std::vector<CSystem>::iterator system = m_Systems.begin();
		system != m_Systems.end(); ++system) {
		system->ResetSystem();;
	}

	// statische Variablen der Starmap freigeben
	CStarmap::DeleteStatics();

	// stop MT
	MYTRACE_DEINIT;
}

BOOL CBotf2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	m_bDataReceived				= false;
	m_bDontExit					= false;
	m_bGameLoaded				= false;
	m_fStardate					= 121000.0f;
	m_bCombatCalc				= false;
	m_bNewGame					= true;

	CIniLoader* pIni = CIniLoader::GetInstance();
	ASSERT(pIni);

	bool bHardwareSound;
	pIni->ReadValue("Audio", "HARDWARESOUND", bHardwareSound);
	CSoundManager::GetInstance()->Init(!bHardwareSound);
	RandomSeed();

	// Standardwerte setzen
	m_ptKO = CPoint(0,0);
	m_bRoundEndPressed			= false;
	m_iShowWhichTechInView3		= 0;
	m_iShowWhichShipInfoInView3 = 0;
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		m_iSelectedView[i] = START_VIEW;

	return TRUE;
}

/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zurück.
/// @return Zeiger auf Majorrace-Rassenobjekt
CString CBotf2Doc::GetPlayersRaceID(void) const
{
	return m_pRaceCtrl->GetMappedRaceID((network::RACE)client.GetClientRace());
}

/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zurück.
/// @return Zeiger auf Majorrace-Rassenobjekt
CMajor* CBotf2Doc::GetPlayersRace(void) const
{
	// zuerst muss eine Netzwerknummer, also RACE1 bis RACE6 (1-6)
	// auf eine bestimmte Rassen-ID gemappt werden. Dies ist dann
	// die Rassen-ID.
	CString s = m_pRaceCtrl->GetMappedRaceID((network::RACE)client.GetClientRace());
	CMajor* pPlayersRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(s));

	return pPlayersRace;
}

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc Serialisierung

void CBotf2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
		ar << m_iRound;
		ar << m_fStardate;
		//not here...MYTRACE("general")(MT::LEVEL_INFO, "Stardate: %f", m_fStardate);
		ar << m_ptKO;
		ar << STARMAP_SECTORS_HCOUNT;
		ar << STARMAP_SECTORS_VCOUNT;
		// Zeitstempel
		MYTRACE("logging")(MT::LEVEL_INFO, _T("Time: %s"), CTime(time(NULL)).Format("%c"));


		// Hauptrassen-Koordinaten speichern
		ar << m_mRaceKO.size();
		for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); ++it)
			ar << it->first << it->second.first << it->second.second;

		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		{
			ar << m_iSelectedView[i];
		}

		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		m_ShipMap.Serialize(ar);

		ar<< m_TroopInfo.GetSize();//Truppen in Savegame speichern
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_TroopInfo.GetAt(i).Serialize(ar);

		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
			ar << CTrade::GetMonopolOwner(j);

		map<CString, short>* mMoralMap = CSystemProd::GetMoralProdEmpireWide();
		ar << mMoralMap->size();
		for (map<CString, short>::const_iterator it = mMoralMap->begin(); it != mMoralMap->end(); ++it)
			ar << it->first << it->second;
	}
	else
	{
		int number;
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		ar >> m_iRound;
		ar >> m_fStardate;
		ar >> m_ptKO;
		ar >> STARMAP_SECTORS_HCOUNT;
		ar >> STARMAP_SECTORS_VCOUNT;
		AllocateSectorsAndSystems();

		// Hauptrassen-Koordinaten laden
		m_mRaceKO.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			pair<int, int> value;
			ar >> key;
			ar >> value.first;
			ar >> value.second;
			m_mRaceKO[key] = value;
		}

		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		{
			ar >> m_iSelectedView[i];
		}

		ar >> number;
		m_ShipInfoArray.RemoveAll();
		m_ShipInfoArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		m_ShipMap.Serialize(ar);
		ar >> number;
		m_TroopInfo.RemoveAll();
		m_TroopInfo.SetSize(number);
		for (int i = 0; i<number; i++)
			m_TroopInfo.GetAt(i).Serialize(ar);

		// Gebäudeinfos werden nun beim Laden neu eingelesen
		BuildingInfo.RemoveAll();
		this->ReadBuildingInfosFromFile();

		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
		{
			CString sOwnerID;
			ar >> sOwnerID;
			CTrade::SetMonopolOwner(j, sOwnerID);
		}
		CSystemProd::GetMoralProdEmpireWide()->clear();
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			(*CSystemProd::GetMoralProdEmpireWide())[key] = value;
		}
	}

	SerializeSectorsAndSystems(ar);

	CMoralObserver::SerializeStatics(ar);

	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	m_Statistics.Serialize(ar);

	m_pRaceCtrl->Serialize(ar);

	if (ar.IsLoading())
	{
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			it->second->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
	}

	m_VictoryObserver.Serialize(ar);
}

void CBotf2Doc::SerializeSectorsAndSystems(CArchive& ar)
{
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			CSector& sector = GetSector(x,y);
			CSystem& system = GetSystem(x,y);
			if (ar.IsLoading())
				system.ResetSystem();
			sector.Serialize(ar);
			if (sector.GetSunSystem())
			{
				if (sector.GetOwnerOfSector() != ""
					|| sector.GetColonyOwner() != ""
					|| sector.GetMinorRace())
					system.Serialize(ar);
			}
		}
	}
}

/// Serialisiert die Daten, welche am Anfang des Spiels einmal gesendet werden müssen.
void CBotf2Doc::SerializeBeginGameData(CArchive& ar)
{
	m_bDataReceived = false;
	// senden auf Serverseite
	if (ar.IsStoring())
	{
		//Kartengröße
		ar << STARMAP_SECTORS_HCOUNT;
		ar << STARMAP_SECTORS_VCOUNT;
		// Hauptrassen-Koordinaten senden
		ar << m_mRaceKO.size();
		for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); ++it)
			ar << it->first << it->second.first << it->second.second;

		// Gebäudeinformationen
		ar << BuildingInfo.GetSize();
		for (int i = 0; i < BuildingInfo.GetSize(); i++)
			BuildingInfo.GetAt(i).Serialize(ar);

		// Truppeninformationen
		ar << m_TroopInfo.GetSize();
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_TroopInfo.GetAt(i).Serialize(ar);
	}
	// Empfangen auf Clientseite
	else
	{
		// Kartengröße und Initialisierung
		ar >> STARMAP_SECTORS_HCOUNT;
		ar >> STARMAP_SECTORS_VCOUNT;
		AllocateSectorsAndSystems();
		// Hauptrassen-Koordinaten empfangen
		m_mRaceKO.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			pair<int, int> value;
			ar >> key;
			ar >> value.first;
			ar >> value.second;
			m_mRaceKO[key] = value;
		}

		int number;
		ar >> number;
		BuildingInfo.RemoveAll();
		BuildingInfo.SetSize(number);
		for (int i = 0; i < number; i++)
			BuildingInfo.GetAt(i).Serialize(ar);

		ar >> number;
		m_TroopInfo.RemoveAll();
		m_TroopInfo.SetSize(number);
		for (int i = 0; i < number; i++)
			m_TroopInfo.GetAt(i).Serialize(ar);
	}

	CMoralObserver::SerializeStatics(ar);
}

void CBotf2Doc::SerializeNextRoundData(CArchive &ar)
{
	m_bDataReceived = false;
	// Daten der nächsten Runde serialisieren; auf Server-Seite senden, auf Client-Seite empfangen
	if (ar.IsStoring())
	{
		ar << m_bCombatCalc;
		// Wenn es einen Kampf gab, dann Schiffe übertragen
		if (m_bCombatCalc)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Server is sending CombatData to client...\n");
			// Sektor des Kampfes übertragen
			ar << m_ptCurrentCombatSector;
			m_ShipMap.SerializeNextRoundData(ar, m_ptCurrentCombatSector);
			return;
		}

		MYTRACE("general")(MT::LEVEL_INFO, "Server is sending NextRoundData to client...\n");
		// Server-Dokument
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
		ar << m_iRound;
		ar << m_fStardate;
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			ar << m_iSelectedView[i];
		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		m_ShipMap.Serialize(ar);

		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
			ar << CTrade::GetMonopolOwner(j);

		map<CString, short>* mMoralMap = CSystemProd::GetMoralProdEmpireWide();
		ar << mMoralMap->size();
		for (map<CString, short>::const_iterator it = mMoralMap->begin(); it != mMoralMap->end(); ++it)
			ar << it->first << it->second;
	}
	else
	{
		ar >> m_bCombatCalc;
		if (m_bCombatCalc)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Client is receiving CombatData from server...\n");
			ar >> m_ptCurrentCombatSector;
			m_ShipMap.SerializeNextRoundData(ar, m_ptCurrentCombatSector);
			return;
		}

		// Client-Dokument
		MYTRACE("general")(MT::LEVEL_INFO, "Client is receiving NextRoundData from server...\n");
		int number;
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		ar >> m_iRound;
		ar >> m_fStardate;
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			ar >> m_iSelectedView[i];
		ar >> number;
		m_ShipInfoArray.RemoveAll();
		m_ShipInfoArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		m_ShipMap.Serialize(ar);
		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
		{
			CString sOwnerID;
			ar >> sOwnerID;
			CTrade::SetMonopolOwner(j, sOwnerID);
		}
		CSystemProd::GetMoralProdEmpireWide()->clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			(*CSystemProd::GetMoralProdEmpireWide())[key] = value;
		}
	}

	SerializeSectorsAndSystems(ar);

	m_pRaceCtrl->Serialize(ar);

	for (int i = HUMAN; i <= DOMINION; i++)
		m_SoundMessages[i].Serialize(ar);


	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	m_Statistics.Serialize(ar);
	m_VictoryObserver.Serialize(ar);

	if (ar.IsLoading())
	{
		CSmallInfoView::SetPlanet(NULL);
		//GenerateStarmap();

		m_bGameOver = false;
		CMajor* pPlayer = GetPlayersRace();
		// bekommt der Client hier keine Spielerrasse zurück, so ist er ausgeschieden
		ASSERT(pPlayer);
		if (pPlayer == NULL)
		{
			AfxMessageBox("Fatal Error ... exit game now");
			m_bGameOver = true;
			return;
		}

		network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());

		// Ausgehend vom Pfad des Schiffes den Sektoren mitteilen, das durch sie ein Schiff fliegt
		for(CShipMap::const_iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y) {
			if (y->second->GetOwnerOfShip() == pPlayer->GetRaceID()) {
				const CArray<Sector>& path = *y->second->GetPath();
				for (int i = 0; i < path.GetSize(); i++)
					GetSector(path.GetAt(i).x, path.GetAt(i).y).AddShipPathPoints(1);
			}
		}
		// Sprachmeldungen an den Soundmanager schicken
		CSoundManager* pSoundManager = CSoundManager::GetInstance();
		ASSERT(pSoundManager);
		pSoundManager->ClearMessages();
		for (int i = 0; i < m_SoundMessages[client].GetSize(); i++)
		{
			SNDMGR_MESSAGEENTRY* entry = &m_SoundMessages[client].GetAt(i);
			pSoundManager->AddMessage(entry->nMessage, entry->nRace, entry->nPriority, entry->fVolume);
		}

		// Systemliste der Imperien erstellen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			it->second->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
	}
	MYTRACE("general")(MT::LEVEL_INFO, "... serialization of NextRoundData succesfull\n");
}

void CBotf2Doc::SerializeEndOfRoundData(CArchive &ar, network::RACE race)
{
	if (ar.IsStoring())
	{
		if (m_bCombatCalc)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Client %d sending CombatData to server...\n", race);

			// nur Informationen über die Taktik der Schiffe bzw. die Taktik des Kampfes senden
			ar << m_nCombatOrder;
			return;
		}

		MYTRACE("general")(MT::LEVEL_INFO, "Client %d sending EndOfRoundData to server...\n", race);
		CMajor* pPlayer = GetPlayersRace();
		// Client-Dokument
		// Anzahl der eigenen Schiffsinfoobjekte ermitteln
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pPlayer->GetRaceShipNumber())
				m_ShipInfoArray.GetAt(i).Serialize(ar);

		m_ShipMap.SerializeEndOfRoundData(ar, pPlayer->GetRaceID());

		vector<CPoint> vSystems;
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if (GetSector(x, y).GetSunSystem() && GetSystem(x, y).GetOwnerOfSystem() == pPlayer->GetRaceID())
					vSystems.push_back(CPoint(x, y));
			}
		}
		ar << vSystems.size();
		for (size_t i = 0; i < vSystems.size(); i++)
		{
			ar << vSystems[i];
			GetSystem(vSystems[i].x, vSystems[i].y).Serialize(ar);
		}

		network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());
		pPlayer->Serialize(ar);
		// aktuelle View mit zum Server senden
		ar << m_iSelectedView[client];
	}
	else
	{
		// vom Client gespielte Majorrace-ID ermitteln
		CString sMajorID = m_pRaceCtrl->GetMappedRaceID(race);

		if (m_bCombatCalc)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Server receiving CombatData from client %d...\n", race);

			// Informationen über die Taktik der Schiffe bzw. die Taktik des Kampfes empfangen
			int nOrder;
			ar >> nOrder;
			COMBAT_ORDER::Typ nCombatOrder = (COMBAT_ORDER::Typ)nOrder;
			if (nCombatOrder != COMBAT_ORDER::NONE)
				m_mCombatOrders[sMajorID] = nCombatOrder;

			return;
		}

		MYTRACE("general")(MT::LEVEL_INFO, "Server receiving EndOfRoundData from client %d...\n", race);
		// Server-Dokument
		CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sMajorID));
		ASSERT(pMajor);
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				m_ShipInfoArray.GetAt(i).Serialize(ar);
				ASSERT(m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber());
			}

		m_ShipMap.SerializeEndOfRoundData(ar, sMajorID);

		int number = 0;
		ar >> number;
		for (int i = 0; i < number; i++)
		{
			CPoint p;
			ar >> p;
			GetSystem(p.x, p.y).Serialize(ar);
		}

		pMajor->Serialize(ar);
		ar >> m_iSelectedView[race];
	}
	MYTRACE("general")(MT::LEVEL_INFO, "... serialization of RoundEndData succesfull\n", race);
}

/// Funktion liest die Ini-Datei neu ein und legt die Werte neu fest.
void CBotf2Doc::ResetIniSettings(void)
{
	CIniLoader* pIni = CIniLoader::GetInstance();
	ASSERT(pIni);

	CString difficulty = "EASY";
	pIni->ReadValue("General", "DIFFICULTY", difficulty);
	difficulty.MakeUpper();
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: DIFFICULTY: %s", difficulty);
	if (difficulty == "BABY")
		m_fDifficultyLevel			= 1.5f;
	else if (difficulty == "EASY")
		m_fDifficultyLevel			= 1.0f;
	else if (difficulty == "NORMAL")
		m_fDifficultyLevel			= 0.5f;
	else if (difficulty == "HARD")
		m_fDifficultyLevel			= 0.33f;
	else if (difficulty == "IMPOSSIBLE")
		m_fDifficultyLevel			= 0.2f;
	else
		m_fDifficultyLevel			= 0.5f;
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: m_fDifficultyLevel: %f", m_fDifficultyLevel);


	CSoundManager* pSoundManager = CSoundManager::GetInstance();
	ASSERT(pSoundManager);

	bool bHardwareSound;
	pIni->ReadValue("Audio", "HARDWARESOUND", bHardwareSound);
	CSoundManager::GetInstance()->Init(!bHardwareSound);

	bool bUseMusic;
	pIni->ReadValue("Audio", "MUSIC", bUseMusic);
	if (bUseMusic)
	{
		CMajor* pPlayer = GetPlayersRace();
		ASSERT(pPlayer);
		network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());

		float fMusicVolume;
		pIni->ReadValue("Audio", "MUSICVOLUME", fMusicVolume);
		pSoundManager->StartMusic(client, fMusicVolume);
	}
	else
		pSoundManager->StopMusic();


	bool bUseSound;
	pIni->ReadValue("Audio", "SOUND", bUseSound);
	if (!bUseSound)
		pSoundManager->SetSoundMasterVolume(NULL);
	else
		pSoundManager->SetSoundMasterVolume(0.5f);
	MYTRACE("general")(MT::LEVEL_INFO, "Init sound ready...\n");

	RandomSeed();
}

/// Funktion gibt die Koordinate des Hauptsystems einer Majorrace zurück.
/// @param sMajor Rassen-ID
/// @return Koordinate auf der Galaxiemap
CPoint CBotf2Doc::GetRaceKO(const CString& sMajorID) const
{
	const std::map<CString, std::pair<int, int>>::const_iterator race = m_mRaceKO.find(sMajorID);
	if (race == m_mRaceKO.end())
		return CPoint(-1,-1);
	return CPoint(race->second.first, race->second.second);
}

void CBotf2Doc::SetKO(int x, int y)
{
	m_ptKO = CPoint(x, y);
	CSmallInfoView::SetPlanet(NULL);

	if (resources::pMainFrame->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW)
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
}

void CBotf2Doc::SetCurrentShip(const CShipMap::iterator& position)
{
	m_ShipMap.SetCurrentShip(position);
	dynamic_cast<CGalaxyMenuView*>(resources::pMainFrame->GetView(RUNTIME_CLASS(CGalaxyMenuView)))->SetNewShipPath();
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
}
void CBotf2Doc::SetFleetShip(const CShipMap::iterator& position)
{
	m_ShipMap.SetFleetShip(position);
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
}
void CBotf2Doc::SetShipInFleet(const CShipMap::iterator& position)
{
	FleetShip()->second->SetCurrentShip(position);
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_FLEET_MENU_VIEW);
}

/// Funktion lädt für die ausgewählte Spielerrasse alle Grafiken für die Views.
void CBotf2Doc::LoadViewGraphics(void)
{
	CMajor* pPlayersRace = GetPlayersRace();
	ASSERT(pPlayersRace);
	MYTRACE("general")(MT::LEVEL_INFO, "pPlayersRace: %s", pPlayersRace->GetRaceName());

	CGalaxyMenuView::SetPlayersRace(pPlayersRace);
	CMainBaseView::SetPlayersRace(pPlayersRace);
	CBottomBaseView::SetPlayersRace(pPlayersRace);
	CMenuChooseView::SetPlayersRace(pPlayersRace);

	// Views die rassenspezifischen Grafiken laden lassen
	std::map<CWnd *, UINT>* views = &resources::pMainFrame->GetSplitterWindow()->views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		if (it->second == GALAXY_VIEW)
			continue;
		else if (it->second == MENUCHOOSE_VIEW)
			((CMenuChooseView*)(it->first))->LoadRaceGraphics();
		else if (IS_MAIN_VIEW(it->second))
			((CMainBaseView*)(it->first))->LoadRaceGraphics();
		else if (IS_BOTTOM_VIEW(it->second))
			((CBottomBaseView*)(it->first))->LoadRaceGraphics();
	}

	// Ini-Werte lesen und setzen
	ResetIniSettings();

	// ab jetzt müssen keine neuen Grafiken mehr geladen werden
	m_bNewGame = false;

	// Views ihre Arbeit zu Beginn jeder neuen Runde machen lassen
	DoViewWorkOnNewRound();

	network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayersRace->GetRaceID());
	// wenn neues Spiel gestartet wurde, dann initial auf die Galaxiekarte stellen
	if (m_iSelectedView[client] == 0)
	{
		// zum Schluss die Galxieview auswählen (nicht eher, da gibts manchmal Probleme beim Scrollen ganz nach rechts)
		resources::pMainFrame->SelectMainView(GALAXY_VIEW, pPlayersRace->GetRaceID());
	}
}

void CBotf2Doc::DoViewWorkOnNewRound()
{
	// Playersrace in Views festlegen
	CMajor* pPlayersRace = GetPlayersRace();
	ASSERT(pPlayersRace);

	CGalaxyMenuView::SetPlayersRace(pPlayersRace);
	CMainBaseView::SetPlayersRace(pPlayersRace);
	CBottomBaseView::SetPlayersRace(pPlayersRace);
	CMenuChooseView::SetPlayersRace(pPlayersRace);

	// Views ihre Arbeiten zu Beginn einer neuen Runde durchführen lassen
	std::map<CWnd *, UINT>* views = &resources::pMainFrame->GetSplitterWindow()->views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		if (it->second == GALAXY_VIEW)
			((CGalaxyMenuView*)(it->first))->OnNewRound();
		else if (IS_MAIN_VIEW(it->second))
			((CMainBaseView*)(it->first))->OnNewRound();
		else if (IS_BOTTOM_VIEW(it->second))
			((CBottomBaseView*)(it->first))->OnNewRound();
	}

	network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayersRace->GetRaceID());

	// anzuzeigende View in neuer Runde auswählen
	// Wenn EventScreens für den Spieler vorhanden sind, so werden diese angezeigt.
	if (pPlayersRace->GetEmpire()->GetEventMessages()->GetSize() > 0)
	{
		resources::pMainFrame->FullScreenMainView(true);
		resources::pMainFrame->SelectMainView(EVENT_VIEW, pPlayersRace->GetRaceID());
	}
	else
	{
		resources::pMainFrame->FullScreenMainView(false);
		resources::pMainFrame->SelectMainView(m_iSelectedView[client], pPlayersRace->GetRaceID());
		m_iSelectedView[client] = 0;
	}

	// wurde Rundenende geklickt zurücksetzen
	m_bRoundEndPressed = false;
	m_bDataReceived = true;

	// alle angezeigten Views neu zeichnen lassen
	UpdateAllViews(NULL);
}

// Generiert ein neues Spiel
void CBotf2Doc::PrepareData()
{
	MYTRACE("general")(MT::LEVEL_INFO, "Begin preparing game data...\n");

	if (!m_bGameLoaded)
	{
		// neue Majors anlegen
		if (!m_pRaceCtrl->Init())
		{
			AfxMessageBox("CBotf2Doc::PrepareData(): Could not initiate races!");
			exit(1);
		}
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
			// wird das Imperium von einem Menschen oder vom Computer gespielt
			if (client != network::RACE_NONE && server.IsPlayedByClient(client))
				it->second->SetHumanPlayer(true);
			else
				it->second->SetHumanPlayer(false);
		}

		// ALPHA6 DEBUG alle Rassen untereinander bekanntgeben
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap()) {
			map<CString, CRace*>* pmRaces = m_pRaceCtrl->GetRaces();
			for (map<CString, CRace*>::iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
				for (map<CString, CRace*>::const_iterator jt = pmRaces->begin(); jt != pmRaces->end(); ++jt)
					if (it->first != jt->first && it->second->IsMajor() && jt->second->IsMajor())
						it->second->SetIsRaceContacted(jt->first, true);
		}

		m_iRound = 1;

		// Generierungssektornamenklasse wieder neu starten
		m_ShipMap.Reset(true);
		m_ShipInfoArray.RemoveAll();

		ReadBuildingInfosFromFile();	// Gebäude einlesen aus data-Datei
		ReadShipInfosFromFile();		// Schiffe einlesen aus data-Datei
		ReadTroopInfosFromFile();		// Truppen einlesen aus data-Datei

		// Schiffsnamen einlesen
		m_GenShipName.Init(this);

		// Werte für Moraländerungen auf verschiedene Ereignisse laden
		CMoralObserver::InitMoralMatrix();

		// Sektoren generieren
		GenerateGalaxy();
		ApplyShipsAtStartup();
		ApplyBuildingsAtStartup();
		ApplyTroopsAtStartup();

		// Siegbedingungen initialisieren und erstmalig überwachen
		m_VictoryObserver.Init();
		m_VictoryObserver.Observe();

		MYTRACE("general")(MT::LEVEL_INFO, "Preparing game data ready...\n");
		/*
		double habis = 0;
		CString s;
		double hab = 0;
		double temp;
		CPoint poi;
		for (int y = 0; y < 20; y++)
			for (int x = 0; x < 30; x++)
			{
				temp = 0;
				hab = 0;
				int number=GetSector(x, y).GetNumberOfPlanets();
				for (int i=0;i<number;i++)
				{
					CPlanet* planet = GetSector(x, y).GetPlanet(i);
					temp+=planet->GetMaxHabitant();
					hab = temp;
				}
				if (habis < hab)
				{
					habis = hab;
					poi.x = x; poi.y = y;
				}
			}
		s.Format("Größtes System ist %s mit %lf Bevölkerung\nin Sektor %d:%d",m_Sectors.at(poi.x+(poi.y)*STARMAP_SECTORS_HCOUNT).GetName(),habis,poi.x,poi.y);
		AfxMessageBox(s);
		*/

		if(clp->GetTest()) {
			const CTest* const test = CTest::GetInstance(*this);
			test->Run();
		}
	}
	// wenn geladen wird
	else
	{
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
			// wird das Imperium von einem Menschen oder vom Computer gespielt
			if (client != network::RACE_NONE && server.IsPlayedByClient(client))
				it->second->SetHumanPlayer(true);
			else
				it->second->SetHumanPlayer(false);
		}
	}
}

/// Funktion generiert die Galaxiemap inkl. der ganzen Systeme und Planeten zu Beginn eines neuen Spiels.
void CBotf2Doc::GenerateGalaxy()
{
	///////////////////////////////////////////////////////////////////////
	// Galaxiengröße festlegen
	CIniLoader* pIni = CIniLoader::GetInstance();

	int nMapHeight = STARMAP_SECTORS_VCOUNT;
	if (pIni->ReadValue("Special", "MAPSIZEV", nMapHeight))
		STARMAP_SECTORS_VCOUNT = nMapHeight;

	int nMapWidth = STARMAP_SECTORS_HCOUNT;
	if (pIni->ReadValue("Special", "MAPSIZEH", nMapWidth))
		STARMAP_SECTORS_HCOUNT = nMapWidth;

	// Vektoren für Sektoren und Systeme anlegen
	AllocateSectorsAndSystems();

	///////////////////////////////////////////////////////////////////////
	// Alles auf Ausgangswerte setzen
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			// Alle Werte der Systemklasse wieder auf NULL setzen
			GetSystem(x, y).ResetSystem();
			// Alle Werte der Sektorklasse wieder auf NULL setzen
			GetSector(x, y).Reset();
			GetSector(x, y).SetKO(x,y);
		}
	}
	m_mRaceKO.clear();

	///////////////////////////////////////////////////////////////////////
	// Galaxieform anpassen
	int nGenerationMode = 0; // 0 == Standard  sonst Pattern verwenden
	pIni->ReadValue("Special", "GENERATIONMODE", nGenerationMode);

	std::vector<std::vector<bool>> nGenField(STARMAP_SECTORS_HCOUNT, std::vector<bool>(STARMAP_SECTORS_VCOUNT, true));
	if (nGenerationMode != 0)
	{
		CString sAppPath = CIOData::GetInstance()->GetAppPath();
		CString filePath = "";
		filePath.Format("%sGraphics\\Galaxies\\pattern%d.boj",sAppPath,nGenerationMode);
		std::auto_ptr<Bitmap> GalaxyPattern(Bitmap::FromFile(CComBSTR(filePath)));
		if (GalaxyPattern.get() == NULL)
		{
			sAppPath.Format("pattern%d.boj not found! using standart pattern", nGenerationMode);
			AfxMessageBox(sAppPath);

		}
		else
		{
			FCObjImage img;
			FCWin32::GDIPlus_LoadBitmap(*GalaxyPattern, img);
			if (img.IsValidImage())
			{
				img.Stretch(STARMAP_SECTORS_HCOUNT,STARMAP_SECTORS_VCOUNT);
				GalaxyPattern.reset(FCWin32::GDIPlus_CreateBitmap(img));
				Gdiplus::Color nColor;
				for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					{
						GalaxyPattern->GetPixel(x,y,&nColor);
						if (nColor.GetR() > 50 && nColor.GetB() > 50 && nColor.GetG() > 50)
							nGenField[x][y] = false;
					}
			}
			else
			{
				sAppPath.Format("pattern%d.boj not found! using standard pattern", nGenerationMode);
				AfxMessageBox(sAppPath);
			}
		}
	}

	// Die sechs Hauptrassen werden zufällig auf der Karte verteilt. Dabei ist aber zu beachten, dass die Entfernungen
	// zwischen den Systemen groß genug sind. Außerdem müssen um jedes der Hauptsysteme zwei weitere Systeme liegen.
	// Diese werden wenn nötig auch generiert.
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	// minimaler Abstand der Majorraceheimatsysteme
	int nMinDiff =(int)sqrt((double)STARMAP_SECTORS_VCOUNT*STARMAP_SECTORS_HCOUNT)/2+2  - pmMajors->size(); //Term der Abstand ungefähr an Feld größe anpasst
	// minimal 4 Felder abstand
	nMinDiff = max(nMinDiff, 4);

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); )
	{
		// Zufällig einen Sektor ermitteln. Dies wird solange getan, solange der ermittelte Sektor noch nicht die
		// if Bedingung erfüllt oder die Abbruchbedingung erreicht ist.
		bool bRestart = false;
		int nCount = 0;

		do
		{
			m_mRaceKO[it->first].first = rand()%(STARMAP_SECTORS_HCOUNT);
			m_mRaceKO[it->first].second= rand()%(STARMAP_SECTORS_VCOUNT);

			for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); ++jt)
			{
				if (it->first != jt->first && GetRaceKO(jt->first) != CPoint(-1,-1) && abs(GetRaceKO(it->first).x - GetRaceKO(jt->first).x) < nMinDiff && abs(GetRaceKO(it->first).y - GetRaceKO(jt->first).y) < nMinDiff||GetRaceKO(it->first) != CPoint(-1,-1)&&nGenField[GetRaceKO(it->first).x][GetRaceKO(it->first).y]==false)
					m_mRaceKO[it->first] = pair<int,int>(-1,-1);
			}
			// Abbruchbedingung
			if (++nCount > max((STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT) / 4, 20))
				bRestart = true;
		}
		while (GetRaceKO(it->first) == CPoint(-1,-1) && bRestart == false);

		// nächsten Major auswählen
		++it;

		// Es konnte kein Sektor nach 250 Durchläufen gefunden werden, welcher der obigen if Bedingung genügt, so
		// wird die for Schleife nochmal neu gestartet.
		if (bRestart)
		{
			m_mRaceKO.clear();
			it = pmMajors->begin();
		}
	}

	// alle potentiellen Minorracesystemnamen holen
	map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();
	CStringArray vMinorRaceSystemNames;
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;
		if (!pMinor)
		{
			ASSERT(pMinor);
			continue;
		}

		// keine Minors ohne Heimatsystem ins Spiel bringen (Aliens haben kein Heimatsystem)
		if (!pMinor->IsAlienRace())
			vMinorRaceSystemNames.Add(pMinor->GetHomesystemName());
	}

	// Namensgenerator initialisieren
	CGenSectorName::GetInstance()->Init(vMinorRaceSystemNames);

	int nStarDensity = 35;
	int nMinorDensity = 30;
	int nAnomalyDensity = 9;
	CIniLoader::GetInstance()->ReadValue("Special", "STARDENSITY", nStarDensity);
	CIniLoader::GetInstance()->ReadValue("Special", "MINORDENSITY", nMinorDensity);
	CIniLoader::GetInstance()->ReadValue("Special", "ANOMALYDENSITY", nAnomalyDensity);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: STARDENSITY: %i", nStarDensity);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: MINORDENSITY: %i", nMinorDensity);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: ANOMALYDENSITY: %i", nAnomalyDensity);

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor const* const pMajor = it->second;
		const CPoint& raceKO = GetRaceKO(it->first);
		CSector& sector = GetSector(raceKO.x, raceKO.y);
		CSystem& system = GetSystem(raceKO.x, raceKO.y);

		sector.SetSectorsName(pMajor->GetHomesystemName());
		sector.SetSunSystem(TRUE);
		sector.SetFullKnown(it->first);
		sector.SetOwned(TRUE);
		sector.SetOwnerOfSector(it->first);
		sector.SetColonyOwner(it->first);
		sector.CreatePlanets(it->first);
		system.SetOwnerOfSystem(it->first);
		system.SetResourceStore(TITAN, 1000);
		system.SetResourceStore(DERITIUM, 3);

		// Zwei Sonnensysteme in unmittelbarer Umgebung des Heimatsystems anlegen
		BYTE nextSunSystems = 0;
		while (nextSunSystems < 2)
		{
			// Punkt mit Koordinaten zwischen -1 und +1 generieren
			CPoint dist(rand()%3 - 1, rand()%3 - 1);
			CPoint pt(raceKO.x + dist.x, raceKO.y + dist.y);
			if (pt.x < STARMAP_SECTORS_HCOUNT && pt.x > -1 && pt.y < STARMAP_SECTORS_VCOUNT && pt.y > -1)
				if (!GetSector(pt.x, pt.y).GetSunSystem())
				{
					GetSector(pt.x, pt.y).GenerateSector(100, nMinorDensity);
					nextSunSystems++;
				}
		};

		// In einem Radius von einem Feld um das Hauptsystem die Sektoren scannen
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= 1; x++)
			{
				CPoint pt(raceKO.x + x, raceKO.y + y);
				if (pt == raceKO)
					continue;

				if (pt.x < STARMAP_SECTORS_HCOUNT && pt.x > -1 && pt.y < STARMAP_SECTORS_VCOUNT && pt.y > -1)
					GetSector(pt.x, pt.y).SetScanned(it->first);
			}
	}

	// nun die Sektoren generieren
	// diese nutzen die voreingestellten Patterns, so dass Galaxieformen möglich werden
	vector<CPoint> vSectorsToGenerate;
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if ((!GetSector(x, y).GetSunSystem()) && nGenField[x][y]==true)
				vSectorsToGenerate.push_back(CPoint(x,y));

	while (!vSectorsToGenerate.empty())
	{
		int nSector = rand()%vSectorsToGenerate.size();
		int x = vSectorsToGenerate[nSector].x;
		int y = vSectorsToGenerate[nSector].y;
		// behandelten Sektor entfernen
		vSectorsToGenerate.erase(vSectorsToGenerate.begin() + nSector);

		// den Sektor generieren
		// die Wahrscheinlichkeiten sind abhängig von den Systemen in der unmittelbaren Nähe. Ist ein Majorrace-
		// hauptsystem in der Nähe, so wird hier kein System generiert, da diese schon weiter oben angelegt
		// wurden
		int sunSystems = 0;
		int minorRaces = 0;
		int nAnomalys  = 0;
		for (int j = -1; j <= 1; j++)
		{
			for (int i = -1; i <= 1; i++)
			{
				CPoint pt(x + i, y + j);
				if (pt.x < STARMAP_SECTORS_HCOUNT && pt.x > -1 && pt.y < STARMAP_SECTORS_VCOUNT && pt.y > -1)
				{
					if (GetSector(pt.x, pt.y).GetSunSystem())
					{
						if (GetSector(pt.x, pt.y).GetMinorRace())
							minorRaces++;
						sunSystems++;
					}
					else if (GetSector(pt.x, pt.y).GetAnomaly())
					{
						nAnomalys++;
					}

					for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); ++it)
					{
						if (it->second.first == x + i && it->second.second == y + j)
						{
							sunSystems	+= 100;
							nAnomalys	+= 100;
						}
					}
				}
			}
		}

		int sunSystemProb = nStarDensity  - sunSystems * 15;
		int minorRaceProb = nMinorDensity - minorRaces * 15;
		if (minorRaceProb < 0)
			minorRaceProb = 0;
		if (sunSystemProb > 0)
			GetSector(x, y).GenerateSector(sunSystemProb, minorRaceProb);

		// Wenn keine Minorrace in dem System generiert wurde
		if (!GetSector(x, y).GetMinorRace())
		{
			// möglicherweise eine Anomalie im Sektor generieren
			if (!GetSector(x, y).GetSunSystem())
			{
				if (rand()%100 >= (100 - (nAnomalyDensity - nAnomalys * 10)))
					GetSector(x, y).CreateAnomaly();
			}
		}
	}

	// Vektor der verwendeten Minors, diese nehmen aktiv am Spiel teil.
	set<CString> sUsedMinors;
	// Nun alle generierten Sektoren durchgehen und die vorhandenen Minorraces parametriesieren
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (GetSector(x, y).GetMinorRace())
			{
				// Nun die Minorrace parametrisieren
				CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(x, y).GetName());
				if (!pMinor)
				{
					AfxMessageBox("Error in function CBotf2Doc::GenerateGalaxy(): Could not create Minorrace");
				}
				else
				{
					pMinor->SetRaceKO(CPoint(x,y));
					GetSector(x, y).SetOwnerOfSector(pMinor->GetRaceID());
					GetSystem(x, y).SetOwnerOfSystem("");
					sUsedMinors.insert(pMinor->GetRaceID());
					// wenn die Minorrace Schiffe bauen kann, sie aber kein Deritium im System besitzt, so wird
					// ein Deritium auf dem ersten kolonisierten Planeten hinzugefügt
					if (pMinor->GetSpaceflightNation())
					{
						BOOLEAN bRes[DERITIUM + 1] = {FALSE};
						GetSector(x, y).GetAvailableResources(bRes, true);
						// gibt es kein Deritium=
						if (!bRes[DERITIUM])
						{
							for (int p = 0; p < static_cast<int>(GetSector(x, y).GetPlanets().size()); p++)
							{
								if (GetSector(x, y).GetPlanet(p)->GetCurrentHabitant() > 0 && GetSector(x, y).GetPlanet(p)->GetColonized())
								{
									GetSector(x, y).GetPlanet(p)->SetBoni(DERITIUM, TRUE);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	// nun können alle nicht verwendeten Minors entfernt werden
	vector<CString> vDelMinors;
	for (map<CString, CMinor*>::iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		if (sUsedMinors.find(it->first) == sUsedMinors.end())
		{
			// keine Aliens ohne Heimatsystem rauslöschen
			if (!it->second->IsAlienRace())
				vDelMinors.push_back(it->first);
		}
	}

	for (UINT i = 0; i < vDelMinors.size(); i++)
		m_pRaceCtrl->RemoveRace(vDelMinors[i]);
}

////////////////////////////////////////////////
//BEGINN: helper functions for NextRound()
static bool HumanPlayerInCombat(const CShipMap& ships, const CPoint& CurrentCombatSector,
		const std::map<CString, CMajor*>& majors) {

	for(CShipMap::const_iterator i = ships.begin(); i != ships.end(); ++i)
	{
		if (i->second->GetKO() != CurrentCombatSector)
			continue;
		const std::map<CString, CMajor*>::const_iterator major = majors.find(i->second->GetOwnerOfShip());
		if (major != majors.end() && major->second->IsHumanPlayer())
			return true;
	}
	return false;
}
//END: helper functions for NextRound()
////////////////////////////////////////////////
void CBotf2Doc::NextRound()
{
	// gibt es für diese Runde Sektoren in welchen ein Kampf stattfand
	bool bCombatInCurrentRound = !m_sCombatSectors.empty();

	// Es fand noch kein Kampf die Runde statt. Dies tritt ein, wenn entweder wirklich kein Kampf diese Runde war
	// oder das erste Mal in diese Funktion gesprungen wurde.
	if (bCombatInCurrentRound == false)
	{
		MYTRACE("general")(MT::LEVEL_INFO, "#### START NEXT ROUND (round: %d) ####", GetCurrentRound());

		// Seed initialisieren
		RandomSeed();

		// Soundnachrichten aus alter Runde löschen
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			m_SoundMessages[i].RemoveAll();

		// ausgelöschte Rassen gleich zu Beginn der neuen Runde entfernen. Menschliche Spieler sollten jetzt schon disconnected sein!!!
		vector<CString> vDelMajors;
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->second->GetEmpire()->GetNumberOfSystems() == 0)
				vDelMajors.push_back(it->first);
		for (UINT i = 0; i < vDelMajors.size(); i++)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Race %d is out of game", vDelMajors[i]);
			m_pRaceCtrl->RemoveRace(vDelMajors[i]);
			pmMajors = m_pRaceCtrl->GetMajors();
		}

		// Starmap für alle Rassen berechnen und anlegen
		GenerateStarmap();

		// KI-Berechnung
		m_pSectorAI->Clear();
		m_pSectorAI->CalculateDangers();
		m_pSectorAI->CalcualteSectorPriorities();

		CShipAI ShipAI(this);
		ShipAI.CalculateShipOrders(m_pSectorAI);

		m_pAIPrios->Clear();
		m_pAIPrios->CalcShipPrios(m_pSectorAI);
		m_pAIPrios->GetIntelAI()->CalcIntelligence(this);

		// Alle Statistiken des Spiels berechnen (erst nachdem die Sector-AI berechnet wurde!
		// Nimmt von dort gleich die Schiffsstärken)
		m_Statistics.CalcStats(this);

		this->CalcPreDataForNextRound();
		// Diplomatie nach dem Hochzählen der Runde aber noch vor der Schiffsbewegung durchführen
		this->CalcDiplomacy();
		this->CalcShipOrders();
		this->CalcShipMovement();
		// prüfen ob ein Kampf stattfindet
		if (IsShipCombat())
		{
			// Ist ein menschlicher Spieler beteiligt?
			// kein menschlicher Spieler beteiligt -> gleich weiter
			if (!HumanPlayerInCombat(m_ShipMap, m_ptCurrentCombatSector, *pmMajors))
				NextRound();

			// findet ein Kampf statt, so sofort aus der Funktion rausgehen und die Kampfberechnungen durchführen
			return;
		}
	}
	// Es findet ein Kampf statt
	else
	{
		MYTRACE("general")(MT::LEVEL_INFO, "COMBAT ROUND\n");
		// Kampf berechnen
		CalcShipCombat();
		// Wenn wieder ein Kampf stattfindet, so aus der Funktion springen
		if (IsShipCombat())
		{
			map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
			// Ist ein menschlicher Spieler beteiligt?
			// kein menschlicher Spieler beteiligt -> gleich weiter
			if (!HumanPlayerInCombat(m_ShipMap, m_ptCurrentCombatSector, *pmMajors))
				NextRound();

			// findet ein Kampf statt, so sofort aus der Funktion rausgehen und die Kampfberechnungen durchführen
			return;
		}
	}

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Minors erst nach einem Kampf berechnen, so dass nicht in der gleichen Runde deren Schiff gegen ein anderes Kämpfen kann
	// Minors ausbreiten, Akzeptanzpunkte berechnen und Ressourcen verbrauchen
	map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;

		// Die Punkte für längere gute Beziehungen berechnen
		pMinor->CalcAcceptancePoints(this);

		// wurde die Rasse erobert oder gehört jemanden, dann nicht hier weitermachen
		if (pMinor->GetSubjugated())
			continue;
		bool bMember = false;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (pMinor->IsMemberTo(it->first))
			{
				bMember = true;
				break;
			}
		}
		if (bMember)
			continue;

		CPoint ko = pMinor->GetRaceKO();

		if (ko != CPoint(-1,-1) && GetSystem(ko.x, ko.y).GetOwnerOfSystem() == "" && GetSector(ko.x, ko.y).GetOwnerOfSector() == pMinor->GetRaceID())
		{
			// Vielleicht kolonisiert die Minorrace weitere Planeten in ihrem System
			if (pMinor->PerhapsExtend(this))
				// dann sind im System auch weitere Einwohner hinzugekommen
				GetSystem(ko.x, ko.y).SetHabitants(this->GetSector(ko.x, ko.y).GetCurrentHabitants());

			// Den Verbrauch der Rohstoffe der kleinen Rassen in jeder Runde berechnen
			pMinor->ConsumeResources(this);
			// Vielleicht baut die Rasse ein Raumschiff
			pMinor->PerhapsBuildShip(this);
		}
	}
	
	// Zufallsereignisse berechnen
	CReManager* pRandomEventManager = CReManager::GetInstance();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		pRandomEventManager->CalcEvents(it->second);	

	this->CalcSystemAttack();
	this->CalcIntelligence();
	this->CalcResearch();

	// alten Creditbestand festhalten
	map<CString, long> mOldCredits;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		mOldCredits[it->first] = pMajor->GetEmpire()->GetCredits();
	}

	// Auswirkungen von Alienschiffen beachten
	this->CalcAlienShipEffects();
	// alle Systeme berechnen (Bauliste, Moral, Energie usw.)
	this->CalcOldRoundData();
	// Aliens zufällig ins Spiel bringen (vor der Berechnung der Schiffsauswirkungen)
	this->CalcRandomAlienEntities();
	// Schiffsauswirkungen berechnen (Scanstärken, Erfahrung usw.)
	this->CalcShipEffects();
	this->CalcNewRoundData();
	this->CalcTrade();
	
	// Zufallsereignis Hüllenvirus berechnen
	pRandomEventManager->CalcShipEvents();

	this->CalcEndDataForNextRound();

	// Creditänderung berechnen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		pMajor->GetEmpire()->SetCreditsChange((int)(pMajor->GetEmpire()->GetCredits() - mOldCredits[pMajor->GetRaceID()]));
	}

	// In dieser Runde stattgefundene Kämpfe löschen
	m_sCombatSectors.clear();
	m_bCombatCalc = false;

	bool bAutoSave = false;
	CIniLoader::GetInstance()->ReadValue("General", "AUTOSAVE", bAutoSave);
	if (bAutoSave)
		DoSave(CIOData::GetInstance()->GetAutoSavePath(m_iRound), FALSE);
	SetModifiedFlag();

	MYTRACE("general")(MT::LEVEL_INFO, "\nNEXT ROUND calculation successfull\n", GetCurrentRound());
}

void CBotf2Doc::ApplyShipsAtStartup()
{
	// Name des zu öffnenden Files
	CString fileName= CIOData::GetInstance()->GetAppPath() + "Data\\Ships\\StartShips.data";
	CStdioFile file;
	// Datei wird geöffnet
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		// auf csInput wird die jeweilige Zeile gespeichert
		CString csInput;
		while (file.ReadString(csInput))
		{
			if (csInput.Left(2) == "//" || csInput.IsEmpty())
				continue;
			int pos = 0;
			// Besitzer des Schiffes auslesen
			CString s = csInput.Tokenize(":", pos);
			CString sOwner = s;
			// Namen des Systems holen
			CString systemName = s = csInput.Tokenize(":", pos);
			// Systemnamen auf der Map suchen
			bool bFoundSector = false;
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			{
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					// Sektornamen gefunden
					if (GetSector(x, y).GetName(TRUE) == s)
					{
						bFoundSector = true;
						// Schiffsklassen durchgehen, die dort stationiert werden sollen
						while (pos < csInput.GetLength())
						{
							s = csInput.Tokenize(",", pos);
							bool bFoundShip = false;
							for (int j = 0; j < m_ShipInfoArray.GetSize(); j++)
							{
								// Wurde eine entsprechende Schiffsklasse gefunden, diese bauen
								if (m_ShipInfoArray.GetAt(j).GetShipClass() == s)
								{
									bFoundShip = true;
									BuildShip(m_ShipInfoArray.GetAt(j).GetID(), CPoint(x,y), sOwner);
									break;
								}
							}
							// Wurde die Schiffsklasse nicht gefunden, sprich es ist ein Fehler in der Datei
							if (!bFoundShip)
							{
								CString shipClass;
								shipClass.Format("Could not find ship class \"%s\"\nPlease check your StartShips.data file!", s);
								AfxMessageBox(shipClass);
							}
						}
						break;
					}

				if (bFoundSector)
					break;
			}
			// Wurde das System nicht gefunden, sprich es ist ein Fehler in der Datei
			if (!bFoundSector)
			{
				s.Format("Could not find system with name \"%s\"\nPlease check your StartShips.data file!", systemName);
				AfxMessageBox(s);
			}
		}
	}
	else
		AfxMessageBox("ERROR! Could not open file \"StartShips.data\"...");
	// Datei schließen
	file.Close();

	CIniLoader* pIni = CIniLoader::GetInstance();
	ASSERT(pIni);
	if (pIni && pIni->ReadValueDefault("Special", "ALIENENTITIES", true))
	{
		// Nehmen die Ehlenen am Spiel teil, so den Ehlenen-Beschützer (Station) in deren System bauen
		if (CMinor* pEhlen = dynamic_cast<CMinor*>(GetRaceCtrl()->GetRace("EHLEN")))
		{
			// Schiff Ehlenen-Beschützer suchen (blöd das hier Sprachunabhängigkeit bei der ID existiert)
			for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			{
				CShipInfo* pShipInfo = &m_ShipInfoArray.GetAt(i);
				if (pShipInfo->GetOnlyInSystem() != pEhlen->GetHomesystemName())
					continue;

				BuildShip(m_ShipInfoArray.GetAt(i).GetID(), pEhlen->GetRaceKO(), pEhlen->GetRaceID());
				break;
			}
		}

		// Die Kampfstation zufällig in irgendeinem unbewohnten Sonnensystem auf der Galaxiekarte platzieren
		int nDeadloopCounter = 0;
		do
		{
			// Endlosschleife vermeiden
			if (++nDeadloopCounter > 1000000)
			{
				AfxMessageBox("Info: It was not possible to generate the Battlestation, because no free sunsystem is ingame!");
				break;
			}

			CPoint p(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
			if (!GetSector(p.x, p.y).GetSunSystem())
				continue;

			if (GetSector(p.x, p.y).GetOwnerOfSector() != "")
				continue;

			if (GetSector(p.x, p.y).GetMinorRace())
				continue;

			if (GetSector(p.x, p.y).GetAnomaly())
				continue;
			
			for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			{
				CShipInfo* pShipInfo = &m_ShipInfoArray.GetAt(i);
				if (pShipInfo->GetOnlyInSystem() != KAMPFSTATION)
					continue;

				// Kampfstation platzieren und abbrechen
				BuildShip(m_ShipInfoArray.GetAt(i).GetID(), p, KAMPFSTATION);
				break;
			}

			// Sektor war okay
			break;

		} while (true);
	}
}

void CBotf2Doc::ApplyTroopsAtStartup()
{
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Troops\\StartTroops.data";
	CStdioFile file;
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))// Datei StartTroops.data öffnen um Start Truppen zu definieren
	{
		CString csInput;
		while (file.ReadString(csInput))
		{
			if (csInput.Left(2) == "//" || csInput.IsEmpty())
				continue;
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			CString systemName = s;
			// Systemnamen auf der Map suchen
			BOOLEAN found = FALSE;
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			{
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					if (GetSector(x, y).GetName() == s)//Wenn der Name gefunden wurde
					{
						while (pos < csInput.GetLength())
						{
							// ID des Gebäudes holen
							s = csInput.Tokenize(",", pos);
							BuildTroop(atoi(s), CPoint(x,y));
						}
						found = TRUE;
						break;
					}
				if (found)
					break;
			}
			// Wurde das System nicht gefunden, sprich es ist ein Fehler in der Datei
			if (!found)
			{
				s.Format("Could not find system with name \"%s\"\nPlease check your StartTroops.data file!", systemName);
				AfxMessageBox(s);
			}
		}
	}
	else
		AfxMessageBox("ERROR! Could not open file \"StartTroops.data\"...");
	// Datei schließen
	file.Close();
}



void CBotf2Doc::ApplyBuildingsAtStartup()
{
	// Name des zu öffnenden Files
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Buildings\\StartBuildings.data";
	CStdioFile file;
	// Datei wird geöffnet
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		// auf csInput wird die jeweilige Zeile gespeichert
		CString csInput;
		while (file.ReadString(csInput))
		{
			if (csInput.Left(2) == "//" || csInput.IsEmpty())
				continue;
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			CString systemName = s;
			// Systemnamen auf der Map suchen
			BOOLEAN found = FALSE;
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			{
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					if (GetSector(x, y).GetName() == s)
					{
						while (pos < csInput.GetLength())
						{
							// ID des Gebäudes holen
							s = csInput.Tokenize(",", pos);
							BuildBuilding(atoi(s), CPoint(x,y));
						}
						found = TRUE;
						break;
					}
				if (found)
					break;
			}
			// Wurde das System nicht gefunden, sprich es ist ein Fehler in der Datei
			if (!found)
			{
				s.Format("Could not find system with name \"%s\"\nPlease check your StartBuildings.data file!", systemName);
				AfxMessageBox(s);
			}
		}
	}
	else
		AfxMessageBox("ERROR! Could not open file \"StartBuildings.data\"...");
	// Datei schließen
	file.Close();

	// testweise mal in allen Systemen alles berechnen
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Starmaps berechnen, sofern diese noch nicht existieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		pMajor->CreateStarmap();
	}
	// Anomalien beachten (ist für jede Starmap gleich, daher statisch)
	CStarmap::SynchronizeWithAnomalies(m_Sectors);

	for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
	{
		CSystem& system = GetSystemForSector(*sector);
		if (sector->GetSunSystem() == TRUE)
		{
			system.SetHabitants(sector->GetCurrentHabitants());
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				if (system.GetOwnerOfSystem() == it->first)
				{
					CMajor* pMajor = it->second;
					ASSERT(pMajor);
					// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
					// baubare Gebäude, Schiffe und Truppen berechnen
					system.CalculateNumberOfWorkbuildings(&this->BuildingInfo);
					system.SetWorkersIntoBuildings();
					system.CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), sector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					// alle produzierten FP und SP der Imperien berechnen und zuweisen
					int currentPoints;
					currentPoints = system.GetProduction()->GetResearchProd();
					pMajor->GetEmpire()->AddFP(currentPoints);
					currentPoints = system.GetProduction()->GetSecurityProd();
					pMajor->GetEmpire()->AddSP(currentPoints);
					// Schiffsunterstützungskosten eintragen
					float fCurrentHabitants = sector->GetCurrentHabitants();
					pMajor->GetEmpire()->AddPopSupportCosts((USHORT)fCurrentHabitants * POPSUPPORT_MULTI);
				}
			}
			for (int i = 0; i < system.GetAllBuildings()->GetSize(); i++)
			{
				USHORT nID = system.GetAllBuildings()->GetAt(i).GetRunningNumber();
				CString sRaceID = system.GetOwnerOfSystem();
				if (GetBuildingInfo(nID).GetMaxInEmpire() > 0)
					m_GlobalBuildings.AddGlobalBuilding(sRaceID, nID);
			}
		}
	}//for(std::vector<CSector>::const_iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)

	this->CalcNewRoundData();
	this->CalcShipEffects();

	// hier das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse des jeweiligen Imperiums
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	// Dies gilt nur für Majorsraces.
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				// nur aktuell baubare Schiffe dürfen überprüft werden, wenn wir die Beamwaffen checken
				if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(researchLevels))
				{
					// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
					pMajor->GetWeaponObserver()->CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
					pMajor->GetWeaponObserver()->CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
				}
			}

		// Systemliste erstellen und baubare Gebäude, Schiffe und Truppen berechnen
		pMajor->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
		for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
		{
			CPoint p = pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko;
			GetSystem(p.x, p.y).CalculateBuildableBuildings(&GetSector(p.x, p.y), &BuildingInfo, pMajor, &m_GlobalBuildings);
			GetSystem(p.x, p.y).CalculateBuildableShips(this, p);
			GetSystem(p.x, p.y).CalculateBuildableTroops(&m_TroopInfo, pMajor->GetEmpire()->GetResearch());
		}
	}
}

void CBotf2Doc::ReadTroopInfosFromFile()
{
//Neu: Truppen werden aus Datei gelesen
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Troops\\Troops.data";
	CStdioFile file;
	// Datei wird geöffnet
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString data[22];
		CString csInput;
		int i=0;
		CTroopInfo* troopInfo;
		m_TroopInfo.RemoveAll();
		while (file.ReadString(csInput))
		{
			if (csInput.Left(2) == "//" || csInput.IsEmpty())
				continue;
			// Daten lesen
			data[i++] = csInput;
			if (i == 22)
			{
				i = 0;
				BYTE techs[6]={atoi(data[7]),atoi(data[8]),atoi(data[9]),atoi(data[10]),atoi(data[11]),atoi(data[12])};
				USHORT res[5] = {atoi(data[13]),atoi(data[14]),atoi(data[15]),atoi(data[16]),atoi(data[17])};
				troopInfo = new CTroopInfo(CResourceManager::GetString(data[1]), CResourceManager::GetString(data[2]),data[3],atoi(data[4]),atoi(data[5]),atoi(data[6]),techs,res,atoi(data[18]),atoi(data[19]),data[0].GetString(),atoi(data[20]),atoi(data[21]));
				m_TroopInfo.Add(*troopInfo);
				delete troopInfo;
			}
		}

	}
	else
	{
		AfxMessageBox("ERROR! Could not open file \"Troops.data\"...");
		exit(1);
	}
	file.Close();
}


void CBotf2Doc::ReadBuildingInfosFromFile()
{
	for (int i = 0; i < BuildingInfo.GetSize(); )
		BuildingInfo.RemoveAt(i);
	BuildingInfo.RemoveAll();
	CBuildingInfo info;
	CString csInput;
	CString data[140];
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Buildings\\Buildings.data";		// Name des zu Öffnenden Files
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		USHORT i = 0;
		while (file.ReadString(csInput))
		{
			// Daten lesen
			data[i++] = csInput;
			if (i == 140)
			{
				i = 0;
				info.SetRunningNumber(atoi(data[0]));
				info.SetOwnerOfBuilding(atoi(data[1]));
				info.SetBuildingName(data[2]);
				info.SetBuildingDescription(data[4]);
				info.SetUpgradeable(atoi(data[6]));
				info.SetGraphikFileName(data[7]);
				info.SetMaxInSystem(atoi(data[8]),atoi(data[9]));
				info.SetMaxInEmpire(atoi(data[10]),atoi(data[11]));
				if (atoi(data[10]) > 0 && atoi(data[11]) != atoi(data[0]))
				{
					CString s;
					s.Format("Error in Buildings.data: Building \"%s\": ID from \"max in empire\" has to be the same as the building id", info.GetBuildingName());
					AfxMessageBox(s);
				}
				info.SetOnlyHomePlanet(atoi(data[12]));
				info.SetOnlyOwnColony(atoi(data[13]));
				info.SetOnlyMinorRace(atoi(data[14]));
				info.SetOnlyTakenSystem(atoi(data[15]));
				info.SetOnlyInSystemWithName(data[16]);
				info.SetMinHabitants(atoi(data[17]));
				info.SetMinInSystem(atoi(data[18]),atoi(data[19]));
				info.SetMinInEmpire(atoi(data[20]),atoi(data[21]));
				if (atoi(data[20]) > 0)
				{
					CString s;
					s.Format("Error in Buildings.data: Building \"%s\": \"Min in empire\" is not supported in this version!", info.GetBuildingName());
					AfxMessageBox(s);
				}
				info.SetOnlyRace(atoi(data[22]));
				info.SetPlanetTypes(PLANETCLASS_A,atoi(data[23]));
				info.SetPlanetTypes(PLANETCLASS_B,atoi(data[24]));
				info.SetPlanetTypes(PLANETCLASS_C,atoi(data[25]));
				info.SetPlanetTypes(PLANETCLASS_E,atoi(data[26]));
				info.SetPlanetTypes(PLANETCLASS_F,atoi(data[27]));
				info.SetPlanetTypes(PLANETCLASS_G,atoi(data[28]));
				info.SetPlanetTypes(PLANETCLASS_H,atoi(data[29]));
				info.SetPlanetTypes(PLANETCLASS_I,atoi(data[30]));
				info.SetPlanetTypes(PLANETCLASS_J,atoi(data[31]));
				info.SetPlanetTypes(PLANETCLASS_K,atoi(data[32]));
				info.SetPlanetTypes(PLANETCLASS_L,atoi(data[33]));
				info.SetPlanetTypes(PLANETCLASS_M,atoi(data[34]));
				info.SetPlanetTypes(PLANETCLASS_N,atoi(data[35]));
				info.SetPlanetTypes(PLANETCLASS_O,atoi(data[36]));
				info.SetPlanetTypes(PLANETCLASS_P,atoi(data[37]));
				info.SetPlanetTypes(PLANETCLASS_Q,atoi(data[38]));
				info.SetPlanetTypes(PLANETCLASS_R,atoi(data[39]));
				info.SetPlanetTypes(PLANETCLASS_S,atoi(data[40]));
				info.SetPlanetTypes(PLANETCLASS_T,atoi(data[41]));
				info.SetPlanetTypes(PLANETCLASS_Y,atoi(data[42]));
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
				info.SetNeededDeritium(atoi(data[56]));
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
				info.SetDeritiumProd(atoi(data[67]));
				info.SetCreditsProd(atoi(data[68]));
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
				info.SetDeritiumBoni(atoi(data[81]));
				info.SetAllRessourceBoni(atoi(data[82]));
				info.SetCreditsBoni(atoi(data[83]));
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
				info.SetBuildableShipTypes((SHIP_SIZE::Typ)atoi(data[98]));
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
				info.SetAllwaysOnline(atoi(data[124]));
				info.SetWorker(atoi(data[125]));
				info.SetNeverReady(atoi(data[126]));
				info.SetEquivalent(0,0);		// niemand-index immer auf NULL setzen
				for (int p = HUMAN; p <= DOMINION; p++)
					info.SetEquivalent(p,atoi(data[126+p]));
				for (int res = TITAN; res <= DERITIUM; res++)
					info.SetResourceDistributor(res, atoi(data[133+res]));
				info.SetNeededSystems(atoi(data[139]));

				// Information in das Gebäudeinfofeld schreiben
				BuildingInfo.Add(info);
			}
		}
	}
	else
	{
		AfxMessageBox("ERROR! Could not open file \"Buildings.data\"...");
		exit(1);
	}
	// Datei wird geschlossen
	file.Close();

	/*	for (int i = 0; i < BuildingInfo.GetSize(); i++)
	{
		CString test;
		test.Format("FoodProd: %i\nName: %s\nGrafikdatei: %s\nlaufende Nummer: %i\nnötige Biotechstufe: %i\nnötige Energietechstufe %i\nnötige Computertechstufe %i\nnötige Antriebstechstufe %i\nnötige Konstruktionstechstufe %i\nnötige Waffentechstufe %i\nKlasse K: %i\nKlasse M: %i\nKlasse O: %i\nVorgänger: %i\nArbeiter: %i"
			,BuildingInfo.GetAt(i).GetFoodProd(),
			BuildingInfo.GetAt(i).GetBuildingName(),
			BuildingInfo.GetAt(i).GetGraphikFileName(),
			BuildingInfo.GetAt(i).GetRunningNumber(),
			BuildingInfo.GetAt(i).GetBioTech(),
			BuildingInfo.GetAt(i).GetEnergyTech(),
			BuildingInfo.GetAt(i).GetCompTech(),
			BuildingInfo.GetAt(i).GetPropulsionTech(),
			BuildingInfo.GetAt(i).GetConstructionTech(),
			BuildingInfo.GetAt(i).GetWeaponTech(),
			BuildingInfo.GetAt(i).GetPlanetTypes(K),
			BuildingInfo.GetAt(i).GetPlanetTypes(M),
			BuildingInfo.GetAt(i).GetPlanetTypes(O),
			BuildingInfo.GetAt(i).GetPredecessorID(),
			BuildingInfo.GetAt(i).GetWorker());
			AfxMessageBox(test);
	}*/
}

// Funktion ließt aus der Datei die Informationen zu allen Schiffen ein und speichert diese im dynamischen Feld
// später können wir ingame diese Informationen ändern und uns unsere eigenen Schiffchen bauen
void CBotf2Doc::ReadShipInfosFromFile()
{
	CShipInfo ShipInfo;
	int i = 0;
	USHORT j = 0;
	int weapons = 0;	// Zähler um Waffen hinzuzufügen
	BOOL torpedo = FALSE;
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString data[40];
	CString torpedoData[9];
	CString beamData[12];
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Ships\\Shiplist.data";				// Name des zu Öffnenden Files
	CStdioFile file;														// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput != "$END_OF_SHIPDATA$")
			{
				if (csInput == "$Torpedo$")
				{
					weapons = 9;	// weil wir 9 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 12;	// weil wir 12 Informationen für einen Beam brauchen
					torpedo = FALSE;
				}
				else if (torpedo == TRUE && weapons > 0)
				{
					torpedoData[9-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Torpedodaten hinzufügen
						CTorpedoWeapons torpedoWeapon;
						torpedoWeapon.ModifyTorpedoWeapon(atoi(torpedoData[0]),atoi(torpedoData[1]),
							atoi(torpedoData[2]),atoi(torpedoData[3]),torpedoData[4],atoi(torpedoData[5]),atoi(torpedoData[6]));

						// folgende Zeile neu in Alpha5
						torpedoWeapon.GetFirearc()->SetValues(atoi(torpedoData[7]), atoi(torpedoData[8]));

						ShipInfo.GetTorpedoWeapons()->Add(torpedoWeapon);
					}
				}
				else if (torpedo == FALSE && weapons > 0)
				{
					beamData[12-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Beamdaten hinzufügen
						CBeamWeapons beamWeapon;
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),
							beamData[3],atoi(beamData[4]),atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
						// folgende Zeile neu in Alpha5
						beamWeapon.GetFirearc()->SetValues(atoi(beamData[10]), atoi(beamData[11]));

						ShipInfo.GetBeamWeapons()->Add(beamWeapon);
					}
				}
				else
				{
					data[i] = csInput;
					i++;
				}
			}
			else
			{
				// sonstige Informationen an das Objekt übergeben
				ShipInfo.SetRace(atoi(data[0]));
				// ALPHA5	-> Alle Minorraceschiffe haben im Editor noch die Nummer UNKNOWN.
				//			-> diese Schiffe werden nun auf die MINORNUMBER gesetzt
				if (ShipInfo.GetRace() == UNKNOWNRACE)
					ShipInfo.SetRace(MINORNUMBER);
				ShipInfo.SetID(j);
				ShipInfo.SetShipClass(data[1]);
				ShipInfo.SetShipDescription(data[2]);
				ShipInfo.SetShipType((SHIP_TYPE::Typ)atoi(data[3]));
				ShipInfo.SetShipSize((SHIP_SIZE::Typ)atoi(data[4]));
				ShipInfo.SetManeuverability(atoi(data[5]));
				ShipInfo.SetBioTech(atoi(data[6]));
				ShipInfo.SetEnergyTech(atoi(data[7]));
				ShipInfo.SetComputerTech(atoi(data[8]));
				ShipInfo.SetPropulsionTech(atoi(data[9]));
				ShipInfo.SetConstructionTech(atoi(data[10]));
				ShipInfo.SetWeaponTech(atoi(data[11]));
				ShipInfo.SetNeededIndustry(atoi(data[12]));
				ShipInfo.SetNeededTitan(atoi(data[13]));
				ShipInfo.SetNeededDeuterium(atoi(data[14]));
				ShipInfo.SetNeededDuranium(atoi(data[15]));
				ShipInfo.SetNeededCrystal(atoi(data[16]));
				ShipInfo.SetNeededIridium(atoi(data[17]));
				ShipInfo.SetNeededDeritium(atoi(data[18]));
				ShipInfo.SetOnlyInSystem(data[19]);
				ShipInfo.GetHull()->ModifyHull(atoi(data[22]),atoi(data[20]),atoi(data[21]),atoi(data[23]),atoi(data[24]));
				ShipInfo.GetShield()->ModifyShield(atoi(data[25]),atoi(data[26]),atoi(data[27]));
				ShipInfo.SetSpeed(atoi(data[28]));
				ShipInfo.SetRange((SHIP_RANGE::Typ)atoi(data[29]));
				ShipInfo.SetScanPower(atoi(data[30]));
				ShipInfo.SetScanRange(atoi(data[31]));
				ShipInfo.SetStealthGrade(atoi(data[32]));
				ShipInfo.SetStorageRoom(atoi(data[33]));
				ShipInfo.SetColonizePoints(atoi(data[34]));
				ShipInfo.SetStationBuildPoints(atoi(data[35]));
				ShipInfo.SetMaintenanceCosts(atoi(data[36]));
				ShipInfo.SetSpecial(0, (SHIP_SPECIAL::Typ)atoi(data[37]));
				ShipInfo.SetSpecial(1, (SHIP_SPECIAL::Typ)atoi(data[38]));
				ShipInfo.SetObsoleteShipClass(data[39]);
				ShipInfo.CalculateFinalCosts();
				ShipInfo.SetStartOrder();
				ShipInfo.SetStartTactic();
				m_ShipInfoArray.Add(ShipInfo);
				ShipInfo.DeleteWeapons();
				i = 0;
				j++;
			}
		}
	}
	else
	{
		AfxMessageBox("ERROR! Could not open file \"Shiplist.data\"...");
		exit(1);
	}
	// Datei wird geschlossen
	file.Close();

	// gibt es mehrere Majors mit dem gleichen Shipset dann müssen die Schiffe dupliziert werden und dem Feld
	// nochmals hinzugefügt werden. Dabei wird die Schiffsbaurasse auf die Rasse mit dem doppelten Shipset gesetzt.
	int nAdd = 1;
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		for (map<CString, CMajor*>::iterator jt = pmMajors->begin(); jt != pmMajors->end(); ++jt)
			if (it->first != jt->first && it->second->GetRaceShipNumber() == jt->second->GetRaceShipNumber())
			{
				jt->second->SetRaceShipNumber(pmMajors->size() + nAdd);
				// nun alle Schiffe mit der Raceshipnumber des ersten Majors für den zweiten Major anhängen
				for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
					if (m_ShipInfoArray.GetAt(i).GetRace() == it->second->GetRaceShipNumber())
					{
						CShipInfo shipInfo = m_ShipInfoArray[i];
						shipInfo.SetRace(pmMajors->size() + nAdd);
						shipInfo.SetID(m_ShipInfoArray.GetSize());
						m_ShipInfoArray.Add(shipInfo);
					}
				nAdd++;
			}
}

// Später noch hinzufügen, dass auch andere Rassen bauen können
void CBotf2Doc::BuildBuilding(USHORT id, const CPoint& KO)
{
	CBuilding building(id);
	BOOLEAN isOnline = this->GetBuildingInfo(id).GetAllwaysOnline();
	building.SetIsBuildingOnline(isOnline);
	GetSystem(KO.x, KO.y).AddNewBuilding(building);
}

CShipMap::iterator CBotf2Doc::BuildShip(int nID, const CPoint& KO, const CString& sOwnerID)
{
	CRace* pOwner = m_pRaceCtrl->GetRace(sOwnerID);
	if (!pOwner)
	{
		AfxMessageBox("Error in BuildShip(): RaceID " + sOwnerID + " doesn't exist!");
		return m_ShipMap.end();
	}

	ASSERT(nID >= 10000);
	nID -= 10000;

	CString sOwner = sOwnerID;
	const CShipMap::iterator it = m_ShipMap.Add(new CShips(m_ShipInfoArray.GetAt(nID)));
	it->second->SetOwnerOfShip(sOwner);
	it->second->SetKO(KO.x, KO.y);

	// Schiffsnamen vergeben
	it->second->SetShipName(m_GenShipName.GenerateShipName(sOwner, it->second->IsStation()));

	// den Rest nur machen, wenn das Schiff durch eine Majorrace gebaut wurde
	if (!pOwner->IsMajor())
		return it;

	CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
	ASSERT(pMajor);

	// Spezialforschungsboni dem Schiff hinzufügen
	AddSpecialResearchBoniToShip(it->second, pMajor);

	pMajor->GetShipHistory()->AddShip(it->second, GetSector(KO.x, KO.y).GetName(), m_iRound);
	return it;
}

/// Funktion zum Löschen des Schiffes aus dem Schiffsarray.
/// @param ship Iterator des Schiffes im Array
/// iterator is updated to the new position of the element following the deleted one
void CBotf2Doc::RemoveShip(CShipMap::iterator& ship)
{
	if (ship->second->HasFleet())
	{
		CShips* new_fleetship = ship->second->GiveFleetToFleetsFirstShip();
		m_ShipMap.Add(new_fleetship);
	}
	m_ShipMap.EraseAt(ship, true);
}

/// Funktion beachtet die erforschten Spezialforschungen einer Rasse und verbessert die
/// Eigenschaften der übergebenen Schiffes.
/// @param pShip Schiff welches durch Spezialforschungen eventuell verbessert wird
/// @param pShipOwner Zeiger auf den Besitzer des Schiffes
void CBotf2Doc::AddSpecialResearchBoniToShip(CShips* pShip, CMajor* pShipOwner) const
{
	if (!pShip || !pShipOwner)
		return;

	CResearchInfo* pInfo = pShipOwner->GetEmpire()->GetResearch()->GetResearchInfo();
	if (!pInfo)
		return;

	// mögliche Verbesserungen durch die Spezialforschung werden hier beachtet
	// Spezialforschung #0: "Waffentechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% erhoehter Phaserschaden
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < pShip->GetBeamWeapons()->GetSize(); i++)
			{
				USHORT oldPower = pShip->GetBeamWeapons()->GetAt(i).GetBeamPower();
				pShip->GetBeamWeapons()->GetAt(i).SetBeamPower(oldPower + (oldPower * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(1) / 100));
			}
		}
		// 20% erhoehte Torpedogenauigkeit
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < pShip->GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldAcc = pShip->GetTorpedoWeapons()->GetAt(i).GetAccuracy();
				pShip->GetTorpedoWeapons()->GetAt(i).SetAccuracy(oldAcc + (oldAcc * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(2) / 100));
			}
		}
		// 20% erhoehte Schussfreuquenz
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < pShip->GetBeamWeapons()->GetSize(); i++)
			{
				BYTE oldRate = pShip->GetBeamWeapons()->GetAt(i).GetRechargeTime();
				pShip->GetBeamWeapons()->GetAt(i).SetRechargeTime(oldRate	- (oldRate * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(3) / 100));
			}
			for (int i = 0; i < pShip->GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldRate = pShip->GetTorpedoWeapons()->GetAt(i).GetTupeFirerate();
				pShip->GetTorpedoWeapons()->GetAt(i).SetTubeFirerate(oldRate - (oldRate * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(3) / 100));
			}
		}
	}
	// Spezialforschung #1: "Konstruktionstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% bessere Schilde
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			UINT maxShield = pShip->GetShield()->GetMaxShield();
			BYTE shieldType = pShip->GetShield()->GetShieldType();
			BOOLEAN regenerative = pShip->GetShield()->GetRegenerative();
			pShip->GetShield()->ModifyShield((maxShield + (maxShield * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(1) / 100)), shieldType, regenerative);
		}
		// 20% bessere Hülle
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			BOOLEAN doubleHull = pShip->GetHull()->GetDoubleHull();
			BOOLEAN ablative = pShip->GetHull()->GetAblative();
			BOOLEAN polarisation = pShip->GetHull()->GetPolarisation();
			UINT baseHull = pShip->GetHull()->GetBaseHull();
			BYTE hullMaterial = pShip->GetHull()->GetHullMaterial();
			pShip->GetHull()->ModifyHull(doubleHull, (baseHull + (baseHull * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(2) / 100)), hullMaterial,ablative,polarisation);
		}
		// 50% stärkere Scanner
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		{
			USHORT scanPower = pShip->GetScanPower();
			pShip->SetScanPower(scanPower + (scanPower * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(3) / 100));
		}
	}
	// Spezialforschung #2: "allgemeine Schiffstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// erhoehte Reichweite für Schiffe mit zuvor kurzer Reichweite
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			if (pShip->GetRange(false) == SHIP_RANGE::SHORT)
				pShip->SetRange((SHIP_RANGE::Typ)(pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(1)));
		}
		// erhoehte Geschwindigkeit für Schiffe mit Geschwindigkeit 1
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			if (pShip->GetSpeed(false) == 1)
				pShip->SetSpeed((BYTE)(pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(2)));
		}
	}
	// Spezialforschung #3: "friedliche Schiffstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED && pShip->GetShipType() <= SHIP_TYPE::COLONYSHIP)
	{
		// 25% erhoehte Transportkapazitaet
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			USHORT storage = pShip->GetStorageRoom();
			pShip->SetStorageRoom(storage + (storage * pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(1) / 100));
		}
		// keine Unterhaltskosten
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			pShip->SetMaintenanceCosts(0);
		}
	}
}

/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
void CBotf2Doc::BuildTroop(BYTE ID, CPoint ko)
{
	// Mal Testweise paar Truppen anlegen
	GetSystem(ko.x, ko.y).AddTroop((CTroop*)&m_TroopInfo.GetAt(ID));
	CString sRace = GetSector(ko.x, ko.y).GetOwnerOfSector();
	if (sRace == "")
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sRace));
	ASSERT(pMajor);

	int n = GetSystem(ko.x, ko.y).GetTroops()->GetUpperBound();

	// Spezialforschung #4: "Truppen"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% verbesserte Offensive
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			BYTE power = GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).GetOffense();
			GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).SetOffense(
				power + (power * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(1) / 100));
		}
		// 500 Erfahrungspunkte dazu -> erste Stufe
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).AddExperiancePoints(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(2));
		}
	}

}

// Funktion generiert die Starmaps, so wie sie nach Rundenberechnung auch angezeigt werden können.
void CBotf2Doc::GenerateStarmap(const CString& sOnlyForRaceID)
{
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Starmaps aller Majors löschen und neu anlegen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
			continue;

		CMajor* pMajor = it->second;
		pMajor->CreateStarmap();
	}
	// Anomalien beachten (ist für jede Starmap gleich, daher statisch)
	CStarmap::SynchronizeWithAnomalies(m_Sectors);

	// Starmaps generieren
	for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
	{
		const CSystem& system = GetSystemForSector(*sector);
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
				continue;

			CMajor* pMajor = it->second;
			// Wenn in dem System eine aktive Werft ist bzw. eine Station/Werft im Sektor ist
			if ((system.GetProduction()->GetShipYard() == TRUE && system.GetOwnerOfSystem() == pMajor->GetRaceID())
				|| sector->GetShipPort(pMajor->GetRaceID()))
			{
				pMajor->GetStarmap()->AddBase(Sector(sector->GetKO()),
					pMajor->GetEmpire()->GetResearch()->GetPropulsionTech());
			}

			if (sector->GetSunSystem())
			{
				if (sector->GetOwnerOfSector() == it->first || sector->GetOwnerOfSector() == "")
				{
					CMajor* pMajor = it->second;
					pMajor->GetStarmap()->AddKnownSystem(Sector(sector->GetKO()));
				}
			}
		}
	}

	// Jetzt die Starmap abgleichen, das wir nicht auf Gebiete fliegen können, wenn wir einen NAP mit einer Rasse haben
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
			continue;

		set<CString> NAPRaces;
		for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			if (it->first != itt->first && it->second->GetAgreement(itt->first) == DIPLOMATIC_AGREEMENT::NAP)
				NAPRaces.insert(itt->first);
		// interne Starmap für KI syncronisieren
		it->second->GetStarmap()->SynchronizeWithMap(m_Sectors, &NAPRaces);
	}

	// nun die Berechnung für den Außenpostenbau vornehmen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
			continue;

		CMajor* pMajor = it->second;
		if (!pMajor->AHumanPlays())
			pMajor->GetStarmap()->SetBadAIBaseSectors(m_Sectors, it->first);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc Diagnose

#ifdef _DEBUG
void CBotf2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBotf2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc Befehle

/////////////////////////////////////////////////////////////////////////////
// private Funktionen
/////////////////////////////////////////////////////////////////////////////

/// Diese Funktion führt allgemeine Berechnung durch, die immer zu Beginn der NextRound-Calculation stattfinden
/// müssen. So werden z.B. alte Nachrichten gelöscht, die Statistiken berechnet usw..
void CBotf2Doc::CalcPreDataForNextRound()
{
	m_iRound++;

	ASSERT(GetPlayersRace());

	// Berechnungen der neuen Runde
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// Alle alten Nachrichten  aus letzter Runde löschen
		pMajor->GetEmpire()->ClearMessagesAndEvents();
		// Bevölkerungsunterstützungskosten auf NULL setzen
		pMajor->GetEmpire()->SetPopSupportCosts(0);
		// verbleibende Vertragsdauer mit anderen Majorraces berechnen und gegebenenfalls Nachrichten und diplomatische Auswirkungen anwenden
		network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
		if (pMajor->DecrementAgreementsDuration(pmMajors))
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		// wird das Imperium von einem Menschen oder vom Computer gespielt
		if (client != network::RACE_NONE && server.IsPlayedByClient(client))
			pMajor->SetHumanPlayer(true);
		else
			pMajor->SetHumanPlayer(false);
	}

	// Schiffe, welche nur auf einem bestimmten System baubar sind, z.B. Schiffe von Minorraces, den Besitzer wieder
	// auf MINORNUMBER setzen. In der Funktion, welche in einem System die baubaren Schiffe berechnet, wird dieser
	// Wert dann auf die richtige Rasse gesetzt. Jeder der das System dann besitzt, kann dieses Schiff bauen
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		if (!m_ShipInfoArray.GetAt(i).GetOnlyInSystem().IsEmpty())
			m_ShipInfoArray.GetAt(i).SetRace(MINORNUMBER);


	for(std::vector<CSector>::const_iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
	{
		if (sector->GetSunSystem())
		{
			CSystem& system = GetSystemForSector(*sector);
			system.SetBlockade(0);
			system.ClearDisabledProductions();
		}
	}

	//f(x):=min(731,max(14,trunc(743-x^3)))
	m_fStardate += (float)(min(731, max(14, 743-pow((float)m_Statistics.GetAverageTechLevel(),3.0f))));
}

/// Diese Funktion berechnet den kompletten Systemangriff.
void CBotf2Doc::CalcSystemAttack()
{
	// Systemangriff durchführen
	// Set mit allen Minors, welche während eines Systemangriffs vernichtet wurden. Diese werden am Ende der
	// Berechnung aus der Liste entfernt
	set<CString> sKilledMinors;
	CArray<CPoint> fightInSystem;
	for (CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		if (y->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			BOOLEAN okay = TRUE;
			// Checken dass in diesem System nicht schon ein Angriff durchgeführt wurde
			for (int x = 0; x < fightInSystem.GetSize(); x++)
				if (fightInSystem.GetAt(x) == y->second->GetKO())
				{
					okay = FALSE;
					break;
				}

			// nur wenn das Schiff und Schiffe in der Flotte ungetarnt sind
			if(!y->second->CanHaveOrder(SHIP_ORDER::ATTACK_SYSTEM, false)) {
				y->second->UnsetCurrentOrder();
				okay = FALSE;
			}

			// wenn in dem System noch kein Angriff durchgeführt wurde kann angegriffen werden
			if (okay)
			{
				CPoint p = y->second->GetKO();
				// Besitzer des Systems (hier Sector wegen Minors) vor dem Systemangriff
				CString sDefender = GetSector(p.x, p.y).GetOwnerOfSector();
				// Angreifer bzw. neuer Besitzer des Systems nach dem Angriff
				set<CString> attackers;
				for (CShipMap::const_iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
				{
					if (i->second->GetKO() == p && i->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
					{
						const CString& sOwner = i->second->GetOwnerOfShip();
						if (!sOwner.IsEmpty())
							attackers.insert(sOwner);
					}
				}

				CAttackSystem* attackSystem = new CAttackSystem();

				CRace* defender = NULL;
				if (!sDefender.IsEmpty())
					defender = m_pRaceCtrl->GetRace(sDefender);
				// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
				if (defender && defender->IsMinor() && GetSector(p.x, p.y).GetTakenSector() == FALSE)
				{
					attackSystem->Init(defender, &GetSystem(p.x, p.y), &m_ShipMap, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn eine Majorrace in dem System lebt
				else if (defender && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
				{
					attackSystem->Init(defender, &GetSystem(p.x, p.y), &m_ShipMap, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn niemand mehr in dem System lebt, z.B. durch Rebellion
				else
				{
					attackSystem->Init(NULL, &GetSystem(p.x, p.y), &m_ShipMap, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Ein Systemangriff verringert die Moral in allen Systemen, die von uns schon erobert wurden und zuvor
				// der Majorrace gehörten, deren System hier angegriffen wird
				if (!sDefender.IsEmpty())
					for (int j = 0 ; j < STARMAP_SECTORS_VCOUNT; j++)
						for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
							if (GetSector(i, j).GetTakenSector() == TRUE && GetSector(i, j).GetColonyOwner() == sDefender
								&& attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
								GetSystem(i, j).SetMoral(-rand()%5);

				// Wurde das System mit Truppen erobert, so wechselt es den Besitzer
				if (attackSystem->Calculate())
				{
					CString attacker = GetSystem(p.x, p.y).GetOwnerOfSystem();
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(attacker));
					ASSERT(pMajor);
					//* Der Besitzer des Systems wurde in der Calculate() Funktion schon auf den neuen Besitzer
					//* umgestellt. Der Besitzer des Sektors ist aber noch der alte, wird hier dann auf einen
					//* eventuell neuen Besitzer umgestellt.

					// Wenn in dem System eine Minorrace beheimatet ist und das System nicht vorher schon von jemand
					// anderem militärisch erobert wurde oder die Minorace bei einem anderen Imperium schon vermitgliedelt
					// wurde, dann muss diese zuerst die Gebäude bauen
					if (GetSector(p.x, p.y).GetMinorRace() == TRUE && GetSector(p.x, p.y).GetTakenSector() == FALSE && defender != NULL && defender->IsMinor())
					{
						CMinor* pMinor = dynamic_cast<CMinor*>(defender);
						ASSERT(pMinor);
						pMinor->SetSubjugated(true);
						// Wenn das System noch keiner Majorrace gehört, dann Gebäude bauen
						GetSystem(p.x, p.y).BuildBuildingsForMinorRace(&GetSector(p.x, p.y), &BuildingInfo, m_Statistics.GetAverageTechLevel(), pMinor);
						// Sektor gilt ab jetzt als erobert.
						GetSector(p.x, p.y).SetTakenSector(TRUE);
						GetSector(p.x, p.y).SetOwned(TRUE);
						GetSector(p.x, p.y).SetOwnerOfSector(attacker);
						// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
						pMinor->SetRelation(attacker, -100);
						// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
						GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
						// ist die Moral unter 50, so wird sie auf 50 gesetzt
						if (GetSystem(p.x, p.y).GetMoral() < 50)
							GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());
						CString param = GetSector(p.x, p.y).GetName();
						CString eventText = "";

						// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
						// bekannte Imperien die Nachricht der Unterwerfung senden
						pMinor->GetIncomingDiplomacyNews()->clear();
						pMinor->GetOutgoingDiplomacyNews()->clear();
						map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
						{
							// ausgehende Nachrichten löschen
							for (UINT i = 0; i < it->second->GetOutgoingDiplomacyNews()->size(); i++)
								if (it->second->GetOutgoingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
									|| it->second->GetOutgoingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
									it->second->GetOutgoingDiplomacyNews()->erase(it->second->GetOutgoingDiplomacyNews()->begin() + i--);
							// eingehende Nachrichten löschen
							for (UINT i = 0; i < it->second->GetIncomingDiplomacyNews()->size(); i++)
								if (it->second->GetIncomingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
									|| it->second->GetIncomingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
									it->second->GetIncomingDiplomacyNews()->erase(it->second->GetIncomingDiplomacyNews()->begin() + i--);

							// An alle Majors die die Minor kennen die Nachricht schicken, dass diese unterworfen wurde
							if (it->second->IsRaceContacted(pMinor->GetRaceID()))
							{
								CMessage message;
								message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetRaceName()), MESSAGE_TYPE::MILITARY, param, p, 0);
								it->second->GetEmpire()->AddMessage(message);
								if (it->second->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
						// Eventnachricht an den Eroberer (System erobert)
						eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CMessage message;
							message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
						}
					}
					// Wenn das System einer Minorrace gehört, eingenommen wurde und somit befreit wird
					else if (GetSector(p.x, p.y).GetMinorRace() == TRUE && GetSector(p.x, p.y).GetTakenSector() == TRUE && defender != NULL && defender->IsMajor())
					{
						// Die Beziehung zur Majorrace, die das System vorher besaß verschlechtert sich
						defender->SetRelation(attacker, -rand()%50);
						// Die Beziehung zu der Minorrace verbessert sich auf Seiten des Retters
						CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(p.x, p.y).GetName());
						ASSERT(pMinor);
						pMinor->SetRelation(attacker, rand()%50);
						pMinor->SetSubjugated(false);
						// Eventnachricht an den, der das System verloren hat (erobertes Minorracesystem wieder verloren)
						CString param = GetSector(p.x, p.y).GetName();

						CMajor* def = dynamic_cast<CMajor*>(defender);
						CString eventText(def->GetMoralObserver()->AddEvent(17, def->GetRaceMoralNumber(), param));
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CMessage message;
							message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
							def->GetEmpire()->AddMessage(message);
							if (def->IsHumanPlayer())
							{
								network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
						}
						// Eventnachricht an den Eroberer (Minorracesystem befreit)
						param = pMinor->GetRaceName();
						eventText = pMajor->GetMoralObserver()->AddEvent(13, pMajor->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CMessage message;
							message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
						}
						// Sektor gilt ab jetzt als nicht mehr erobert.
						GetSector(p.x, p.y).SetTakenSector(FALSE);
						GetSector(p.x, p.y).SetOwned(FALSE);
						GetSector(p.x, p.y).SetOwnerOfSector(pMinor->GetRaceID());
						GetSystem(p.x, p.y).SetOwnerOfSystem("");
						// Moral in dem System um rand()%50+25 erhöhen
						GetSystem(p.x, p.y).SetMoral(rand()%50+25);
					}
					// Eine andere Majorrace besaß das System
					else
					{
						// Beziehung zum ehemaligen Besitzer verschlechtert sich
						if (defender != NULL && defender->GetRaceID() != attacker)
							defender->SetRelation(attacker, -rand()%50);
						// Wenn das System zurückerobert wird, dann gilt es als befreit
						if (GetSector(p.x, p.y).GetColonyOwner() == attacker)
						{
							GetSector(p.x, p.y).SetTakenSector(FALSE);
							CString param = GetSector(p.x, p.y).GetName();
							// Eventnachricht an den Eroberer (unser ehemaliges System wieder zurückerobert)
							CString eventText(pMajor->GetMoralObserver()->AddEvent(14, pMajor->GetRaceMoralNumber(), param));
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							if (defender != NULL && defender->GetRaceID() != attacker && defender->IsMajor())
							{
								CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
								// Eventnachricht an den, der das System verloren hat (unser erobertes System verloren)
								eventText = pDefenderMajor->GetMoralObserver()->AddEvent(17, pDefenderMajor->GetRaceMoralNumber(), param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									CMessage message;
									message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
									pDefenderMajor->GetEmpire()->AddMessage(message);
									if (pDefenderMajor->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
							}
							// Moral in dem System um rand()%25+10 erhöhen
							GetSystem(p.x, p.y).SetMoral(rand()%25+10);
						}
						// Handelte es sich dabei um das Heimatsystem einer Rasse
						else if (defender != NULL && defender->IsMajor() && GetSector(p.x, p.y).GetOwnerOfSector() == defender->GetRaceID() && GetSector(p.x, p.y).GetName() == dynamic_cast<CMajor*>(defender)->GetHomesystemName())
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
							CString param = GetSector(p.x, p.y).GetName();
							CString eventText(pDefenderMajor->GetMoralObserver()->AddEvent(15, pDefenderMajor->GetRaceMoralNumber(), param));
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
								pDefenderMajor->GetEmpire()->AddMessage(message);
								if (pDefenderMajor->IsHumanPlayer())
								{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							// Eventnachricht an den Eroberer (System erobert)
							eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							// Sektor gilt ab jetzt als erobert.
							GetSector(p.x, p.y).SetTakenSector(TRUE);
							// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
							GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
							// ist die Moral unter 50, so wird sie auf 50 gesetzt
							if (GetSystem(p.x, p.y).GetMoral() < 50)
								GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());
						}
						// wurde das System erobert und obere Bedingungen traten nicht ein
						else
						{
							CString param = GetSector(p.x, p.y).GetName();
							// Hat eine andere Majorrace die Minorrace vermitgliedelt, so unterwerfen wir auch diese Minorrace
							if (GetSector(p.x, p.y).GetMinorRace())
							{
								CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(p.x, p.y).GetName());
								ASSERT(pMinor);
								pMinor->SetSubjugated(true);
								// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
								pMinor->SetRelation(attacker, -100);

								CString param = GetSector(p.x, p.y).GetName();

								// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
								// bekannte Imperien die Nachricht der Unterwerfung senden
								pMinor->GetIncomingDiplomacyNews()->clear();
								pMinor->GetOutgoingDiplomacyNews()->clear();
								map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
								for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
								{
									// ausgehende Nachrichten löschen
									for (UINT i = 0; i < it->second->GetOutgoingDiplomacyNews()->size(); i++)
										if (it->second->GetOutgoingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
											|| it->second->GetOutgoingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
											it->second->GetOutgoingDiplomacyNews()->erase(it->second->GetOutgoingDiplomacyNews()->begin() + i--);
									// eingehende Nachrichten löschen
									for (UINT i = 0; i < it->second->GetIncomingDiplomacyNews()->size(); i++)
										if (it->second->GetIncomingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
											|| it->second->GetIncomingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
											it->second->GetIncomingDiplomacyNews()->erase(it->second->GetIncomingDiplomacyNews()->begin() + i--);

									// An alle Majors die die Minor kennen die Nachricht schicken, dass diese unterworden wurde
									if (it->second->IsRaceContacted(pMinor->GetRaceID()))
									{
										CMessage message;
										message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetRaceName()), MESSAGE_TYPE::MILITARY, param, p, 0);
										it->second->GetEmpire()->AddMessage(message);
										if (it->second->IsHumanPlayer())
										{
											network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
											m_iSelectedView[client] = EMPIRE_VIEW;
										}
									}
								}
							}

							// Sektor gilt ab jetzt als erobert.
							GetSector(p.x, p.y).SetTakenSector(TRUE);
							// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
							GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
							// ist die Moral unter 50, so wird sie auf 50 gesetzt
							if (GetSystem(p.x, p.y).GetMoral() < 50)
								GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());

							CString eventText;
							// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)

							//We later were dereferencing defender anyway; after casting to CMajor.
							//Not sure whether defender should be allowed to be NULL here.
							assert(defender);
							if (defender->GetRaceID() != attacker && defender->IsMajor()) {
								CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
								eventText = pDefenderMajor->GetMoralObserver()->AddEvent(16, pDefenderMajor->GetRaceMoralNumber(), param);
							}
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
								//defender seems to be of type MAJOR here for sure ?
								assert(defender->IsMajor());
								CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
								assert(pDefenderMajor);
								pDefenderMajor->GetEmpire()->AddMessage(message);
								if (pDefenderMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}

							// Eventnachricht an den Eroberer (System erobert)
							eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
						GetSector(p.x, p.y).SetOwnerOfSector(attacker);
					}
					// Gebäude die nach Eroberung automatisch zerstört werden
					GetSystem(p.x, p.y).RemoveSpecialRaceBuildings(&this->BuildingInfo);
					// Variablen berechnen und Gebäude besetzen
					GetSystem(p.x, p.y).CalculateNumberOfWorkbuildings(&this->BuildingInfo);
					GetSystem(p.x, p.y).SetWorkersIntoBuildings();

					// war der Verteidiger eine Majorrace und wurde sie durch die Eroberung komplett ausgelöscht,
					// so bekommt der Eroberer einen kräftigen Moralschub
					if (defender != NULL && defender->IsMajor() && !attacker.IsEmpty() && pMajor && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
						// Anzahl der noch verbleibenden Systeme berechnen
						pDefenderMajor->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
						// hat der Verteidiger keine Systeme mehr, so bekommt der neue Besitzer den Bonus
						if (pDefenderMajor->GetEmpire()->GetSystemList()->GetSize() == 0)
						{
							CString param = pDefenderMajor->GetRaceName();
							CString eventText = pMajor->GetMoralObserver()->AddEvent(0, pMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
					}

					// erfolgreiches Invasionsevent für den Angreifer einfügen (sollte immer ein Major sein)
					if (!attacker.IsEmpty() && pMajor && pMajor->IsHumanPlayer())
						pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(attacker, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_" + pMajor->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));

					// Invasionsevent für den Verteidiger einfügen
					if (defender != NULL && defender->IsMajor() && dynamic_cast<CMajor*>(defender)->IsHumanPlayer() && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
						dynamic_cast<CMajor*>(defender)->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(sDefender, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_" + defender->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));
				}
				// Wurde nur bombardiert, nicht erobert
				else
				{
					for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
					{
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
						if (!pMajor)
							continue;

						// Erstmal die Beziehung zu der Rasse verschlechtern, der das System gehört
						if (defender != NULL && defender->GetRaceID() != pMajor->GetRaceID())
							defender->SetRelation(pMajor->GetRaceID(), -rand()%10);
					}
					
					// Wenn die Bevölkerung des Systems auf NULL geschrumpft ist, dann ist dieses System verloren
					if (GetSystem(p.x, p.y).GetHabitants() <= 0.000001f)
					{
						// Bei einer Minorrace wird es komplizierter. Wenn diese keine Systeme mehr hat, dann ist diese
						// aus dem Spiel verschwunden. Alle Einträge in der Diplomatie müssen daher gelöscht werden
						if (GetSector(p.x, p.y).GetMinorRace())
						{
							if (CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(p.x, p.y).GetName()))
							{
								// Alle Effekte, Events usw. wegen der Auslöschung der Minorrace verarbeiten
								CalcEffectsMinorEleminated(pMinor);

								// Eventnachricht: #21	Eliminate a Minor Race Entirely
								for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
								{
									CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
									if (!pMajor)
										continue;
									
									CString param = pMinor->GetRaceName();
									CString eventText = pMajor->GetMoralObserver()->AddEvent(21, pMajor->GetRaceMoralNumber(), param);
									CMessage message;
									message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
									pMajor->GetEmpire()->AddMessage(message);
								}

								// Rasse zum Löschen vormerken
								sKilledMinors.insert(pMinor->GetRaceID());
							}
							else
							{
								ASSERT(FALSE);
								GetSector(p.x, p.y).SetMinorRace(false);
							}
						}
						// Bei einer Majorrace verringert sich nur die Anzahl der Systeme (auch konnte dies das
						// Minorracesystem von oben gewesen sein, hier verliert es aber die betroffene Majorrace)
						if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							
							CString eventText = "";
							CString param = GetSector(p.x, p.y).GetName();
							if (GetSector(p.x, p.y).GetName() == pDefenderMajor->GetHomesystemName())
							{
								// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
								eventText = pDefenderMajor->GetMoralObserver()->AddEvent(15, pDefenderMajor->GetRaceMoralNumber(), param);
							}
							else
							{
								// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
								eventText = pDefenderMajor->GetMoralObserver()->AddEvent(16, pDefenderMajor->GetRaceMoralNumber(), param);
							}
							
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CMessage message;
								message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
								pDefenderMajor->GetEmpire()->AddMessage(message);
								if (pDefenderMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}

						GetSystem(p.x, p.y).SetOwnerOfSystem("");
						GetSector(p.x, p.y).SetOwnerOfSector("");
						GetSector(p.x, p.y).SetColonyOwner("");
						GetSector(p.x, p.y).SetTakenSector(FALSE);
						GetSector(p.x, p.y).SetOwned(FALSE);

						// war der Verteidiger eine Majorrace und wurde sie durch den Verlust des Systems komplett ausgelöscht,
						// so bekommt der Eroberer einen kräftigen Moralschub
						if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
								if (!pMajor)
									continue;
								
								// Anzahl der noch verbleibenden Systeme berechnen
								pDefenderMajor->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
								// hat der Verteidiger keine Systeme mehr, so bekommt der neue Besitzer den Bonus
								if (pDefenderMajor->GetEmpire()->GetSystemList()->IsEmpty())
								{
									CString sParam		= pDefenderMajor->GetRaceName();
									CString sEventText	= pMajor->GetMoralObserver()->AddEvent(0, pMajor->GetRaceMoralNumber(), sParam);
									// Eventnachricht hinzufügen
									if (!sEventText.IsEmpty())
									{
										CMessage message;
										message.GenerateMessage(sEventText, MESSAGE_TYPE::MILITARY, sParam, p, 0, 1);
										pMajor->GetEmpire()->AddMessage(message);
										if (pMajor->IsHumanPlayer())
										{
											network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
											m_iSelectedView[client] = EMPIRE_VIEW;
										}
									}
								}
							}
						}
					}
					// Bombardierung hat die Rasse nicht komplett ausgelöscht
					else
					{
						// Eventnachrichten nicht jedesmal, sondern nur wenn Gebäude vernichtet wurden oder
						// mindst. 3% der Bevölkerung vernichtet wurden
						if (attackSystem->GetDestroyedBuildings() > 0 || attackSystem->GetKilledPop() >= GetSystem(p.x, p.y).GetHabitants() * 0.03)
						{							
							CString param = GetSector(p.x, p.y).GetName();
							for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
								if (!pMajor)
									continue;
								
								CString eventText = "";
								// Wenn das System nicht durch eine Rebellion verloren ging, sondern noch irgendwem gehört
								if (defender != NULL)
									eventText = pMajor->GetMoralObserver()->AddEvent(19, pMajor->GetRaceMoralNumber(), param);
								// Wenn das System mal uns gehört hatte und durch eine Rebellion verloren ging
								else if (GetSector(p.x, p.y).GetColonyOwner() == pMajor->GetRaceID() && defender == NULL)
									eventText = pMajor->GetMoralObserver()->AddEvent(20, pMajor->GetRaceMoralNumber(), param);
								// Eventnachricht für Agressor hinzufügen
								if (!eventText.IsEmpty())
								{
									CMessage message;
									message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
									pMajor->GetEmpire()->AddMessage(message);
									if (pMajor->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
							}
							
							// Eventnachricht über Bombardierung für Verteidiger erstellen und hinzufügen
							if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
							{
								CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
								CString eventText = pDefenderMajor->GetMoralObserver()->AddEvent(22, pDefenderMajor->GetRaceMoralNumber(), param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									CMessage message;
									message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
									pDefenderMajor->GetEmpire()->AddMessage(message);								
									if (pDefenderMajor->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
							}
						}
					}
					// Eventgrafiken hinzufügen
					// für den/die Angreifer
					for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
					{
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
						if (!pMajor)
							continue;
						
						// reine Bombardierung
						if (pMajor->IsHumanPlayer())
						{
							if (!attackSystem->IsTroopsInvolved())
								pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(pMajor->GetRaceID(), "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_AGRESSOR_" + pMajor->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));
							// gescheitere Invasion
							else if (GetSystem(p.x, p.y).GetHabitants() > 0.000001f)
								pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(pMajor->GetRaceID(), "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_" + pMajor->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));
						}

					}
					// für den Verteidiger
					if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
						if (pDefenderMajor->IsHumanPlayer())
						{
							// reine Bombardierung
							if (!attackSystem->IsTroopsInvolved())
								pDefenderMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(defender->GetRaceID(), "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_DEFENDER_" + defender->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));
							// gescheitere Invasion
							else if (GetSystem(p.x, p.y).GetHabitants() > 0.000001f)
								pDefenderMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(defender->GetRaceID(), "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, GetSector(p.x, p.y).GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_" + defender->GetRaceID(), FALSE, GetSector(p.x, p.y).GetName())));
						}
					}
				}

				// Nachrichten hinzufügen
				for (int i = 0; i < attackSystem->GetNews()->GetSize(); )
				{
					for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
					{
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
						if (!pMajor)
							continue;
						
						CMessage message;
						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), MESSAGE_TYPE::MILITARY, GetSector(p.x, p.y).GetName(), p, 0);
						pMajor->GetEmpire()->AddMessage(message);
						if (pMajor->IsHumanPlayer())
						{
							network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
							m_iSelectedView[client] = EMPIRE_VIEW;
						}
					}
					if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
						CMessage message;
						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), MESSAGE_TYPE::MILITARY, GetSector(p.x, p.y).GetName(), p, 0);
						pDefenderMajor->GetEmpire()->AddMessage(message);
						if (pDefenderMajor->IsHumanPlayer())
						{
							network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
							m_iSelectedView[client] = EMPIRE_VIEW;
						}
					}
					attackSystem->GetNews()->RemoveAt(i);
				}
				delete attackSystem;
				fightInSystem.Add(p);
			}
		}
	}

	// alle vernichteten Minorraces nun aus dem Feld löschen
	for (set<CString>::const_iterator it = sKilledMinors.begin(); it != sKilledMinors.end(); ++it)
		m_pRaceCtrl->RemoveRace(*it);

	// Schiffsfeld nochmal durchgehen und alle Schiffe ohne Hülle aus dem Feld entfernen.
	// Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte.
	if (fightInSystem.GetSize() > 0)
		for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end();)
		{
			CRace* race = m_pRaceCtrl->GetRace(i->second->GetOwnerOfShip());
			assert(race);
			if(i->second->RemoveDestroyed(*race, m_iRound, CResourceManager::GetString("SYSTEMATTACK"),
					CResourceManager::GetString("DESTROYED"))) {
				++i;
				continue;
			}
			RemoveShip(i);
		}
}

/// Diese Funktion berechnet alles im Zusammenhang mit dem Geheimdienst.
void CBotf2Doc::CalcIntelligence()
{
	// der Geheimdienst muss vor der Forschung abgehandelt werden, da es durch Geheimdienstaktionen dazu kommen kann,
	// dass aktuell produzierte Forschungspunkte gestohlen werden. Diese werden dem Opfer abgezogen und dem Akteur
	// hinzugefügt. Erst danach sollte die Forschung behandelt werden. Wird die Forschung zuvor aufgerufen macht es
	// spielmechanisch keinen Sinn.
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// das Objekt für alle Berechnung mit Geheimdienstkontext anlegen
	CIntelCalc* intel = new CIntelCalc(this);
	if (intel)
	{
		// zuerst werden die ganzen Berechnungen durchgeführt, ohne das Punkte vorher von irgendeiner Rasse dazugerechnet werden.
		// Dadurch haben alle Rassen die selbe Chance.
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			intel->StartCalc(it->second);
		// danach werden die Punkte dazuaddiert und zum Schluss werden die einzelnen Depotpunkte etwas reduziert.
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			intel->AddPoints(it->second);
			// die Intelpunkte in den Lagern werden ganz am Ende abgezogen.
			intel->ReduceDepotPoints(it->second);
			CIntelligence* pIntel = it->second->GetEmpire()->GetIntelligence();
			// Boni wieder löschen, damit für die neue und nächste Runde neue berechnet werden können
			pIntel->ClearBoni();
			// wenn neue Geheimdienstnachrichten vorhanden sind die Meldung im Impieriumsmenü erstellen
			if (pIntel->GetIntelReports()->IsReportAdded())
			{
				// die Sortierung der Geheimdienstberichte
				pIntel->GetIntelReports()->SortAllReports();

				CMessage message;
				message.GenerateMessage(CResourceManager::GetString("WE_HAVE_NEW_INTELREPORTS"), MESSAGE_TYPE::SECURITY, "", NULL, FALSE, 4);
				it->second->GetEmpire()->AddMessage(message);

				network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
				SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_INTELNEWS, client, 0, 1.0f};
				m_SoundMessages[client].Add(entry);

				BOOLEAN addSpy = FALSE;
				BOOLEAN addSab = FALSE;
				for (int j = pIntel->GetIntelReports()->GetAllReports()->GetUpperBound(); j >= 0; j--)
				{
					CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(j);
					if (intelObj->GetEnemy() != it->first && intelObj->GetRound() == this->GetCurrentRound())
					{
						CString eventText = "";
						if (intelObj->GetIsSpy() && !addSpy)
						{
							eventText = it->second->GetMoralObserver()->AddEvent(59, it->second->GetRaceMoralNumber());
							addSpy = TRUE;
						}
						else if (intelObj->GetIsSabotage() && !addSab)
						{
							eventText = it->second->GetMoralObserver()->AddEvent(60, it->second->GetRaceMoralNumber());
							addSab = TRUE;
						}
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, MESSAGE_TYPE::SECURITY, "", NULL, FALSE, 4);
							it->second->GetEmpire()->AddMessage(message);
						}
					}
					if (addSpy && addSab)
						break;
				}
			}
		}
		delete intel;
		intel = NULL;
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->second->IsHumanPlayer())
			if (it->second->GetEmpire()->GetIntelligence()->GetIntelReports()->IsReportAdded())
			{
				network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
}

/// Diese Funktion berechnet die Forschung eines Imperiums
void CBotf2Doc::CalcResearch()
{
	// Forschungsboni, die die Systeme machen holen. Wir benötigen diese dann für die CalculateResearch Funktion
	std::map<CString, CSystemProd::RESEARCHBONI> researchBoni;
	for(std::vector<CSystem>::const_iterator system = m_Systems.begin(); system != m_Systems.end(); ++system) {
		const CString& owner = system->GetOwnerOfSystem();
		if(owner == "") continue;
		const CSystemProd* prod = system->GetProduction();
		researchBoni[owner] += prod->GetResearchBoni();
	}

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;

		pMajor->GetEmpire()->GetResearch()->SetResearchBoni(researchBoni[it->first].nBoni);
		const CString* news = pMajor->GetEmpire()->GetResearch()->CalculateResearch(pMajor->GetEmpire()->GetFP());
		network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

		for (int j = 0; j < 8; j++)		// aktuell 8 verschiedene Nachrichten mgl, siehe CResearch Klasse
		{
			// Wenn irgendwas erforscht wurde, wir also eine Nachricht erstellen wollen
			if (news[j] != "")
			{
				CMessage message;

				// flag setzen, wenn wir eine Spezialforschung erforschen dürfen
				if (j == 7)
				{
					// Spezialforschung kann erforscht werden
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_SCIENTISTNEWS, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
					message.GenerateMessage(news[j], MESSAGE_TYPE::RESEARCH, "", 0, FALSE, 1);
				}
				else
				{
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_NEWTECHNOLOGY, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;

						// Eventscreen für Forschung erstellen
						// gilt nur für die sechs normalen Forschungen
						if (j < 6)
							pMajor->GetEmpire()->GetEventMessages()->Add(new CEventResearch(pMajor->GetRaceID(), CResourceManager::GetString("RESEARCHEVENT_HEADLINE"), j));
					}
					message.GenerateMessage(news[j], MESSAGE_TYPE::RESEARCH, "", 0, FALSE);
				}

				pMajor->GetEmpire()->AddMessage(message);
			}
		}
	}

	// künstliche Intelligenz für Forschung
	CResearchAI AI;
	AI.Calc(this);
}

/// Diese Funktion berechnet die Auswirkungen von diplomatischen Angeboten und ob Minorraces Angebote an
/// Majorraces abgeben.
void CBotf2Doc::CalcDiplomacy()
{
	using namespace network;

	// zuerst alle Angebote senden
	CDiplomacyController::Send();

	// danach empfangen und reagieren
	CDiplomacyController::Receive();

	// Hinweis das Nachrichten eingegangen sind erstellen
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor->IsHumanPlayer() && pMajor->GetIncomingDiplomacyNews()->size() > 0)
		{
			network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
			SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_DIPLOMATICNEWS, client, 0, 1.0f};
			m_SoundMessages[client].Add(entry);
			m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}
}

////////////////////////////////////////////////
//BEGIN: helper functions for CalcOldRoundData()
static void ClearAllPoints(const std::map<CString, CMajor*>* pmMajors) {
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// hier setzen wir wieder die gesamten FP, SP und die gesamten Lager auf 0
		it->second->GetEmpire()->ClearAllPoints();
	}
}
void CBotf2Doc::UpdateGlobalBuildings(const CSystem& system) {
	// Jedes Sonnensystem wird durchgegangen und alle Gebäude des Systems werden in die Variable
	// m_GlobalBuildings geschrieben. Damit wissen welche Gebäude in der Galaxie stehen. Benötigt wird
	// dies z.B. um zu Überprüfen, ob max. X Gebäude einer bestimmten ID in einem Imperium stehen.
	for (int i = 0; i < system.GetAllBuildings()->GetSize(); i++)
	{
		USHORT nID = system.GetAllBuildings()->GetAt(i).GetRunningNumber();
		CString sRaceID = system.GetOwnerOfSystem();
		if (GetBuildingInfo(nID).GetMaxInEmpire() > 0)
			m_GlobalBuildings.AddGlobalBuilding(sRaceID, nID);
	}
	// Alle Gebäude und Updates, die sich aktuell auch in der Bauliste befinden, werden dem Feld hinzugefügt
	for (int i = 0; i < ALE; i++)
		if (system.GetAssemblyList()->GetAssemblyListEntry(i) > 0 && system.GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
		{
			USHORT nID = abs(system.GetAssemblyList()->GetAssemblyListEntry(i));
			CString sRaceID = system.GetOwnerOfSystem();
			if (GetBuildingInfo(nID).GetMaxInEmpire() > 0)
				m_GlobalBuildings.AddGlobalBuilding(sRaceID, nID);
		}
}
//END: helper functions for CalcOldRoundData()
////////////////////////////////////////////////
/// Diese Funktion berechnet das Planetenwachstum, die Aufträge in der Bauliste und sonstige Einstellungen aus der
/// alten Runde.
void CBotf2Doc::CalcOldRoundData()
{
	COldRoundDataCalculator calc(this);
	m_GlobalBuildings.Reset();
	ClearAllPoints(m_pRaceCtrl->GetMajors());

	for (std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
	{
		CSystem& system = GetSystemForSector(*sector);
		// Mögliche Is? Variablen für Terraforming und Stationbau erstmal auf FALSE setzen
		sector->ClearAllPoints();

		// Wenn im Sektor ein Sonnensystem existiert
		if (!sector->GetSunSystem())
			continue;
			
		if (system.GetOwnerOfSystem() == "")
		{
			// Planetenwachstum in jedem Sektor durchführen
			sector->LetPlanetsGrowth();
			continue;
		}
			
		CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(system.GetOwnerOfSystem()));
		assert(pMajor);
		if (!pMajor)
			continue;
		
		// Jetzt das produzierte Credits im System dem jeweiligen Imperium geben, Gebäude abreisen, Moral im System berechnen
		COldRoundDataCalculator::CreditsDestructionMoral(pMajor, system, this->BuildingInfo, m_fDifficultyLevel);

		// KI Anpassungen (KI bekommt zufälig etwas Deritium geschenkt)
		int diliAdd = COldRoundDataCalculator::DeritiumForTheAI(pMajor->AHumanPlays(), *sector, system, m_fDifficultyLevel);
		
		// Das Lager berechnen
		const BOOLEAN bIsRebellion = system.CalculateStorages(pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), diliAdd);
		// Wenn true zurückkommt, dann hat sich das System losgesagt
		if (bIsRebellion)
			calc.ExecuteRebellion(*sector, system, pMajor);
		
		// Hier mit einbeziehen, wenn die Bevölkerung an Nahrungsmangel stirbt
		if (system.GetFoodStore() < 0)
		{
			calc.ExecuteFamine(*sector, system, pMajor);
		}
		else
		{
			// Planetenwachstum für Hauptrassen durchführen
			sector->LetPlanetsGrowth();
		}
	
		// Wenn es keine Rebellion gab, dann Bau und KI im System berechnen
		if (!bIsRebellion)
		{
			assert(system.GetOwnerOfSystem() != "");
			calc.HandlePopulationEffects(*sector, system, pMajor);
			system.CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), sector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());

			// hier könnte die Energie durch Weltraummonster weggenommen werden!
			// Gebäude die Energie benötigen checken
			if (system.CheckEnergyBuildings(&this->BuildingInfo))
				calc.SystemMessage(*sector, pMajor, "BUILDING_TURN_OFF", MESSAGE_TYPE::SOMETHING, 2);
			
			// Die Bauaufträge in dem System berechnen. Außerdem wird hier auch die System-KI ausgeführt.
			if (!pMajor->AHumanPlays() || system.GetAutoBuild())
			{
				CSystemAI SAI(this);
				SAI.ExecuteSystemAI(sector->GetKO());
			}

			calc.Build(*sector, system, pMajor, this->BuildingInfo);
			// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
			system.CalculateNumberOfWorkbuildings(&this->BuildingInfo);
			// freie Arbeiter den Gebäuden zuweisen
			system.SetWorkersIntoBuildings();
		}
	
		// Globale Gebäude (X mal pro Imperium baubar) hinzufügen
		UpdateGlobalBuildings(system);
	}
}

/// Diese Funktion berechnet die Produktion der Systeme, was in den Baulisten gebaut werden soll und sonstige
/// Daten für die neue Runde.
void CBotf2Doc::CalcNewRoundData()
{
	CNewRoundDataCalculator new_round_data_calc(this);

	new_round_data_calc.CalcPreLoop();

	const map<CString, CMajor*>* const pmMajors = m_pRaceCtrl->GetMajors();

	// Hier werden jetzt die baubaren Gebäude für die nächste Runde und auch die Produktion in den einzelnen
	// Systemen berechnet. Können das nicht in obiger Hauptschleife machen, weil dort es alle globalen Gebäude
	// gesammelt werden müssen und ich deswegen alle Systeme mit den fertigen Bauaufträgen in dieser Runde einmal
	// durchgegangen sein muß.
	for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector) {
		CSystem& system = GetSystemForSector(*sector);
#ifdef DEVELOPMENT_VERSION
		CSanity::GetInstance()->SanityCheckSectorAndSystem(*sector, system, *this);
#endif
		const CString& system_owner = system.GetOwnerOfSystem();
		if (sector->GetSunSystem() && system_owner != "")
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(system_owner));
			assert(pMajor);
			CEmpire* empire = pMajor->GetEmpire();

			// Hier die Credits durch Handelsrouten berechnen und
			// Ressourcenrouten checken
			new_round_data_calc.CheckRoutes(*sector, system, pMajor);

			system.CalculateVariables(&this->BuildingInfo,
				empire->GetResearch()->GetResearchInfo(),
				sector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());

			const CSystemProd* const production = system.GetProduction();
			// Haben wir eine online Schiffswerft im System, dann ShipPort in dem Sektor setzen
			if (production->GetShipYard())
				sector->SetShipPort(TRUE, system_owner);
			CNewRoundDataCalculator::CalcMoral(*sector, system, m_TroopInfo);

			// Hier die gesamten Sicherheitsboni der Imperien berechnen
			CNewRoundDataCalculator::CalcIntelligenceBoni(production, empire->GetIntelligence());

			// Anzahl aller Ressourcen in allen eigenen Systemen berechnen
			for (int res = TITAN; res <= DERITIUM; res++)
				empire->SetStorageAdd(res, system.GetResourceStore(res));
		}//if (sector.GetSunSystem() && system_owner != "")

		// für jede Rasse Sektorsachen berechnen
		// Hier wird berechnet, was wir von der Karte alles sehen, welche Sektoren wir durchfliegen können
		// alles abhängig von unseren diplomatischen Beziehungen
		// Nun auch überprüfen, ob sich unsere Grenzen erweitern, wenn die MinorRasse eine Spaceflight-Rasse ist und wir mit
		// ihr eine Kooperations oder ein Bündnis haben
		CNewRoundDataCalculator::CalcExtraVisibilityAndRangeDueToDiplomacy(*sector, pmMajors, m_pRaceCtrl->GetMinors());
	}//for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector) {

	// Forschungsboni aus Spezialforschungen setzen, nachdem wir diese aus allen Systemen geholt haben
	//GetResearchBoniFromSpecialTechsAndSetThem(researchBonis, pmMajors);
	// Geheimdienstboni aus Spezialforschungen holen
	CNewRoundDataCalculator::GetIntelligenceBoniFromSpecialTechsAndSetThem(pmMajors);
}

/// Diese Funktion berechnet die kompletten Handelsaktivitäten.
void CBotf2Doc::CalcTrade()
{
	// Hier berechnen wir alle Handelsaktionen
	USHORT taxMoney[] = {0,0,0,0,0};	// alle Steuern auf eine Ressource

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	if (pmMajors->empty())
		AfxMessageBox("Error in CBotf2Doc::CalcTrade(): Could not get any major from race controller!");

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			float newTax = (float)pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(1);
			newTax = 1.0f + newTax / 100;
			pMajor->GetTrade()->SetTax(newTax);
		}
		pMajor->GetTrade()->CalculateTradeActions(pMajor, m_Systems, m_Sectors, taxMoney);
		for (int j = TITAN; j <= IRIDIUM; j++)
		{
			// plus Steuern, die durch Sofortkäufe von Bauaufträgen entstanden sind holen
			if (CTrade::GetMonopolOwner(j).IsEmpty() == false)
				if (CTrade::GetMonopolOwner(j) == pMajor->GetRaceID() || pMajor->IsRaceContacted(CTrade::GetMonopolOwner(j)) == true)
					taxMoney[j] += pMajor->GetTrade()->GetTaxesFromBuying()[j];
		}
	}
	// die Steuern durch den Handel den Monopolbesitzern gutschreiben und nach Monopolkäufen Ausschau halten
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		CString resName;
		switch(i)
		{
		case TITAN:		resName = CResourceManager::GetString("TITAN");		break;
		case DEUTERIUM: resName = CResourceManager::GetString("DEUTERIUM");	break;
		case DURANIUM:	resName = CResourceManager::GetString("DURANIUM");	break;
		case CRYSTAL:	resName = CResourceManager::GetString("CRYSTAL");	break;
		case IRIDIUM:	resName = CResourceManager::GetString("IRIDIUM");	break;
		}

		if (CTrade::GetMonopolOwner(i).IsEmpty() == false)
		{
			//CString hh;
			//hh.Format("Steuern auf %d: %d Credits",i,taxMoney[i]);
			//AfxMessageBox(hh);
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(CTrade::GetMonopolOwner(i)));
			ASSERT(pMajor);
			if (pMajor)
				pMajor->GetEmpire()->SetCredits(taxMoney[i]);
		}

		// Hier die gekauften Monopole den Rassen zuschreiben. Wer am meisten bezahlt hat (falls mehrere Rassen
		// in der selben Runde ein Monopol kaufen möchten) bekommt das Monopol. Die anderen bekommen ihr Credits zurück
		double max = 0.0f;				// meiste Credits was für ein Monopol gegeben wurde
		CString sMonopolRace = "";		// Rasse die das Monopol erlangt hat


		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			ASSERT(pMajor);

			if (pMajor->GetTrade()->GetMonopolBuying()[i] > max)
			{
				max = pMajor->GetTrade()->GetMonopolBuying()[i];
				sMonopolRace = pMajor->GetRaceID();
				CTrade::SetMonopolOwner(i, sMonopolRace);
			}
		}

		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			ASSERT(pMajor);

			CString sNews = "";
			// Die anderen Rassen bekommen ihr Geld zurück
			if (pMajor->GetRaceID() != sMonopolRace && pMajor->GetTrade()->GetMonopolBuying()[i] != 0)
			{
				pMajor->GetEmpire()->SetCredits((long)pMajor->GetTrade()->GetMonopolBuying()[i]);
				// Nachricht generieren, dass wir es nicht geschafft haben ein Monopol zu kaufen
				sNews = CResourceManager::GetString("WE_DONT_GET_MONOPOLY",FALSE,resName);
			}
			// Nachricht an unser Imperium, dass wir ein Monopol erlangt haben
			else if (pMajor->GetRaceID() == sMonopolRace)
				sNews = CResourceManager::GetString("WE_GET_MONOPOLY",FALSE,resName);

			if (!sNews.IsEmpty())
			{
				CMessage message;
				message.GenerateMessage(sNews,MESSAGE_TYPE::SOMETHING,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer())
				{
					network::RACE clientID = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
					m_iSelectedView[clientID] = EMPIRE_VIEW;
				}
				pMajor->GetTrade()->SetMonopolBuying(i,0.0f);
			}


			// Nachrichten an die einzelnen Imperien verschicken, das eine Rasse das Monopol erlangt hat
			if (sMonopolRace.IsEmpty() == false && sMonopolRace != pMajor->GetRaceID())
			{
				CMajor* pMonopolRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sMonopolRace));
				ASSERT(pMonopolRace);

				CString sRace = CResourceManager::GetString("UNKNOWN");
				if (pMajor->IsRaceContacted(sMonopolRace))
					sRace = pMonopolRace->GetRaceNameWithArticle();

				CString news = CResourceManager::GetString("SOMEBODY_GET_MONOPOLY",TRUE,sRace,resName);
				CMessage message;
				message.GenerateMessage(news,MESSAGE_TYPE::SOMETHING,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer())
				{
					network::RACE clientID = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
					m_iSelectedView[clientID] = EMPIRE_VIEW;
				}
			}
		}
	}
	// Hier den neuen Kurs der Waren an den Handelsbörsen berechnen, dürfen wir erst machen, wenn wir für alle Tradeobjekte
	// die einzelnen Kurse berechnet haben
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		pMajor->GetTrade()->CalculatePrices(pmMajors, pMajor);
		// Hier die aktuellen Kursdaten in die History schreiben

		USHORT* resPrices = pMajor->GetTrade()->GetRessourcePrice();
		pMajor->GetTrade()->GetTradeHistory()->SaveCurrentPrices(resPrices, pMajor->GetTrade()->GetTax());
	}
}

/// Diese Funktion berechnet die Schiffsbefehle. Der Systemangriffsbefehl ist davon ausgenommen.
void CBotf2Doc::CalcShipOrders()
{
	// Hier kommt die Auswertung der Schiffsbefehle
	bool increment = false;
	for(CShipMap::iterator y = m_ShipMap.begin();;)
	{
		if(increment)
			++y;
		increment = true;
		if(y == m_ShipMap.end())
			break;

#ifdef DEVELOPMENT_VERSION
		CSanity::GetInstance()->SanityCheckFleet(*y->second);
#endif

		CSector* pSector = &GetSector(y->second->GetKO().x, y->second->GetKO().y);
  		CSystem* pSystem = &GetSystem(y->second->GetKO().x, y->second->GetKO().y);

		// Hier wird überprüft, ob der Systemattack-Befehl noch gültig ist
		// Alle Schiffe, welche einen Systemangriffsbefehl haben überprüfen, ob dieser Befehl noch gültig ist
		if (y->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			assert(pSector->GetSunSystem());
			// Wenn die Bevölkerung komplett vernichtet wurde
			if (pSystem->GetHabitants() == 0.0f)
				y->second->SetCurrentOrderAccordingToType();
			// Wenn das System der angreifenden Rasse gehört
			else if (pSystem->GetOwnerOfSystem() == y->second->GetOwnerOfShip())
				y->second->SetCurrentOrderAccordingToType();
			// Wenn eine Rasse in dem System lebt
			else if (!pSector->GetOwnerOfSector().IsEmpty()
				&& pSector->GetOwnerOfSector() != y->second->GetOwnerOfShip())
			{
				CRace* pRace = m_pRaceCtrl->GetRace(pSector->GetOwnerOfSector());
				assert(pRace);
				CRace* pShipOwner = m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip());
				assert(pShipOwner);
				if (pRace->GetAgreement(pShipOwner->GetRaceID()) != DIPLOMATIC_AGREEMENT::WAR && !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
					y->second->SetCurrentOrderAccordingToType();
			}
		}

		 // Planet soll kolonisiert werden
		if (y->second->GetCurrentOrder() == SHIP_ORDER::COLONIZE)
		{
			// Überprüfen das der Sector auch nur mir oder niemandem geh?rt
			if ((pSector->GetOwnerOfSector() == y->second->GetOwnerOfShip() || pSector->GetOwnerOfSector() == ""))
			{
				// Wieviele Einwohner bekommen wir in dem System durch die Kolonisierung?
				float startHabitants = (float)(y->second->GetColonizePoints() * 4);
				// Wenn keine Nummer eines Planeten zum Kolonisieren angegeben ist, dann werden alle geterraformten
				// Planeten kolonisiert. Dazu wird die Bevölkerung, welche bei der Kolonisierung auf das System kommt
				// auf die einzelnen Planeten gleichmäßig aufgeteilt.
				assert(y->second->GetTerraform() == -1);
				if (y->second->GetTerraform() == -1)
				{
					BYTE terraformedPlanets = 0;
					for (int i = 0; i < pSector->GetNumberOfPlanets(); i++)
						if (pSector->GetPlanets().at(i).GetTerraformed() == TRUE
							&& pSector->GetPlanets().at(i).GetColonized() == FALSE)
							terraformedPlanets++;
					if (terraformedPlanets > 0)
					{
						float tmpHab = startHabitants /= terraformedPlanets;
						float tmpHab2 = 0.0f;
						float oddHab = 0.0f;	// Überschüssige Kolonisten, wenn ein Planet zu klein ist
						// Geterraformte Planeten durchgehen und die Bevölkerung auf diese verschieben
						for (int i = 0; i < pSector->GetNumberOfPlanets(); i++)
							if (pSector->GetPlanets().at(i).GetTerraformed() == TRUE
								&& pSector->GetPlanets().at(i).GetColonized() == FALSE)
							{
								if (startHabitants > pSector->GetPlanet(i)->GetMaxHabitant())
								{
									oddHab += (startHabitants - pSector->GetPlanet(i)->GetMaxHabitant());
									startHabitants = pSector->GetPlanet(i)->GetMaxHabitant();
								}
								tmpHab2 += startHabitants;
								pSector->GetPlanet(i)->SetCurrentHabitant(startHabitants);
								pSector->GetPlanet(i)->SetColonisized(TRUE);
								startHabitants = tmpHab;
							}
						startHabitants = tmpHab2;
						// die übrigen Kolonisten auf die Planeten verteilen
						if (oddHab > 0.0f)
							for (int i = 0; i < pSector->GetNumberOfPlanets(); i++)
								if (pSector->GetPlanets().at(i).GetTerraformed() == TRUE
									&& pSector->GetPlanets().at(i).GetCurrentHabitant() > 0.0f)
								{
									if ((oddHab + pSector->GetPlanets().at(i).GetCurrentHabitant())
										> pSector->GetPlanet(i)->GetMaxHabitant())
									{
										oddHab -= (pSector->GetPlanet(i)->GetMaxHabitant()
											- pSector->GetPlanets().at(i).GetCurrentHabitant());
										pSector->GetPlanet(i)->SetCurrentHabitant(pSector->GetPlanet(i)->GetMaxHabitant());
									}
									else
									{
										pSector->GetPlanet(i)->SetCurrentHabitant(
											pSector->GetPlanets().at(i).GetCurrentHabitant() + oddHab);
										break;
									}
								}
					}
					else
					{
						y->second->UnsetCurrentOrder();
						continue;
					}
				}
				else
				{
					assert(false);
					if (pSector->GetPlanet(y->second->GetTerraform())->GetColonized() == FALSE
						&& pSector->GetPlanet(y->second->GetTerraform())->GetTerraformed() == TRUE)
					{
						if (startHabitants > pSector->GetPlanet(y->second->GetTerraform())->GetMaxHabitant())
							startHabitants = pSector->GetPlanet(y->second->GetTerraform())->GetMaxHabitant();
						pSector->GetPlanet(y->second->GetTerraform())->SetCurrentHabitant(startHabitants);
						pSector->GetPlanet(y->second->GetTerraform())->SetColonisized(TRUE);
					}
					else
					{
						y->second->UnsetCurrentOrder();
						continue;
					}
				}
				CString s;
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
				ASSERT(pMajor);
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

				// Gebäude bauen, wenn wir das System zum ersten Mal kolonisieren, sprich das System noch niemanden gehört
				if (pSystem->GetOwnerOfSystem() == "")
				{
					// Sector- und Systemwerte ändern
					pSector->SetOwned(TRUE);
					pSector->SetOwnerOfSector(y->second->GetOwnerOfShip());
					pSector->SetColonyOwner(y->second->GetOwnerOfShip());
					pSystem->SetOwnerOfSystem(y->second->GetOwnerOfShip());
					// Gebäude nach einer Kolonisierung bauen
					pSystem->BuildBuildingsAfterColonization(pSector,&BuildingInfo,y->second->GetColonizePoints());
					// Nachricht an das Imperium senden, das ein System neu kolonisiert wurde
					s = CResourceManager::GetString("FOUND_COLONY_MESSAGE",FALSE,pSector->GetName());
					CMessage message;
					message.GenerateMessage(s,MESSAGE_TYPE::SOMETHING,pSector->GetName(),pSector->GetKO(),FALSE);
					pMajor->GetEmpire()->AddMessage(message);

					// zusätzliche Eventnachricht (Colonize a system #12) wegen der Moral an das Imperium
					message.GenerateMessage(pMajor->GetMoralObserver()->AddEvent(12, pMajor->GetRaceMoralNumber(), pSector->GetName()), MESSAGE_TYPE::SOMETHING, "", pSector->GetKO(), FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_CLAIMSYSTEM, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;

						CEventColonization* eventScreen = new CEventColonization(pMajor->GetRaceID(), CResourceManager::GetString("COLOEVENT_HEADLINE", FALSE, pSector->GetName()), CResourceManager::GetString("COLOEVENT_TEXT_" + pMajor->GetRaceID(), FALSE, pSector->GetName()));
						pMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
						s.Format("Added Colonization-Eventscreen for Race %s in System %s", pMajor->GetRaceName(), pSector->GetName());
						MYTRACE("general")(MT::LEVEL_INFO, s);
					}
				}
				else
				{
					// Nachricht an das Imperium senden, das ein Planet kolonisiert wurde
					s = CResourceManager::GetString("NEW_PLANET_COLONIZED",FALSE,pSector->GetName());
					CMessage message;
					message.GenerateMessage(s,MESSAGE_TYPE::SOMETHING,pSector->GetName(),pSector->GetKO(),FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
						m_iSelectedView[client] = EMPIRE_VIEW;
				}
				pSystem->SetHabitants(pSector->GetCurrentHabitants());

				pSystem->CalculateNumberOfWorkbuildings(&this->BuildingInfo);
				pSystem->CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), pSector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());

				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				s.Format("%s %s",CResourceManager::GetString("COLONIZATION"), pSector->GetName());
				pMajor->AddToLostShipHistory(*y->second, s, CResourceManager::GetString("DESTROYED"),
					m_iRound);
				// Schiff entfernen
				y->second->UnsetCurrentOrder();
				RemoveShip(y);
				increment = false;
				continue;
			}
			else
			{
				y->second->UnsetCurrentOrder();
			}
		}
		// hier wird ein Planet geterraformed
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
		{
			assert(y->second->GetTerraform() != -1);
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			if (pSector->GetPlanet(y->second->GetTerraform())->GetTerraformed() == FALSE)
			{
				if (pSector->GetPlanet(y->second->GetTerraform())->SetNeededTerraformPoints(y->second->GetColonizePoints()))
				{
					// Hier wurde ein Planet erfolgreich geterraformt
					y->second->UnsetCurrentOrder();
					// Nachricht generieren, dass Terraforming abgeschlossen wurde
					CString s = CResourceManager::GetString("TERRAFORMING_FINISHED",FALSE,pSector->GetName());
					CMessage message;
					message.GenerateMessage(s,MESSAGE_TYPE::SOMETHING,pSector->GetName(),pSector->GetKO(),FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
					// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
					if (pSector->GetOwnerOfSector() != "" && pSector->GetMinorRace() == TRUE && pSystem->GetOwnerOfSystem() == "")
					{
						CMinor* pMinor = m_pRaceCtrl->GetMinorRace(pSector->GetName());
						if (pMinor)
							pMinor->SetRelation(pMajor->GetRaceID(), +rand()%11);
					}
				}
			}
			else	// wenn der Plani aus irgendeinen Grund schon geterraformed ist
			{
				y->second->SetTerraform(-1);
			}
			// Wenn das Schiff eine Flotte anf?hrt, dann k?nnen auch die Schiffe in der Flotte ihre Terraformpunkte mit
			// einbringen
			if (y->second->HasFleet() && y->second->GetTerraform() != -1)
			{
				unsigned colonize_points_sum = y->second->GetColonizePoints();
				for (CShips::const_iterator x = y->second->begin(); x != y->second->end(); ++x)
				{
					//if a ship in a fleet isn't able to take over this command, it should have terraforming_planet=-1
					if(x->second->GetCurrentOrder() != SHIP_ORDER::TERRAFORM) {
						assert(x->second->GetTerraform() == -1);
						continue;
					}
					if (pSector->GetPlanet(y->second->GetTerraform())->GetTerraformed() == FALSE)
					{
						const unsigned colonize_points = x->second->GetColonizePoints();
						colonize_points_sum += colonize_points;
						if (pSector->GetPlanet(x->second->GetTerraform())->SetNeededTerraformPoints(colonize_points))
						{
							y->second->UnsetCurrentOrder();
							// Nachricht generieren, dass Terraforming abgeschlossen wurde
							CString s = CResourceManager::GetString("TERRAFORMING_FINISHED",FALSE,pSector->GetName());
							CMessage message;
							message.GenerateMessage(s,MESSAGE_TYPE::SOMETHING,pSector->GetName(),pSector->GetKO(),FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
							if (pSector->GetOwnerOfSector() != "" && pSector->GetMinorRace() == TRUE && pSystem->GetOwnerOfSystem() == "")
							{
								CMinor* pMinor = m_pRaceCtrl->GetMinorRace(pSector->GetName());
								if (pMinor)
									pMinor->SetRelation(pMajor->GetRaceID(), +rand()%11);
							}
							break;
						}
					}
					else	// wenn der Plani aus irgendeinen Grund schon geterraformed ist
					{
						y->second->UnsetCurrentOrder();
						break;
					}
				}//for (CShips::const_iterator x = y->second->begin(); x != y->second->end(); ++x)
				//Gib eine Warnung aus falls Kolonisierungspunkte verschwendet werden würden.
				//Es ist hoffentlich nicht möglich, dass ein Schiff einer Schiffsgruppe einen anderen Planeten
				//terraformt als das die Gruppe anführende Schiff...
				const unsigned terraforming_planet = y->second->GetTerraform();
				if (terraforming_planet != -1)//wird immernoch geterraformt ?
				{
					const unsigned needed_terraform_points = pSector->GetPlanet(terraforming_planet)->GetNeededTerraformPoints();
					if(colonize_points_sum > needed_terraform_points)
					{
						CString s;
						s.Format("%u", colonize_points_sum - needed_terraform_points);
						s = CResourceManager::GetString("TERRAFORMING_POINTS_WASTED",FALSE,pSector->GetName(), s);
						CMessage message;
						message.GenerateMessage(s,MESSAGE_TYPE::SOMETHING,pSector->GetName(),pSector->GetKO(),FALSE);
						pMajor->GetEmpire()->AddMessage(message);
					}
				}
			}
		}
		// hier wird ein Aussenposten gebaut
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)	// es soll eine Station gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// hier wird vereinfacht angenommen, das an teurerer Aussenposten auch ein besserer ist
			short id = -1;
			// Wenn wir in dem Sektor noch keinen Außenposten und noch keine Sternbasis stehen haben
			if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == FALSE
				&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
					id = pMajor->BestBuildableVariant(SHIP_TYPE::OUTPOST, m_ShipInfoArray);
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, k?nnen wir diese dort auch errichten
			if (id != -1)
			{
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				if (pSector->IsStationBuildable(SHIP_TYPE::OUTPOST, pMajor->GetRaceID()))
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					pSector->SetIsStationBuilding(TRUE, y->second->GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (pSector->GetStartStationPoints(y->second->GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						pSector->SetStartStationPoints(m_ShipInfoArray.GetAt((id-10000)).GetBaseIndustry(),y->second->GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anf?hrt, dann erstmal die Au?enpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					for(CShipMap::iterator x = y->second->begin(); x != y->second->end();)
					{
						if (pSector->SetNeededStationPoints(x->second->GetStationBuildPoints(),y->second->GetOwnerOfShip()))
						{
							// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
							if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == FALSE
								&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
							{
								pSector->SetOutpost(y->second->GetOwnerOfShip());
								pSector->SetOwnerOfSector(y->second->GetOwnerOfShip());
								pSector->SetScanned(y->second->GetOwnerOfShip());
								pSector->SetOwned(TRUE);
								pSector->SetShipPort(TRUE,y->second->GetOwnerOfShip());
								// Nachricht generieren, dass der Aussenpostenbau abgeschlossen wurde
								CMessage message;
								message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
								pMajor->GetEmpire()->AddMessage(message);
								// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
								pMajor->AddToLostShipHistory(*x->second, CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"), m_iRound);
								if (pMajor->IsHumanPlayer())
								{
									SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, client, 0, 1.0f};
									m_SoundMessages[client].Add(entry);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
								// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
								for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
								{
									pSector->SetIsStationBuilding(FALSE, it->first);
									pSector->SetStartStationPoints(0, it->first);
								}
								// Das Schiff, welches die Station fertiggestellt hat aus der Flotte entfernen
								y->second->RemoveShipFromFleet(x, true);
								BuildShip(id, pSector->GetKO(), y->second->GetOwnerOfShip());
								// Wenn hier ein Au?enposten gebaut wurde den Befehl f?r die Flotte auf Meiden stellen
								y->second->UnsetCurrentOrder();
								break;
							}
							else
								++x;
						}
						else
							++x;
					}//for(CShipMap::iterator x = y->second->begin(); x != y->second->end();)
					if (pSector->GetIsStationBuilding(y->second->GetOwnerOfShip()) == TRUE
						&& pSector->SetNeededStationPoints(y->second->GetStationBuildPoints(),y->second->GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == FALSE
							&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
						{
							pSector->SetOutpost(y->second->GetOwnerOfShip());
							pSector->SetOwnerOfSector(y->second->GetOwnerOfShip());
							pSector->SetScanned(y->second->GetOwnerOfShip());
							pSector->SetOwned(TRUE);
							pSector->SetShipPort(TRUE,y->second->GetOwnerOfShip());
							// Nachricht generieren, dass der Aussenpostenbau abgeschlossen wurde
							CMessage message;
							message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							pMajor->AddToLostShipHistory(*y->second, CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"), m_iRound);
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
							{
								pSector->SetIsStationBuilding(FALSE, it->first);
								pSector->SetStartStationPoints(0, it->first);
							}
							// Hier den Aussenposten bauen
							BuildShip(id, pSector->GetKO(), y->second->GetOwnerOfShip());

							// Wenn hier ein Aussenposten gebaut wurde den Befehl für die Flotte auf Meiden stellen
							y->second->UnsetCurrentOrder();
							RemoveShip(y);
							increment = false;
							continue;
						}
					}
				}
				else
					y->second->UnsetCurrentOrder();
			}
			else
				y->second->UnsetCurrentOrder();
		}
		// hier wird eine Sternbasis gebaut
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::BUILD_STARBASE)	// es soll eine Sternbasis gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen k?nnten.
			// um eine Sternbasis bauen zu k?nnen mu? schon ein Aussenposten in dem Sektor stehen
			// hier wird vereinfacht angenommen, das eine teurere Sternbasis auch eine bessere ist
			// oder wir haben einen Aussenposten und wollen diesen zur Sternbasis updaten
			short id = -1;
			if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == TRUE
				&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
					id = pMajor->BestBuildableVariant(SHIP_TYPE::STARBASE, m_ShipInfoArray);
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				if (pSector->IsStationBuildable(SHIP_TYPE::STARBASE, pMajor->GetRaceID()))
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					pSector->SetIsStationBuilding(TRUE, y->second->GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (pSector->GetStartStationPoints(y->second->GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						pSector->SetStartStationPoints(m_ShipInfoArray.GetAt(id-10000).GetBaseIndustry(),y->second->GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anf?hrt, dann erstmal die Au?enpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					for(CShipMap::iterator x = y->second->begin(); x != y->second->end();)
					{
						if (pSector->SetNeededStationPoints(x->second->GetStationBuildPoints(),y->second->GetOwnerOfShip()))
						{
							// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
							if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == TRUE
								&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
							{
								pSector->UnsetOutpost(y->second->GetOwnerOfShip());
								pSector->SetStarbase(y->second->GetOwnerOfShip());
								pSector->SetOwnerOfSector(y->second->GetOwnerOfShip());
								pSector->SetScanned(y->second->GetOwnerOfShip());
								pSector->SetOwned(TRUE);
								pSector->SetShipPort(TRUE,y->second->GetOwnerOfShip());
								// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
								CMessage message;
								message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
								pMajor->GetEmpire()->AddMessage(message);
								// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
								pMajor->AddToLostShipHistory(*x->second, CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"), m_iRound);
								if (pMajor->IsHumanPlayer())
								{
									SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, client, 0, 1.0f};
									m_SoundMessages[client].Add(entry);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
								// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
								for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
								{
									pSector->SetIsStationBuilding(FALSE, it->first);
									pSector->SetStartStationPoints(0, it->first);
								}
								// Das Schiff, welches die Station fertiggestellt hat aus der Flotte entfernen
								y->second->RemoveShipFromFleet(x, true);
								this->BuildShip(id, pSector->GetKO(), y->second->GetOwnerOfShip());
								// Wenn wir jetzt die Sternbasis gebaut haben, dann müssen wir den alten Aussenposten aus der
								// Schiffsliste nehmen
								for(CShipMap::iterator k = m_ShipMap.begin(); k != m_ShipMap.end(); ++k)
									if (k->second->GetShipType() == SHIP_TYPE::OUTPOST && k->second->GetKO() == pSector->GetKO())
									{
										// ebenfalls muss der Au?enposten aus der Shiphistory der aktuellen Schiffe entfernt werden
										pMajor->GetShipHistory()->RemoveShip(k->second);
										assert(k != y);
										m_ShipMap.EraseAt(k, true);
										break;
									}
								// Wenn hier eine Station gebaut wurde den Befehl für die Flotte auf Meiden stellen
								y->second->UnsetCurrentOrder();
								break;
							}
							else
								++x;
						}
						else
							++x;
					}
					if (pSector->GetIsStationBuilding(y->second->GetOwnerOfShip()) == TRUE
						&& pSector->SetNeededStationPoints(y->second->GetStationBuildPoints(),y->second->GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (pSector->GetOutpost(y->second->GetOwnerOfShip()) == TRUE
							&& pSector->GetStarbase(y->second->GetOwnerOfShip()) == FALSE)
						{
							pSector->UnsetOutpost(y->second->GetOwnerOfShip());
							pSector->SetStarbase(y->second->GetOwnerOfShip());
							pSector->SetOwnerOfSector(y->second->GetOwnerOfShip());
							pSector->SetScanned(y->second->GetOwnerOfShip());
							pSector->SetOwned(TRUE);
							pSector->SetShipPort(TRUE,y->second->GetOwnerOfShip());
							// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
							CMessage message;
							message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							pMajor->AddToLostShipHistory(*y->second, CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"), m_iRound);
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
							{
								pSector->SetIsStationBuilding(FALSE, it->first);
								pSector->SetStartStationPoints(0, it->first);
							}
							// Sternbasis bauen
							BuildShip(id, pSector->GetKO(), y->second->GetOwnerOfShip());
							// Wenn hier eine Station gebaut wurde den Befehl für die Flotte auf Meiden stellen
							y->second->UnsetCurrentOrder();

							// Wenn die Sternbasis gebaut haben, dann den alten Au?enposten aus der Schiffsliste nehmen
							for(CShipMap::iterator k = m_ShipMap.begin(); k != m_ShipMap.end(); ++k)
								if (k->second->GetShipType() == SHIP_TYPE::OUTPOST && k->second->GetKO() == pSector->GetKO())
								{
									// ebenfalls muss der Au?enposten aus der Shiphistory der aktuellen Schiffe entfernt werden
									pMajor->GetShipHistory()->RemoveShip(k->second);
									assert(k != y);
									m_ShipMap.EraseAt(k, true);
									break;
								}
							RemoveShip(y);
							increment = false;
							continue;
						}
					}
				}
				else
					y->second->UnsetCurrentOrder();
			}
			else
				y->second->UnsetCurrentOrder();
		}
		// Wenn wir das Schiff abracken/zerst?ren/demontieren wollen
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::DESTROY_SHIP)	// das Schiff wird demontiert
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
			ASSERT(pMajor);
			/*network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());*/

			// wenn wir in dem Sector wo wir das Schiff demoniteren ein uns gehörendes System haben, dann bekommen wir
			// teilweise Rohstoffe aus der Demontage zurück (vlt. auch ein paar Credits)
			if (pSystem->GetOwnerOfSystem() == y->second->GetOwnerOfShip())
			{
				USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 ausw?hlen
				// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
				proz += pSystem->GetProduction()->GetShipRecycling();
				USHORT id = y->second->GetID() - 10000;
				pSystem->SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
				pSystem->SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
				pSystem->SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
				pSystem->SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
				pSystem->SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
				pMajor->GetEmpire()->SetCredits((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
			}
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			pMajor->GetShipHistory()->ModifyShip(y->second, pSector->GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"),	CResourceManager::GetString("DESTROYED"));

			// Wenn das Schiff eine Flotte anf?hrt, dann auch die Schiffe in der Flotte demontieren
			for(CShips::const_iterator x = y->second->begin(); x != y->second->end(); ++x)
			{
				if (pSystem->GetOwnerOfSystem() == y->second->GetOwnerOfShip())
				{
					USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 ausw?hlen
					// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
					proz += pSystem->GetProduction()->GetShipRecycling();
					USHORT id = x->second->GetID() - 10000;
					pSystem->SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
					pSystem->SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
					pSystem->SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
					pSystem->SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
					pSystem->SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
					pMajor->GetEmpire()->SetCredits((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
				}
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				pMajor->GetShipHistory()->ModifyShip(x->second, pSector->GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"), CResourceManager::GetString("DESTROYED"));
			}

			// Wenn es ein Au?enposten oder eine Sternbasis ist, dann dem Sektor bekanntgeben, dass in ihm keine Station mehr ist
			if (y->second->IsStation())
			{
				pSector->UnsetOutpost(y->second->GetOwnerOfShip());
				pSector->UnsetStarbase(y->second->GetOwnerOfShip());
			}

			m_ShipMap.EraseAt(y, true);
			increment = false;
			continue;	// continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
		}

		// Wenn wir ein Schiff zum Flagschiff ernennen wollen (nur ein Schiff pro Imperium kann ein Flagschiff sein!)
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::ASSIGN_FLAGSHIP)
		{
			assert(!y->second->HasFleet());
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip()));
			assert(pMajor);
			const network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff finden, diesem den
			// Titel wegnehmen
			for(CShipMap::iterator n = m_ShipMap.begin(); n != m_ShipMap.end(); ++n)
			{
				if (n->second->GetOwnerOfShip() != y->second->GetOwnerOfShip())
					continue;
				n->second->UnassignFlagship(CShip::UNASSIGN_FLAGSHIP_MODE_STATUS);
			}
			// Jetzt das neue Schiff zum Flagschiff ernennen
			y->second->SetIsShipFlagShip(TRUE);
			y->second->SetCurrentOrderAccordingToType();
			// Nachricht generieren, dass ein neues Schiff zum Flagschiff ernannt wurde
			const CString& s = CResourceManager::GetString("ASSIGN_FLAGSHIP_MESSAGE",FALSE,
				y->second->GetShipName(),y->second->GetShipTypeAsString());
			CMessage message;
			message.GenerateMessage(s,MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::TRAIN_SHIP)
		{
			// Checken ob der Befehl noch Gültigkeit hat
			if (pSector->GetSunSystem() == TRUE &&
				pSystem->GetOwnerOfSystem() == y->second->GetOwnerOfShip())
			{
				// Wenn ein Schiff mit Veteranenstatus (Level 4) in der Trainingsflotte ist, dann verdoppelt sich der Erfahrungsgewinn
				// für die niedrigstufigen Schiffe
				int XP = pSystem->GetProduction()->GetShipTraining();
				y->second->ApplyTraining(XP);
			}
		}
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::ENCLOAK)
		{
			assert(y->second->CanCloak(true));
			y->second->SetCloak(true);
			y->second->UnsetCurrentOrder();
		}
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::DECLOAK)
		{
			assert(y->second->CanCloak(true));
			y->second->SetCloak(false);
			y->second->UnsetCurrentOrder();
		}
		// Blockadebefehl
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::BLOCKADE_SYSTEM)
		{
			BOOLEAN blockadeStillActive = FALSE;
			// Überprüfen ob der Blockadebefehl noch Gültigkeit hat
			if (pSystem->GetOwnerOfSystem() != y->second->GetOwnerOfShip())
				// handelt es sich beim Systembesitzer um eine andere Majorrace
				if (pSystem->GetOwnerOfSystem() != "")
				{
					CString systemOwner = pSystem->GetOwnerOfSystem();
					CString shipOwner   = y->second->GetOwnerOfShip();
					CRace* pShipOwner	= m_pRaceCtrl->GetRace(shipOwner);
					// haben wir einen Vertrag kleiner einem Freundschaftsvertrag mit der Majorrace
					if (pShipOwner->GetAgreement(systemOwner) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						int blockadeValue = pSystem->GetBlockade();
						//this command shouldn't be possible for ships without blockade special
						assert(y->second->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP));
						blockadeValue += rand()%20 + 1;
						blockadeStillActive = TRUE;
						y->second->CalcExp();
						// Wenn das Schiff eine Flotte anführt, dann erhöhen auch alle Schiffe in der Flotte mit
						// Blockadeeigenschaft den Blockadewert
						for (CShips::iterator x = y->second->begin(); x != y->second->end(); ++x)
						{
							assert(x->second->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP));
							blockadeValue += rand()%20 + 1;
							blockadeStillActive = TRUE;
							x->second->CalcExp();
						}

						pSystem->SetBlockade((BYTE)blockadeValue);
						// Die Beziehung zum Systembesitzer verringert sich um bis zu maximal 10 Punkte
						CRace* pSystemOwner	= m_pRaceCtrl->GetRace(systemOwner);
						pSystemOwner->SetRelation(shipOwner, -rand()%(blockadeValue/10 + 1));
					}
				}
			// kann der Blockadebefehl nicht mehr ausgeführt werden, so wird der Befehl automatisch gelöscht
			if (!blockadeStillActive)
				y->second->UnsetCurrentOrder();
			// wird das System schlussendlich blockiert, so produzieren die Handelsrouten kein Credits mehr
			if (pSystem->GetBlockade() > NULL)
			{
				// Wird das System blockiert, so generiert die Handelsroute kein Credits
				for (int i = 0; i < pSystem->GetTradeRoutes()->GetSize(); i++)
					pSystem->GetTradeRoutes()->GetAt(i).SetCredits(NULL);

				// Eventscreen für den Angreifer und den Blockierten anlegen
				CRace* pShipOwner = m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip());
				CMajor* pShipOwnerMajor = NULL;
				if (pShipOwner != NULL && pShipOwner->IsMajor() && (pShipOwnerMajor = dynamic_cast<CMajor*>(pShipOwner))->IsHumanPlayer())
				{
					CEventBlockade* eventScreen = new CEventBlockade(y->second->GetOwnerOfShip(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, pSector->GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_" + pShipOwner->GetRaceID(), FALSE, pSector->GetName()));
					pShipOwnerMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
				}
				if (pSystem->GetOwnerOfSystem() != "")
				{
					CRace* pSystemOwner = m_pRaceCtrl->GetRace(pSystem->GetOwnerOfSystem());
					CMajor* pSystemOwnerMajor = NULL;
					if (pSystemOwner != NULL && pSystemOwner->IsMajor() && (pSystemOwnerMajor = dynamic_cast<CMajor*>(pSystemOwner))->IsHumanPlayer())
					{
						CEventBlockade* eventScreen = new CEventBlockade(pSystem->GetOwnerOfSystem(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, pSector->GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_" + pSystemOwner->GetRaceID(), FALSE, pSector->GetName()));
						pSystemOwnerMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
					}
				}
			}
		}
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::WAIT_SHIP_ORDER)
		{
			//Do nothing, but only for this round.
			y->second->UnsetCurrentOrder();
		}
		//else if (y->second->GetCurrentOrder() == SHIP_ORDER::SENTRY_SHIP_ORDER)
			//Do nothing for this and all following rounds until an explicit player input.
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::REPAIR)
		{
			//The actual Hull reparing is currenty done in CalcShipMovement(),
			//after the call to this function.
		}

		// Vor der Schiffsbewegung aber nach einer möglichen Demontage dort ?berall einen ShipPort setzen wo
		// eine Sternbasis oder ein Au?enposten steht
		if (y->second->IsStation())
		{
			pSector->SetShipPort(TRUE, y->second->GetOwnerOfShip());
		}
	}
}

/// Diese Funktion berechnet die Schiffsbewegung und noch weitere kleine Sachen im Zusammenhang mit Schiffen.
void CBotf2Doc::CalcShipMovement()
{
	// CHECK WW: Das kann theoretisch weg, die Diplomatie wird erst nach der Bewegung berechnet
	// Jetzt die Starmap abgleichen, das wir nicht auf Gebiete fliegen können, wenn wir einen NAP mit einer Rasse haben
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	bool bAnomaly = false;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// Schiffunterstützungskosten auf NULL setzen
		pMajor->GetEmpire()->SetShipCosts(0);
		set<CString> races;
		for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			if (it->first != itt->first && pMajor->GetAgreement(itt->first) == DIPLOMATIC_AGREEMENT::NAP)
				races.insert(itt->first);
		pMajor->GetStarmap()->SynchronizeWithMap(m_Sectors, &races);
	}

#ifdef DEVELOPMENT_VERSION
	std::set<CString> already_encountered_ships_for_sanity_check;
#endif
	CShipMap repaired_ships;
	// Hier kommt die Schiffsbewegung (also keine anderen Befehle werden hier noch ausgewertet, lediglich wird überprüft,
	// dass manche Befehle noch ihre Gültigkeit haben
	for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
#ifdef DEVELOPMENT_VERSION
		CSanity::GetInstance()->CheckShipUniqueness(*y->second, already_encountered_ships_for_sanity_check);
#endif

		// Prüfen, dass ein Terraformbefehl noch gültig ist
		if (y->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
		{
			const CPoint& p = y->second->GetKO();
			if (GetSector(p.x, p.y).GetPlanet(y->second->GetTerraform())->GetTerraformed())
				y->second->SetTerraform(-1);
		}
		// Prüfen, dass ein Aussenpostenbaubefehl noch gültig ist
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)
		{
			if (GetSector(y->second->GetKO().x, y->second->GetKO().y).GetOutpost(y->second->GetOwnerOfShip()) == TRUE)
				y->second->UnsetCurrentOrder();
		}
		// Prüfen, dass ein Sternbasenbaubefehl noch gültig ist
		else if (y->second->GetCurrentOrder() == SHIP_ORDER::BUILD_STARBASE)
		{
			if (GetSector(y->second->GetKO().x, y->second->GetKO().y).GetStarbase(y->second->GetOwnerOfShip()) == TRUE)
				y->second->UnsetCurrentOrder();
		}
		// weiter mit Schiffsbewegung
		Sector shipKO((char)y->second->GetKO().x,(char)y->second->GetKO().y);
		Sector targetKO((char)y->second->GetTargetKO().x,(char)y->second->GetTargetKO().y);
		Sector nextKO(-1,-1);

		assert(shipKO.on_map());
		//no target set by targetKO == current coords is no longer allowed
		assert(shipKO != targetKO);

		// Weltraummonster gesondert behandeln (Geschwindigkeit der Flotte sollte egal sein, nur das Alien muss fliegen können)
		if (y->second->IsAlien() && y->second->GetSpeed(true) > 0)
		{
			// wenn bei einem Weltraummonster kein Ziel vorhanden ist, dann wird zufüllig ein neues generiert
			if (targetKO.x == -1)
			{
				// irgend ein zufülliges neues Ziel generieren, welches nicht auf einer Anomalie endet
				while (true)
				{
					targetKO = Sector(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
					if (targetKO == shipKO)
						continue;

					if (GetSector(targetKO.x, targetKO.y).GetAnomaly())
						continue;

					// Anaerobe Makroben fliegen nur im freien Raum oder in Sektoren mit grünen Sonnen
					if (y->second->GetOwnerOfShip() == ANAEROBE_MAKROBE)
					{
						if (GetSector(targetKO.x, targetKO.y).GetSunSystem() && GetSector(targetKO.x, targetKO.y).GetSunColor() != 1)
							continue;						
					}
				
					y->second->SetTargetKO(CPoint(targetKO.x, targetKO.y));
					break;
				}
			}
			// nur in ca. jeder dritten Runde fliegt das Weltraummonster weiter
			else if (rand()%3 != 0)
			{
				targetKO = Sector(-1,-1);
			}
		}

		if (targetKO.x != -1)
		{
			const unsigned range = 3 - y->second->GetRange(true);
			const unsigned speed = y->second->GetSpeed(true);

			CRace* pRace = NULL;

			// Weltraummonster gesondert behandeln
			if (y->second->IsAlien())
			{
				CStarmap* pStarmap = new CStarmap(0);
				vector<Sector> vExceptions;
				// Anaerobe Makroben fliegen nur im freien Raum oder in Sektoren mit grünen Sonnen
				if (y->second->GetOwnerOfShip() == ANAEROBE_MAKROBE)
				{
					for (std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
					{
						// Ausnahmen hinzufügen, wenn es sich um ein Sonnensystem mit nicht grüner Sonne handelt
						// und es nicht der aktuelle Ort ist (wegfliegen soll immer möglich sein)
						if (sector->GetSunSystem() && sector->GetSunColor() != 1 && sector->GetKO() != y->second->GetKO())
							vExceptions.push_back(Sector(sector->GetKO()));
					}
				}

				pStarmap->SetFullRangeMap(SM_RANGE_NEAR, vExceptions);

				// Anomalien werden schon beachtet, da dies eine statische Variable ist und in NextRound() schon
				// berechnet wurde.
				nextKO = pStarmap->CalcPath(shipKO,targetKO,range,speed,*y->second->GetPath());

				delete pStarmap;
				pStarmap = NULL;
			}
			else
			{
				pRace = m_pRaceCtrl->GetRace(y->second->GetOwnerOfShip());
				if (pRace != NULL && pRace->IsMajor())
				{
					nextKO = dynamic_cast<CMajor*>(pRace)->GetStarmap()->CalcPath(shipKO,targetKO,range,speed,*y->second->GetPath());
				}
			}

			// Ziel zum Anfliegen vorhanden
			if (nextKO != Sector(-1,-1))
			{
				y->second->SetKO(nextKO.x, nextKO.y);
				// Die Anzahl speed ersten Felder in Pfad des Schiffes löschen
				if (nextKO == targetKO)
				{
					y->second->GetPath()->RemoveAll();
					y->second->SetTargetKO(CPoint(-1,-1));
				}
				
				// Berechnet Zufallsentdeckung in dem Sektor den das Schiff anfliegt
				if (pRace != NULL && pRace->IsMajor() && !(this->GetSector(nextKO.x,nextKO.y).GetFullKnown(y->second->GetOwnerOfShip())))
				{
					CReManager* pRandomEventManager = CReManager::GetInstance();
					pRandomEventManager->CalcExploreEvent(CPoint((int)nextKO.x,(int)nextKO.y),dynamic_cast<CMajor*>(pRace),&m_ShipMap);
				}

				int high = speed;
				while (high > 0 && high < y->second->GetPath()->GetSize())
				{
					y->second->GetPath()->RemoveAt(0);
					high--;
				}
			}
		}

		// Gibt es eine Anomalie, wodurch die Schilde schneller aufgeladen werden
		bool bFasterShieldRecharge = false;
		if (GetSector(y->second->GetKO().x, y->second->GetKO().y).GetAnomaly())
			if (GetSector(y->second->GetKO().x, y->second->GetKO().y).GetAnomaly()->GetType() == BINEBULA)
				bFasterShieldRecharge = true;

		// Nach der Bewegung, aber noch vor einem möglichen Kampf werden die Schilde nach ihrem Typ wieder aufgeladen,
		// wenn wir auf einem Shipport sind, dann wird auch die Hülle teilweise wieder repariert
		//FIXME: The shipports are not yet updated for changes due to diplomacy at this spot.
		//If we declared war and are on a shipport of the former friend, the ship is repaired,
		//and a possible repair command isn't unset though it can no longer be set by the player this turn then.
		const CPoint& co = y->second->GetKO();
		const CSector& sector = GetSector(co.x, co.y);
		const bool port = sector.GetShipPort(y->second->GetOwnerOfShip());
		if(y->second->GetCurrentOrder() == SHIP_ORDER::REPAIR)
			y->second->RepairCommand(port, bFasterShieldRecharge, repaired_ships);
		else
			y->second->TraditionalRepair(port, bFasterShieldRecharge);

		// wenn eine Anomalie vorhanden, deren m?gliche Auswirkungen auf das Schiff berechnen
		if (GetSector(y->second->GetKO().x, y->second->GetKO().y).GetAnomaly())
		{
			GetSector(y->second->GetKO().x, y->second->GetKO().y).GetAnomaly()->CalcShipEffects(y->second);
			bAnomaly = true;
		}
	}
	m_ShipMap.Append(repaired_ships);

	if (!bAnomaly)
		return;
	CheckShipsDestroyedByAnomaly();
}
/////BEGIN: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipMovement()
void CBotf2Doc::CheckShipsDestroyedByAnomaly() {
	// prüfen ob irgendwelche Schiffe durch eine Anomalie zerstört wurden
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end();)
	{
		const CPoint& co = i->second->GetKO();
		if(!GetSector(co.x, co.y).GetAnomaly()) {
			++i;
			continue;
		}
		const CString& anomaly = GetSector(co.x, co.y).GetAnomaly()
			->GetMapName(co);
		CRace* pRace = m_pRaceCtrl->GetRace(i->second->GetOwnerOfShip());
		if(i->second->RemoveDestroyed(*pRace, m_iRound, anomaly,
				CResourceManager::GetString("DESTROYED"), NULL, anomaly)) {
			++i;
			continue;
		}
		RemoveShip(i);
	}
}
/////END: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipMovement()

/// Funktion überprüft, ob irgendwo ein Schiffskampf stattfindet. Wenn ja und es sind menschliche Spieler beteiligt,
/// so werden ihnen alle jeweils beteiligten Schiffe geschickt, so dass sie dort Befehle geben können.
bool CBotf2Doc::IsShipCombat()
{
	m_bCombatCalc = false;

	// Jetzt gehen wir nochmal alle Sektoren durch, wenn in einem Sektor Schiffe mehrerer verschiedener Rassen sind,
	// die Schiffe nicht auf Meiden gestellt sind und die Rassen untereinander nicht alle mindst. einen Freundschafts-
	// vertrag haben, dann kommt es in diesem Sektor zum Kampf
	for(CShipMap::const_iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CPoint& p = y->second->GetKO();
		const CString& sector = GetSector(p.x, p.y).GetName(TRUE);
		// Wenn unser Schiff auf Angreifen gestellt ist
		// Wenn in dem Sektor des Schiffes schon ein Kampf stattgefunden hat, dann findet hier keiner mehr statt
		if (y->second->GetCombatTactic() != COMBAT_TACTIC::CT_ATTACK
			|| m_sCombatSectors.find(sector) != m_sCombatSectors.end())
			continue;
		// Wenn noch kein Kampf in dem Sektor stattfand, dann kommt es möglicherweise hier zum Kampf
		for(CShipMap::const_iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
		{
			const CString& sOwner1 = y->second->GetOwnerOfShip();
			const CString& sOwner2 = i->second->GetOwnerOfShip();
			// nur weiter, wenn das Schiff nicht unserer Rasse gehört
			// und wenn das Schiff sich im gleichen Sektor befindet
			if (sOwner2 == sOwner1 || i->second->GetKO() != p)
				continue;
			const CRace* pRace1 = m_pRaceCtrl->GetRace(sOwner1);
			const CRace* pRace2 = m_pRaceCtrl->GetRace(sOwner2);
			// Wenn sich die Rassen aus diplomatischer Beziehung heraus angreifen können
			if (!CCombat::CheckDiplomacyStatus(pRace1, pRace2))
				continue;
			m_bCombatCalc = true;
			m_ptCurrentCombatSector = p;
			m_sCombatSectors.insert(sector);
			m_mCombatOrders.clear();
			MYTRACE("general")(MT::LEVEL_INFO, "Combat in Sector %d/%d\n", p.x, p.y);
			return true;
		}
	}

	// Wenn nur die KI am Kampf beteiligt ist, so kann der Kampf gleich berechnet werden
	// Dadurch müssen die Daten nicht immer an die ganzen Clients geschickt werden
	// TODO

	return false;
}

/// Diese Funktion berechnet einen möglichen Weltraumkampf und dessen Auswirkungen.
void CBotf2Doc::CalcShipCombat()
{
	if (!m_bCombatCalc)
		return;

	// Alle Schiffe im zuvor berechneten Kampfsektor holen
	CArray<CShips*> vInvolvedShips;
	CPoint p = m_ptCurrentCombatSector;
	// Jetzt gehen wir nochmal alle Sektoren durch und schauen ob ein Schiff im Kampfsektor ist
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
	{
		if (i->second->GetKO() != m_ptCurrentCombatSector)
			continue;

		vInvolvedShips.Add(i->second);

		// Wenn das Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
		for (CShips::iterator j = i->second->begin(); j != i->second->end(); ++j)
			vInvolvedShips.Add(j->second);
	}

	// es sollten immer Schiffe im Array sein, sonst hätte in diesem Sektor kein Kampf stattfinden dürfen
	assert(!vInvolvedShips.IsEmpty());

	// Kampf-KI
	CCombatAI AI;
	bool bCombat = AI.CalcCombatTactics(vInvolvedShips, m_pRaceCtrl->GetRaces(), m_mCombatOrders, GetSector(p.x, p.y).GetAnomaly());
	if (!bCombat)
		return;

	// Jetzt können wir einen Kampf stattfinden lassen
	CCombat Combat;
	Combat.SetInvolvedShips(&vInvolvedShips, m_pRaceCtrl->GetRaces(), GetSector(p.x, p.y).GetAnomaly());
	if (!Combat.GetReadyForCombat())
		return;

	Combat.PreCombatCalculation();
	map<CString, BYTE> winner;
	// Kampf berechnen
	Combat.CalculateCombat(winner);

	// Möglichen Rückzugssektor für Rasse aus diesem Kampf ermitteln
	// Diese Schiffe werden auf einem zufälligen Feld um den Kampfsektor platziert
	for (map<CString, COMBAT_ORDER::Typ>::const_iterator it = m_mCombatOrders.begin(); it != m_mCombatOrders.end(); ++it)
	{
		CPoint pt = m_ptCurrentCombatSector;
		pair<int, int> ptCombatSector(pt.x, pt.y);
		CString sRace = it->first;

		// Zufallig einen Sektor um den Kampfsektor ermitteln.
		while (true)
		{
			int x = rand()%3 - 1;
			int y = rand()%3 - 1;
			if (CPoint(pt.x + x, pt.y + y) != pt && pt.y + y < STARMAP_SECTORS_VCOUNT && pt.y + y > -1 && pt.x + x < STARMAP_SECTORS_HCOUNT && pt.x + x > -1)
			{
				CPoint ptRetreatSector = CPoint(pt.x + x, pt.y + y);

				// ermittelten Rückzugssektor für diese Rasse in diesem Sektor festlegen
				m_mShipRetreatSectors[sRace][ptCombatSector] = ptRetreatSector;
				break;
			}
		}
	}

	map<CString, CRace*>* pmRaces = m_pRaceCtrl->GetRaces();
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
	{
		CString sSectorName;
		// ist der Sektor bekannt?
		if (GetSector(p.x, p.y).GetKnown(it->first))
			sSectorName = GetSector(p.x, p.y).GetName(true);
		else
			sSectorName.Format("%s %c%i", CResourceManager::GetString("SECTOR"), (char)(p.y+97), p.x + 1);

		// gewonnen
		if (winner[it->first] == 1 && it->second->IsMajor())
		{
			// dem Siegbedingungsüberwacher den Sieg mitteilen
			m_VictoryObserver.AddCombatWin(it->first);

			CMajor* pMajor = dynamic_cast<CMajor*>(it->second);
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			CMessage message;
			message.GenerateMessage(CResourceManager::GetString("WIN_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", p, FALSE);
			pMajor->GetEmpire()->AddMessage(message);
			// win a minor battle
			CString eventText = pMajor->GetMoralObserver()->AddEvent(3, pMajor->GetRaceMoralNumber());
			message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", p, 0);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
		// verloren
		else if (winner[it->first] == 2)
		{
			if (it->second->IsMajor())
			{
				CMajor* pMajor = dynamic_cast<CMajor*>(it->second);
				ASSERT(pMajor);
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

				CMessage message;
				message.GenerateMessage(CResourceManager::GetString("LOSE_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", p,0);
				pMajor->GetEmpire()->AddMessage(message);
				// lose a minorbattle
				CString eventText = pMajor->GetMoralObserver()->AddEvent(6, pMajor->GetRaceMoralNumber());
				message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", p, 0);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer())
					m_iSelectedView[client] = EMPIRE_VIEW;
			}
			// Die Beziehung zum Gewinner verschlechtert sich dabei. Treffen zwei computergesteuerte Rassen
			// aufeinander, so ist die Beziehungsveringerung geringer
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first != itt->first && winner[itt->first] == 1)
				{
					CMajor* pMajorWin = itt->second;
					if (pMajorWin->AHumanPlays() == false)
						it->second->SetRelation(pMajorWin->GetRaceID(), -(rand()%4));
					else
						it->second->SetRelation(pMajorWin->GetRaceID(), -(rand()%6 + 5));
				}
			}
		}
		// unentschieden
		else if (winner[it->first] == 3 && it->second->IsMajor())
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(it->second);
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			CMessage message;
			message.GenerateMessage(CResourceManager::GetString("DRAW_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", p,0);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}

	// Bevor die Schiffe zerstört werden, prüfen wieviel Schiffe durch einen Boseaner (Alien) zerstört wurden.
	// Um diese Anzahl mehr werden neue Boseaner ins Spiel gebracht.
	if (const map<CShips*, std::set<const CShips*> >* pmKilledShips = Combat.GetKilledShipsInfos())
	{
		for (map<CShips*, std::set<const CShips*> >::const_iterator it = pmKilledShips->begin(); it != pmKilledShips->end(); ++it)
		{
			CShips* pBoseaner = it->first;
			if (!pBoseaner || pBoseaner->GetOwnerOfShip() != BOSEANER || !pBoseaner->IsAlive())
				continue;

			int nCount = it->second.size();
			if (nCount <= 0)
				continue;

			// Das Führungsschiff der Flotte vom Boseaner in der Schiffsmap suchen. Es kann sein das
			// der Boseaner selbst das Führungsschiff ist oder in einer Flotte steckt.
			CShips* pLeader = m_ShipMap.GetLeader(pBoseaner);
			if (!pLeader)
			{
				AfxMessageBox("Error: Found no leader ship of Bosean.\nPlease make a bug-report");
				continue;
			}
				
			for (int i = 0; i < nCount; i++)
			{
				// Es gibt eine schwierigkeitsgradabhängige Wahrscheinlichkeit, dass tatsächlich ein neuer Boseaner daraus entsteht.
				// BABY: 10%, EASY: 15%, NORMAL: 30%, HARD: 45%, IMPOSSIBLE: 75%
				int nProb = 15 / m_fDifficultyLevel;
				if (rand()%100 >= nProb)
					continue;

				// Erst das Schiff bauen
				CShipMap::iterator pNewShip = BuildShip(pBoseaner->GetID(), m_ptCurrentCombatSector, pBoseaner->GetOwnerOfShip());

				// neuen Boseaner in Gruppe stecken und Befehle gleich mit übernehmen
				pLeader->AddShipToFleet(pNewShip->second);
				m_ShipMap.EraseAt(pNewShip, false);
			}
		}
	}

	// Nach einem Kampf muß ich das Feld der Schiffe durchgehen und alle Schiffe aus diesem nehmen, die
	// keine Hülle mehr besitzen. Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte
	CStringArray destroyedShips;
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end();)
	{
		if (i->second->GetKO() != m_ptCurrentCombatSector)
		{
			++i;
			continue;
		}

		CRace* pOwner = m_pRaceCtrl->GetRace(i->second->GetOwnerOfShip());
		assert(pOwner);
		if (i->second->RemoveDestroyed(*pOwner, m_iRound, CResourceManager::GetString("COMBAT"),	CResourceManager::GetString("DESTROYED"), &destroyedShips))
		{
			++i;
			continue;
		}

		RemoveShip(i);
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// Hat die Rasse an dem Kampf teilgenommen, also gewonnen oder verloren oder unentschieden
		if (winner[it->first] != 0)
		{
			for (int j = 0; j < destroyedShips.GetSize(); j++)
			{
				CString s;
				s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,destroyedShips[j]));
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", p, 0);
				pMajor->GetEmpire()->AddMessage(message);
			}
		}
	}	
}

/////BEGIN: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipEffects()
void CBotf2Doc::CalcShipRetreat() {
	// Schiffe mit Rückzugsbefehl auf ein Feld neben dem aktuellen Feld setzen
	for(CShipMap::iterator ship = m_ShipMap.begin(); ship != m_ShipMap.end(); ++ship) {
		const CString& ship_owner = ship->second->GetOwnerOfShip();
		// Hat das Schiff den Rückzugsbefehl
		if (ship->second->GetCombatTactic() != COMBAT_TACTIC::CT_RETREAT)
			continue;

		// Rückzugssektor für dieses Schiff in diesem Sektor holen
		const std::map<CString, std::map<std::pair<int, int>, CPoint>>::const_iterator&
			SectorRetreatSectorPairs = m_mShipRetreatSectors.find(ship_owner);
		if (SectorRetreatSectorPairs == m_mShipRetreatSectors.end())
			continue;
		const std::map<std::pair<int, int>, CPoint>& mSectorRetreatSectorPairs
			= SectorRetreatSectorPairs->second;
		const CPoint& co = ship->second->GetKO();
		const pair<int, int> CurrentSector(co.x, co.y);
		const std::map<std::pair<int, int>, CPoint>::const_iterator& RetreatSector
			= mSectorRetreatSectorPairs.find(CurrentSector);
		if (RetreatSector == mSectorRetreatSectorPairs.end())
			continue;

		ship->second->Retreat(RetreatSector->second);

		if(!ship->second->HasFleet())
			continue;
		// sind alle Schiffe in einer Flotte im Rückzug, so kann die ganze Flotte
		// in den Rückzugssektor
		const bool bCompleteFleetRetreat = ship->second->GetSpeed(true) > 0
			&& ship->second->AllOnTactic(COMBAT_TACTIC::CT_RETREAT);

		// haben alle Schiffe in der Flotte den Rückzugsbefehl oder hat das Schiff keine Flotte
		// -> Rückzugssektor festlegen
		if (bCompleteFleetRetreat)
			ship->second->RetreatFleet(RetreatSector->second);
		// Schiffe aus der Flotte nehmen und ans Ende des Schiffsarrays packen. Diese werden
		// dann auch noch behandelt
		else
		{
			//we need to be careful, since we iterate over the array we're modifying, which
			//would normally invalidate the iterator "ship"
			const CShipMap& fleet = ship->second->Fleet();
			m_ShipMap.Append(fleet);
			ship->second->Reset(false);
		}
	}//	for (int i = 0; i < m_ShipMap.GetSize(); i++)
	m_mShipRetreatSectors.clear();
}
/////END: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipEffects()

/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
void CBotf2Doc::CalcShipEffects()
{
	CalcShipRetreat();

	// Nach einem möglichen Kampf, aber natürlich auch generell die Schiffe und Stationen den Sektoren bekanntgeben
	for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CString& sRace = y->second->GetOwnerOfShip();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		const CPoint& p = y->second->GetKO();
		CSector& sector = GetSector(p.x, p.y);

		// Anomalien beachten
		bool bDeactivatedShipScanner = false;
		bool bBetterScanner = false;
		const CAnomaly* const anomaly = sector.GetAnomaly();
		if(anomaly) {
			bDeactivatedShipScanner = anomaly->IsShipScannerDeactivated();
			bBetterScanner = anomaly->GetType() == QUASAR;
		}

		y->second->CalcEffects(sector, pRace, bDeactivatedShipScanner, bBetterScanner);
		// Dem Sektor nochmal bekanntgeben, dass in ihm eine Sternbasis oder ein Außenposten steht. Weil wenn im Kampf
		// eine Station teilnahm, dann haben wir den Shipport in dem Sektor vorläufig entfernt. Es kann ja passieren,
		// dass die Station zerstört wird. Haben wir jetzt aber immernoch eine Station, dann bleibt der Shipport dort auch
		// bestehen
		if (y->second->IsStation()) {
			sector.SetShipPort(TRUE, sRace);
			const SHIP_TYPE::Typ ship_type = y->second->GetShipType();
			if (ship_type == SHIP_TYPE::OUTPOST)
				sector.SetOutpost(sRace);
			else
				sector.SetStarbase(sRace);
		}
		else {
			// Dem Sektor bekanntgeben, dass in ihm ein Schiff ist
			sector.SetOwnerOfShip(TRUE, sRace);
		}
	}
}

///////////////////////////////////////////////////////////////////////
/////BEGINN: HELPER FUNCTIONS FOR void CBotf2Doc::CalcContactNewRaces()

void CBotf2Doc::CalcContactClientWork(CMajor& Major, const CRace& ContactedRace) {
	const network::RACE client = m_pRaceCtrl->GetMappedClientID(Major.GetRaceID());
	m_iSelectedView[client] = EMPIRE_VIEW;
	if(!Major.IsHumanPlayer())
		return;
	// Audiovorstellung der kennengelernten race
	SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_ALIENCONTACT, client, 1, 1.0f};
	if(ContactedRace.IsMajor()) {
		entry.nMessage = SNDMGR_MSG_FIRSTCONTACT;
		entry.nRace = m_pRaceCtrl->GetMappedClientID(ContactedRace.GetRaceID());
		entry.nPriority = 2;
	}
	m_SoundMessages[client].Add(entry);
}

void CBotf2Doc::CalcContactShipToMajorShip(CRace& Race, const CSector& sector, const CPoint& p) {
	// treffen mit einem Schiff eines anderen Majors
	// wenn zwei Schiffe verschiedener Rasse in diesem Sektor stationiert sind, so können sich die Besitzer auch kennenlernen
	const std::map<CString, CMajor*>& mMajors = *m_pRaceCtrl->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = mMajors.begin(); it != mMajors.end(); ++it) {
		CMajor* pMajor = it->second;
		// kann der andere Schiffsbesitzer Rassen kennenlernen?
		const CString& sMajorID = it->first;
		if (!sector.GetOwnerOfShip(sMajorID) || !pMajor->CanBeContactedBy(Race.GetRaceID()) ||
			Race.GetRaceID() == sMajorID)
			continue;
		CalcContactCommutative(*pMajor, Race, p);
	}
}

void CBotf2Doc::CalcContactCommutative(CMajor& Major,
	CRace& ContactedRace, const CPoint& p) {

	Major.Contact(ContactedRace, p);
	CalcContactClientWork(Major, ContactedRace);
	ContactedRace.Contact(Major, p);
	if(ContactedRace.IsMajor())
		CalcContactClientWork(dynamic_cast<CMajor&>(ContactedRace), Major);
}

void CBotf2Doc::CalcContactMinor(CMajor& Major, const CSector& sector, const CPoint& p) {
	if(!sector.GetMinorRace())
		return;
	// in dem Sektor lebt eine Minorrace
	CMinor* pMinor = m_pRaceCtrl->GetMinorRace(sector.GetName());
	assert(pMinor);
	// kann der Sektorbesitzer andere Rassen kennenlernen?
	if (pMinor->CanBeContactedBy(Major.GetRaceID()) && !pMinor->GetSubjugated())
		// die Rasse ist noch nicht bekannt und nicht unterworfen
		CalcContactCommutative(Major, *pMinor, p);
}

/////END: HELPER FUNCTIONS FOR void CBotf2Doc::CalcContactNewRaces()
///////////////////////////////////////////////////////////////////////

/// Diese Funktion überprüft, ob neue Rassen kennengelernt wurden.
void CBotf2Doc::CalcContactNewRaces()
{
	for(CShipMap::const_iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CString& sRace = y->second->GetOwnerOfShip();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		// kann die Rasse andere Rassen kennenlernen?
		if(pRace->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
			continue;
		const CPoint& p = y->second->GetKO();
		const CSector& sector = GetSector(p.x, p.y);
		const CString& sOwnerOfSector = sector.GetOwnerOfSector();
		CalcContactShipToMajorShip(*pRace, sector, p);
		if(sOwnerOfSector.IsEmpty() || sOwnerOfSector == sRace)
			continue;
		CRace* pOwnerOfSector = m_pRaceCtrl->GetRace(sOwnerOfSector);
		assert(pOwnerOfSector);
		if(pRace->IsMinor()) {
			if(pOwnerOfSector->CanBeContactedBy(sRace) && pOwnerOfSector->IsMajor()) {
				CMajor* pMajor = dynamic_cast<CMajor*>(pOwnerOfSector);
				assert(pMajor);
				CalcContactCommutative(*pMajor, *pRace, p);
			}
			continue;
		}
		//At present, a race is always either a major or a minor.
		//If this changes, this code needs to be adapted.
		CMajor* pMajor = dynamic_cast<CMajor*>(pRace);
		assert(pMajor);
		CalcContactMinor(*pMajor, sector, p);
		if (!pOwnerOfSector->CanBeContactedBy(sRace))
			continue;
		//At this point, pOwnerOfSector must be of type major, since independent or no diplo minors are handled.
		assert(pOwnerOfSector->IsMajor());
		CalcContactCommutative(*pMajor, *pOwnerOfSector, p);
	}//for (int y = 0; y < m_ShipMap.GetSize(); y++)
}

/// Funktion berechnet die Auswirkungen wenn eine Minorrace eleminiert wurde und somit aus dem Spiel ausscheidet.
/// @param pMinor Minorrace welche aus dem Spiel ausscheidet
void CBotf2Doc::CalcEffectsMinorEleminated(CMinor* pMinor)
{
	if (!pMinor)
	{
		ASSERT(pMinor);
		return;
	}

	if (!pMinor->IsAlienRace())
		GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetMinorRace(false);
	
	// Diplomatie/Nachrichten entfernen
	pMinor->GetIncomingDiplomacyNews()->clear();
	pMinor->GetOutgoingDiplomacyNews()->clear();
	
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (!pMajor)
			continue;

		// ausgehende Nachrichten löschen
		for (UINT i = 0; i < pMajor->GetOutgoingDiplomacyNews()->size(); i++)
		{
			if (pMajor->GetOutgoingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
				|| pMajor->GetOutgoingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
				it->second->GetOutgoingDiplomacyNews()->erase(pMajor->GetOutgoingDiplomacyNews()->begin() + i--);
		}

		// eingehende Nachrichten löschen
		for (UINT i = 0; i < pMajor->GetIncomingDiplomacyNews()->size(); i++)
		{
			if (pMajor->GetIncomingDiplomacyNews()->at(i).m_sFromRace == pMinor->GetRaceID()
				|| pMajor->GetIncomingDiplomacyNews()->at(i).m_sToRace == pMinor->GetRaceID())
				pMajor->GetIncomingDiplomacyNews()->erase(pMajor->GetIncomingDiplomacyNews()->begin() + i--);
		}

		// An alle Majors die die Minor kennen die Nachricht schicken, dass diese vernichtet wurde
		if (pMinor->IsRaceContacted(pMajor->GetRaceID()))
		{
			CString news = CResourceManager::GetString("ELIMINATE_MINOR", FALSE, pMinor->GetRaceName());
			CMessage message;
			if (pMinor->IsAlienRace())
				message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, "", 0, 0);
			else
				message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetName(), pMinor->GetRaceKO(), 0);
			
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
			{
				// Event über die Rassenauslöschung einfügen
				CEventRaceKilled* eventScreen = new CEventRaceKilled(it->first, pMinor->GetRaceID(), pMinor->GetRaceName(), pMinor->GetGraphicFileName());
				pMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);

				network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
		}
		
		// die vernichtete Minor auf unbekannt schalten und alle Verträge entfernen	
		pMajor->SetIsRaceContacted(pMinor->GetRaceID(), false);
		pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
	}

	// zuletzt noch alle Schiffe der Minorrace entfernen
	for (CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); )
	{
		if (i->second->GetOwnerOfShip() == pMinor->GetRaceID())
		{
			m_ShipMap.EraseAt(i, true);
			continue;
		}

		++i;
	}
}

/// Diese Funktion führt allgemeine Berechnung durch, die immer zum Ende der NextRound-Calculation stattfinden müssen.
void CBotf2Doc::CalcEndDataForNextRound()
{
	// Nachdem Außenposten und Sternbasen auch den Sektoren wieder bekanntgegeben wurden, können wir die Besitzerpunkte
	// für die Sektoren berechnen.
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	// ausgelöschte Hauptrassen behandeln
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		pMajor->GetEmpire()->GenerateSystemList(m_Systems, m_Sectors);
		pMajor->GetEmpire()->SetNumberOfSystems(pMajor->GetEmpire()->GetSystemList()->GetSize());

		// Wenn das Imperium keine Systeme mehr besitzt, so wird es für alle anderen Rassen auf unbekannt gestellt.
		// Scheidet somit aus dem Spiel aus
		if (pMajor->GetEmpire()->GetNumberOfSystems() == 0)
		{
			// Allen anderen bekannten Imperien die Nachricht zukommen lassen, dass die Rasse vernichtet wurde
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (pMajor->GetRaceID() == itt->first)
					continue;

				if (itt->second->IsRaceContacted(pMajor->GetRaceID()))
				{
					// Nachricht über Rassenauslöschung (hier die gleiche wie bei Minorauslöschung
					CString news = CResourceManager::GetString("ELIMINATE_MINOR", FALSE, pMajor->GetRaceName());
					CMessage message;
					message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, "", 0, 0);
					itt->second->GetEmpire()->AddMessage(message);
					if (itt->second->IsHumanPlayer())
					{
						// Event über die Rassenauslöschung einfügen
						CEventRaceKilled* eventScreen = new CEventRaceKilled(itt->first, pMajor->GetRaceID(), pMajor->GetRaceName(), pMajor->GetGraphicFileName());
						itt->second->GetEmpire()->GetEventMessages()->Add(eventScreen);

						network::RACE client = m_pRaceCtrl->GetMappedClientID(itt->first);
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
				}
			}

			// Alle Nachrichten und Events löschen
			for (int i = 0; i < pMajor->GetEmpire()->GetEventMessages()->GetSize(); i++)
				delete pMajor->GetEmpire()->GetEventMessages()->GetAt(i);

			pMajor->GetEmpire()->GetEventMessages()->RemoveAll();
			pMajor->GetEmpire()->GetMessages()->RemoveAll();
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
			m_SoundMessages[client].RemoveAll();

			// alle anderen Rassen durchgehen und die vernichtete Rasse aus deren Maps entfernen
			map<CString, CRace*>* mRaces = m_pRaceCtrl->GetRaces();
			for (map<CString, CRace*>::const_iterator itt = mRaces->begin(); itt != mRaces->end(); ++itt)
			{
				if (pMajor->GetRaceID() == itt->first)
					continue;

				CRace* pLivingRace = itt->second;

				pLivingRace->SetIsRaceContacted(pMajor->GetRaceID(), false);
				pLivingRace->SetAgreement(pMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);

				pMajor->SetIsRaceContacted(pLivingRace->GetRaceID(), false);
				pMajor->SetAgreement(pLivingRace->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);

				// alle Diplomatischen Angebote und Antworten löschen
				pMajor->GetIncomingDiplomacyNews()->clear();
				pMajor->GetOutgoingDiplomacyNews()->clear();

				for (UINT i = 0; i < pLivingRace->GetIncomingDiplomacyNews()->size(); i++)
				{
					CDiplomacyInfo* pInfo = &pLivingRace->GetIncomingDiplomacyNews()->at(i);
					if (pInfo->m_sCorruptedRace == pMajor->GetRaceID() || pInfo->m_sFromRace == pMajor->GetRaceID() || pInfo->m_sToRace == pMajor->GetRaceID() || pInfo->m_sWarpactEnemy == pMajor->GetRaceID() || pInfo->m_sWarPartner == pMajor->GetRaceID())
						pLivingRace->GetIncomingDiplomacyNews()->erase(pLivingRace->GetIncomingDiplomacyNews()->begin() + i--);
				}
				for (UINT i = 0; i < pLivingRace->GetOutgoingDiplomacyNews()->size(); i++)
				{
					CDiplomacyInfo* pInfo = &pLivingRace->GetOutgoingDiplomacyNews()->at(i);
					if (pInfo->m_sCorruptedRace == pMajor->GetRaceID() || pInfo->m_sFromRace == pMajor->GetRaceID() || pInfo->m_sToRace == pMajor->GetRaceID() || pInfo->m_sWarpactEnemy == pMajor->GetRaceID() || pInfo->m_sWarPartner == pMajor->GetRaceID())
						pLivingRace->GetOutgoingDiplomacyNews()->erase(pLivingRace->GetOutgoingDiplomacyNews()->begin() + i--);
				}

				if (pLivingRace->IsMajor())
				{
					CMajor* pLivingMajor = dynamic_cast<CMajor*>(pLivingRace);
					pLivingMajor->SetDefencePact(pMajor->GetRaceID(), false);
					pMajor->SetDefencePact(pLivingRace->GetRaceID(), false);

					// Geheimdienstzuweiseungen anpassen
					// Spionage auf 0 setzen
					pLivingMajor->GetEmpire()->GetIntelligence()->GetAssignment()->SetGlobalPercentage(0, 0, pLivingMajor, pMajor->GetRaceID(), pmMajors);
					// Sabotage auf 0 setzen
					pLivingMajor->GetEmpire()->GetIntelligence()->GetAssignment()->SetGlobalPercentage(0, 0, pLivingMajor, pMajor->GetRaceID(), pmMajors);
					if (pLivingMajor->GetEmpire()->GetIntelligence()->GetResponsibleRace() == pMajor->GetRaceID())
						pLivingMajor->GetEmpire()->GetIntelligence()->SetResponsibleRace(pLivingRace->GetRaceID());
					pLivingMajor->GetEmpire()->GetIntelligence()->GetAssignment()->RemoveRaceFromAssignments(pMajor->GetRaceID());
				}
			}

			// Alle Schiffe entfernen
			for(CShipMap::iterator j = m_ShipMap.begin(); j != m_ShipMap.end();)
			{
				if (j->second->GetOwnerOfShip() == pMajor->GetRaceID())
				{
					// Alle noch "lebenden" Schiffe aus der Schiffshistory ebenfalls als zerstört ansehen
					pMajor->GetShipHistory()->ModifyShip(j->second,
								GetSector(j->second->GetKO().x, j->second->GetKO().y).GetName(TRUE), m_iRound,
								CResourceManager::GetString("UNKNOWN"), CResourceManager::GetString("DESTROYED"));
					m_ShipMap.EraseAt(j, true);
				}
				else
					++j;
			}

			// Sektoren und Systeme neutral schalten
			for(std::vector<CSector>::iterator se = m_Sectors.begin(); se != m_Sectors.end(); ++se) {
				const CString& ID = pMajor->GetRaceID();
				if (se->GetOwnerOfSector() == ID)
				{
					se->SetOwnerOfSector("");
					GetSystemForSector(*se).SetOwnerOfSystem("");
					se->SetOwned(false);
					se->SetTakenSector(false);
				}
				if (se->GetColonyOwner() == ID)
					se->SetColonyOwner("");
				// in allen Sektoren alle Schiffe aus den Sektoren nehmen
				se->SetIsStationBuilding(false, ID);
				se->UnsetOutpost(ID);
				se->SetOwnerOfShip(false, ID);
				se->SetShipPort(false, ID);
				se->UnsetStarbase(ID);
			}

			// Wenn es ein menschlicher Spieler ist, so bekommt er den Eventscreen für die Niederlage angezeigt
			if (pMajor->IsHumanPlayer())
			{
				// einen neuen (und auch einzigen Event) einfügen
				CEventGameOver* eventScreen = new CEventGameOver(pMajor->GetRaceID());
				pMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
			}
		}
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// Moralveränderungen aufgrund möglicher Ereignisse berechnen. Erst nach der Schiffsbewegung und allem anderen
		pMajor->GetMoralObserver()->CalculateEvents(m_Systems, pMajor->GetRaceID(), pMajor->GetRaceMoralNumber());
		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> kein Abzug beim Stellaren Lager
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
			pMajor->GetEmpire()->GetGlobalStorage()->SetLosing(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(2));
		// Ressourcentransfers im globalen Lager vornehmen
		pMajor->GetEmpire()->GetGlobalStorage()->Calculate(m_Systems);
		pMajor->GetEmpire()->GetGlobalStorage()->SetMaxTakenRessources(1000 * pMajor->GetEmpire()->GetNumberOfSystems());
		// Befindet sich irgendeine Ressource im globalen Lager, bekommt der Spieler eine Imperiumsmeldung
		if (pMajor->GetEmpire()->GetGlobalStorage()->IsFilled())
		{
			CString s = CResourceManager::GetString("RESOURCES_IN_GLOBAL_STORAGE");
			CMessage message;
			message.GenerateMessage(s, MESSAGE_TYPE::ECONOMY, "", NULL, FALSE, 4);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
			{
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
		}

		// Schiffskosten berechnen
		int popSupport = pMajor->GetEmpire()->GetPopSupportCosts();
		int shipCosts  = pMajor->GetEmpire()->GetShipCosts();

		int costs = popSupport - shipCosts;
		if (costs < 0)
			pMajor->GetEmpire()->SetCredits(costs);
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CString sID = it->first;
		for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				// Befindet sich ein Außenposten oder ein System in einem der umliegenden Sektoren, so bekommt der
				// Sektor einen Besitzerpunkt. Bei einer Sternbasis sind es sogar zwei Besitzerpunkte.
				BYTE ownerPoints = 0;
				if (GetSystem(x, y).GetOwnerOfSystem() == sID)
					ownerPoints += 1;
				if (GetSector(x, y).GetOutpost(sID) == TRUE)
					ownerPoints += 1;
				if (GetSector(x, y).GetStarbase(sID) == TRUE)
					ownerPoints += 2;
				if (ownerPoints > 0)
				{
					for (int j = -1; j <= 1; j++)
						for (int i = -1; i <= 1; i++)
							if ((y+j < STARMAP_SECTORS_VCOUNT && y+j > -1) && (x+i < STARMAP_SECTORS_HCOUNT && x+i > -1))
								GetSector(x + i, y + j).AddOwnerPoints(ownerPoints, sID);

					// in vertikaler und horizontaler Ausrichtung gibt es sogar 2 Felder vom Sector entfernt noch
					// Besitzerpunkte
					if (x-2 >= 0)
						GetSector(x - 2, y).AddOwnerPoints(ownerPoints, sID);
					if (x+2 < STARMAP_SECTORS_HCOUNT)
						GetSector(x + 2, y).AddOwnerPoints(ownerPoints, sID);
					if (y-2 >= 0)
						GetSector(x, y - 2).AddOwnerPoints(ownerPoints, sID);
					if (y+2 < STARMAP_SECTORS_VCOUNT)
						GetSector(x, y + 2).AddOwnerPoints(ownerPoints, sID);
				}
			}
		}
	}

	// Jetzt die Besitzer berechnen und die Variablen, welche nächste Runde auch angezeigt werden sollen.
	for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
	{
		CSystem& system = GetSystemForSector(*sector);
		sector->CalculateOwner(system.GetOwnerOfSystem());
		if (sector->GetSunSystem() == TRUE && system.GetOwnerOfSystem() != "")
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(system.GetOwnerOfSystem()));
			if (!pMajor || !pMajor->IsMajor())
				continue;

			// baubare Gebäude, Schiffe und Truppen berechnen
			system.CalculateBuildableBuildings(&*sector, &BuildingInfo, pMajor, &m_GlobalBuildings);
			system.CalculateBuildableShips(this, sector->GetKO());
			system.CalculateBuildableTroops(&m_TroopInfo, pMajor->GetEmpire()->GetResearch());
			system.CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), sector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());

			// alle produzierten FP und SP der Imperien berechnen und zuweisen
			int currentPoints;
			currentPoints = system.GetProduction()->GetResearchProd();
			pMajor->GetEmpire()->AddFP(currentPoints);
			currentPoints = system.GetProduction()->GetSecurityProd();
			pMajor->GetEmpire()->AddSP(currentPoints);
		}

		// Gibt es eine Anomalie im Sektor, so vielleicht die Scanpower niedriger setzen
		if (sector->GetAnomaly())
			sector->GetAnomaly()->ReduceScanPower(sector->GetKO());
	}

	// Nachdem die Besitzerpunkte der Sektoren berechnet wurden kann versucht werden neue Rassen kennenzuelernen
	CalcContactNewRaces();

	// Nun das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse aller Imperien
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	// Dies gilt nur für Majorsraces.
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;

		BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		{
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				// nur aktuell baubare Schiffe dürfen überprüft werden, wenn wir die Beamwaffen checken
				if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(researchLevels))
				{
					// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
					pMajor->GetWeaponObserver()->CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
					pMajor->GetWeaponObserver()->CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
				}
			}
		}
	}

	m_VictoryObserver.Observe();
	if (m_VictoryObserver.IsVictory())
	{
		// Victoryeventscreen für die Siegerrasse
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			// Alle Nachrichten und Events löschen
			for (int i = 0; i < pMajor->GetEmpire()->GetEventMessages()->GetSize(); i++)
				delete pMajor->GetEmpire()->GetEventMessages()->GetAt(i);

			pMajor->GetEmpire()->GetEventMessages()->RemoveAll();
			pMajor->GetEmpire()->GetMessages()->RemoveAll();
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
			m_SoundMessages[client].RemoveAll();

			// Wenn es ein menschlicher Spieler ist, so bekommt er den Eventscreen für den Sieg angezeigt
			if (pMajor->IsHumanPlayer())
			{
				// einen neuen (und auch einzigen Event) einfügen
				CString sImageName;
				if (pMajor->GetRaceID() == m_VictoryObserver.GetVictoryRace())
					sImageName = "Victory" + pMajor->GetRaceID();
				else
					sImageName = "GameOver";
				CEventVictory* eventScreen = new CEventVictory(pMajor->GetRaceID(), m_VictoryObserver.GetVictoryRace(), (int)m_VictoryObserver.GetVictoryType(), sImageName);
				pMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
			}
		}
	}

	// Bestimmte Views zurücksetzen
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_ICON);
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		if (m_iSelectedView[i] == FLEET_VIEW)
			m_iSelectedView[i] = GALAXY_VIEW;

}

/// Funktion berechnet, ob zufällig Alienschiffe ins Spiel kommen.
void CBotf2Doc::CalcRandomAlienEntities()
{
	const CIniLoader* pIni = CIniLoader::GetInstance();
	if (!pIni->ReadValueDefault("Special", "ALIENENTITIES", true))
		return;

	// Aliens zufällig ins Spiel bringen
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
	{
		CShipInfo* pShipInfo = &m_ShipInfoArray.GetAt(i);
		if (!pShipInfo->IsAlien())
			continue;

		// zugehörige Minorrace finden
		if (CMinor* pAlien = dynamic_cast<CMinor*>(m_pRaceCtrl->GetRace(pShipInfo->GetOnlyInSystem())))
		{
			if (!pAlien->IsAlienRace())
			{
				ASSERT(pAlien->IsAlienRace());
				continue;
			}

			// Keine Alien-Kampfstation erneut bauen
			if (pAlien->GetRaceID() == KAMPFSTATION)
				continue;

			// Prüfen ob das Alienschiff zum Galaxieweiten technologischen Fortschritt passt
			// Alienschiff das höhere Voraussetzungen als der technologische Fortschritt hat
			// kommt nicht ins Spiel. Ältere Alienschiffe, die viel geringer als der Fortschritt
			// sind kommen mit niedrigerer Wahrscheinlichkeit ins Spiel.
			BYTE byAvgTechLevel = m_Statistics.GetAverageTechLevel();
			BYTE researchLevels[6] = {byAvgTechLevel, byAvgTechLevel, byAvgTechLevel, byAvgTechLevel, byAvgTechLevel, byAvgTechLevel};
			if (!pShipInfo->IsThisShipBuildableNow(researchLevels))
				continue;

			// jedes Level unterhalb der durchschnittlichen Techstufe verringert sich die Wahrscheinlichkeit
			// des Auftauchens des Alienschiffes
			BYTE byAvgShipTech = (pShipInfo->GetBioTech() + pShipInfo->GetEnergyTech() + pShipInfo->GetComputerTech() + pShipInfo->GetConstructionTech() + pShipInfo->GetPropulsionTech() + pShipInfo->GetWeaponTech()) / 6;
			int nMod = max(byAvgTechLevel - byAvgShipTech, 0) * 5;

			// nur ca. aller X + Techmodifikator Runden kommt das Alienschiff ins Spiel
			// X ist abhängig von der Galaxiegröße. Pro Sektor gibt es eine Wahrscheinlichkeit von 0.01%, das in diesem
			// ein Alien entsteht. Umso mehr Sektoren es gibt, desto höher ist die Gesamtwahrscheinlichkeit.
			// Ist die Galaxie kleiner, so kommen weniger Aliens ins Spiel, ist sie größer, kommen mehr Aliens ins Spiel.
			int nValue = STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT;
			// Pro Techlevel verringert sich die virtuelle Anzahl der Sektoren um 25% -> geringere Wahrscheinlichkeit
			nValue /= ((nMod * 4 + 100.0) / 100.0);
			float fSteuerParameter = 1.50f;	// Hiermit kann man leicht die Wahrscheinlichkeit steuern, aktuell 50% niedriger!
			if (rand()%((int)(10000 * fSteuerParameter)) > nValue)
				continue;

			// zufälligen Sektor am Rand der Map ermitteln
			while (true)
			{
				// Schiff irgendwo an einem Rand der Map auftauchen lassen
				CPoint p;
				switch(rand()%4)
				{
				case 0: p = CPoint(0, rand()%STARMAP_SECTORS_VCOUNT); break;
				case 1: p = CPoint(STARMAP_SECTORS_HCOUNT - 1, rand()%STARMAP_SECTORS_VCOUNT); break;
				case 2: p = CPoint(rand()%STARMAP_SECTORS_HCOUNT, 0); break;
				case 3: p = CPoint(rand()%STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT - 1); break;
				default: p = CPoint(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
				}

				// nicht auf einer Anomalie!
				if (!GetSector(p.x, p.y).GetAnomaly())
				{
					CShipMap::iterator pShip = BuildShip(pShipInfo->GetID(), p, pAlien->GetRaceID());
					// unterschiedliche Aliens unterschieden und Schiffseigenschaften festlegen
					if (pAlien->GetRaceID() == IONISIERENDES_GASWESEN)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == GABALLIANER_SEUCHENSCHIFF)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == BLIZZARD_PLASMAWESEN)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == MORLOCK_RAIDER)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						// zufällig gleich mehrere Raider bauen. Umso höher der technische Durchschnitt
						// in der Galaxie ist, desto mehr Raider kommen auf dem System ins Spiel.
						if (nMod > 0)
						{
							int nCount = rand()%(nMod + 1);
							while (nCount > 0)
							{
								// Erst das Schiff bauen
								CShipMap::iterator pFleetShip = BuildShip(pShipInfo->GetID(), p,
									pAlien->GetRaceID());

								// Raider in Gruppe stecken und Befehle gleich mit übernehmen
								pShip->second->AddShipToFleet(pFleetShip->second);
								m_ShipMap.EraseAt(pFleetShip, false);

								nCount--;
							}
						}
					}
					else if (pAlien->GetRaceID() == BOSEANER)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == KRYONITWESEN)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == MIDWAY_ZEITREISENDE)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == ANAEROBE_MAKROBE)
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						// zufällig gleich mehrere Anaerobe Makroben bauen. Umso höher der technische Durchschnitt
						// in der Galaxie ist, desto mehr Anaerobe Makroben kommen auf dem System ins Spiel.
						if (nMod > 0)
						{
							// viele Anaerobe Makroben bauen, (daher hier mal 2)
							int nCount = rand()%(nMod * 2 + 1);
							while (nCount > 0)
							{
								// Erst das Schiff bauen
								CShipMap::iterator pFleetShip = BuildShip(pShipInfo->GetID(), p,
									pAlien->GetRaceID());

								// Anaerobe Makroben in Gruppe stecken und Befehle gleich mit übernehmen
								pShip->second->AddShipToFleet(pFleetShip->second);
								m_ShipMap.EraseAt(pFleetShip, false);

								nCount--;
							}
						}
					}
					else if (pAlien->GetRaceID() == ISOTOPOSPHAERISCHES_WESEN)
					{
						// 50% auf Meiden, zu 50% auf angreifen
						if (rand()%2 == 0)
							pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						else
							pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}

					break;
				}
			}
		}
	}
}

/// Funktion berechnet Auswirkungen von Alienschiffe auf Systeme, über denen sie sich befinden.
void CBotf2Doc::CalcAlienShipEffects()
{
	bool bBattleStationIngame = false;	// merkt sich ob die Kampfstation noch im Spiel ist

	for (CShipMap::const_iterator ship = m_ShipMap.begin(); ship != m_ShipMap.end(); ++ship)
	{
		if (!ship->second->IsAlien())
			continue;

		CMinor* pAlien = dynamic_cast<CMinor*>(m_pRaceCtrl->GetRace(ship->second->GetOwnerOfShip()));
		if (!pAlien || !pAlien->IsAlienRace())
		{
			ASSERT(FALSE);
			continue;
		}

		const CPoint& co = ship->second->GetKO();
		
		// verschiedene Alienrassen unterscheiden
		if (pAlien->GetRaceID() == IONISIERENDES_GASWESEN)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			CString sSystemOwner = GetSystem(co.x, co.y).GetOwnerOfSystem();
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner));
			if (!pOwner)
				continue;

			// Energie im System auf 0 setzen
			GetSystem(co.x, co.y).SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (GetSystem(co.x, co.y).GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CResourceManager::GetString("EVENT_IONISIERENDES_GASWESEN", FALSE, GetSector(co.x, co.y).GetName());
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(co.x, co.y).GetName(), co, 0);
				pOwner->GetEmpire()->AddMessage(message);
				if (pOwner->IsHumanPlayer())
				{
					CEventAlienEntity* eventScreen = new CEventAlienEntity(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetRaceName(), s);
					pOwner->GetEmpire()->GetEventMessages()->Add(eventScreen);

					network::RACE client = m_pRaceCtrl->GetMappedClientID(pOwner->GetRaceID());
					m_iSelectedView[client] = EMPIRE_VIEW;
				}
			}
		}
		else if (pAlien->GetRaceID() == GABALLIANER_SEUCHENSCHIFF)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			CString sSystemOwner = GetSystem(co.x, co.y).GetOwnerOfSystem();
			if (CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner)))
			{
				// Nahrung im System auf 0 setzen
				GetSystem(co.x, co.y).SetDisabledProduction(WORKER::FOOD_WORKER);
				GetSystem(co.x, co.y).SetFoodStore(GetSystem(co.x, co.y).GetFoodStore() / 2);

				// Wenn Nahrung produziert oder vorhanden ist, dann die Nachricht bringen über Nahrung verseucht
				if (GetSystem(co.x, co.y).GetProduction()->GetMaxFoodProd() > 0 || GetSystem(co.x, co.y).GetFoodStore() > 0)
				{
					// Nachricht und Event einfügen
					CString s = CResourceManager::GetString("EVENT_GABALLIANER_SEUCHENSCHIFF", FALSE, GetSector(co.x, co.y).GetName());
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(co.x, co.y).GetName(), co, 0);
					pOwner->GetEmpire()->AddMessage(message);
					if (pOwner->IsHumanPlayer())
					{
						CEventAlienEntity* eventScreen = new CEventAlienEntity(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetRaceName(), s);
						pOwner->GetEmpire()->GetEventMessages()->Add(eventScreen);

						network::RACE client = m_pRaceCtrl->GetMappedClientID(pOwner->GetRaceID());
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
				}
			}

			// befinden sich Schiffe in diesem Sektor, so werden diese ebenfalls zu Seuchenschiffen (33%)
			if (GetSector(co.x, co.y).GetIsShipInSector() && rand()%3 == 0)
			{
				// alle Schiffe im Sektor zu Seuchenschiffen machen
				for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
				{
					// Schiff im gleichen Sektor?
					// keine anderen Alienschiffe
					// keine Außenposten und Sternenbasen
					if (y->second->GetKO() != co || y->second->IsAlien() || y->second->IsStation())
						continue;
					std::vector<CShips*> vShips;
					vShips.push_back(y->second);
					vShips.reserve(y->second->GetFleetSize());
					for(CShips::iterator i = y->second->begin(); i != y->second->end(); ++i)
						vShips.push_back(i->second);

					for(unsigned i = 0; i < vShips.size(); ++i)
					{
						CShips* pShip = vShips.at(i);
						// Schiffe mit Rückzugsbefehl werden nie vom Virus befallen
						if (pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
							continue;

						pShip->SetShipType(SHIP_TYPE::ALIEN);
						pShip->SetTargetKO(CPoint(-1, -1));
						pShip->UnsetCurrentOrder();
						pShip->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						pShip->SetIsShipFlagShip(FALSE);

						CMajor* pShipOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip()));
						assert(pShipOwner);
						// für jedes Schiff eine Meldung über den Verlust machen
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						pShipOwner->AddToLostShipHistory(*pShip, CResourceManager::GetString("COMBAT"), CResourceManager::GetString("MISSED"), m_iRound);
						CString s;
						s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,pShip->GetShipName()));
						CMessage message;
						message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", pShip->GetKO(), 0);
						pShipOwner->GetEmpire()->AddMessage(message);
						//actually change the owner last, to make the above calls work correctly
						pShip->SetOwnerOfShip(pAlien->GetRaceID());
					}
				}
			}
		}
		else if (pAlien->GetRaceID() == BLIZZARD_PLASMAWESEN)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			CString sSystemOwner = GetSystem(co.x, co.y).GetOwnerOfSystem();
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner));
			if (!pOwner)
				continue;

			// Energie im System auf 0 setzen
			GetSystem(co.x, co.y).SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (GetSystem(co.x, co.y).GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CResourceManager::GetString("EVENT_BLIZZARD_PLASMAWESEN", FALSE, GetSector(co.x, co.y).GetName());
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(co.x, co.y).GetName(), co, 0);
				pOwner->GetEmpire()->AddMessage(message);
				if (pOwner->IsHumanPlayer())
				{
					CEventAlienEntity* eventScreen = new CEventAlienEntity(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetRaceName(), s);
					pOwner->GetEmpire()->GetEventMessages()->Add(eventScreen);

					network::RACE client = m_pRaceCtrl->GetMappedClientID(pOwner->GetRaceID());
					m_iSelectedView[client] = EMPIRE_VIEW;
				}
			}
		}
		else if (pAlien->GetRaceID() == MORLOCK_RAIDER)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			// Creditproduktion auf 0 stellen
			CSystem* pSystem = &GetSystem(co.x, co.y);
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pSystem->GetOwnerOfSystem()));
			if (!pOwner)
				continue;

			// existiert keine Freundschaft zum Major
			if (pAlien->GetAgreement(pOwner->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				continue;

			// alte Credits merken und aktuell auf 0 stellen
			short nCreditProd = pSystem->GetProduction()->GetCreditsProd();
			// Nichts machen wenn keine Credits im System produziert werden, z.B. durch einen
			// vorherigen Raider.
			if (nCreditProd <= 0)
				continue;

			pSystem->GetProduction()->DisableCreditsProduction();

			// Nachricht und Event einfügen
			CString sCredits = "";
			sCredits.Format("%d", nCreditProd);
			CString s = CResourceManager::GetString("EVENT_MORLOCK_RAIDER", FALSE, sCredits, GetSector(co.x, co.y).GetName());
			CMessage message;
			message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(co.x, co.y).GetName(), co, 0);
			pOwner->GetEmpire()->AddMessage(message);
			if (pOwner->IsHumanPlayer())
			{
				CEventAlienEntity* eventScreen = new CEventAlienEntity(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetRaceName(), s);
				pOwner->GetEmpire()->GetEventMessages()->Add(eventScreen);

				network::RACE client = m_pRaceCtrl->GetMappedClientID(pOwner->GetRaceID());
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
		}
		else if (pAlien->GetRaceID() == BOSEANER)
		{
			// zufällig die anfangs auf Meiden gestellten Boseaner auf Angreifen stellen
			// Sie sind auf der Suche nach Nahrung ;-)
			if (rand()%10 == 0)
				ship->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
		}
		else if (pAlien->GetRaceID() == KAMPFSTATION)
		{
			bBattleStationIngame = true;
		}
		else if (pAlien->GetRaceID() == MIDWAY_ZEITREISENDE)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			// Befindet sich das Midway Schlachtschiff über einem System von einem Major,
			// so wird dieses im Kriegsfall bombardiert. Ansonsten verringert sich die Beziehung mit
			// jeder Runde. Umso länger das Midway-Schlachtschiff über einem System steht, umso
			// wahrscheinlicher ist es, dass Krieg erklärt wird (nur den Midways helfen indem man Credits
			// übergibt kann dagegen helfen).
			CSystem* pSystem = &GetSystem(co.x, co.y);
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pSystem->GetOwnerOfSystem()));
			assert(pOwner->IsMajor());
			if (pOwner->GetAgreement(pAlien->GetRaceID()) == DIPLOMATIC_AGREEMENT::WAR)
				ship->second->SetCurrentOrder(SHIP_ORDER::ATTACK_SYSTEM);
			else
				pAlien->SetRelation(pOwner->GetRaceID(), -rand()%20);
		}
		else if (pAlien->GetRaceID() == ISOTOPOSPHAERISCHES_WESEN)
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			// Zu 20% teleportiert sich das Isotoposphärische Wesen an einen zufälligen Ort der Galaxie (nicht auf Anomalien).
			// Befinden sich Schiffe im selben Sektor, so werden diese mitgerissen.
			if (rand()%5 == 0)
			{
				// irgend ein zufülliges neues Ziel generieren, welches nicht auf einer Anomalie endet
				while (true)
				{
					CPoint ptTarget = CPoint(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
					if (ptTarget == ship->second->GetKO())
						continue;

					if (GetSector(ptTarget.x, ptTarget.y).GetAnomaly())
						continue;

					// Alle Schiffe in diesem Sektor werden mit teleportiert (außer andere Aliens)
					for (CShipMap::const_iterator shipInSector = m_ShipMap.begin(); shipInSector != m_ShipMap.end(); ++shipInSector)
					{
						if (shipInSector->second->GetKO() != ship->second->GetKO())
							continue;

						if (shipInSector->second->IsAlien())
							continue;

						if (shipInSector->second->IsStation())
							continue;

						if (shipInSector->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
							continue;

						if (shipInSector->second == ship->second)
							continue;

						// erst alle Befehle rückgängig machen
						shipInSector->second->UnsetCurrentOrder();

						// dann das Ziel entfernen
						shipInSector->second->SetTargetKO(CPoint(-1, -1));
						shipInSector->second->GetPath()->RemoveAll();

						// dann aktuelle Koordinate setzen
						shipInSector->second->SetKO(ptTarget.x, ptTarget.y);
					}

					// Isotoposphärisches Wesen selbst teleportieren
					ship->second->SetTargetKO(CPoint(-1, -1));
					ship->second->GetPath()->RemoveAll();
					ship->second->SetKO(ptTarget.x, ptTarget.y);

					break;
				}

				// weiter nicht gleich die Energie lahmlagen
				continue;
			}

			// Wenn über einem System von einem Major, dann die Energie auf 0 setzen
			CString sSystemOwner = GetSystem(co.x, co.y).GetOwnerOfSystem();
			if (CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner)))
			{
				// Energie im System auf 0 setzen
				GetSystem(co.x, co.y).SetDisabledProduction(WORKER::ENERGY_WORKER);

				// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
				if (GetSystem(co.x, co.y).GetProduction()->GetMaxEnergyProd() > 0)
				{
					// Nachricht und Event einfügen
					CString s = CResourceManager::GetString("EVENT_ISOTOPOSPHAERISCHES_WESEN", FALSE, GetSector(co.x, co.y).GetName());
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(co.x, co.y).GetName(), co, 0);
					pOwner->GetEmpire()->AddMessage(message);
					if (pOwner->IsHumanPlayer())
					{
						CEventAlienEntity* eventScreen = new CEventAlienEntity(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetRaceName(), s);
						pOwner->GetEmpire()->GetEventMessages()->Add(eventScreen);

						network::RACE client = m_pRaceCtrl->GetMappedClientID(pOwner->GetRaceID());
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
				}
			}			
		}
	}

	// Wurde die Kampfstation vernichtet und die Alienrasse "Kampfstation" gibt es noch, dann Nachticht
	// über deren Vernichtung erstellen
	if (!bBattleStationIngame)
	{
		if (CMinor* pBattleStation = dynamic_cast<CMinor*>(m_pRaceCtrl->GetRace(KAMPFSTATION)))
		{
			CalcEffectsMinorEleminated(pBattleStation);
			m_pRaceCtrl->RemoveRace(pBattleStation->GetRaceID());
		}
	}
}

void CBotf2Doc::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: Fügen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->Enable(FALSE);
}

void CBotf2Doc::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Fügen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->Enable(FALSE);
}

BOOL CBotf2Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
//	return __super::OnOpenDocument(lpszPathName);

	MYTRACE("general")(MT::LEVEL_INFO, "loading savegame \"%s\"\n", lpszPathName);

	CFile file;
	BYTE *lpBuf = NULL;

	{
	CFileException ex;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: could not open file (%s)\n", cause);
		return FALSE;
	}

	// Header lesen
	UINT nSize = 4 + 2 * sizeof(UINT);
	lpBuf = new BYTE[nSize];
	UINT nDone = file.Read(lpBuf, nSize);
	if (nDone < nSize)
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: unexpected end of file\n");
		goto error;
	}

	// Magic Number prüfen
	BYTE *p = lpBuf;
	if (memcmp(p, "BotE", 4) != 0)
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: invalid magic number\n");
		goto error;
	}
	p += 4;

	// Versionsnummer prüfen
	UINT nVersion = 0;
	memcpy(&nVersion, p, sizeof(UINT));
	p += sizeof(UINT);
	if (nVersion != DOCUMENT_VERSION)
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: wrong version\n");
		goto error;
	}

	// Länge der Daten lesen
	memcpy(&nSize, p, sizeof(UINT));
	p += sizeof(UINT);

	// Daten aus Datei in Puffer lesen
	delete[] lpBuf;
	lpBuf = new BYTE[nSize];

	nDone = file.Read(lpBuf, nSize);
	if (nDone < nSize)
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: unexpected end of file\n");
		goto error;
	}

	// Dekomprimieren
	CMemFile memFile;
	if (!BotE_LzmaDecompress(lpBuf, nSize, memFile))
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: error during decompression\n");
		goto error;
	}
	memFile.Seek(0, CFile::begin);

	// Deserialisieren
	CArchive ar(&memFile, CArchive::load);
//	Reset();
	SetModifiedFlag();
	Serialize(ar);
	ar.Close();
	SetModifiedFlag(FALSE);

	// aufräumen
	delete[] lpBuf;
	file.Close();
	return TRUE;
	}

error:
	if (lpBuf) delete[] lpBuf;
	file.Close();
	return FALSE;
}

BOOL CBotf2Doc::OnSaveDocument(LPCTSTR lpszPathName)
{
//	return __super::OnSaveDocument(lpszPathName);

	MYTRACE("general")(MT::LEVEL_INFO, "storing savegame \"%s\"\n", lpszPathName);

	// Savegame schreiben
	CFileException ex;
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeBinary,
		&ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: could not open file (%s)\n", cause);
		return FALSE;
	}

	// Magic Number
	file.Write("BotE", 4);

	// Versionsnummer
	UINT nVersion = DOCUMENT_VERSION;
	file.Write(&nVersion, sizeof(UINT));

	// Platzhalter für Länge der Daten
	UINT nSize = 0;
	file.Write(&nSize, sizeof(UINT));

	// Daten serialisieren
	CMemFile memFile;
	CArchive ar(&memFile, CArchive::store);
	Serialize(ar);
	ar.Close();

	// komprimieren, in Datei schreiben
	nSize = memFile.GetLength();
	BYTE *lpBuf = memFile.Detach();
        //MYTRACE("general")(MT::LEVEL_INFO, "rainer-Test", lpszPathName);
	if (!BotE_LzmaCompress(lpBuf, nSize, file))
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "savegame: error during compression\n");
		free(lpBuf);
		file.Close();
		try
		{
			CFile::Remove(lpszPathName);
		}
		catch (CFileException *pEx)
		{
			TCHAR cause[255];
			pEx->GetErrorMessage(cause, 255);
			MYTRACE("general")(MT::LEVEL_ERROR, "savegame: could not delete corrupted savegame (%s)\n", cause);
			pEx->Delete();
		}
		return FALSE;
	}
	free(lpBuf);

	// Länge des komprimierten Puffers an Stelle des Platzhalters eintragen
	nSize = file.GetLength() - 4 - 2 * sizeof(UINT);
	file.Seek(4 + sizeof(UINT), CFile::begin);
	file.Write(&nSize, sizeof(UINT));

	file.Close();
	SetModifiedFlag(FALSE);
	return TRUE;
}

void CBotf2Doc::AllocateSectorsAndSystems()
{
	STARMAP_TOTALWIDTH = STARMAP_SECTORS_HCOUNT * STARMAP_SECTOR_WIDTH;
	STARMAP_TOTALHEIGHT = STARMAP_SECTORS_VCOUNT * STARMAP_SECTOR_HEIGHT;

	const unsigned size = STARMAP_SECTORS_HCOUNT*STARMAP_SECTORS_VCOUNT;
	m_Sectors.resize(size);
	m_Systems.resize(size);
 }

void CBotf2Doc::RandomSeed(const int* OnlyIfDifferentThan) {
	const CIniLoader* pIni = CIniLoader::GetInstance();
	int nSeed = pIni->ReadValueDefault("Special", "RANDOMSEED", -1);
	if(OnlyIfDifferentThan && *OnlyIfDifferentThan == nSeed)
		return;
	if(nSeed < 0)
		// zufälligen Seed verwenden
		nSeed = (unsigned)time(NULL);
	// sonst festen vorgegeben Seed verwenden
	srand(nSeed);
	MYTRACE("general")(MT::LEVEL_INFO, "Used seed for randomgenerator: %i", nSeed);
}
