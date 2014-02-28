// BotEDoc.cpp : Implementierung der Klasse CBotEDoc
//

#include "stdafx.h"
#include "resources.h"
#include "BotE.h"
#include "BotEDoc.h"
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
#include "General/Loc.h"
#include "GraphicPool.h"
#include "Galaxy/GenSectorName.h"

#include "Races\RaceController.h"
#include "Races\DiplomacyController.h"
#include "Ships\Combat.h"
#include "System\AttackSystem.h"
#include "Intel\IntelCalc.h"
#include "RandomEventCtrl.h"
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
#include "ClientWorker.h"
#include "SettingsDlg.h"
#include "AssertBotE.h"
#include "Races/Alien.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBotEDoc

IMPLEMENT_DYNCREATE(CBotEDoc, CDocument)

BEGIN_MESSAGE_MAP(CBotEDoc, CDocument)
	//{{AFX_MSG_MAP(CBotEDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CBotEDoc::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CBotEDoc::OnUpdateFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotEDoc Konstruktion/Destruktion

CBotEDoc::CBotEDoc() :
	m_iRound(1),
	m_fDifficultyLevel(1.0f),
	m_fStardate(121000.0f),
	m_ptKO(0, 0),
	m_Systems(),
	m_ShipMap(&m_Systems),
	m_bDataReceived(false),
	m_bRoundEndPressed(false),
	m_bDontExit(false),
	m_bGameLoaded(false),
	m_ptCurrentCombatSector(-1, -1),
	m_bCombatCalc(false),
	m_nCombatOrder(COMBAT_ORDER::NONE),
	m_bNewGame(true)
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
	CLoc::Init();

	m_pGraphicPool = new CGraphicPool(CIOData::GetInstance()->GetAppPath() + "Graphics\\");

	m_pRaceCtrl = new CRaceController();
	m_pClientWorker = CClientWorker::GetInstance();
	resources::BuildingInfo = &BuildingInfo;

	m_pAIPrios = new CAIPrios(this);
	m_pSectorAI= new CSectorAI(this);

	m_pNetworkHandler = new CNetworkHandler(this);
	server.AddServerListener(m_pNetworkHandler);
	client.AddClientListener(m_pNetworkHandler);
}

CBotEDoc::~CBotEDoc()
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

	// stop MT
	MYTRACE_DEINIT;
}

BOOL CBotEDoc::OnNewDocument()
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
	AssertBotE(pIni);

	bool bHardwareSound;
	pIni->ReadValue("Audio", "HARDWARESOUND", bHardwareSound);
	CSoundManager::GetInstance()->Init(!bHardwareSound);
	RandomSeed();

	// Standardwerte setzen
	m_ptKO = CPoint(0,0);
	m_bRoundEndPressed			= false;

	return TRUE;
}

/// Funktion schließt die Verbindung zum Server und beendet Bote.
void CBotEDoc::GameOver()
{
	// vom Server trennen
	client.Disconnect();
	Sleep(2000);

	SetModifiedFlag(FALSE);

	// Spiel verlassen
	PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CLOSE, NULL, NULL);
}

/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zurück.
/// @return Zeiger auf Majorrace-Rassenobjekt
CString CBotEDoc::GetPlayersRaceID(void) const
{
	return m_pClientWorker->GetMappedRaceID(client.GetClientRace());
}

/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zurück.
/// @return Zeiger auf Majorrace-Rassenobjekt
CMajor* CBotEDoc::GetPlayersRace(void) const
{
	// zuerst muss eine Netzwerknummer, also RACE1 bis RACE6 (1-6)
	// auf eine bestimmte Rassen-ID gemappt werden. Dies ist dann
	// die Rassen-ID.
	const CString& s = GetPlayersRaceID();
	CMajor* pPlayersRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(s));
	AssertBotE(pPlayersRace);

	return pPlayersRace;
}

/////////////////////////////////////////////////////////////////////////////
// CBotEDoc Serialisierung

void CBotEDoc::Serialize(CArchive& ar)
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


		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);

		ar<< m_TroopInfo.GetSize();//Truppen in Savegame speichern
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_TroopInfo.GetAt(i).Serialize(ar);

		// statische Variablen serialisieren
		for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
			ar << CTrade::GetMonopolOwner(j);

		// Forschungsmodifikator
		ar << CResearchInfo::m_dResearchSpeedFactor;

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
		AllocateSystems();

		ar >> number;
		m_ShipInfoArray.RemoveAll();
		m_ShipInfoArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		ar >> number;
		m_TroopInfo.RemoveAll();
		m_TroopInfo.SetSize(number);
		for (int i = 0; i<number; i++)
			m_TroopInfo.GetAt(i).Serialize(ar);

		// Gebäudeinfos werden nun beim Laden neu eingelesen
		BuildingInfo.RemoveAll();
		this->ReadBuildingInfosFromFile();

		// statische Variablen serialisieren
		for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
		{
			CString sOwnerID;
			ar >> sOwnerID;
			CTrade::SetMonopolOwner(j, sOwnerID);
		}

		// Forschungsmodifikator
		ar >> CResearchInfo::m_dResearchSpeedFactor;

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

	m_ShipMap.Serialize(ar);
	SerializeSectorsAndSystems(ar);

	CMoralObserver::SerializeStatics(ar);

	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	m_Statistics.Serialize(ar);

	m_pRaceCtrl->Serialize(ar);
	m_pClientWorker->Serialize(ar, false);

	if (ar.IsLoading())
	{
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			it->second->GetEmpire()->GenerateSystemList(m_Systems);
	}

	m_VictoryObserver.Serialize(ar);
}

void CBotEDoc::SerializeSectorsAndSystems(CArchive& ar)
{
	for(std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
		it->Serialize(ar, false);
}

/// Serialisiert die Daten, welche am Anfang des Spiels einmal gesendet werden müssen.
void CBotEDoc::SerializeBeginGameData(CArchive& ar)
{
	m_bDataReceived = false;
	// senden auf Serverseite
	if (ar.IsStoring())
	{
		//Kartengröße
		ar << STARMAP_SECTORS_HCOUNT;
		ar << STARMAP_SECTORS_VCOUNT;

		// Gebäudeinformationen
		ar << BuildingInfo.GetSize();
		for (int i = 0; i < BuildingInfo.GetSize(); i++)
			BuildingInfo.GetAt(i).Serialize(ar);

		// Truppeninformationen
		ar << m_TroopInfo.GetSize();
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_TroopInfo.GetAt(i).Serialize(ar);

		// Forschungsmodifikator
		ar << CResearchInfo::m_dResearchSpeedFactor;
	}
	// Empfangen auf Clientseite
	else
	{
		// Kartengröße und Initialisierung
		ar >> STARMAP_SECTORS_HCOUNT;
		ar >> STARMAP_SECTORS_VCOUNT;
		AllocateSystems();

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

		// Forschungsmodifikator
		ar >> CResearchInfo::m_dResearchSpeedFactor;
	}

	CMoralObserver::SerializeStatics(ar);
}

void CBotEDoc::SerializeNextRoundData(CArchive &ar)
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
		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);

		// statische Variablen serialisieren
		for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
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
		ar >> number;
		m_ShipInfoArray.RemoveAll();
		m_ShipInfoArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		// statische Variablen serialisieren
		for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
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

	m_ShipMap.Serialize(ar);
	SerializeSectorsAndSystems(ar);
	m_pRaceCtrl->Serialize(ar);
	m_pClientWorker->Serialize(ar, true);

	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	m_Statistics.Serialize(ar);
	m_VictoryObserver.Serialize(ar);

	if (ar.IsLoading())
	{
		CSmallInfoView::SetPlanet(NULL);
		//GenerateStarmap();

		CMajor* pPlayer = GetPlayersRace();
		// bekommt der Client hier keine Spielerrasse zurück, so ist er ausgeschieden
		AssertBotE(pPlayer);
		if (pPlayer == NULL)
		{
			AfxMessageBox("Fatal Error ... exit game now");
			GameOver();
			return;
		}

		// Sprachmeldungen an den Soundmanager schicken
		m_pClientWorker->CommitSoundMessages(CSoundManager::GetInstance(), *pPlayer);

		// Systemliste der Imperien erstellen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			it->second->GetEmpire()->GenerateSystemList(m_Systems);
	}
	MYTRACE("general")(MT::LEVEL_INFO, "... serialization of NextRoundData succesfull\n");
}

void CBotEDoc::SerializeEndOfRoundData(CArchive &ar, network::RACE race)
{
	if (ar.IsStoring())
	{
		CMajor* pPlayer = GetPlayersRace();
		if (m_bCombatCalc)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Client %d sending CombatData to server...\n", race);

			// nur Informationen über die Taktik der Schiffe bzw. die Taktik des Kampfes senden
			ar << m_nCombatOrder;
			return;
		}

		MYTRACE("general")(MT::LEVEL_INFO, "Client %d sending EndOfRoundData to server...\n", race);
		// Client-Dokument
		// Anzahl der eigenen Schiffsinfoobjekte ermitteln
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pPlayer->GetRaceShipNumber())
				m_ShipInfoArray.GetAt(i).Serialize(ar);


		m_ShipMap.SerializeEndOfRoundData(ar, pPlayer->GetRaceID());

		std::vector<int> systems;
		for(std::vector<CSystem>::const_iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
		{
			if(it->OwnerID() == pPlayer->GetRaceID() && it->Majorized())
				systems.push_back(it - m_Systems.begin());
		}
		ar << systems.size();
		for(std::vector<int>::const_iterator it = systems.begin(); it != systems.end(); ++it)
		{
			ar << *it;
			m_Systems.at(*it).Serialize(ar, true);
		}

		pPlayer->Serialize(ar);
		// aktuelle View mit zum Server senden
		ar << static_cast<unsigned short>(m_pClientWorker->GetSelectedViewFor(pPlayer->GetRaceID()));
	}
	else
	{
		// vom Client gespielte Majorrace-ID ermitteln
		const CString& sMajorID = m_pClientWorker->GetMappedRaceID(race);
		CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sMajorID));
		AssertBotE(pMajor);

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
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				m_ShipInfoArray.GetAt(i).Serialize(ar);
				AssertBotE(m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber());
			}

		m_ShipMap.SerializeEndOfRoundData(ar, sMajorID);

		unsigned number = 0;
		ar >> number;
		for (unsigned i = 0; i < number; i++)
		{
			int index;;
			ar >> index;
			m_Systems.at(index).Serialize(ar, true);
		}

		pMajor->Serialize(ar);
		unsigned short view;
		ar >> view;
		m_pClientWorker->SetSelectedViewForTo(*pMajor, view);
	}
	MYTRACE("general")(MT::LEVEL_INFO, "... serialization of RoundEndData succesfull\n", race);
}

/// Funktion liest die Ini-Datei neu ein und legt die Werte neu fest.
void CBotEDoc::ResetIniSettings(void)
{
	CIniLoader* pIni = CIniLoader::GetInstance();
	AssertBotE(pIni);

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
	AssertBotE(pSoundManager);

	bool bHardwareSound;
	pIni->ReadValue("Audio", "HARDWARESOUND", bHardwareSound);
	CSoundManager::GetInstance()->Init(!bHardwareSound);

	bool bUseMusic;
	pIni->ReadValue("Audio", "MUSIC", bUseMusic);
	if (bUseMusic)
		m_pClientWorker->StartMusic(*pIni, *pSoundManager, *GetPlayersRace());
	else
		pSoundManager->StopMusic();


	bool bUseSound;
	pIni->ReadValue("Audio", "SOUND", bUseSound);
	if (!bUseSound)
	{
		pSoundManager->SetSoundMasterVolume(0.0f);
		pSoundManager->SetMessageMasterVolume(0.0f);
	}
	else
	{
		pSoundManager->SetSoundMasterVolume(0.5f);
		pSoundManager->SetMessageMasterVolume(0.5f);
	}
	MYTRACE("general")(MT::LEVEL_INFO, "Init sound ready...\n");

	RandomSeed();
}

void CBotEDoc::SetKO(int x, int y)
{
	m_ptKO = CPoint(x, y);
	CSmallInfoView::SetPlanet(NULL);

	if (resources::pMainFrame->GetActiveView(1, 1) == VIEWS::PLANET_BOTTOM_VIEW)
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
}

void CBotEDoc::SetCurrentShip(const CShipMap::iterator& position)
{
	m_ShipMap.SetCurrentShip(position);
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
}
void CBotEDoc::SetFleetShip(const CShipMap::iterator& position)
{
	m_ShipMap.SetFleetShip(position);
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
}
void CBotEDoc::SetShipInFleet(const CShipMap::iterator& position)
{
	FleetShip()->second->SetCurrentShip(position);
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_FLEET_MENU_VIEW);
}

/// Funktion lädt für die ausgewählte Spielerrasse alle Grafiken für die Views.
void CBotEDoc::LoadViewGraphics(void)
{
	CMajor* pPlayersRace = GetPlayersRace();
	AssertBotE(pPlayersRace);
	MYTRACE("general")(MT::LEVEL_INFO, "pPlayersRace: %s", pPlayersRace->GetName());

	CGalaxyMenuView::SetPlayersRace(pPlayersRace);
	CMainBaseView::SetPlayersRace(pPlayersRace);
	CBottomBaseView::SetPlayersRace(pPlayersRace);
	CMenuChooseView::SetPlayersRace(pPlayersRace);

	// Views die rassenspezifischen Grafiken laden lassen
	std::map<CWnd *, UINT>* views = &resources::pMainFrame->GetSplitterWindow()->views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		if (it->second == VIEWS::GALAXY_VIEW)
			((CGalaxyMenuView*)(it->first))->LoadRaceGraphics();
		else if (it->second == VIEWS::MENU_CHOOSE_VIEW)
			((CMenuChooseView*)(it->first))->LoadRaceGraphics();
		else if (VIEWS::is_main_view(it->second))
			((CMainBaseView*)(it->first))->LoadRaceGraphics();
		else if (VIEWS::is_bottom_view(it->second))
			((CBottomBaseView*)(it->first))->LoadRaceGraphics();
	}

	// Ini-Werte lesen und setzen
	ResetIniSettings();

	// ab jetzt müssen keine neuen Grafiken mehr geladen werden
	m_bNewGame = false;

	// Views ihre Arbeit zu Beginn jeder neuen Runde machen lassen
	DoViewWorkOnNewRound();

	// wenn neues Spiel gestartet wurde, dann initial auf die Galaxiekarte stellen
	if (m_pClientWorker->GetSelectedViewFor(pPlayersRace->GetRaceID()) == VIEWS::NULL_VIEW)
	{
		// zum Schluss die Galxieview auswählen (nicht eher, da gibts manchmal Probleme beim Scrollen ganz nach rechts)
		resources::pMainFrame->SelectMainView(VIEWS::GALAXY_VIEW, pPlayersRace->GetRaceID());
	}

	// Menü anzeigen bzw. verstecken
	bool bHideMenu;
	CIniLoader::GetInstance()->ReadValue("Control", "HIDEMENUBAR", bHideMenu);
	if (bHideMenu)
		resources::pMainFrame->SetMenuBarState(AFX_MBS_HIDDEN);
	else
		resources::pMainFrame->SetMenuBarState(AFX_MBS_VISIBLE);
}

void CBotEDoc::DoViewWorkOnNewRound()
{
	// Playersrace in Views festlegen
	CMajor* pPlayersRace = GetPlayersRace();
	AssertBotE(pPlayersRace);

	CGalaxyMenuView::SetPlayersRace(pPlayersRace);
	CMainBaseView::SetPlayersRace(pPlayersRace);
	CBottomBaseView::SetPlayersRace(pPlayersRace);
	CMenuChooseView::SetPlayersRace(pPlayersRace);

	// alle angezeigten Views neu zeichnen lassen
	// (nicht ans Ende der Funktion packen, das gibt Probleme mit dem Scrollen in der Galaxieansicht,
	// deren OnUpdate() muss vor dem View-Wechsel aufgerufen werden.)
	UpdateAllViews(NULL);

	// Views ihre Arbeiten zu Beginn einer neuen Runde durchführen lassen
	std::map<CWnd *, UINT>* views = &resources::pMainFrame->GetSplitterWindow()->views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		if (it->second == VIEWS::GALAXY_VIEW)
			((CGalaxyMenuView*)(it->first))->OnNewRound();
		else if (VIEWS::is_main_view(it->second))
			((CMainBaseView*)(it->first))->OnNewRound();
		else if (VIEWS::is_bottom_view(it->second))
			((CBottomBaseView*)(it->first))->OnNewRound();
		else if (it->second == VIEWS::MENU_CHOOSE_VIEW)
			((CMenuChooseView*)(it->first))->OnNewRound();
	}

	m_pClientWorker->DoViewWorkOnNewRound(*pPlayersRace);

#ifdef CONSISTENCY_CHECKS
	CSanity::GetInstance()->SanityCheckRacePtrUseCounts(*this);
#endif

	// wurde Rundenende geklickt zurücksetzen
	m_bRoundEndPressed = false;
	m_bDataReceived = true;
}

// Generiert ein neues Spiel
void CBotEDoc::PrepareData()
{
	MYTRACE("general")(MT::LEVEL_INFO, "Begin preparing game data...\n");

	if (!m_bGameLoaded)
	{
		m_pClientWorker->SetMajorsToHumanOrAi(*m_pRaceCtrl->GetMajors());

		// ALPHA6 DEBUG alle Rassen untereinander bekanntgeben
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap()) {
			for (CRaceController::const_iterator it = m_pRaceCtrl->begin(); it != m_pRaceCtrl->end(); ++it)
				for (CRaceController::const_iterator jt = m_pRaceCtrl->begin(); jt != m_pRaceCtrl->end(); ++jt)
					if (it->first != jt->first && it->second->IsMajor() && jt->second->IsMajor())
						it->second->SetIsRaceContacted(jt->first, true);
		}

		m_iRound = 1;

		// Generierungssektornamenklasse wieder neu starten
		m_ShipMap.Reset();
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

		// Forschungsgeschwindigkeitsmodifikator setzen
		CResearchInfo::m_dResearchSpeedFactor = 1.25;
		CIniLoader::GetInstance()->ReadValue("Special", "RESEARCHSPEED", CResearchInfo::m_dResearchSpeedFactor);

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
				int number=GetSystem(x, y).GetNumberOfPlanets();
				for (int i=0;i<number;i++)
				{
					CPlanet* planet = GetSystem(x, y).GetPlanet(i);
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
		m_pClientWorker->SetMajorsToHumanOrAi(*m_pRaceCtrl->GetMajors());
}

std::vector<std::string> CBotEDoc::GetForcedMinors() const
{
	std::vector<std::string> result;
	CString forced_minors;
	if(CIniLoader::GetInstance()->ReadValue("Special", "MINORS", forced_minors))
	{
		const std::string minors(CStrToStr(forced_minors));
		std::string::const_iterator s = minors.begin();
		std::string::const_iterator i = s;
		for(;;)
		{
			if(i != minors.end() && *i != ',')
			{
				++i;
				continue;
			}
			std::string minor(s, i);
			if(!m_pRaceCtrl->GetMinorRace(StrToCStr(minor)))
			{
				CString text;
				text.Format("Error: Minorrace not found: %s, ignoring.\n It needs to be that minor's home system name.", StrToCStr(minor));
				AfxMessageBox(text);
			}
			else
				result.push_back(minor);
			if(i == minors.end())
				break;
			++i;
			s = i;
		}
	}
	return result;
}

/// Funktion generiert die Galaxiemap inkl. der ganzen Systeme und Planeten zu Beginn eines neuen Spiels.
void CBotEDoc::GenerateGalaxy()
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
	AllocateSystems();

	///////////////////////////////////////////////////////////////////////
	// Galaxieform anpassen
	int nGenerationMode = 0; // 0 == Standard  sonst Pattern verwenden
	pIni->ReadValue("Special", "GENERATIONMODE", nGenerationMode);

	std::vector<std::vector<bool>> nGenField(STARMAP_SECTORS_HCOUNT, std::vector<bool>(STARMAP_SECTORS_VCOUNT, true));
	if (nGenerationMode != 0)
	{
		CString sAppPath = CIOData::GetInstance()->GetAppPath();
		CString filePath = "";
		filePath.Format("%sGraphics\\Galaxies\\shapes\\pattern%d.boj",sAppPath,nGenerationMode);
		std::auto_ptr<Bitmap> GalaxyPattern(Bitmap::FromFile(CComBSTR(filePath)));
		if (GalaxyPattern.get() == NULL)
		{
			sAppPath.Format("pattern%d.boj not found! using standard pattern", nGenerationMode);
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

	std::map<CString, CPoint> RaceKO;
	const int cease_at = max((STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT) / 4, 20);
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); )
	{
		// Zufällig einen Sektor ermitteln. Dies wird solange getan, solange der ermittelte Sektor noch nicht die
		// if Bedingung erfüllt oder die Abbruchbedingung erreicht ist.
		bool bRestart = false;
		int nCount = 0;
		CPoint co(-1, -1);
		do
		{
			co = CPoint(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);

			for (std::map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != it; ++jt)
			{
				const std::map<CString, CPoint>::const_iterator their_co = RaceKO.find(jt->first);
				AssertBotE(their_co != RaceKO.end() && their_co->second != CPoint(-1,-1) && co != CPoint(-1 -1));
				if (!nGenField[co.x][co.y] ||
					abs(co.x - their_co->second.x) < nMinDiff && abs(co.y - their_co->second.y) < nMinDiff)
				{
					co = CPoint(-1, -1);
					break;
				}
			}
			// Abbruchbedingung
			if (++nCount > cease_at)
				bRestart = true;
		}
		while (co == CPoint(-1,-1) && bRestart == false);

		RaceKO[it->first] = co;
		// nächsten Major auswählen
		++it;

		// Es konnte kein Sektor nach 250 Durchläufen gefunden werden, welcher der obigen if Bedingung genügt, so
		// wird die for Schleife nochmal neu gestartet.
		if (bRestart)
		{
			RaceKO.clear();
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
			AssertBotE(pMinor);
			continue;
		}

		// keine Minors ohne Heimatsystem ins Spiel bringen (Aliens haben kein Heimatsystem)
		if (!pMinor->IsAlien())
			vMinorRaceSystemNames.Add(pMinor->GetHomesystemName());
	}

	// Namensgenerator initialisieren
	CGenSectorName::GetInstance()->Init(vMinorRaceSystemNames, GetForcedMinors());

	int nStarDensity = 35;
	int nMinorDensity = 30;
	int nAnomalyDensity = 9;
	CIniLoader::GetInstance()->ReadValue("Special", "STARDENSITY", nStarDensity);
	CIniLoader::GetInstance()->ReadValue("Special", "MINORDENSITY", nMinorDensity);
	CIniLoader::GetInstance()->ReadValue("Special", "ANOMALYDENSITY", nAnomalyDensity);
	// Bei anderen Galaxieformen die Sternendichte verdoppeln, damit mehr Systeme generiert werden.
	// Sonst ist die Galaxie doch arg leer.
	if (nGenerationMode != 0)
		nStarDensity = min(nStarDensity * 2, 100);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: STARDENSITY: %i", nStarDensity);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: MINORDENSITY: %i", nMinorDensity);
	MYTRACE("general")(MT::LEVEL_INFO, "relevant only at new game: Bote.ini: ANOMALYDENSITY: %i", nAnomalyDensity);

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor const* const pMajor = it->second;
		const CPoint& raceKO = RaceKO[it->first];
		CSystem& system = GetSystem(raceKO.x, raceKO.y);

		system.SetHomeOf(it->first);
		system.SetName(pMajor->GetHomesystemName());
		it->second->SetRaceKO(raceKO);
		system.SetSunSystem(TRUE);
		system.SetFullKnown(it->first);
		system.SetColonyOwner(it->first);
		system.CreatePlanets(it->first);
		system.ChangeOwner(it->first, CSystem::OWNING_STATUS_COLONIZED_AFFILIATION_OR_HOME);
		system.SetResourceStore(RESOURCES::TITAN, 1000);
		system.SetResourceStore(RESOURCES::DERITIUM, 3);

		// Zwei Sonnensysteme in unmittelbarer Umgebung des Heimatsystems anlegen
		BYTE nextSunSystems = 0;
		while (nextSunSystems < 2)
		{
			// Punkt mit Koordinaten zwischen -1 und +1 generieren
			CPoint dist(rand()%3 - 1, rand()%3 - 1);
			CPoint pt(raceKO.x + dist.x, raceKO.y + dist.y);
			if (pt.x < STARMAP_SECTORS_HCOUNT && pt.x > -1 && pt.y < STARMAP_SECTORS_VCOUNT && pt.y > -1)
				if (!GetSystem(pt.x, pt.y).GetSunSystem())
				{
					GetSystem(pt.x, pt.y).GenerateSector(100, nMinorDensity);
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
					GetSystem(pt.x, pt.y).SetScanned(it->first);
			}
	}

	// nun die Sektoren generieren
	// diese nutzen die voreingestellten Patterns, so dass Galaxieformen möglich werden
	vector<CPoint> vSectorsToGenerate;
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if ((!GetSystem(x, y).GetSunSystem()) && nGenField[x][y]==true)
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
					if (GetSystem(pt.x, pt.y).GetSunSystem())
					{
						if (GetSystem(pt.x, pt.y).GetMinorRace())
							minorRaces++;
						sunSystems++;
					}
					else if (GetSystem(pt.x, pt.y).GetAnomaly())
					{
						nAnomalys++;
					}

					for (map<CString, CPoint >::const_iterator it = RaceKO.begin(); it != RaceKO.end(); ++it)
					{
						if (it->second.x == x + i && it->second.y == y + j)
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
			GetSystem(x, y).GenerateSector(sunSystemProb, minorRaceProb);

		// Wenn keine Minorrace in dem System generiert wurde
		if (!GetSystem(x, y).GetMinorRace())
		{
			// möglicherweise eine Anomalie im Sektor generieren
			if (!GetSystem(x, y).GetSunSystem())
			{
				if (rand()%100 >= (100 - (nAnomalyDensity - nAnomalys * 10)))
					GetSystem(x, y).CreateAnomaly();
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
			if (GetSystem(x, y).GetMinorRace())
			{
				// Nun die Minorrace parametrisieren
				const boost::shared_ptr<CMinor>& pMinor = boost::dynamic_pointer_cast<CMinor>(GetSystem(x, y).HomeOf());
				if (!pMinor)
				{
					AfxMessageBox("Error in function CBotEDoc::GenerateGalaxy(): Could not create Minorrace");
				}
				else
				{
					pMinor->SetRaceKO(CPoint(x,y));
					GetSystem(x, y).ChangeOwner(pMinor->GetRaceID(), CSystem::OWNING_STATUS_INDEPENDENT_MINOR);
					sUsedMinors.insert(pMinor->GetRaceID());
					// wenn die Minorrace Schiffe bauen kann, sie aber kein Deritium im System besitzt, so wird
					// ein Deritium auf dem ersten kolonisierten Planeten hinzugefügt
					if (pMinor->GetSpaceflightNation())
						GetSystem(x, y).CreateDeritiumForSpaceflightMinor();
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
			if (!it->second->IsAlien())
				vDelMinors.push_back(it->first);
		}
	}

	for (UINT i = 0; i < vDelMinors.size(); i++)
		m_pRaceCtrl->RemoveRace(vDelMinors[i]);
}

////////////////////////////////////////////////
//BEGINN: helper functions for NextRound()
static bool HumanPlayerInCombat(const CShipMap& ships, const CPoint& CurrentCombatSector) {

	for(CShipMap::const_iterator i = ships.begin(); i != ships.end(); ++i)
	{
		if (i->second->GetCo() != CurrentCombatSector)
			continue;
		const boost::shared_ptr<CMajor>& major = boost::dynamic_pointer_cast<CMajor>(i->second->Owner());
		if (major && major->IsHumanPlayer())
			return true;
	}
	return false;
}
//END: helper functions for NextRound()
////////////////////////////////////////////////
void CBotEDoc::NextRound()
{
#ifdef CONSISTENCY_CHECKS
	CSanity::GetInstance()->SanityCheckShipsInSectors(*this);
#endif

	// gibt es für diese Runde Sektoren in welchen ein Kampf stattfand
	bool bCombatInCurrentRound = !m_CombatSectors.empty();

	// Es fand noch kein Kampf die Runde statt. Dies tritt ein, wenn entweder wirklich kein Kampf diese Runde war
	// oder das erste Mal in diese Funktion gesprungen wurde.
	if (bCombatInCurrentRound == false)
	{
		MYTRACE("general")(MT::LEVEL_INFO, "############ START NEXT ROUND (round: %d) ############", GetCurrentRound());

		// Seed initialisieren
		RandomSeed();

		// Soundnachrichten aus alter Runde löschen
		m_pClientWorker->ClearSoundMessages();

		// ausgelöschte Rassen gleich zu Beginn der neuen Runde entfernen. Menschliche Spieler sollten jetzt schon disconnected sein!!!
		vector<CString> vDelMajors;
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->second->GetEmpire()->CountSystems() == 0)
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
			if (!HumanPlayerInCombat(m_ShipMap, m_ptCurrentCombatSector))
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
			// Ist ein menschlicher Spieler beteiligt?
			// kein menschlicher Spieler beteiligt -> gleich weiter
			if (!HumanPlayerInCombat(m_ShipMap, m_ptCurrentCombatSector))
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

		CPoint ko = pMinor->GetCo();

		if (ko != CPoint(-1,-1) && !GetSystem(ko.x, ko.y).Majorized() && GetSystem(ko.x, ko.y).OwnerID() == pMinor->GetRaceID())
		{
			// Vielleicht kolonisiert die Minorrace weitere Planeten in ihrem System
			if (pMinor->PerhapsExtend(this))
				// dann sind im System auch weitere Einwohner hinzugekommen
				GetSystem(ko.x, ko.y).SetHabitants(this->GetSystem(ko.x, ko.y).GetCurrentHabitants());

			// Den Verbrauch der Rohstoffe der kleinen Rassen in jeder Runde berechnen
			pMinor->ConsumeResources(this);
			// Vielleicht baut die Rasse ein Raumschiff
			pMinor->PerhapsBuildShip(this);
		}
	}

	// Zufallsereignisse berechnen
	CRandomEventCtrl* pRandomEventCtrl = CRandomEventCtrl::GetInstance();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		pRandomEventCtrl->CalcEvents(it->second);

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
	pRandomEventCtrl->CalcShipEvents();

	this->CalcEndDataForNextRound();

	// Creditänderung berechnen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		pMajor->GetEmpire()->SetCreditsChange((int)(pMajor->GetEmpire()->GetCredits() - mOldCredits[pMajor->GetRaceID()]));
	}

	// In dieser Runde stattgefundene Kämpfe löschen
	m_CombatSectors.clear();
	m_bCombatCalc = false;

	bool bAutoSave = false;
	CIniLoader::GetInstance()->ReadValue("General", "AUTOSAVE", bAutoSave);
	if (bAutoSave)
		DoSave(CIOData::GetInstance()->GetAutoSavePath(m_iRound), FALSE);
	SetModifiedFlag();

	MYTRACE("general")(MT::LEVEL_INFO, "\nNEXT ROUND calculation successfull\n", GetCurrentRound());
}

void CBotEDoc::ApplyShipsAtStartup()
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
					if (GetSystem(x, y).GetName() == s)
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
	AssertBotE(pIni);
	float frequency = 0;
	AssertBotE(pIni->ReadValue("Special", "ALIENENTITIES", frequency));
	if (frequency > 0.0f)
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

				BuildShip(m_ShipInfoArray.GetAt(i).GetID(), pEhlen->GetCo(), pEhlen->GetRaceID());
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
			if (!GetSystem(p.x, p.y).GetSunSystem())
				continue;

			if (!GetSystem(p.x, p.y).Free())
				continue;

			if (GetSystem(p.x, p.y).GetMinorRace())
				continue;

			if (GetSystem(p.x, p.y).GetAnomaly())
				continue;

			for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			{
				CShipInfo* pShipInfo = &m_ShipInfoArray.GetAt(i);
				if (pShipInfo->GetOnlyInSystem() != StrToCStr(KAMPFSTATION))
					continue;

				// Kampfstation platzieren und abbrechen
				BuildShip(m_ShipInfoArray.GetAt(i).GetID(), p, StrToCStr(KAMPFSTATION));
				break;
			}

			// Sektor war okay
			break;

		} while (true);
	}
}

void CBotEDoc::ApplyTroopsAtStartup()
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
					if (GetSystem(x, y).GetName() == s)//Wenn der Name gefunden wurde
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



void CBotEDoc::ApplyBuildingsAtStartup()
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
					if (GetSystem(x, y).GetName() == s)
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
	CStarmap::SynchronizeWithAnomalies(m_Systems);

	for(std::vector<CSystem>::iterator system = m_Systems.begin(); system != m_Systems.end(); ++system)
	{
		if (system->GetSunSystem() == TRUE)
		{
			system->SetHabitants(system->GetCurrentHabitants());
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				if (system->OwnerID() == it->first)
				{
					CMajor* pMajor = it->second;
					AssertBotE(pMajor);
					// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
					// baubare Gebäude, Schiffe und Truppen berechnen
					system->CalculateNumberOfWorkbuildings(&this->BuildingInfo);
					system->SetWorkersIntoBuildings();
					system->CalculateVariables();
					// alle produzierten FP und SP der Imperien berechnen und zuweisen
					int currentPoints;
					currentPoints = system->GetProduction()->GetResearchProd();
					pMajor->GetEmpire()->AddFP(currentPoints);
					currentPoints = system->GetProduction()->GetSecurityProd();
					pMajor->GetEmpire()->AddSP(currentPoints);
					// Schiffsunterstützungskosten eintragen
					float fCurrentHabitants = system->GetCurrentHabitants();
					pMajor->GetEmpire()->AddPopSupportCosts((UINT)fCurrentHabitants * POPSUPPORT_MULTI);
				}
			}
			for (int i = 0; i < system->GetAllBuildings()->GetSize(); i++)
			{
				USHORT nID = system->GetAllBuildings()->GetAt(i).GetRunningNumber();
				const CString& sRaceID = system->OwnerID();
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
		pMajor->GetEmpire()->GenerateSystemList(m_Systems);
		for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
		{
			CPoint p = pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko;
			GetSystem(p.x, p.y).CalculateBuildableBuildings(&m_GlobalBuildings);
			GetSystem(p.x, p.y).CalculateBuildableShips();
			GetSystem(p.x, p.y).CalculateBuildableTroops(&m_TroopInfo, pMajor->GetEmpire()->GetResearch());
		}
	}
}

void CBotEDoc::ReadTroopInfosFromFile()
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
				CTroopInfo troopInfo(CLoc::GetString(data[1]), CLoc::GetString(data[2]),data[3],atoi(data[4]),atoi(data[5]),atoi(data[6]),techs,res,atoi(data[18]),atoi(data[19]),data[0].GetString(),atoi(data[20]),atoi(data[21]));
				m_TroopInfo.Add(troopInfo);
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


void CBotEDoc::ReadBuildingInfosFromFile()
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
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_A,atoi(data[23]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_B,atoi(data[24]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_C,atoi(data[25]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_E,atoi(data[26]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_F,atoi(data[27]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_G,atoi(data[28]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_H,atoi(data[29]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_I,atoi(data[30]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_J,atoi(data[31]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_K,atoi(data[32]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_L,atoi(data[33]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_M,atoi(data[34]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_N,atoi(data[35]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_O,atoi(data[36]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_P,atoi(data[37]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_Q,atoi(data[38]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_R,atoi(data[39]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_S,atoi(data[40]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_T,atoi(data[41]));
				info.SetPlanetTypes(PLANET_CLASSES::PLANETCLASS_Y,atoi(data[42]));
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
				for (int p = PLAYER_RACES::MAJOR1; p <= PLAYER_RACES::MAJOR6; p++)
					info.SetEquivalent(p,atoi(data[126+p]));
				for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
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
void CBotEDoc::ReadShipInfosFromFile()
{
	CShipInfo ShipInfo;
	BOOL torpedo = FALSE;
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString data[40];
	CString torpedoData[9];
	CString beamData[12];
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Ships\\Shiplist.data";				// Name des zu Öffnenden Files
	CStdioFile file;														// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		int i = 0;
		USHORT j = 0;
		int weapons = 0;	// Zähler um Waffen hinzuzufügen
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
				if (ShipInfo.GetRace() == PLAYER_RACES::UNKNOWNRACE)
					ShipInfo.SetRace(PLAYER_RACES::MINORNUMBER);
				ShipInfo.SetID(j);
				ShipInfo.SetShipClass(data[1]);
				ShipInfo.SetDescription(data[2]);
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
void CBotEDoc::BuildBuilding(USHORT id, const CPoint& KO)
{
	CBuilding building(id);
	BOOLEAN isOnline = this->GetBuildingInfo(id).GetAllwaysOnline();
	building.SetIsBuildingOnline(isOnline);
	GetSystem(KO.x, KO.y).AddNewBuilding(building);
}

CShipMap::iterator CBotEDoc::BuildShip(int nID, const CPoint& KO, const CString& sOwnerID)
{
	CRace* pOwner = m_pRaceCtrl->GetRace(sOwnerID);
	if (!pOwner)
	{
		AfxMessageBox("Error in BuildShip(): RaceID " + sOwnerID + " doesn't exist!");
		return m_ShipMap.end();
	}

	AssertBotE(nID >= 10000);
	nID -= 10000;

	CShipInfo ship_info(m_ShipInfoArray.GetAt(nID));
	ship_info.SetCo(KO.x, KO.y);
	ship_info.SetOwner(pOwner->GetRaceID());
	// Schiffsnamen vergeben
	ship_info.SetName(m_GenShipName.GenerateShipName(pOwner->GetRaceID(), pOwner->GetName(), ship_info.IsStation()));
	const boost::shared_ptr<CShips> ships(new CShips(ship_info));
	const CShipMap::iterator it = m_ShipMap.Add(ships);


	// den Rest nur machen, wenn das Schiff durch eine Majorrace gebaut wurde
	if (!pOwner->IsMajor())
		return it;

	CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
	AssertBotE(pMajor);
	MYTRACE("general")(MT::LEVEL_DEBUG, "New Ship for Major %d\n", pMajor);

	// Spezialforschungsboni dem Schiff hinzufügen
	it->second->AddSpecialResearchBoni();

	pMajor->GetShipHistory()->AddShip(it->second->ShipHistoryInfo(), GetSystem(KO.x, KO.y).CoordsName(true), m_iRound);
	return it;
}

/// Funktion zum Löschen des Schiffes aus dem Schiffsarray.
/// @param ship Iterator des Schiffes im Array
/// iterator is updated to the new position of the element following the deleted one
void CBotEDoc::RemoveShip(CShipMap::iterator& ship)
{
	if (ship->second->HasFleet())
	{
		const boost::shared_ptr<CShips>& new_fleetship = ship->second->GiveFleetToFleetsFirstShip();
		m_ShipMap.Add(new_fleetship);
	}
	m_ShipMap.EraseAt(ship);
}

/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
void CBotEDoc::BuildTroop(BYTE ID, CPoint ko)
{
	// Mal Testweise paar Truppen anlegen
	GetSystem(ko.x, ko.y).AddTroop((CTroop*)&m_TroopInfo.GetAt(ID));
	const RacePtr& pRace = GetSystem(ko.x, ko.y).Owner();
	if (!pRace)
		return;

	int n = GetSystem(ko.x, ko.y).GetTroops()->GetUpperBound();

	// Spezialforschung #4: "Truppen"
	if (pRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% verbesserte Offensive
		if (pRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			BYTE power = GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).GetOffense();
			GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).SetOffense(
				power + (power * pRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(1) / 100));
		}
		// 500 Erfahrungspunkte dazu -> erste Stufe
		else if (pRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			GetSystem(ko.x, ko.y).GetTroops()->GetAt(n).AddExperiancePoints(pRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(2));
		}
	}

}

// Funktion generiert die Starmaps, so wie sie nach Rundenberechnung auch angezeigt werden können.
void CBotEDoc::GenerateStarmap(const CString& sOnlyForRaceID)
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
	CStarmap::SynchronizeWithAnomalies(m_Systems);

	// Starmaps generieren
	for(std::vector<CSystem>::iterator system = m_Systems.begin(); system != m_Systems.end(); ++system)
	{
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
				continue;

			CMajor* pMajor = it->second;
			// Wenn in dem System eine aktive Werft ist bzw. eine Station/Werft im Sektor ist
			if ((system->GetProduction()->GetShipYard() == TRUE && system->OwnerID() == pMajor->GetRaceID())
				|| system->GetShipPort(pMajor->GetRaceID()))
			{
				pMajor->GetStarmap()->AddBase(Sector(system->GetCo()),
					pMajor->GetEmpire()->GetResearch()->GetPropulsionTech());
			}

			if (system->GetSunSystem())
			{
				if (system->OwnerID() == it->first || system->Free())
				{
					CMajor* pMajor = it->second;
					pMajor->GetStarmap()->AddKnownSystem(Sector(system->GetCo()));
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
		it->second->GetStarmap()->SynchronizeWithMap(m_Systems, &NAPRaces);
	}

	// nun die Berechnung für den Außenpostenbau vornehmen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (sOnlyForRaceID != "" && sOnlyForRaceID != it->first)
			continue;

		CMajor* pMajor = it->second;
		if (!pMajor->AHumanPlays())
			pMajor->GetStarmap()->SetBadAIBaseSectors(m_Systems, it->first);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBotEDoc Diagnose

#ifdef _DEBUG
void CBotEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBotEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBotEDoc Befehle

/////////////////////////////////////////////////////////////////////////////
// private Funktionen
/////////////////////////////////////////////////////////////////////////////

/// Diese Funktion führt allgemeine Berechnung durch, die immer zu Beginn der NextRound-Calculation stattfinden
/// müssen. So werden z.B. alte Nachrichten gelöscht, die Statistiken berechnet usw..
void CBotEDoc::CalcPreDataForNextRound()
{
	m_iRound++;

	AssertBotE(GetPlayersRace());

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
		if (pMajor->DecrementAgreementsDuration(pmMajors))
			m_pClientWorker->SetToEmpireViewFor(*it->second);
		m_pClientWorker->SetMajorToHumanOrAi(*it->second);
	}

	// Schiffe, welche nur auf einem bestimmten System baubar sind, z.B. Schiffe von Minorraces, den Besitzer wieder
	// auf MINORNUMBER setzen. In der Funktion, welche in einem System die baubaren Schiffe berechnet, wird dieser
	// Wert dann auf die richtige Rasse gesetzt. Jeder der das System dann besitzt, kann dieses Schiff bauen
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		if (!m_ShipInfoArray.GetAt(i).GetOnlyInSystem().IsEmpty())
			m_ShipInfoArray.GetAt(i).SetRace(PLAYER_RACES::MINORNUMBER);


	for(std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
	{
		if (it->GetSunSystem())
		{
			it->SetBlockade(0);
			it->ClearDisabledProductions();
		}
	}

	//f(x):=min(731,max(14,trunc(743-x^3)))
	m_fStardate += (float)(min(731, max(14, 743-pow((float)m_Statistics.GetAverageTechLevel(),3.0f))));
}

/// Diese Funktion berechnet den kompletten Systemangriff.
void CBotEDoc::CalcSystemAttack()
{
	// Systemangriff durchführen
	// Set mit allen Minors, welche während eines Systemangriffs vernichtet wurden. Diese werden am Ende der
	// Berechnung aus der Liste entfernt
	set<CString> sKilledMinors;
	MYTRACE("general")(MT::LEVEL_INFO, "sKilledMinors in this turn %s\n", sKilledMinors);
	CArray<CPoint> fightInSystem;
	MYTRACE("general")(MT::LEVEL_INFO, "fightInSystem in this turn %s\n", fightInSystem);
	for (CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		if (y->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			bool bAttackSystem = true;
			// Checken dass in diesem System nicht schon ein Angriff durchgeführt wurde
			for (int x = 0; x < fightInSystem.GetSize(); x++)
				if (fightInSystem.GetAt(x) == y->second->GetCo())
				{
					bAttackSystem = false;
					break;
				}

			// nur wenn das Schiff und Schiffe in der Flotte ungetarnt sind
			if (!y->second->CanHaveOrder(SHIP_ORDER::ATTACK_SYSTEM, false))
			{
				y->second->UnsetCurrentOrder();
				bAttackSystem = false;
			}

			// wenn in dem System noch kein Angriff durchgeführt wurde kann angegriffen werden
			if (!bAttackSystem)
				continue;

			CPoint p = y->second->GetCo();
			// Besitzer des Systems (hier Sector wegen Minors) vor dem Systemangriff
			CString sDefender = GetSystem(p.x, p.y).OwnerID();
	MYTRACE("general")(MT::LEVEL_INFO, "Attack of System ???, Defender %s\n", sDefender);
			// Angreifer bzw. neuer Besitzer des Systems nach dem Angriff
			set<CString> attackers;
			for (CShipMap::const_iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
			{
				if (i->second->GetCo() == p && i->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
				{
					const CString& sOwner = i->second->OwnerID();
//					MYTRACE("general")(MT::LEVEL_INFO, "Owner after attack %s\n", p, sOwner);
					if (!sOwner.IsEmpty())
						attackers.insert(sOwner);
				}
			}

			CAttackSystem* pSystemAttack = new CAttackSystem();

			CRace* defender = NULL;
			if (!sDefender.IsEmpty())
				defender = m_pRaceCtrl->GetRace(sDefender);
			// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
			if (defender && defender->IsMinor() && !GetSystem(p.x, p.y).Taken())
			{
				pSystemAttack->Init(defender, &GetSystem(p.x, p.y), &m_ShipMap, &GetSystem(p.x, p.y));
			}
			// Wenn eine Majorrace in dem System lebt
			else if (defender && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(sDefender, &attackers))
			{
				pSystemAttack->Init(defender, &GetSystem(p.x, p.y), &m_ShipMap, &GetSystem(p.x, p.y));
			}
			// Wenn niemand mehr in dem System lebt, z.B. durch Rebellion
			else
			{
				pSystemAttack->Init(NULL, &GetSystem(p.x, p.y), &m_ShipMap, &GetSystem(p.x, p.y));
			}

			// keine Schiffe sind am Angriff beteiligt -> z.B. alle im Sektor sind am Rückzug
			if (!pSystemAttack->IsAttack())
			{
				delete pSystemAttack;
				pSystemAttack = NULL;
				fightInSystem.Add(p);
				continue;
			}

			// Ein Systemangriff verringert die Moral in allen Systemen, die von uns schon erobert wurden und zuvor
			// der Majorrace gehörten, deren System hier angegriffen wird
			if (!sDefender.IsEmpty())
				for (int j = 0 ; j < STARMAP_SECTORS_VCOUNT; j++)
					for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
						if (GetSystem(i, j).Taken() && GetSystem(i, j).GetColonyOwner() == sDefender
							&& pSystemAttack->IsDefenderNotAttacker(sDefender, &attackers))
							GetSystem(i, j).SetMoral(-rand()%5);

			CString attacker;
			// Wurde das System mit Truppen erobert, so wechselt es den Besitzer
			if (pSystemAttack->Calculate(attacker))
			{
				const boost::shared_ptr<CMajor> pMajor = m_pRaceCtrl->GetMajorSafe(attacker);
				AssertBotE(pMajor);

				// Wenn in dem System eine Minorrace beheimatet ist und das System nicht vorher schon von jemand
				// anderem militärisch erobert wurde oder die Minorace bei einem anderen Imperium schon vermitgliedelt
				// wurde, dann muss diese zuerst die Gebäude bauen
				if (GetSystem(p.x, p.y).GetMinorRace() && !GetSystem(p.x, p.y).Taken() && defender != NULL && defender->IsMinor())
				{
					CMinor* pMinor = dynamic_cast<CMinor*>(defender);
					AssertBotE(pMinor);
					pMinor->SetSubjugated(true);
					// Wenn das System noch keiner Majorrace gehört, dann Gebäude bauen
					GetSystem(p.x, p.y).BuildBuildingsForMinorRace(&BuildingInfo, m_Statistics.GetAverageTechLevel(), pMinor);
					// Sektor gilt ab jetzt als erobert.
					GetSystem(p.x, p.y).ChangeOwner(attacker, CSystem::OWNING_STATUS_TAKEN, false);
					pMinor->SetOwner(pMajor);
					// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
					pMinor->SetRelation(attacker, -100);
					// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
					GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
					// ist die Moral unter 50, so wird sie auf 50 gesetzt
					if (GetSystem(p.x, p.y).GetMoral() < 50)
						GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());
					CString param = GetSystem(p.x, p.y).GetName();
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
							CEmpireNews message;
							message.CreateNews(CLoc::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetName()), EMPIRE_NEWS_TYPE::MILITARY, param, p);
							it->second->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*it->second);
						}
					}
					// Eventnachricht an den Eroberer (System erobert)
					eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
					// Eventnachricht hinzufügen
					if (!eventText.IsEmpty())
					{
						CEmpireNews message;
						message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
						pMajor->GetEmpire()->AddMsg(message);
						m_pClientWorker->SetToEmpireViewFor(*pMajor);
					}
				}
				// Wenn das System einer Minorrace gehört, eingenommen wurde und somit befreit wird
				else if (GetSystem(p.x, p.y).GetMinorRace() && GetSystem(p.x, p.y).Taken() && defender != NULL && defender->IsMajor())
				{
					// Die Beziehung zur Majorrace, die das System vorher besaß verschlechtert sich
					defender->SetRelation(attacker, -rand()%50);
					// Die Beziehung zu der Minorrace verbessert sich auf Seiten des Retters
					const boost::shared_ptr<CMinor>& pMinor =
						boost::dynamic_pointer_cast<CMinor>(GetSystem(p.x, p.y).HomeOf());
					AssertBotE(pMinor);
					pMinor->SetRelation(attacker, rand()%50);
					pMinor->SetSubjugated(false);
					// Eventnachricht an den, der das System verloren hat (erobertes Minorracesystem wieder verloren)
					CString param = GetSystem(p.x, p.y).GetName();

					CMajor* def = dynamic_cast<CMajor*>(defender);
					CString eventText(def->GetMoralObserver()->AddEvent(17, def->GetRaceMoralNumber(), param));
					// Eventnachricht hinzufügen
					if (!eventText.IsEmpty())
					{
						CEmpireNews message;
						message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
						def->GetEmpire()->AddMsg(message);
						m_pClientWorker->SetToEmpireViewFor(*def);
					}
					// Eventnachricht an den Eroberer (Minorracesystem befreit)
					param = pMinor->GetName();
					eventText = pMajor->GetMoralObserver()->AddEvent(13, pMajor->GetRaceMoralNumber(), param);
					// Eventnachricht hinzufügen
					if (!eventText.IsEmpty())
					{
						CEmpireNews message;
						message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
						pMajor->GetEmpire()->AddMsg(message);
						m_pClientWorker->SetToEmpireViewFor(*pMajor);
					}
					// Sektor gilt ab jetzt als nicht mehr erobert.
					GetSystem(p.x, p.y).ChangeOwner(pMinor->GetRaceID(), CSystem::OWNING_STATUS_INDEPENDENT_MINOR);
					boost::shared_ptr<CMajor> empty;
					pMinor->SetOwner(empty);
					// Moral in dem System um rand()%50+25 erhöhen
					GetSystem(p.x, p.y).SetMoral(rand()%50+25);
				}
				// Eine andere Majorrace besaß das System
				else
				{
					CSystem::OWNING_STATUS new_owning_status = CSystem::OWNING_STATUS_TAKEN;
					// Beziehung zum ehemaligen Besitzer verschlechtert sich
					if (defender != NULL && defender->GetRaceID() != attacker)
						defender->SetRelation(attacker, -rand()%50);
					// Wenn das System zurückerobert wird, dann gilt es als befreit
					if (GetSystem(p.x, p.y).GetColonyOwner() == attacker)
					{
						new_owning_status = CSystem::OWNING_STATUS_COLONIZED_AFFILIATION_OR_HOME;
						CString param = GetSystem(p.x, p.y).GetName();
						// Eventnachricht an den Eroberer (unser ehemaliges System wieder zurückerobert)
						CString eventText(pMajor->GetMoralObserver()->AddEvent(14, pMajor->GetRaceMoralNumber(), param));
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pMajor);
						}
						if (defender != NULL && defender->GetRaceID() != attacker && defender->IsMajor())
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							// Eventnachricht an den, der das System verloren hat (unser erobertes System verloren)
							eventText = pDefenderMajor->GetMoralObserver()->AddEvent(17, pDefenderMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CEmpireNews message;
								message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
								pDefenderMajor->GetEmpire()->AddMsg(message);
								m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
							}
						}
						// Moral in dem System um rand()%25+10 erhöhen
						GetSystem(p.x, p.y).SetMoral(rand()%25+10);
					}
					// Handelte es sich dabei um das Heimatsystem einer Rasse
					else if (defender != NULL && defender->IsMajor() && GetSystem(p.x, p.y).OwnerID() == defender->GetRaceID() && GetSystem(p.x, p.y).GetName() == dynamic_cast<CMajor*>(defender)->GetHomesystemName())
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
						// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
						CString param = GetSystem(p.x, p.y).GetName();
						CString eventText(pDefenderMajor->GetMoralObserver()->AddEvent(15, pDefenderMajor->GetRaceMoralNumber(), param));
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
							pDefenderMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
						}
						// Eventnachricht an den Eroberer (System erobert)
						eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pMajor);
						}
						// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
						GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
						// ist die Moral unter 50, so wird sie auf 50 gesetzt
						if (GetSystem(p.x, p.y).GetMoral() < 50)
							GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());
					}
					// wurde das System erobert und obere Bedingungen traten nicht ein
					else
					{
						CString param = GetSystem(p.x, p.y).GetName();
						// Hat eine andere Majorrace die Minorrace vermitgliedelt, so unterwerfen wir auch diese Minorrace
						if (GetSystem(p.x, p.y).GetMinorRace())
						{
							const boost::shared_ptr<CMinor>& pMinor =
								boost::dynamic_pointer_cast<CMinor>(GetSystem(p.x, p.y).HomeOf());
							AssertBotE(pMinor);
							pMinor->SetSubjugated(true);
							pMinor->SetOwner(pMajor);
							// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
							pMinor->SetRelation(attacker, -100);

							CString param = GetSystem(p.x, p.y).GetName();

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
									CEmpireNews message;
									message.CreateNews(CLoc::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetName()), EMPIRE_NEWS_TYPE::MILITARY, param, p);
									it->second->GetEmpire()->AddMsg(message);
									m_pClientWorker->SetToEmpireViewFor(*it->second);
								}
							}
						}

						// Nur wenn es einen Verteidiger gab
						if (defender)
						{
							// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
							GetSystem(p.x, p.y).SetMoral(-rand()%25-10);
						}
						else
						{
							// Der Fall kein eintreten, wenn ein rebelliertes System oder ein System
							// eines ausgelöschten Majors erobert wurde!

							// Sektor gilt nicht als erobert
							new_owning_status = CSystem::OWNING_STATUS_COLONIZED_AFFILIATION_OR_HOME;
							// Der Angreifer gilt nun als Koloniebesitzer
							GetSystem(p.x, p.y).SetColonyOwner(attacker);
						}

						// ist die Moral unter 50, so wird sie auf 50 gesetzt
						if (GetSystem(p.x, p.y).GetMoral() < 50)
							GetSystem(p.x, p.y).SetMoral(50 - GetSystem(p.x, p.y).GetMoral());

						// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
						// Achtung: Defender kann auch NULL sein, wenn z.B. ein rebelliertes System erobert wurde
						// oder ein System eines vernichteten Majors!
						if (defender != NULL && defender->GetRaceID() != attacker && defender->IsMajor())
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							AssertBotE(pDefenderMajor);
							CString eventText = pDefenderMajor->GetMoralObserver()->AddEvent(16, pDefenderMajor->GetRaceMoralNumber(), param);

							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CEmpireNews message;
								message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
								CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
								pDefenderMajor->GetEmpire()->AddMsg(message);
								m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
							}
						}

						// Eventnachricht an den Eroberer (System erobert)
						CString eventText = pMajor->GetMoralObserver()->AddEvent(11, pMajor->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pMajor);
						}
					}
					GetSystem(p.x, p.y).ChangeOwner(attacker, new_owning_status, false);
				}
				// Gebäude die nach Eroberung automatisch zerstört werden
				GetSystem(p.x, p.y).RemoveSpecialRaceBuildings(&this->BuildingInfo);
				// Variablen berechnen und Gebäude besetzen
				GetSystem(p.x, p.y).CalculateNumberOfWorkbuildings(&this->BuildingInfo);
				GetSystem(p.x, p.y).FreeAllWorkers();
				GetSystem(p.x, p.y).SetWorkersIntoBuildings();

				// war der Verteidiger eine Majorrace und wurde sie durch die Eroberung komplett ausgelöscht,
				// so bekommt der Eroberer einen kräftigen Moralschub
				if (defender != NULL && defender->IsMajor() && !attacker.IsEmpty() && pMajor && pSystemAttack->IsDefenderNotAttacker(sDefender, &attackers))
				{
					CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
					// Anzahl der noch verbleibenden Systeme berechnen
					pDefenderMajor->GetEmpire()->GenerateSystemList(m_Systems);
					// hat der Verteidiger keine Systeme mehr, so bekommt der neue Besitzer den Bonus
					if (pDefenderMajor->GetEmpire()->GetSystemList()->GetSize() == 0)
					{
						CString param = pDefenderMajor->GetName();
						CString eventText = pMajor->GetMoralObserver()->AddEvent(0, pMajor->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pMajor);
						}
					}
				}

				// erfolgreiches Invasionsevent für den Angreifer einfügen (sollte immer ein Major sein)
				if (!attacker.IsEmpty() && pMajor && pMajor->IsHumanPlayer())
					pMajor->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(attacker, "InvasionSuccess", CLoc::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("INVASIONSUCCESSEVENT_TEXT_" + pMajor->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));

				// Invasionsevent für den Verteidiger einfügen
				if (defender != NULL && defender->IsMajor() && dynamic_cast<CMajor*>(defender)->IsHumanPlayer() && pSystemAttack->IsDefenderNotAttacker(sDefender, &attackers))
					dynamic_cast<CMajor*>(defender)->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(sDefender, "InvasionSuccess", CLoc::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("INVASIONSUCCESSEVENT_TEXT_" + defender->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));
			}
			// Wurde nur bombardiert, nicht erobert
			else
			{
				for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
					if (!pMajor)
						continue;

					if(defender)
					{
						// Erstmal die Beziehung zu der Rasse verschlechtern, der das System gehört
						if (defender->GetRaceID() != pMajor->GetRaceID())
							defender->SetRelation(pMajor->GetRaceID(), -rand()%10);

						CSystem& system = GetSystem(p.x, p.y);
						if(!defender->IsMinor() && system.GetMinorRace())
							system.HomeOf()->SetRelation(pMajor->GetRaceID(), -rand()%5);
					}
				}

				// Wenn die Bevölkerung des Systems auf NULL geschrumpft ist, dann ist dieses System verloren
				if (GetSystem(p.x, p.y).GetHabitants() <= 0.000001f)
				{
					// Bei einer Minorrace wird es komplizierter. Wenn diese keine Systeme mehr hat, dann ist diese
					// aus dem Spiel verschwunden. Alle Einträge in der Diplomatie müssen daher gelöscht werden
					if (GetSystem(p.x, p.y).GetMinorRace())
					{
						const boost::shared_ptr<CMinor>& pMinor =
							boost::dynamic_pointer_cast<CMinor>(GetSystem(p.x, p.y).HomeOf());
						// Alle Effekte, Events usw. wegen der Auslöschung der Minorrace verarbeiten
						CalcEffectsMinorEleminated(pMinor);

						// Eventnachricht: #21	Eliminate a Minor Race Entirely
						for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
						{
							CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
							if (!pMajor)
								continue;

							CString param = pMinor->GetName();
							CString eventText = pMajor->GetMoralObserver()->AddEvent(21, pMajor->GetRaceMoralNumber(), param);
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
							pMajor->GetEmpire()->AddMsg(message);
						}

						// Rasse zum Löschen vormerken
						sKilledMinors.insert(pMinor->GetRaceID());
					}
					// Bei einer Majorrace verringert sich nur die Anzahl der Systeme (auch konnte dies das
					// Minorracesystem von oben gewesen sein, hier verliert es aber die betroffene Majorrace)
					if (defender != NULL && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);

						CString eventText = "";
						CString param = GetSystem(p.x, p.y).GetName();
						if (GetSystem(p.x, p.y).GetName() == pDefenderMajor->GetHomesystemName())
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
							CEmpireNews message;
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
							pDefenderMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
						}
					}

					GetSystem(p.x, p.y).ChangeOwner("", CSystem::OWNING_STATUS_EMPTY);
					GetSystem(p.x, p.y).SetColonyOwner("");

					// war der Verteidiger eine Majorrace und wurde sie durch den Verlust des Systems komplett ausgelöscht,
					// so bekommt der Eroberer einen kräftigen Moralschub
					if (defender != NULL && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
						for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
						{
							CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
							if (!pMajor)
								continue;

							// Anzahl der noch verbleibenden Systeme berechnen
							pDefenderMajor->GetEmpire()->GenerateSystemList(m_Systems);
							// hat der Verteidiger keine Systeme mehr, so bekommt der neue Besitzer den Bonus
							if (pDefenderMajor->GetEmpire()->GetSystemList()->IsEmpty())
							{
								CString sParam		= pDefenderMajor->GetName();
								CString sEventText	= pMajor->GetMoralObserver()->AddEvent(0, pMajor->GetRaceMoralNumber(), sParam);
								// Eventnachricht hinzufügen
								if (!sEventText.IsEmpty())
								{
									CEmpireNews message;
									message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::MILITARY, sParam, p, 0, 1);
									pMajor->GetEmpire()->AddMsg(message);
									m_pClientWorker->SetToEmpireViewFor(*pMajor);
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
					if (pSystemAttack->GetDestroyedBuildings() > 0 || pSystemAttack->GetKilledPop() >= GetSystem(p.x, p.y).GetHabitants() * 0.03)
					{
						CString param = GetSystem(p.x, p.y).GetName();
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
							else if (GetSystem(p.x, p.y).GetColonyOwner() == pMajor->GetRaceID() && defender == NULL)
								eventText = pMajor->GetMoralObserver()->AddEvent(20, pMajor->GetRaceMoralNumber(), param);
							// Eventnachricht für Agressor hinzufügen
							if (!eventText.IsEmpty())
							{
								CEmpireNews message;
								message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
								pMajor->GetEmpire()->AddMsg(message);
								m_pClientWorker->SetToEmpireViewFor(*pMajor);
							}
						}

						// Eventnachricht über Bombardierung für Verteidiger erstellen und hinzufügen
						if (defender != NULL && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							CString eventText = pDefenderMajor->GetMoralObserver()->AddEvent(22, pDefenderMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								CEmpireNews message;
								message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, param, p);
								pDefenderMajor->GetEmpire()->AddMsg(message);
								m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
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
						if (!pSystemAttack->IsTroopsInvolved())
							pMajor->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(pMajor->GetRaceID(), "Bombardment", CLoc::GetString("BOMBARDEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("BOMBARDEVENT_TEXT_AGRESSOR_" + pMajor->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));
						// gescheitere Invasion
						else if (GetSystem(p.x, p.y).GetHabitants() > 0.000001f)
							pMajor->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(pMajor->GetRaceID(), "InvasionFailed", CLoc::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("INVASIONFAILUREEVENT_TEXT_" + pMajor->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));
					}

				}
				// für den Verteidiger
				if (defender != NULL && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
				{
					CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
					if (pDefenderMajor->IsHumanPlayer())
					{
						// reine Bombardierung
						if (!pSystemAttack->IsTroopsInvolved())
							pDefenderMajor->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(defender->GetRaceID(), "Bombardment", CLoc::GetString("BOMBARDEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("BOMBARDEVENT_TEXT_DEFENDER_" + defender->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));
						// gescheitere Invasion
						else if (GetSystem(p.x, p.y).GetHabitants() > 0.000001f)
							pDefenderMajor->GetEmpire()->PushEvent(boost::make_shared<CEventBombardment>(defender->GetRaceID(), "InvasionFailed", CLoc::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, GetSystem(p.x, p.y).GetName()), CLoc::GetString("INVASIONFAILUREEVENT_TEXT_" + defender->GetRaceID(), FALSE, GetSystem(p.x, p.y).GetName())));
					}
				}
			}

			// Nachrichten hinzufügen
			for (int i = 0; i < pSystemAttack->GetNews()->GetSize(); )
			{
				for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
					if (!pMajor)
						continue;

					CEmpireNews message;
					message.CreateNews(pSystemAttack->GetNews()->GetAt(i), EMPIRE_NEWS_TYPE::MILITARY, GetSystem(p.x, p.y).GetName(), p);
					pMajor->GetEmpire()->AddMsg(message);
					m_pClientWorker->SetToEmpireViewFor(*pMajor);
				}
				if (defender != NULL && defender->IsMajor() && pSystemAttack->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
				{
					CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
					CEmpireNews message;
					message.CreateNews(pSystemAttack->GetNews()->GetAt(i), EMPIRE_NEWS_TYPE::MILITARY, GetSystem(p.x, p.y).GetName(), p);
					pDefenderMajor->GetEmpire()->AddMsg(message);
					m_pClientWorker->SetToEmpireViewFor(*pDefenderMajor);
				}
				pSystemAttack->GetNews()->RemoveAt(i);
			}

			delete pSystemAttack;
			pSystemAttack = NULL;
			fightInSystem.Add(p);
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
			if(i->second->RemoveDestroyed(*i->second->Owner(), m_iRound, CLoc::GetString("SYSTEMATTACK"),
					CLoc::GetString("DESTROYED"))) {
				++i;
				continue;
			}
			RemoveShip(i);
		}
}

/// Diese Funktion berechnet alles im Zusammenhang mit dem Geheimdienst.
void CBotEDoc::CalcIntelligence()
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

				CEmpireNews message;
				message.CreateNews(CLoc::GetString("WE_HAVE_NEW_INTELREPORTS"), EMPIRE_NEWS_TYPE::SECURITY, 4);
				it->second->GetEmpire()->AddMsg(message);

				m_pClientWorker->AddSoundMessage(SNDMGR_MSG_INTELNEWS, *it->second, 0);

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
							message.CreateNews(eventText, EMPIRE_NEWS_TYPE::SECURITY, 4);
							it->second->GetEmpire()->AddMsg(message);
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
				m_pClientWorker->SetToEmpireViewFor(*it->second);
			}
}

/// Diese Funktion berechnet die Forschung eines Imperiums
void CBotEDoc::CalcResearch()
{
	// Forschungsboni, die die Systeme machen holen. Wir benötigen diese dann für die CalculateResearch Funktion
	std::map<CString, CSystemProd::RESEARCHBONI> researchBoni;
	for(std::vector<CSystem>::const_iterator system = m_Systems.begin(); system != m_Systems.end(); ++system) {
		if(!system->Majorized())
			continue;
		const CSystemProd* prod = system->GetProduction();
		researchBoni[system->OwnerID()] += prod->GetResearchBoni();
	}

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;

		pMajor->GetEmpire()->GetResearch()->SetResearchBoni(researchBoni[it->first].nBoni);
		const CString* news = pMajor->GetEmpire()->GetResearch()->CalculateResearch(pMajor->GetEmpire()->GetFP());

		for (int j = 0; j < 8; j++)		// aktuell 8 verschiedene Nachrichten mgl, siehe CResearch Klasse
		{
			// Wenn irgendwas erforscht wurde, wir also eine Nachricht erstellen wollen
			if (news[j] != "")
			{
				CEmpireNews message;

				// flag setzen, wenn wir eine Spezialforschung erforschen dürfen
				if (j == 7)
				{
					// Spezialforschung kann erforscht werden
					m_pClientWorker->AddSoundMessage(SNDMGR_MSG_SCIENTISTNEWS, *pMajor, 0);
					m_pClientWorker->SetToEmpireViewFor(*pMajor);
					message.CreateNews(news[j], EMPIRE_NEWS_TYPE::RESEARCH, 1);
				}
				else
				{
					if (pMajor->IsHumanPlayer())
					{
						m_pClientWorker->AddSoundMessage(SNDMGR_MSG_NEWTECHNOLOGY, *pMajor, 0);
						m_pClientWorker->SetToEmpireViewFor(*pMajor);

						// Eventscreen für Forschung erstellen
						// gilt nur für die sechs normalen Forschungen
						if (j < 6)
							pMajor->GetEmpire()->PushEvent(boost::make_shared<CEventResearch>(pMajor->GetRaceID(), CLoc::GetString("RESEARCHEVENT_HEADLINE"), j));
					}
					message.CreateNews(news[j], EMPIRE_NEWS_TYPE::RESEARCH);
				}

				pMajor->GetEmpire()->AddMsg(message);
			}
		}
	}

	// künstliche Intelligenz für Forschung
	CResearchAI AI;
	AI.Calc(this);
}

/// Diese Funktion berechnet die Auswirkungen von diplomatischen Angeboten und ob Minorraces Angebote an
/// Majorraces abgeben.
void CBotEDoc::CalcDiplomacy()
{
	// zuerst alle Angebote senden
	CDiplomacyController::Send();

	// danach empfangen und reagieren
	CDiplomacyController::Receive();

	// Hinweis das Nachrichten eingegangen sind erstellen
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor->GetIncomingDiplomacyNews()->size() > 0)
		{
			m_pClientWorker->AddSoundMessage(SNDMGR_MSG_DIPLOMATICNEWS, *pMajor, 0);
			m_pClientWorker->SetToEmpireViewFor(*pMajor);
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
void CBotEDoc::UpdateGlobalBuildings(const CSystem& system) {
	// Jedes Sonnensystem wird durchgegangen und alle Gebäude des Systems werden in die Variable
	// m_GlobalBuildings geschrieben. Damit wissen welche Gebäude in der Galaxie stehen. Benötigt wird
	// dies z.B. um zu Überprüfen, ob max. X Gebäude einer bestimmten ID in einem Imperium stehen.
	for (int i = 0; i < system.GetAllBuildings()->GetSize(); i++)
	{
		USHORT nID = system.GetAllBuildings()->GetAt(i).GetRunningNumber();
		if(!system.Majorized())
			continue;
		const CString& sRaceID = system.OwnerID();
		if (GetBuildingInfo(nID).GetMaxInEmpire() > 0)
			m_GlobalBuildings.AddGlobalBuilding(sRaceID, nID);
	}
	// Alle Gebäude und Updates, die sich aktuell auch in der Bauliste befinden, werden dem Feld hinzugefügt
	for (int i = 0; i < ALE; i++)
		if (system.GetAssemblyList()->GetAssemblyListEntry(i) > 0 && system.GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
		{
			USHORT nID = abs(system.GetAssemblyList()->GetAssemblyListEntry(i));
			if(!system.Majorized())
				continue;
			const CString& sRaceID = system.OwnerID();
			if (GetBuildingInfo(nID).GetMaxInEmpire() > 0)
				m_GlobalBuildings.AddGlobalBuilding(sRaceID, nID);
		}
}
//END: helper functions for CalcOldRoundData()
////////////////////////////////////////////////
/// Diese Funktion berechnet das Planetenwachstum, die Aufträge in der Bauliste und sonstige Einstellungen aus der
/// alten Runde.
void CBotEDoc::CalcOldRoundData()
{
	COldRoundDataCalculator calc(this);
	m_GlobalBuildings.Reset();
	ClearAllPoints(m_pRaceCtrl->GetMajors());

	for (std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
	{
		// Mögliche Is? Variablen für Terraforming und Stationbau erstmal auf FALSE setzen
		it->ClearAllPoints(true);

		// Wenn im Sektor ein Sonnensystem existiert
		if (!it->GetSunSystem())
			continue;

		if (!it->Majorized())
		{
			// Planetenwachstum in jedem Sektor durchführen
			it->LetPlanetsGrowth();
			continue;
		}

		CMajor* pMajor = dynamic_cast<CMajor*>(it->Owner().get());
		AssertBotE(pMajor);

		// Jetzt das produzierte Credits im System dem jeweiligen Imperium geben, Gebäude abreisen, Moral im System berechnen
		COldRoundDataCalculator::CreditsDestructionMoral(pMajor, *it, this->BuildingInfo, m_fDifficultyLevel);

		// KI Anpassungen (KI bekommt zufälig etwas Deritium geschenkt)
		int diliAdd = COldRoundDataCalculator::DeritiumForTheAI(pMajor->AHumanPlays(), *it, m_fDifficultyLevel);

		// Das Lager berechnen
		const BOOLEAN bIsRebellion = it->CalculateStorages(pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), diliAdd);
		// Wenn true zurückkommt, dann hat sich das System losgesagt
		if (bIsRebellion)
			calc.ExecuteRebellion(*it, pMajor);

		// Hier mit einbeziehen, wenn die Bevölkerung an Nahrungsmangel stirbt
		if (it->GetFoodStore() < 0)
		{
			calc.ExecuteFamine(*it, pMajor);
		}
		else
		{
			// Planetenwachstum für Hauptrassen durchführen
			it->LetPlanetsGrowth();
		}

		// Wenn es keine Rebellion gab, dann Bau und KI im System berechnen
		if (!bIsRebellion)
		{
			AssertBotE(!it->Free());
			calc.HandlePopulationEffects(*it, pMajor);
			it->CalculateVariables();

			// hier könnte die Energie durch Weltraummonster weggenommen werden!
			// Gebäude die Energie benötigen checken
			if (it->CheckEnergyBuildings())
				calc.SystemMessage(*it, pMajor, "BUILDING_TURN_OFF", EMPIRE_NEWS_TYPE::SOMETHING, 2);

			// Die Bauaufträge in dem System berechnen. Außerdem wird hier auch die System-KI ausgeführt.
			if (!pMajor->AHumanPlays() || it->GetAutoBuild())
			{
				CSystemAI SAI(this);
				SAI.ExecuteSystemAI(it->GetCo());
			}

			calc.Build(*it, pMajor, this->BuildingInfo);
			// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
			it->CalculateNumberOfWorkbuildings(&this->BuildingInfo);
			// freie Arbeiter den Gebäuden zuweisen
			it->SetWorkersIntoBuildings();
		}

		// Globale Gebäude (X mal pro Imperium baubar) hinzufügen
		UpdateGlobalBuildings(*it);
	}
}

/// Diese Funktion berechnet die Produktion der Systeme, was in den Baulisten gebaut werden soll und sonstige
/// Daten für die neue Runde.
void CBotEDoc::CalcNewRoundData()
{
	CNewRoundDataCalculator new_round_data_calc(this);

	new_round_data_calc.CalcPreLoop();

	const map<CString, CMajor*>* const pmMajors = m_pRaceCtrl->GetMajors();

	// Hier werden jetzt die baubaren Gebäude für die nächste Runde und auch die Produktion in den einzelnen
	// Systemen berechnet. Können das nicht in obiger Hauptschleife machen, weil dort es alle globalen Gebäude
	// gesammelt werden müssen und ich deswegen alle Systeme mit den fertigen Bauaufträgen in dieser Runde einmal
	// durchgegangen sein muß.
	for(std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it) {
#ifdef CONSISTENCY_CHECKS
		CSanity::GetInstance()->SanityCheckSectorAndSystem(*it);
#endif
		const CString& system_owner = it->OwnerID();
		if (it->Majorized())
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(system_owner));
			AssertBotE(pMajor);
			CEmpire* empire = pMajor->GetEmpire();

			// Hier die Credits durch Handelsrouten berechnen und
			// Ressourcenrouten checken
			new_round_data_calc.CheckRoutes(*it, pMajor);

			it->CalculateVariables();

			const CSystemProd* const production = it->GetProduction();
			// Haben wir eine online Schiffswerft im System, dann ShipPort in dem Sektor setzen
			if (production->GetShipYard())
				it->SetShipPort(TRUE, system_owner);
			CNewRoundDataCalculator::CalcMoral(*it, m_TroopInfo);

			// Hier die gesamten Sicherheitsboni der Imperien berechnen
			CNewRoundDataCalculator::CalcIntelligenceBoni(production, empire->GetIntelligence());

			// Anzahl aller Ressourcen in allen eigenen Systemen berechnen
			for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
				empire->SetStorageAdd(res, it->GetResourceStore(res));
		}//if (sector.GetSunSystem() && system_owner != "")

		// für jede Rasse Sektorsachen berechnen
		// Hier wird berechnet, was wir von der Karte alles sehen, welche Sektoren wir durchfliegen können
		// alles abhängig von unseren diplomatischen Beziehungen
		// Nun auch überprüfen, ob sich unsere Grenzen erweitern, wenn die MinorRasse eine Spaceflight-Rasse ist und wir mit
		// ihr eine Kooperations oder ein Bündnis haben
		CNewRoundDataCalculator::CalcExtraVisibilityAndRangeDueToDiplomacy(*it, pmMajors, m_pRaceCtrl->GetMinors());
	}//for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector) {

	// Forschungsboni aus Spezialforschungen setzen, nachdem wir diese aus allen Systemen geholt haben
	//GetResearchBoniFromSpecialTechsAndSetThem(researchBonis, pmMajors);
	// Geheimdienstboni aus Spezialforschungen holen
	CNewRoundDataCalculator::GetIntelligenceBoniFromSpecialTechsAndSetThem(pmMajors);
}

/// Diese Funktion berechnet die kompletten Handelsaktivitäten.
void CBotEDoc::CalcTrade()
{
	// Hier berechnen wir alle Handelsaktionen
	USHORT taxMoney[] = {0,0,0,0,0};	// alle Steuern auf eine Ressource

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	if (pmMajors->empty())
		AfxMessageBox("Error in CBotEDoc::CalcTrade(): Could not get any major from race controller!");

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		AssertBotE(pMajor);

		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			float newTax = (float)pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(1);
			newTax = 1.0f + newTax / 100;
			pMajor->GetTrade()->SetTax(newTax);
		}
		pMajor->GetTrade()->CalculateTradeActions(pMajor, m_Systems, taxMoney);
		for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
		{
			// plus Steuern, die durch Sofortkäufe von Bauaufträgen entstanden sind holen
			if (CTrade::GetMonopolOwner(j).IsEmpty() == false)
				if (CTrade::GetMonopolOwner(j) == pMajor->GetRaceID() || pMajor->IsRaceContacted(CTrade::GetMonopolOwner(j)) == true)
					taxMoney[j] += pMajor->GetTrade()->GetTaxesFromBuying()[j];
		}
	}
	// die Steuern durch den Handel den Monopolbesitzern gutschreiben und nach Monopolkäufen Ausschau halten
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
	{
		CString resName;
		switch(i)
		{
		case RESOURCES::TITAN:		resName = CLoc::GetString("TITAN");		break;
		case RESOURCES::DEUTERIUM: resName = CLoc::GetString("DEUTERIUM");	break;
		case RESOURCES::DURANIUM:	resName = CLoc::GetString("DURANIUM");	break;
		case RESOURCES::CRYSTAL:	resName = CLoc::GetString("CRYSTAL");	break;
		case RESOURCES::IRIDIUM:	resName = CLoc::GetString("IRIDIUM");	break;
		}

		if (CTrade::GetMonopolOwner(i).IsEmpty() == false)
		{
			//CString hh;
			//hh.Format("Steuern auf %d: %d Credits",i,taxMoney[i]);
			//AfxMessageBox(hh);
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(CTrade::GetMonopolOwner(i)));
			AssertBotE(pMajor);
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
			AssertBotE(pMajor);

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
			AssertBotE(pMajor);

			CString sNews = "";
			// Die anderen Rassen bekommen ihr Geld zurück
			if (pMajor->GetRaceID() != sMonopolRace && pMajor->GetTrade()->GetMonopolBuying()[i] != 0)
			{
				pMajor->GetEmpire()->SetCredits((long)pMajor->GetTrade()->GetMonopolBuying()[i]);
				// Nachricht generieren, dass wir es nicht geschafft haben ein Monopol zu kaufen
				sNews = CLoc::GetString("WE_DONT_GET_MONOPOLY",FALSE,resName);
			}
			// Nachricht an unser Imperium, dass wir ein Monopol erlangt haben
			else if (pMajor->GetRaceID() == sMonopolRace)
				sNews = CLoc::GetString("WE_GET_MONOPOLY",FALSE,resName);

			if (!sNews.IsEmpty())
			{
				CEmpireNews message;
				message.CreateNews(sNews,EMPIRE_NEWS_TYPE::SOMETHING);
				pMajor->GetEmpire()->AddMsg(message);
				m_pClientWorker->SetToEmpireViewFor(*pMajor);
				pMajor->GetTrade()->SetMonopolBuying(i,0.0f);
			}


			// Nachrichten an die einzelnen Imperien verschicken, das eine Rasse das Monopol erlangt hat
			if (sMonopolRace.IsEmpty() == false && sMonopolRace != pMajor->GetRaceID())
			{
				CMajor* pMonopolRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sMonopolRace));
				AssertBotE(pMonopolRace);

				CString sRace = CLoc::GetString("UNKNOWN");
				if (pMajor->IsRaceContacted(sMonopolRace))
					sRace = pMonopolRace->GetRaceNameWithArticle();

				CString news = CLoc::GetString("SOMEBODY_GET_MONOPOLY",TRUE,sRace,resName);
				CEmpireNews message;
				message.CreateNews(news,EMPIRE_NEWS_TYPE::SOMETHING);
				pMajor->GetEmpire()->AddMsg(message);
				m_pClientWorker->SetToEmpireViewFor(*pMajor);
			}
		}
	}
	// Hier den neuen Kurs der Waren an den Handelsbörsen berechnen, dürfen wir erst machen, wenn wir für alle Tradeobjekte
	// die einzelnen Kurse berechnet haben
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		AssertBotE(pMajor);

		pMajor->GetTrade()->CalculatePrices(pmMajors, pMajor);
		// Hier die aktuellen Kursdaten in die History schreiben

		USHORT* resPrices = pMajor->GetTrade()->GetRessourcePrice();
		pMajor->GetTrade()->GetTradeHistory()->SaveCurrentPrices(resPrices, pMajor->GetTrade()->GetTax());
	}
}

/////BEGIN: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipOrders()

bool CBotEDoc::BuildStation(CShips& ship, SHIP_ORDER::Typ order, CSystem& system) {
	CMajor* pMajor = dynamic_cast<CMajor*>(ship.Owner().get());
	AssertBotE(pMajor);
	const CString& owner = ship.OwnerID();

	const SHIP_TYPE::Typ type = (order == SHIP_ORDER::BUILD_STARBASE || order == SHIP_ORDER::UPGRADE_STARBASE)
		? SHIP_TYPE::STARBASE : SHIP_TYPE::OUTPOST;
	const short id = pMajor->BestBuildableVariant(type, m_ShipInfoArray);
	AssertBotE(id != -1);
	CShips::StationWorkResult result;
	if (system.IsStationBuildable(order, pMajor->GetRaceID())) {
		system.SetIsStationBuilding(order, owner);
		if (system.GetStartStationPoints(owner) == 0)
			system.SetStartStationPoints(m_ShipInfoArray.GetAt((id-10000)).
					GetBaseIndustry(),owner);
		result = ship.BuildStation(order, system, *pMajor, id);
	}
	else
		ship.UnsetCurrentOrder();
	//If we didn't finish, the leader (or any ship doing the station work) must not be removed.
	AssertBotE(!result.remove_leader || result.finished);

	if(order != SHIP_ORDER::BUILD_OUTPOST && result.finished) {
		// Wenn wir jetzt die Station gebaut haben, dann müssen wir die alten Station aus der
		// Schiffsliste nehmen
		for(CShipMap::iterator k = m_ShipMap.begin(); k != m_ShipMap.end(); ++k)
			if (k->second->IsStation() && k->second->GetCo() == system.GetCo())
			{
				AssertBotE(k->second->Key() != ship.Key());
				if(order != SHIP_ORDER::BUILD_STARBASE)
					k->second->Scrap(*pMajor, system, false);
				m_ShipMap.EraseAt(k);
				break;
			}
	}
	return result.remove_leader;
}

namespace //helpers for CBotEDoc::CalcShipOrders()
{
	bool AttackStillValid(const CShips& ship, const CRaceController& RaceCtrl, const CSystem& sy)
	{
		AssertBotE(sy.GetSunSystem());
		const CString& ownerofship = ship.OwnerID();
		const CString& ownerofsector = sy.OwnerID();
		// Wenn die Bevölkerung komplett vernichtet wurde
		// Wenn das System der angreifenden Rasse gehört
		if (sy.GetHabitants() <= 0.000001 || sy.OwnerID() == ownerofship)
			return false;
		// Wenn eine Rasse in dem System lebt
		else if (!ownerofsector.IsEmpty() && ownerofsector != ownerofship)
		{
			const CRace* pRace = RaceCtrl.GetRace(ownerofsector);
			AssertBotE(pRace);
			const CRace* pShipOwner = RaceCtrl.GetRace(ownerofship);
			AssertBotE(pShipOwner);
			if (pRace->GetAgreement(pShipOwner->GetRaceID()) != DIPLOMATIC_AGREEMENT::WAR &&
				!pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
				return false;
		}
		return true;
	}
}

/////END: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipOrders()

/// Diese Funktion berechnet die Schiffsbefehle. Der Systemangriffsbefehl ist davon ausgenommen.
void CBotEDoc::CalcShipOrders()
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

#ifdef CONSISTENCY_CHECKS
		CSanity::GetInstance()->SanityCheckFleet(*y->second);
#endif

		const CPoint& co = y->second->GetCo();
		const SHIP_ORDER::Typ current_order = y->second->GetCurrentOrder();
		CSystem* pSystem = &GetSystem(co.x, co.y);

		// Alle Schiffe, welche einen Systemangriffsbefehl haben überprüfen, ob dieser Befehl noch gültig ist
		if (current_order == SHIP_ORDER::ATTACK_SYSTEM)
			if(!AttackStillValid(*y->second, *m_pRaceCtrl, *pSystem))
				y->second->SetCurrentOrderAccordingToType();

		CRace* pRace = y->second->Owner().get();
		if(pRace->IsMinor())
			continue;//minors don't currently can do something else
		CMajor* pMajor = dynamic_cast<CMajor*>(pRace);
		AssertBotE(pMajor);

		 // Planet soll kolonisiert werden
		if (current_order == SHIP_ORDER::COLONIZE)
		{
			const int terraformedPlanets = pSystem->CountOfTerraformedPlanets();
			// Überprüfen das der Sector auch nur mir oder niemandem geh?rt
			if(!pSystem->IsColonizable(y->second->OwnerID()))
			{
				y->second->UnsetCurrentOrder();
				continue;
			}
			AssertBotE(y->second->GetTerraform() == -1);
			pSystem->DistributeColonists(y->second->GetStartHabitants() / terraformedPlanets);
			pSystem->Colonize(*y->second, *pMajor);
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			CString s;
			s.Format("%s %s",CLoc::GetString("COLONIZATION"), pSystem->GetName());
			pMajor->AddToLostShipHistory(y->second->ShipHistoryInfo(), s, CLoc::GetString("DESTROYED"), m_iRound,
				y->second->GetCo());
			// Schiff entfernen
			y->second->UnsetCurrentOrder();
			RemoveShip(y);
			increment = false;
			continue;//neccessary since it can happen that y becomes the "end" iterator, being not dereferencable
			//and there's y->second ... at the end
		}
		// hier wird ein Planet geterraformed
		else if (current_order == SHIP_ORDER::TERRAFORM)
		{
			AssertBotE(y->second->GetTerraform() != -1);

			if (pSystem->GetPlanet(y->second->GetTerraform())->GetTerraformed() == FALSE)
			{
				if(pSystem->Terraform(*y->second))
				{
					// Hier wurde ein Planet erfolgreich geterraformt
					y->second->UnsetCurrentOrder();
					// Nachricht generieren, dass Terraforming abgeschlossen wurde
					CString s = CLoc::GetString("TERRAFORMING_FINISHED",FALSE,pSystem->GetName());
					CEmpireNews message;
					message.CreateNews(s,EMPIRE_NEWS_TYPE::SOMETHING,pSystem->GetName(),pSystem->GetCo());
					pMajor->GetEmpire()->AddMsg(message);
					m_pClientWorker->AddSoundMessage(SNDMGR_MSG_TERRAFORM_COMPLETE, *pMajor, 0);
					m_pClientWorker->SetToEmpireViewFor(*pMajor);
					// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
					pSystem->OnTerraformPossibleMinor(*pMajor);
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
						AssertBotE(x->second->GetTerraform() == -1);
						continue;
					}
					if (pSystem->GetPlanet(y->second->GetTerraform())->GetTerraformed() == FALSE)
					{
						const unsigned colonize_points = x->second->GetColonizePoints();
						colonize_points_sum += colonize_points;
						if(pSystem->Terraform(*x->second))
						{
							y->second->UnsetCurrentOrder();
							// Nachricht generieren, dass Terraforming abgeschlossen wurde
							CString s = CLoc::GetString("TERRAFORMING_FINISHED",FALSE,pSystem->GetName());
							CEmpireNews message;
							message.CreateNews(s,EMPIRE_NEWS_TYPE::SOMETHING,pSystem->GetName(),pSystem->GetCo());
							pMajor->GetEmpire()->AddMsg(message);
							m_pClientWorker->AddSoundMessage(SNDMGR_MSG_TERRAFORM_COMPLETE, *pMajor, 0);
							m_pClientWorker->SetToEmpireViewFor(*pMajor);
							// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
							pSystem->OnTerraformPossibleMinor(*pMajor);

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
					const unsigned needed_terraform_points = pSystem->GetPlanet(terraforming_planet)->GetNeededTerraformPoints();
					if(colonize_points_sum > needed_terraform_points)
					{
						CString s;
						s.Format("%u", colonize_points_sum - needed_terraform_points);
						s = CLoc::GetString("TERRAFORMING_POINTS_WASTED",FALSE,pSystem->GetName(), s);
						CEmpireNews message;
						message.CreateNews(s,EMPIRE_NEWS_TYPE::SOMETHING,pSystem->GetName(),pSystem->GetCo());
						pMajor->GetEmpire()->AddMsg(message);
					}
				}
			}
		}
		// hier wird ein Aussenposten/Sternbasis gebaut/geupgradet
		else if (y->second->IsDoingStationWork()) {
			if(BuildStation(*y->second, current_order, *pSystem)) {
				RemoveShip(y);
				increment = false;
				continue;
			}
		}
		// Wenn wir das Schiff abracken/zerst?ren/demontieren wollen
		else if (current_order == SHIP_ORDER::DESTROY_SHIP)	// das Schiff wird demontiert
		{
			y->second->Scrap(*pMajor, *pSystem, true);
			m_ShipMap.EraseAt(y);
			increment = false;
			continue;
		}

		// Wenn wir ein Schiff zum Flagschiff ernennen wollen (nur ein Schiff pro Imperium kann ein Flagschiff sein!)
		else if (current_order == SHIP_ORDER::ASSIGN_FLAGSHIP)
		{
			//SHIP_ORDER::ASSIGN_FLAGSHIP is executed immediately now as opposed to at turn change
			AssertBotE(false);
		}
		else if (current_order == SHIP_ORDER::ENCLOAK)
		{
			AssertBotE(y->second->CanCloak(true));
			y->second->SetCloak(true);
			y->second->UnsetCurrentOrder();
		}
		else if (current_order == SHIP_ORDER::DECLOAK)
		{
			AssertBotE(y->second->CanCloak(true));
			y->second->SetCloak(false);
			y->second->UnsetCurrentOrder();
		}
		// Blockadebefehl
		else if (current_order == SHIP_ORDER::BLOCKADE_SYSTEM)
		{
			BOOLEAN blockadeStillActive = FALSE;
			// Überprüfen ob der Blockadebefehl noch Gültigkeit hat
			if (pSystem->OwnerID() != y->second->OwnerID())
				// handelt es sich beim Systembesitzer um eine andere Majorrace
				if (pSystem->Majorized())
				{
					const CString& systemOwner = pSystem->OwnerID();
					const CString& shipOwner   = y->second->OwnerID();
					CRace* pShipOwner	= m_pRaceCtrl->GetRace(shipOwner);
					// haben wir einen Vertrag kleiner einem Freundschaftsvertrag mit der Majorrace
					if (pShipOwner->GetAgreement(systemOwner) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						int blockadeValue = pSystem->GetBlockade();
						//this command shouldn't be possible for ships without blockade special
						AssertBotE(y->second->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP));
						blockadeValue += rand()%20 + 1;
						blockadeStillActive = TRUE;
						y->second->CalcExp();
						// Wenn das Schiff eine Flotte anführt, dann erhöhen auch alle Schiffe in der Flotte mit
						// Blockadeeigenschaft den Blockadewert
						for (CShips::iterator x = y->second->begin(); x != y->second->end(); ++x)
						{
							AssertBotE(x->second->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP));
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
				CRace* pShipOwner = y->second->Owner().get();
				CMajor* pShipOwnerMajor = NULL;
				if (pShipOwner != NULL && pShipOwner->IsMajor() && (pShipOwnerMajor = dynamic_cast<CMajor*>(pShipOwner))->IsHumanPlayer())
				{
					const boost::shared_ptr<CEventBlockade> eventScreen = boost::make_shared<CEventBlockade>(
						y->second->OwnerID(), CLoc::GetString("BLOCKADEEVENT_HEADLINE", FALSE, pSystem->GetName()), CLoc::GetString("BLOCKADEEVENT_TEXT_" + pShipOwner->GetRaceID(), FALSE, pSystem->GetName()));
					pShipOwnerMajor->GetEmpire()->PushEvent(eventScreen);
				}
				if (pSystem->Majorized())
				{
					CMajor* pSystemOwnerMajor = dynamic_cast<CMajor*>(pSystem->Owner().get());
					AssertBotE(pSystemOwnerMajor);
					if (pSystemOwnerMajor->IsHumanPlayer())
					{
						const boost::shared_ptr<CEventBlockade> eventScreen = boost::make_shared<CEventBlockade>(
							pSystem->OwnerID(), CLoc::GetString("BLOCKADEEVENT_HEADLINE", FALSE, pSystem->GetName()), CLoc::GetString("BLOCKADEEVENT_TEXT_" + pSystemOwnerMajor->GetRaceID(), FALSE, pSystem->GetName()));
						pSystemOwnerMajor->GetEmpire()->PushEvent(eventScreen);
					}
				}
			}
		}
		else if (current_order == SHIP_ORDER::WAIT_SHIP_ORDER)
		{
			//Do nothing, but only for this round.
			y->second->UnsetCurrentOrder();
		}
		//else if (current_order == SHIP_ORDER::SENTRY_SHIP_ORDER)
			//Do nothing for this and all following rounds until an explicit player input.
		else if (current_order == SHIP_ORDER::REPAIR)
		{
			//The actual Hull reparing is currenty done in CalcShipMovement(),
			//after the call to this function.
		}

		// Vor der Schiffsbewegung aber nach einer möglichen Demontage dort ?berall einen ShipPort setzen wo
		// eine Sternbasis oder ein Au?enposten steht
		if (y->second->IsStation())
		{
			pSystem->SetShipPort(TRUE, y->second->OwnerID());
		}
	}
}

/// Diese Funktion berechnet die Schiffsbewegung und noch weitere kleine Sachen im Zusammenhang mit Schiffen.
void CBotEDoc::CalcShipMovement()
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
		pMajor->GetStarmap()->SynchronizeWithMap(m_Systems, &races);
	}

#ifdef CONSISTENCY_CHECKS
	std::set<CString> already_encountered_ships_for_sanity_check;
#endif
	CShipMap ships_from_fleets;
	// Hier kommt die Schiffsbewegung (also keine anderen Befehle werden hier noch ausgewertet, lediglich wird überprüft,
	// dass manche Befehle noch ihre Gültigkeit haben
	for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
#ifdef CONSISTENCY_CHECKS
		CSanity::GetInstance()->CheckShipUniqueness(*y->second, already_encountered_ships_for_sanity_check);
#endif
		const CPoint& p = y->second->GetCo();
		// Prüfen, dass ein Terraformbefehl noch gültig ist
		if (y->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
		{
			if (GetSystem(p.x, p.y).GetPlanet(y->second->GetTerraform())->GetTerraformed())
				y->second->SetTerraform(-1);
		}
		// Prüfen, dass Stationsarbeitsbefehle noch gültig sind
		//This is neccessary, as it can happen, that another major, whose ships are near the start
		//of the shipmap, is still doing station work in this sector in the same round that another major,
		//whose ships come after the first major's in the shipmap, finished its station work in this sector.
		//If it weren't for this ordering in the shipmap, we could just unset the orders of all ships
		//following the finishing event in CalcShipOrders().
		if (y->second->IsDoingStationWork() &&
			!GetSystem(p.x, p.y).IsStationBuildable(y->second->GetCurrentOrder(), y->second->OwnerID()))
			y->second->UnsetCurrentOrder();

		// weiter mit Schiffsbewegung
		Sector shipKO((char)y->second->GetCo().x,(char)y->second->GetCo().y);
		Sector targetKO((char)y->second->GetTargetKO().x,(char)y->second->GetTargetKO().y);
		Sector nextKO(-1,-1);

		AssertBotE(shipKO.on_map());
		//no target set by targetKO == current coords is no longer allowed
		AssertBotE(shipKO != targetKO);

		// Weltraummonster gesondert behandeln (Geschwindigkeit der Flotte sollte egal sein, nur das Alien muss fliegen können)
		if (y->second->IsAlien() && y->second->GetSpeed(true) > 0)
		{
			// wenn bei einem Weltraummonster kein Ziel vorhanden ist und es aktuell auch nichts macht,
			// dann wird zufüllig ein neues generiert
			if (y->second->HasNothingToDo())
			{
				// irgend ein zufülliges neues Ziel generieren, welches nicht auf einer Anomalie endet
				while (true)
				{
					targetKO = Sector(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
					if (targetKO == shipKO)
						continue;

					if (GetSystem(targetKO.x, targetKO.y).GetAnomaly())
						continue;

					// Anaerobe Makroben fliegen nur im freien Raum oder in Sektoren mit grünen Sonnen
					if (y->second->OwnerID() == StrToCStr(ANAEROBE_MAKROBE))
					{
						if (GetSystem(targetKO.x, targetKO.y).GetSunSystem() && GetSystem(targetKO.x, targetKO.y).GetSunColor() != 1)
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
				boost::scoped_ptr<CStarmap> pStarmap(new CStarmap(0));
				vector<Sector> vExceptions;
				// Anaerobe Makroben fliegen nur im freien Raum oder in Sektoren mit grünen Sonnen
				if (y->second->OwnerID() == StrToCStr(ANAEROBE_MAKROBE))
				{
					for (std::vector<CSystem>::iterator sector = m_Systems.begin(); sector != m_Systems.end(); ++sector)
					{
						// Ausnahmen hinzufügen, wenn es sich um ein Sonnensystem mit nicht grüner Sonne handelt
						// und es nicht der aktuelle Ort ist (wegfliegen soll immer möglich sein)
						if (sector->GetSunSystem() && sector->GetSunColor() != 1 && sector->GetCo() != y->second->GetCo())
							vExceptions.push_back(Sector(sector->GetCo()));
					}
				}

				pStarmap->SetFullRangeMap(SM_RANGE_NEAR, vExceptions);

				// Anomalien werden schon beachtet, da dies eine statische Variable ist und in NextRound() schon
				// berechnet wurde.
				nextKO = pStarmap->CalcPath(shipKO,targetKO,range,speed,*y->second->GetPath());
			}
			else
			{
				pRace = y->second->Owner().get();
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
				if (pRace != NULL && pRace->IsMajor() && !(this->GetSystem(nextKO.x,nextKO.y).GetFullKnown(y->second->OwnerID())))
				{
					CRandomEventCtrl* pRandomEventCtrl = CRandomEventCtrl::GetInstance();
					pRandomEventCtrl->CalcExploreEvent(CPoint((int)nextKO.x,(int)nextKO.y),dynamic_cast<CMajor*>(pRace),&m_ShipMap);
				}

				int high = speed;
				while (high > 0 && high < y->second->GetPath()->GetSize())
				{
					y->second->GetPath()->RemoveAt(0);
					high--;
				}
			}
		}

		const CPoint& co = y->second->GetCo();
		const CSystem& system = GetSystem(co.x, co.y);
		// Gibt es eine Anomalie, wodurch die Schilde schneller aufgeladen werden
		bool bFasterShieldRecharge = false;
		if (system.GetAnomaly())
			if (system.GetAnomaly()->GetType() == BINEBULA)
				bFasterShieldRecharge = true;

		// Nach der Bewegung, aber noch vor einem möglichen Kampf werden die Schilde nach ihrem Typ wieder aufgeladen,
		// wenn wir auf einem Shipport sind, dann wird auch die Hülle teilweise wieder repariert
		//FIXME: The shipports are not yet updated for changes due to diplomacy at this spot.
		//If we declared war and are on a shipport of the former friend, the ship is repaired,
		//and a possible repair command isn't unset though it can no longer be set by the player this turn then.
		const bool port = system.GetShipPort(y->second->OwnerID());
		if(y->second->GetCurrentOrder() == SHIP_ORDER::REPAIR)
			y->second->RepairCommand(port, bFasterShieldRecharge, ships_from_fleets);
		else
			y->second->TraditionalRepair(port, bFasterShieldRecharge);

		// Checken ob Training Gültigkeit hat
		if (system.Majorized() && system.OwnerID() == y->second->OwnerID())
		{
			// Wenn ein Schiff mit Veteranenstatus (Level 4) in der Trainingsflotte ist,
			// dann verdoppelt sich der Erfahrungsgewinn für die niedrigstufigen Schiffe
			const int XP = system.GetProduction()->GetShipTraining();
			if(XP > 0)
				y->second->ApplyTraining(XP);
		}

		// wenn eine Anomalie vorhanden, deren m?gliche Auswirkungen auf das Schiff berechnen
		if (system.GetAnomaly())
		{
			system.GetAnomaly()->CalcShipEffects(y->second.get(), ships_from_fleets);
			bAnomaly = true;
		}
	}
	m_ShipMap.Append(ships_from_fleets);

	if (!bAnomaly)
		return;
	CheckShipsDestroyedByAnomaly();
}
/////BEGIN: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipMovement()
void CBotEDoc::CheckShipsDestroyedByAnomaly() {
	// prüfen ob irgendwelche Schiffe durch eine Anomalie zerstört wurden
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end();)
	{
		const CPoint& co = i->second->GetCo();
		if(!GetSystem(co.x, co.y).GetAnomaly()) {
			++i;
			continue;
		}
		const CString& anomaly = GetSystem(co.x, co.y).GetAnomaly()
			->GetMapName(co);
		CRace* pRace = i->second->Owner().get();
		if(i->second->RemoveDestroyed(*pRace, m_iRound, anomaly,
				CLoc::GetString("DESTROYED"), NULL, anomaly)) {
			++i;
			continue;
		}
		RemoveShip(i);
	}
}
/////END: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipMovement()

/// Funktion überprüft, ob irgendwo ein Schiffskampf stattfindet. Wenn ja und es sind menschliche Spieler beteiligt,
/// so werden ihnen alle jeweils beteiligten Schiffe geschickt, so dass sie dort Befehle geben können.
bool CBotEDoc::IsShipCombat()
{
	m_bCombatCalc = false;

	// Jetzt gehen wir nochmal alle Sektoren durch, wenn in einem Sektor Schiffe mehrerer verschiedener Rassen sind,
	// die Schiffe nicht auf Meiden gestellt sind und die Rassen untereinander nicht alle mindst. einen Freundschafts-
	// vertrag haben, dann kommt es in diesem Sektor zum Kampf
	for(CShipMap::const_iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CPoint& p = y->second->GetCo();
		const CSystem& sector = GetSystem(p.x, p.y);
		// Wenn unser Schiff auf Angreifen gestellt ist
		// Wenn in dem Sektor des Schiffes schon ein Kampf stattgefunden hat, dann findet hier keiner mehr statt
		if (y->second->GetCombatTactic() != COMBAT_TACTIC::CT_ATTACK
			|| m_CombatSectors.find(CoordsToIndex(sector.GetCo().x, sector.GetCo().y)) != m_CombatSectors.end())
			continue;
		// Wenn noch kein Kampf in dem Sektor stattfand, dann kommt es möglicherweise hier zum Kampf
		for(CShipMap::const_iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
		{
			const CString& sOwner1 = y->second->OwnerID();
			const CString& sOwner2 = i->second->OwnerID();
			// nur weiter, wenn das Schiff nicht unserer Rasse gehört
			// und wenn das Schiff sich im gleichen Sektor befindet
			if (sOwner2 == sOwner1 || i->second->GetCo() != p)
				continue;
			const CRace* pRace1 = m_pRaceCtrl->GetRace(sOwner1);
			const CRace* pRace2 = m_pRaceCtrl->GetRace(sOwner2);
			// Wenn sich die Rassen aus diplomatischer Beziehung heraus angreifen können
			if (!CCombat::CheckDiplomacyStatus(pRace1, pRace2))
				continue;
			m_bCombatCalc = true;
			m_ptCurrentCombatSector = p;
			m_CombatSectors.insert(CoordsToIndex(sector.GetCo().x, sector.GetCo().y));
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
void CBotEDoc::CalcShipCombat()
{
	if (!m_bCombatCalc)
		return;

	// Alle Schiffe im zuvor berechneten Kampfsektor holen
	CArray<CShips*> vInvolvedShips;
	CPoint p = m_ptCurrentCombatSector;
	// Jetzt gehen wir nochmal alle Sektoren durch und schauen ob ein Schiff im Kampfsektor ist
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
	{
		if (i->second->GetCo() != m_ptCurrentCombatSector)
			continue;

		vInvolvedShips.Add(i->second.get());

		// Wenn das Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
		for (CShips::iterator j = i->second->begin(); j != i->second->end(); ++j)
			vInvolvedShips.Add(j->second.get());
	}

	// es sollten immer Schiffe im Array sein, sonst hätte in diesem Sektor kein Kampf stattfinden dürfen
	AssertBotE(!vInvolvedShips.IsEmpty());

	// Kampf-KI
	CCombatAI AI;
	bool bCombat = AI.CalcCombatTactics(vInvolvedShips, m_pRaceCtrl, m_mCombatOrders, GetSystem(p.x, p.y).GetAnomaly().get());
	if (!bCombat)
	{
		for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); ++i)
		{
			if (i->second->GetCo() != m_ptCurrentCombatSector)
				continue;
			const COMBAT_TACTIC::Typ LeadersCombatTactic = i->second->GetCombatTactic();
			if(LeadersCombatTactic != COMBAT_TACTIC::CT_RETREAT)
				i->second->SetCombatTactic(LeadersCombatTactic, true, false);
		}
		return;
	}

	// Jetzt können wir einen Kampf stattfinden lassen
	CCombat Combat;
	Combat.SetInvolvedShips(&vInvolvedShips, m_pRaceCtrl, GetSystem(p.x, p.y).GetAnomaly().get());
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

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	for (CRaceController::const_iterator it = m_pRaceCtrl->begin(); it != m_pRaceCtrl->end(); ++it)
	{
		const CSystem& system = GetSystem(p.x, p.y);
		// ist der Sektor bekannt?
		const CString& sSectorName = system.CoordsName(system.GetKnown(it->first));

		// gewonnen
		if (winner[it->first] == 1 && it->second->IsMajor())
		{
			// dem Siegbedingungsüberwacher den Sieg mitteilen
			m_VictoryObserver.AddCombatWin(it->first);

			CMajor* pMajor = dynamic_cast<CMajor*>(it->second.get());
			AssertBotE(pMajor);

			CEmpireNews message;
			message.CreateNews(CLoc::GetString("WIN_COMBAT", false, sSectorName), EMPIRE_NEWS_TYPE::MILITARY, "", p);
			pMajor->GetEmpire()->AddMsg(message);
			// win a minor battle
			CString eventText = pMajor->GetMoralObserver()->AddEvent(3, pMajor->GetRaceMoralNumber());
			message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, "", p);
			pMajor->GetEmpire()->AddMsg(message);
			m_pClientWorker->SetToEmpireViewFor(*pMajor);
		}
		// verloren
		else if (winner[it->first] == 2)
		{
			if (it->second->IsMajor())
			{
				CMajor* pMajor = dynamic_cast<CMajor*>(it->second.get());
				AssertBotE(pMajor);

				CEmpireNews message;
				message.CreateNews(CLoc::GetString("LOSE_COMBAT", false, sSectorName), EMPIRE_NEWS_TYPE::MILITARY, "", p);
				pMajor->GetEmpire()->AddMsg(message);
				// lose a minorbattle
				CString eventText = pMajor->GetMoralObserver()->AddEvent(6, pMajor->GetRaceMoralNumber());
				message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, "", p);
				pMajor->GetEmpire()->AddMsg(message);
				m_pClientWorker->SetToEmpireViewFor(*pMajor);
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
			CMajor* pMajor = dynamic_cast<CMajor*>(it->second.get());
			AssertBotE(pMajor);

			CEmpireNews message;
			message.CreateNews(CLoc::GetString("DRAW_COMBAT", false, sSectorName), EMPIRE_NEWS_TYPE::MILITARY, "", p);
			pMajor->GetEmpire()->AddMsg(message);
			m_pClientWorker->SetToEmpireViewFor(*pMajor);
		}
	}

	// Bevor die Schiffe zerstört werden, prüfen wieviel Schiffe durch einen Boseaner (Alien) zerstört wurden.
	// Um diese Anzahl mehr werden neue Boseaner ins Spiel gebracht.
	if (const map<CShips*, std::set<const CShips*> >* pmKilledShips = Combat.GetKilledShipsInfos())
	{
		for (map<CShips*, std::set<const CShips*> >::const_iterator it = pmKilledShips->begin(); it != pmKilledShips->end(); ++it)
		{
			CShips* pBoseaner = it->first;
			if (!pBoseaner || pBoseaner->OwnerID() != StrToCStr(BOSEANER) || !pBoseaner->IsAlive())
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
				CShipMap::iterator pNewShip = BuildShip(pBoseaner->GetID(), m_ptCurrentCombatSector, pBoseaner->OwnerID());

				// neuen Boseaner in Gruppe stecken und Befehle gleich mit übernehmen
				pLeader->AddShipToFleet(pNewShip->second);
				m_ShipMap.EraseAt(pNewShip);
			}
		}
	}

	// Nach einem Kampf muß ich das Feld der Schiffe durchgehen und alle Schiffe aus diesem nehmen, die
	// keine Hülle mehr besitzen. Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte
	CStringArray destroyedShips;
	for(CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end();)
	{
		if (i->second->GetCo() != m_ptCurrentCombatSector)
		{
			++i;
			continue;
		}
		//To ensure consistent combat behavior in a leader/fleet situation, set the combat behavior of every
		//ship in the fleet to the leader's behavior, unless it is RETREAT, this case will be handled in ship
		//retreat code
		const COMBAT_TACTIC::Typ LeadersCombatTactic = i->second->GetCombatTactic();
		if(LeadersCombatTactic != COMBAT_TACTIC::CT_RETREAT)
			i->second->SetCombatTactic(LeadersCombatTactic, true, false);

			CRace* pOwner = i->second->Owner().get();
		AssertBotE(pOwner);
		if (i->second->RemoveDestroyed(*pOwner, m_iRound, CLoc::GetString("COMBAT"),	CLoc::GetString("DESTROYED"), &destroyedShips))
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
				s.Format("%s", CLoc::GetString("DESTROYED_SHIPS_IN_COMBAT",0,destroyedShips[j]));
				CEmpireNews message;
				message.CreateNews(s, EMPIRE_NEWS_TYPE::MILITARY, "", p);
				pMajor->GetEmpire()->AddMsg(message);
			}
		}
	}
}

/////BEGIN: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipEffects()
void CBotEDoc::CalcShipRetreat() {
	// Schiffe mit Rückzugsbefehl auf ein Feld neben dem aktuellen Feld setzen
	for(CShipMap::iterator ship = m_ShipMap.begin(); ship != m_ShipMap.end(); ++ship) {
		const CString& ship_owner = ship->second->OwnerID();
		// Hat das Schiff den Rückzugsbefehl
		const CShips::RETREAT_MODE mode = ship->second->CalcRetreatMode();
		if(mode == CShips::RETREAT_MODE_STAY)
			continue;

		// Rückzugssektor für dieses Schiff in diesem Sektor holen
		const std::map<CString, std::map<std::pair<int, int>, CPoint>>::const_iterator&
			SectorRetreatSectorPairs = m_mShipRetreatSectors.find(ship_owner);
		if (SectorRetreatSectorPairs == m_mShipRetreatSectors.end())
			continue;
		const std::map<std::pair<int, int>, CPoint>& mSectorRetreatSectorPairs
			= SectorRetreatSectorPairs->second;
		const CPoint& co = ship->second->GetCo();
		const pair<int, int> CurrentSector(co.x, co.y);
		const std::map<std::pair<int, int>, CPoint>::const_iterator& RetreatSector
			= mSectorRetreatSectorPairs.find(CurrentSector);
		if (RetreatSector == mSectorRetreatSectorPairs.end())
			continue;

		if(ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
			ship->second->Retreat(RetreatSector->second);
		if(!ship->second->HasFleet())
			continue;
		// sind alle Schiffe in einer Flotte im Rückzug, so kann die ganze Flotte
		// in den Rückzugssektor
		const bool bCompleteFleetRetreat = ship->second->GetSpeed(true) > 0
			&& mode == CShips::RETREAT_MODE_COMPLETE;

		if (bCompleteFleetRetreat) {
			const COMBAT_TACTIC::Typ NewCombatTactic = ship->second->GetCombatTactic();
			ship->second->RetreatFleet(RetreatSector->second, &NewCombatTactic);
		}
		// Schiffe aus der Flotte nehmen und ans Ende des Schiffsarrays packen. Diese werden
		// dann auch noch behandelt
		else
		{
			const CShipMap& fleet = ship->second->Fleet();
			m_ShipMap.Append(fleet);
			ship->second->Reset();
		}
	}//	for (int i = 0; i < m_ShipMap.GetSize(); i++)
	m_mShipRetreatSectors.clear();
}
/////END: HELPER FUNCTIONS FOR void CBotEDoc::CalcShipEffects()

/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
void CBotEDoc::CalcShipEffects()
{
	CalcShipRetreat();

	// Nach einem möglichen Kampf, aber natürlich auch generell die Schiffe und Stationen den Sektoren bekanntgeben
	for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CString& sRace = y->second->OwnerID();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		const CPoint& p = y->second->GetCo();
		CSystem& system = GetSystem(p.x, p.y);

		// Anomalien beachten
		bool bDeactivatedShipScanner = false;
		bool bBetterScanner = false;
		const boost::shared_ptr<const CAnomaly>& anomaly = system.GetAnomaly();
		if(anomaly) {
			bDeactivatedShipScanner = anomaly->IsShipScannerDeactivated();
			bBetterScanner = anomaly->GetType() == QUASAR;
		}

		y->second->CalcEffects(system, pRace, bDeactivatedShipScanner, bBetterScanner);
		// Dem Sektor nochmal bekanntgeben, dass in ihm eine Sternbasis oder ein Außenposten steht. Weil wenn im Kampf
		// eine Station teilnahm, dann haben wir den Shipport in dem Sektor vorläufig entfernt. Es kann ja passieren,
		// dass die Station zerstört wird. Haben wir jetzt aber immernoch eine Station, dann bleibt der Shipport dort auch
		// bestehen
		if (y->second->IsStation())
			system.SetShipPort(TRUE, sRace);
	}
}

///////////////////////////////////////////////////////////////////////
/////BEGINN: HELPER FUNCTIONS FOR void CBotEDoc::CalcContactNewRaces()

void CBotEDoc::CalcContactShipToMajorShip(CRace& Race, const CSector& sector) {
	// treffen mit einem Schiff eines anderen Majors
	// wenn zwei Schiffe verschiedener Rasse in diesem Sektor stationiert sind, so können sich die Besitzer auch kennenlernen
	const std::map<CString, CMajor*>& mMajors = *m_pRaceCtrl->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = mMajors.begin(); it != mMajors.end(); ++it) {
		CMajor* pMajor = it->second;
		// kann der andere Schiffsbesitzer Rassen kennenlernen?
		const CString& sMajorID = it->first;
		if (!sector.GetOwnerOfShip(sMajorID, true) || !pMajor->CanBeContactedBy(Race.GetRaceID()) ||
			Race.GetRaceID() == sMajorID)
			continue;
		CalcContactCommutative(*pMajor, Race, sector);
	}
}

void CBotEDoc::CalcContactCommutative(CMajor& Major,
	CRace& ContactedRace, const CSector& p) {

	Major.Contact(ContactedRace, p);
	m_pClientWorker->CalcContact(Major, ContactedRace);
	ContactedRace.Contact(Major, p);
	if(ContactedRace.IsMajor())
		m_pClientWorker->CalcContact(dynamic_cast<CMajor&>(ContactedRace), Major);
}

void CBotEDoc::CalcContactMinor(CMajor& Major, const CSector& sector) {
	if(!sector.GetMinorRace())
		return;
	// in dem Sektor lebt eine Minorrace
	const boost::shared_ptr<CMinor>& pMinor = boost::dynamic_pointer_cast<CMinor>(sector.HomeOf());
	AssertBotE(pMinor);
	// kann der Sektorbesitzer andere Rassen kennenlernen?
	if (pMinor->CanBeContactedBy(Major.GetRaceID()))
		// die Rasse ist noch nicht bekannt
		CalcContactCommutative(Major, *pMinor, sector);
}

/////END: HELPER FUNCTIONS FOR void CBotEDoc::CalcContactNewRaces()
///////////////////////////////////////////////////////////////////////

/// Diese Funktion überprüft, ob neue Rassen kennengelernt wurden.
void CBotEDoc::CalcContactNewRaces()
{
	for(CShipMap::const_iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
	{
		const CString& sRace = y->second->OwnerID();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		// kann die Rasse andere Rassen kennenlernen?
		if(pRace->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
			continue;
		const CPoint& p = y->second->GetCo();
		const CSystem& system = GetSystem(p.x, p.y);
		const CString& sOwnerOfSector = system.OwnerID();
		CalcContactShipToMajorShip(*pRace, system);
		if(sOwnerOfSector.IsEmpty() || sOwnerOfSector == sRace)
			continue;
		CRace* pOwnerOfSector = m_pRaceCtrl->GetRace(sOwnerOfSector);
		AssertBotE(pOwnerOfSector);
		if(pRace->IsMinor()) {
			if(pOwnerOfSector->CanBeContactedBy(sRace) && pOwnerOfSector->IsMajor()) {
				CMajor* pMajor = dynamic_cast<CMajor*>(pOwnerOfSector);
				AssertBotE(pMajor);
				CalcContactCommutative(*pMajor, *pRace, system);
			}
			continue;
		}
		//At present, a race is always either a major or a minor.
		//If this changes, this code needs to be adapted.
		CMajor* pMajor = dynamic_cast<CMajor*>(pRace);
		AssertBotE(pMajor);
		CalcContactMinor(*pMajor, system);
		if (!pOwnerOfSector->CanBeContactedBy(sRace))
			continue;
		//At this point, pOwnerOfSector must be of type major, since independent or no diplo minors are handled.
		AssertBotE(pOwnerOfSector->IsMajor());
		CalcContactCommutative(*pMajor, *pOwnerOfSector, system);
	}//for (int y = 0; y < m_ShipMap.GetSize(); y++)
}

/// Funktion berechnet die Auswirkungen wenn eine Minorrace eleminiert wurde und somit aus dem Spiel ausscheidet.
/// @param pMinor Minorrace welche aus dem Spiel ausscheidet
void CBotEDoc::CalcEffectsMinorEleminated(const boost::shared_ptr<CMinor>& pMinor)
{
	if (!pMinor)
	{
		AssertBotE(pMinor);
		return;
	}

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
			CString news = CLoc::GetString("ELIMINATE_MINOR", FALSE, pMinor->GetName());
			CEmpireNews message;
			if (pMinor->IsAlien())
				message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING);
			else
				message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING, GetSystem(pMinor->GetCo().x, pMinor->GetCo().y).GetName(), pMinor->GetCo());

			pMajor->GetEmpire()->AddMsg(message);
			if (pMajor->IsHumanPlayer())
			{
				// Event über die Rassenauslöschung einfügen
				const boost::shared_ptr<CEventRaceKilled> eventScreen = boost::make_shared<CEventRaceKilled>(
					it->first, pMinor->GetRaceID(), pMinor->GetName(), pMinor->GetGraphicFileName());
				pMajor->GetEmpire()->PushEvent(eventScreen);

				m_pClientWorker->SetToEmpireViewFor(*it->second);
			}
		}

		// die vernichtete Minor auf unbekannt schalten und alle Verträge entfernen
		pMajor->SetIsRaceContacted(pMinor->GetRaceID(), false);
		pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
	}

	// zuletzt noch alle Schiffe der Minorrace entfernen
	for (CShipMap::iterator i = m_ShipMap.begin(); i != m_ShipMap.end(); )
	{
		if (i->second->OwnerID() == pMinor->GetRaceID())
		{
			m_ShipMap.EraseAt(i);
			continue;
		}

		++i;
	}
}

/// Diese Funktion führt allgemeine Berechnung durch, die immer zum Ende der NextRound-Calculation stattfinden müssen.
void CBotEDoc::CalcEndDataForNextRound()
{
	// Nachdem Außenposten und Sternbasen auch den Sektoren wieder bekanntgegeben wurden, können wir die Besitzerpunkte
	// für die Sektoren berechnen.
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	// ausgelöschte Hauptrassen behandeln
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		pMajor->GetEmpire()->GenerateSystemList(m_Systems);
		pMajor->GetEmpire()->SetNumberOfSystems(pMajor->GetEmpire()->GetSystemList()->GetSize());

		// Wenn das Imperium keine Systeme mehr besitzt, so wird es für alle anderen Rassen auf unbekannt gestellt.
		// Scheidet somit aus dem Spiel aus
		if (pMajor->GetEmpire()->CountSystems() == 0)
		{
			// Allen anderen bekannten Imperien die Nachricht zukommen lassen, dass die Rasse vernichtet wurde
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (pMajor->GetRaceID() == itt->first)
					continue;

				if (itt->second->IsRaceContacted(pMajor->GetRaceID()))
				{
					// Nachricht über Rassenauslöschung (hier die gleiche wie bei Minorauslöschung
					CString news = CLoc::GetString("ELIMINATE_MINOR", FALSE, pMajor->GetName());
					CEmpireNews message;
					message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING);
					itt->second->GetEmpire()->AddMsg(message);
					if (itt->second->IsHumanPlayer())
					{
						// Event über die Rassenauslöschung einfügen
						const boost::shared_ptr<CEventRaceKilled> eventScreen = boost::make_shared<CEventRaceKilled>(
							itt->first, pMajor->GetRaceID(), pMajor->GetName(), pMajor->GetGraphicFileName());
						itt->second->GetEmpire()->PushEvent(eventScreen);

						m_pClientWorker->SetToEmpireViewFor(*itt->second);
					}
				}
			}

			// Alle Nachrichten und Events löschen
			pMajor->GetEmpire()->ClearMessagesAndEvents();

			m_pClientWorker->ClearSoundMessages(*pMajor);

			// alle anderen Rassen durchgehen und die vernichtete Rasse aus deren Maps entfernen
			for (CRaceController::const_iterator itt = m_pRaceCtrl->begin(); itt != m_pRaceCtrl->end(); ++itt)
			{
				if (pMajor->GetRaceID() == itt->first)
					continue;

				CRace* pLivingRace = itt->second.get();

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
				if (j->second->OwnerID() == pMajor->GetRaceID())
				{
					// Alle noch "lebenden" Schiffe aus der Schiffshistory ebenfalls als zerstört ansehen
					pMajor->GetShipHistory()->ModifyShip(j->second->ShipHistoryInfo(),
						GetSystem(j->second->GetCo().x, j->second->GetCo().y).CoordsName(true), m_iRound, CLoc::GetString("UNKNOWN"),
						CLoc::GetString("DESTROYED"));
					m_ShipMap.EraseAt(j);
				}
				else
					++j;
			}

			// Sektoren und Systeme neutral schalten
			for(std::vector<CSystem>::iterator sy = m_Systems.begin(); sy != m_Systems.end(); ++sy) {
				const CString& ID = pMajor->GetRaceID();
				if (sy->OwnerID() == ID)
					sy->ChangeOwner("", CSystem::OWNING_STATUS_EMPTY);
				// Den ersten Besitzer als Historie merken. Diese Variable nicht zurücksetzen!
				// Sonst würde dieses System nicht mehr serialisiert werden, da es ja niemandem mehr gehört...
				// sy->SetColonyOwner("");

				// in allen Sektoren alle Schiffe aus den Sektoren nehmen
				sy->SetIsStationBuilding(SHIP_ORDER::NONE, ID);
				sy->SetShipPort(false, ID);
			}

			// Wenn es ein menschlicher Spieler ist, so bekommt er den Eventscreen für die Niederlage angezeigt
			if (pMajor->IsHumanPlayer())
			{
				// einen neuen (und auch einzigen Event) einfügen
				const boost::shared_ptr<CEventGameOver> eventScreen = boost::make_shared<CEventGameOver>(
					pMajor->GetRaceID());
				pMajor->GetEmpire()->PushEvent(eventScreen);
			}
		}
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor->GetEmpire()->CountSystems() == 0)
			continue;

		// Moralveränderungen aufgrund möglicher Ereignisse berechnen. Erst nach der Schiffsbewegung und allem anderen
		pMajor->GetMoralObserver()->CalculateEvents(m_Systems, pMajor->GetRaceID(), pMajor->GetRaceMoralNumber());
		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> kein Abzug beim Stellaren Lager
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
			pMajor->GetEmpire()->GetGlobalStorage()->SetLosing(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(2));
		// Ressourcentransfers im globalen Lager vornehmen
		pMajor->GetEmpire()->GetGlobalStorage()->Calculate(m_Systems);
		pMajor->GetEmpire()->GetGlobalStorage()->SetMaxTakenRessources(1000 * pMajor->GetEmpire()->CountSystems());
		// Befindet sich irgendeine Ressource im globalen Lager, bekommt der Spieler eine Imperiumsmeldung
		if (pMajor->GetEmpire()->GetGlobalStorage()->IsFilled())
		{
			CString s = CLoc::GetString("RESOURCES_IN_GLOBAL_STORAGE");
			CEmpireNews message;
			message.CreateNews(s, EMPIRE_NEWS_TYPE::ECONOMY, 4);
			pMajor->GetEmpire()->AddMsg(message);
			m_pClientWorker->SetToEmpireViewFor(*pMajor);
		}

		// Schiffskosten berechnen
		UINT popSupport = pMajor->GetEmpire()->GetPopSupportCosts();
		UINT shipCosts  = pMajor->GetEmpire()->GetShipCosts();

		int costs = popSupport - shipCosts;
		if (costs < 0)
			pMajor->GetEmpire()->SetCredits(costs);
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (it->second->GetEmpire()->CountSystems() == 0)
			continue;

		const CString& sID = it->first;
		for(std::vector<CSystem>::const_iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
		{
			const CPoint& p = it->GetCo();
			const int x = p.x;
			const int y = p.y;
			// Befindet sich ein Außenposten oder ein System in einem der umliegenden Sektoren, so bekommt der
			// Sektor einen Besitzerpunkt. Bei einer Sternbasis sind es sogar zwei Besitzerpunkte.
			int ownerPoints = 0;
			if (it->Majorized() && it->OwnerID() == sID)
				ownerPoints += 1;
			if (it->GetStation(SHIP_TYPE::OUTPOST, sID))
				ownerPoints += 1;
			if (it->GetStation(SHIP_TYPE::STARBASE, sID))
				ownerPoints += 2;
			if (ownerPoints > 0)
			{
				for (int j = -1; j <= 1; j++)
					for (int i = -1; i <= 1; i++)
						if ((y+j < STARMAP_SECTORS_VCOUNT && y+j > -1) && (x+i < STARMAP_SECTORS_HCOUNT && x+i > -1))
							GetSystem(x + i, y + j).AddOwnerPoints(ownerPoints, sID);

				// in vertikaler und horizontaler Ausrichtung gibt es sogar 2 Felder vom Sector entfernt noch
				// Besitzerpunkte
				if (x-2 >= 0)
					GetSystem(x - 2, y).AddOwnerPoints(ownerPoints, sID);
				if (x+2 < STARMAP_SECTORS_HCOUNT)
					GetSystem(x + 2, y).AddOwnerPoints(ownerPoints, sID);
				if (y-2 >= 0)
					GetSystem(x, y - 2).AddOwnerPoints(ownerPoints, sID);
				if (y+2 < STARMAP_SECTORS_VCOUNT)
					GetSystem(x, y + 2).AddOwnerPoints(ownerPoints, sID);
			}
		}
	}

	// Jetzt die Besitzer berechnen und die Variablen, welche nächste Runde auch angezeigt werden sollen.
	for(std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
	{
		it->CalculateOwner();
		if (it->Majorized())
		{
			AssertBotE(it->GetSunSystem() && !it->Free());
			CMajor* pMajor = dynamic_cast<CMajor*>(it->Owner().get());
			AssertBotE(pMajor && pMajor->IsMajor());

			// baubare Gebäude, Schiffe und Truppen berechnen
			it->CalculateBuildableBuildings(&m_GlobalBuildings);
			it->CalculateBuildableShips();
			it->CalculateBuildableTroops(&m_TroopInfo, pMajor->GetEmpire()->GetResearch());
			it->CalculateVariables();

			// alle produzierten FP und SP der Imperien berechnen und zuweisen
			int currentPoints;
			currentPoints = it->GetProduction()->GetResearchProd();
			pMajor->GetEmpire()->AddFP(currentPoints);
			currentPoints = it->GetProduction()->GetSecurityProd();
			pMajor->GetEmpire()->AddSP(currentPoints);

			it->ExecuteManager(*pMajor, true);
		}

		// Gibt es eine Anomalie im Sektor, so vielleicht die Scanpower niedriger setzen
		if (it->GetAnomaly())
			it->GetAnomaly()->ReduceScanPower(it->GetCo());
	}

	// Nachdem die Besitzerpunkte der Sektoren berechnet wurden kann versucht werden neue Rassen kennenzuelernen
	CalcContactNewRaces();

	// Nun das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse aller Imperien
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	// Dies gilt nur für Majorsraces.
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor->GetEmpire()->CountSystems() == 0)
			continue;

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
			pMajor->GetEmpire()->ClearMessagesAndEvents();

			m_pClientWorker->ClearSoundMessages(*pMajor);

			// Wenn es ein menschlicher Spieler ist, so bekommt er den Eventscreen für den Sieg angezeigt
			if (pMajor->IsHumanPlayer())
			{
				// einen neuen (und auch einzigen Event) einfügen
				CString sImageName;
				if (pMajor->GetRaceID() == m_VictoryObserver.GetVictoryRace())
					sImageName = "Victory" + pMajor->GetRaceID();
				else
					sImageName = "GameOver";
				const boost::shared_ptr<CEventVictory> eventScreen = boost::make_shared<CEventVictory>(
					pMajor->GetRaceID(), m_VictoryObserver.GetVictoryRace(), (int)m_VictoryObserver.GetVictoryType(), sImageName);
				pMajor->GetEmpire()->PushEvent(eventScreen);
			}
		}
	}

	// Bestimmte Views zurücksetzen
	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_ICON);
	m_pClientWorker->ResetViews();

}

/// Funktion berechnet, ob zufällig Alienschiffe ins Spiel kommen.
void CBotEDoc::CalcRandomAlienEntities()
{
	const CIniLoader* pIni = CIniLoader::GetInstance();
	float frequency = 0;
	AssertBotE(pIni->ReadValue("Special", "ALIENENTITIES", frequency));
	if (frequency == 0.0f)
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
			if (!pAlien->IsAlien())
			{
				AssertBotE(false);
				continue;
			}

			// Keine Alien-Kampfstation erneut bauen
			if (pAlien->GetRaceID() == StrToCStr(KAMPFSTATION))
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
			// Hiermit kann man leicht die Wahrscheinlichkeit steuern,
			// aktuell 5mal niedriger!
			float fSteuerParameter = (CSettingsDlg::max_alien_frequency - frequency)
				/ frequency;
			if(fSteuerParameter != 0.0f)
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
				if (!GetSystem(p.x, p.y).GetAnomaly())
				{
					CShipMap::iterator pShip = BuildShip(pShipInfo->GetID(), p, pAlien->GetRaceID());
					// unterschiedliche Aliens unterschieden und Schiffseigenschaften festlegen
					if (pAlien->GetRaceID() == StrToCStr(IONISIERENDES_GASWESEN))
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == StrToCStr(GABALLIANER_SEUCHENSCHIFF))
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == StrToCStr(BLIZZARD_PLASMAWESEN))
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == StrToCStr(MORLOCK_RAIDER))
					{
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						// zufällig gleich mehrere Raider bauen. Umso höher der technische Durchschnitt
						// in der Galaxie ist, desto mehr Raider kommen auf dem System ins Spiel.
						if (nMod > 0)
						{
							int nCount = rand()%(nMod + 1);
							while (nCount > 0)
							{
								MYTRACE("general")(MT::LEVEL_DEBUG, "new MORLOCK RAIDER is ingame\n");
								// Erst das Schiff bauen
								CShipMap::iterator pFleetShip = BuildShip(pShipInfo->GetID(), p,
									pAlien->GetRaceID());

								// Raider in Gruppe stecken und Befehle gleich mit übernehmen
								pShip->second->AddShipToFleet(pFleetShip->second);
								m_ShipMap.EraseAt(pFleetShip);

								nCount--;
							}
							MYTRACE("general")(MT::LEVEL_DEBUG, "new MORLOCK RAIDER: Amount: %d\n", nCount);
						}
					}
					else if (pAlien->GetRaceID() == StrToCStr(BOSEANER))
					{
						MYTRACE("general")(MT::LEVEL_DEBUG, "new BOSEAN is ingame\n");
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == StrToCStr(KRYONITWESEN))
					{
						MYTRACE("general")(MT::LEVEL_DEBUG, "new Kryonit Entity is ingame\n");
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					}
					else if (pAlien->GetRaceID() == StrToCStr(MIDWAY_ZEITREISENDE))
					{
						MYTRACE("general")(MT::LEVEL_DEBUG, "new MIDWAY BATTLESHIP is ingame\n");
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
					else if (pAlien->GetRaceID() == StrToCStr(ANAEROBE_MAKROBE))
					{
						MYTRACE("general")(MT::LEVEL_DEBUG, "new ANAEROBE_MAKROBE is ingame\n");
						pShip->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						// zufällig gleich mehrere Anaerobe Makroben bauen. Umso höher der technische Durchschnitt
						// in der Galaxie ist, desto mehr Anaerobe Makroben kommen auf dem System ins Spiel.
						if (nMod > 0)
						{
							// viele Anaerobe Makroben bauen, (daher hier mal 2)
							int nCount = rand()%(nMod * 2 + 1);
							while (nCount > 0)
							{
								MYTRACE("general")(MT::LEVEL_DEBUG, "new ANAEROBE_MAKROBE: Amount: %d\n", nCount);
								// Erst das Schiff bauen
								CShipMap::iterator pFleetShip = BuildShip(pShipInfo->GetID(), p,
									pAlien->GetRaceID());

								// Anaerobe Makroben in Gruppe stecken und Befehle gleich mit übernehmen
								pShip->second->AddShipToFleet(pFleetShip->second);
								m_ShipMap.EraseAt(pFleetShip);

								nCount--;
							}
						}
					}
					else if (pAlien->GetRaceID() == StrToCStr(ISOTOPOSPHAERISCHES_WESEN))
					{
						MYTRACE("general")(MT::LEVEL_DEBUG, "new Isotopospheric Entity is ingame\n");
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
void CBotEDoc::CalcAlienShipEffects()
{
	bool bBattleStationIngame = false;	// merkt sich ob die Kampfstation noch im Spiel ist

	for (CShipMap::const_iterator ship = m_ShipMap.begin(); ship != m_ShipMap.end(); ++ship)
	{
		if (!ship->second->IsAlien())
			continue;

		const boost::shared_ptr<CAlien> pAlien = boost::dynamic_pointer_cast<CAlien>(ship->second->Owner());
		if (!pAlien || !pAlien->IsAlien())
		{
			AssertBotE(FALSE);
			continue;
		}

		const CPoint& co = ship->second->GetCo();
		CSystem& system = GetSystem(co.x, co.y);
		const boost::shared_ptr<CMajor> pOwner = boost::dynamic_pointer_cast<CMajor>(system.Owner());

		// verschiedene Alienrassen unterscheiden
		if (pAlien->GetRaceID() == StrToCStr(IONISIERENDES_GASWESEN))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			if(!system.Majorized())
				continue;

			AssertBotE(pOwner);

			// Energie im System auf 0 setzen
			system.SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (system.GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CLoc::GetString("EVENT_IONISIERENDES_GASWESEN", FALSE, system.GetName());
				CEmpireNews message;
				message.CreateNews(s, EMPIRE_NEWS_TYPE::SOMETHING, system.GetName(), co);
				pOwner->GetEmpire()->AddMsg(message);
				if (pOwner->IsHumanPlayer())
				{
					boost::shared_ptr<CEventAlienEntity> eventScreen = boost::make_shared<CEventAlienEntity>(
						pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetName(), s);
					pOwner->GetEmpire()->PushEvent(eventScreen);

					m_pClientWorker->SetToEmpireViewFor(*pOwner);
				}
			}
		}
		else if (pAlien->GetRaceID() == StrToCStr(GABALLIANER_SEUCHENSCHIFF))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			if (system.Majorized())
			{
				// Nahrung im System auf 0 setzen
				system.SetDisabledProduction(WORKER::FOOD_WORKER);
				system.SetFoodStore(system.GetFoodStore() / 2);

				// Wenn Nahrung produziert oder vorhanden ist, dann die Nachricht bringen über Nahrung verseucht
				if (system.GetProduction()->GetMaxFoodProd() > 0 || system.GetFoodStore() > 0)
				{
					// Nachricht und Event einfügen
					CString s = CLoc::GetString("EVENT_GABALLIANER_SEUCHENSCHIFF", FALSE, system.GetName());
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::SOMETHING, system.GetName(), co);
					pOwner->GetEmpire()->AddMsg(message);
					if (pOwner->IsHumanPlayer())
					{
						const boost::shared_ptr<CEventAlienEntity> eventScreen = boost::make_shared<CEventAlienEntity>
							(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetName(), s);
						pOwner->GetEmpire()->PushEvent(eventScreen);

						m_pClientWorker->SetToEmpireViewFor(*pOwner);
					}
				}
			}

			// befinden sich Schiffe in diesem Sektor, so werden diese ebenfalls zu Seuchenschiffen (50%)
			if (system.GetIsShipInSector() && rand()%2 == 0)
			{
				// alle Schiffe im Sektor zu Seuchenschiffen machen
				for(CShipMap::iterator y = m_ShipMap.begin(); y != m_ShipMap.end(); ++y)
				{
					// Schiff im gleichen Sektor?
					// keine anderen Alienschiffe
					// keine Außenposten und Sternenbasen
					if (y->second->GetCo() != co || y->second->IsAlien() || y->second->IsStation())
						continue;
					std::vector<boost::shared_ptr<CShips>> vShips;
					vShips.push_back(y->second);
					vShips.reserve(y->second->GetFleetSize());
					for(CShips::iterator i = y->second->begin(); i != y->second->end(); ++i)
						vShips.push_back(i->second);

					for(std::vector<boost::shared_ptr<CShips>>::iterator i = vShips.begin(); i != vShips.end(); ++i)
					{
						const boost::shared_ptr<CShips>& pShip = *i;
						// Schiffe mit Rückzugsbefehl werden nie vom Virus befallen
						if (pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
							continue;

						CRace* race = pShip->Owner().get();
						if(race->IsMinor())
							continue;
						CMajor* pShipOwner = dynamic_cast<CMajor*>(race);
						AssertBotE(pShipOwner);

						pShip->SetShipType(SHIP_TYPE::ALIEN);
						pShip->SetTargetKO(CPoint(-1, -1));
						pShip->UnsetCurrentOrder();
						pShip->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
						pShip->SetIsShipFlagShip(FALSE);

						// für jedes Schiff eine Meldung über den Verlust machen
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						pShipOwner->AddToLostShipHistory(pShip->ShipHistoryInfo(), CLoc::GetString("COMBAT"), CLoc::GetString("MISSED"), m_iRound, pShip->GetCo());
						CString s;
						s.Format("%s", CLoc::GetString("DESTROYED_SHIPS_IN_COMBAT",0,pShip->GetName()));
						CEmpireNews message;
						message.CreateNews(s, EMPIRE_NEWS_TYPE::MILITARY, "", pShip->GetCo());
						pShipOwner->GetEmpire()->AddMsg(message);
						//actually change the owner last, to make the above calls work correctly
						pShip->SetOwner(pAlien->GetRaceID());
					}
				}
			}
		}
		else if (pAlien->GetRaceID() == StrToCStr(BLIZZARD_PLASMAWESEN))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			if(!system.Majorized())
				continue;

			// Energie im System auf 0 setzen
			system.SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (system.GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CLoc::GetString("EVENT_BLIZZARD_PLASMAWESEN", FALSE, system.GetName());
				CEmpireNews message;
				message.CreateNews(s, EMPIRE_NEWS_TYPE::SOMETHING, system.GetName(), co);
				pOwner->GetEmpire()->AddMsg(message);
				if (pOwner->IsHumanPlayer())
				{
					const boost::shared_ptr<CEventAlienEntity> eventScreen = boost::make_shared<CEventAlienEntity>
						(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetName(), s);
					pOwner->GetEmpire()->PushEvent(eventScreen);

					m_pClientWorker->SetToEmpireViewFor(*pOwner);
				}
			}
		}
		else if (pAlien->GetRaceID() == StrToCStr(MORLOCK_RAIDER))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			if(!system.Majorized())
				continue;

			// existiert keine Freundschaft zum Major
			if (pAlien->GetAgreement(pOwner->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				continue;

			// alte Credits merken und aktuell auf 0 stellen
			short nCreditProd = system.GetProduction()->GetCreditsProd();
			// Nichts machen wenn keine Credits im System produziert werden, z.B. durch einen
			// vorherigen Raider.
			if (nCreditProd <= 0)
				continue;

			system.GetProduction()->DisableCreditsProduction();

			// Nachricht und Event einfügen
			CString sCredits = "";
			sCredits.Format("%d", nCreditProd);
			CString s = CLoc::GetString("EVENT_MORLOCK_RAIDER", FALSE, sCredits, system.GetName());
			CEmpireNews message;
			message.CreateNews(s, EMPIRE_NEWS_TYPE::SOMETHING, system.GetName(), co);
			pOwner->GetEmpire()->AddMsg(message);
			if (pOwner->IsHumanPlayer())
			{
				const boost::shared_ptr<CEventAlienEntity> eventScreen = boost::make_shared<CEventAlienEntity>
					(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetName(), s);
				pOwner->GetEmpire()->PushEvent(eventScreen);

				m_pClientWorker->SetToEmpireViewFor(*pOwner);
			}
		}
		else if (pAlien->GetRaceID() == StrToCStr(BOSEANER))
		{
			// zufällig die anfangs auf Meiden gestellten Boseaner auf Angreifen stellen
			// Sie sind auf der Suche nach Nahrung ;-)
			if (rand()%10 == 0)
				ship->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
		}
		else if (pAlien->GetRaceID() == StrToCStr(KAMPFSTATION))
		{
			bBattleStationIngame = true;
		}
		else if (pAlien->GetRaceID() == StrToCStr(MIDWAY_ZEITREISENDE))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			// Befindet sich das Midway Schlachtschiff über einem System von einem Major,
			// so wird dieses im Kriegsfall bombardiert. Ansonsten verringert sich die Beziehung mit
			// jeder Runde. Umso länger das Midway-Schlachtschiff über einem System steht, umso
			// wahrscheinlicher ist es, dass Krieg erklärt wird (nur den Midways helfen indem man Credits
			// übergibt kann dagegen helfen).
			if (system.Majorized())
			{
				if (pOwner->GetAgreement(pAlien->GetRaceID()) != DIPLOMATIC_AGREEMENT::WAR)
				{
					pAlien->SetRelation(pOwner->GetRaceID(), -rand()%20);
				}
				else
				{
					// In the case of war, we set current order ATTACK_SYSTEM in CShipAI
				}
			}
		}
		else if (pAlien->GetRaceID() == StrToCStr(ISOTOPOSPHAERISCHES_WESEN))
		{
			// Aliens mit Rückzugsbefehl machen nix
			if (ship->second->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;

			// Zu 25% teleportiert sich das Isotoposphärische Wesen an einen zufälligen Ort der Galaxie (nicht auf Anomalien).
			// Befinden sich Schiffe im selben Sektor, so werden diese mitgerissen.
			if (rand()%4 == 0)
			{
				// irgend ein zufülliges neues Ziel generieren, welches nicht auf einer Anomalie endet
				while (true)
				{
					CPoint ptTarget = CPoint(rand()%STARMAP_SECTORS_HCOUNT, rand()%STARMAP_SECTORS_VCOUNT);
					if (ptTarget == ship->second->GetCo())
						continue;

					if (GetSystem(ptTarget.x, ptTarget.y).GetAnomaly())
						continue;

					// Alle Schiffe in diesem Sektor werden mit teleportiert (außer andere Aliens)
					for (CShipMap::const_iterator shipInSector = m_ShipMap.begin(); shipInSector != m_ShipMap.end(); ++shipInSector)
					{
						if (shipInSector->second->GetCo() != ship->second->GetCo())
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
			if (system.Majorized())
			{
				// Energie im System auf 0 setzen
				system.SetDisabledProduction(WORKER::ENERGY_WORKER);

				// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
				if (system.GetProduction()->GetMaxEnergyProd() > 0)
				{
					// Nachricht und Event einfügen
					CString s = CLoc::GetString("EVENT_ISOTOPOSPHAERISCHES_WESEN", FALSE, system.GetName());
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::SOMETHING, system.GetName(), co);
					pOwner->GetEmpire()->AddMsg(message);
					if (pOwner->IsHumanPlayer())
					{
						boost::shared_ptr<CEventAlienEntity> eventScreen = boost::make_shared<CEventAlienEntity>
							(pOwner->GetRaceID(), pAlien->GetRaceID(), pAlien->GetName(), s);
						pOwner->GetEmpire()->PushEvent(eventScreen);

						m_pClientWorker->SetToEmpireViewFor(*pOwner);
					}
				}
			}
		}
	}

	// Wurde die Kampfstation vernichtet und die Alienrasse "Kampfstation" gibt es noch, dann Nachticht
	// über deren Vernichtung erstellen
	if (!bBattleStationIngame)
	{
		const boost::shared_ptr<CMinor>& pBattleStation =
			boost::dynamic_pointer_cast<CMinor>(m_pRaceCtrl->GetRaceSafe(StrToCStr(KAMPFSTATION)));
		if (pBattleStation)
		{
			CalcEffectsMinorEleminated(pBattleStation);
			m_pRaceCtrl->RemoveRace(pBattleStation->GetRaceID());
		}
	}
}

void CBotEDoc::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: Fügen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->Enable(FALSE);
}

void CBotEDoc::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Fügen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->Enable(FALSE);
}

BOOL CBotEDoc::OnOpenDocument(LPCTSTR lpszPathName)
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

BOOL CBotEDoc::OnSaveDocument(LPCTSTR lpszPathName)
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

void CBotEDoc::AllocateSystems()
{
	STARMAP_TOTALWIDTH = STARMAP_SECTORS_HCOUNT * STARMAP_SECTOR_WIDTH;
	STARMAP_TOTALHEIGHT = STARMAP_SECTORS_VCOUNT * STARMAP_SECTOR_HEIGHT;

	const unsigned size = STARMAP_SECTORS_HCOUNT*STARMAP_SECTORS_VCOUNT;
	if(m_Systems.size() == size)
	{
		for(std::vector<CSystem>::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
			it->Reset(true);
		return;
	}
	m_Systems.clear();
	m_Systems.reserve(size);
	for(int y = 0; y < STARMAP_SECTORS_VCOUNT; ++y)
		for(int x = 0; x < STARMAP_SECTORS_HCOUNT; ++x)
			m_Systems.push_back(CSystem(x, y));

	CStarmap::AllocateStatics();
}

void CBotEDoc::RandomSeed(const int* OnlyIfDifferentThan) {
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
