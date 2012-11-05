// botf2Doc.cpp : Implementierung der Klasse CBotf2Doc
//

#include "stdafx.h"
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

#include "Races\RaceController.h"
#include "Races\DiplomacyController.h"
#include "Ships\Combat.h"
#include "Ships\Fleet.h"
#include "System\AttackSystem.h"
#include "Intel\IntelCalc.h"
#include "Sanity.h"

#include "AI\AIPrios.h"
#include "AI\SectorAI.h"
#include "AI\SystemAI.h"
#include "AI\ShipAI.h"
#include "AI\CombatAI.h"
#include "AI\ResearchAI.h"

#include "Galaxy\Anomaly.h"

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
	m_NumberOfTheShipInArray(-1),
	m_bCombatCalc(false),
	m_bDataReceived(false),
	m_bDontExit(false),
	m_bGameLoaded(false),
	m_bGameOver(false),
	m_bNewGame(true),
	m_bRoundEndPressed(false),
	m_fDifficultyLevel(1.0f),
	m_fStardate(121000.0f),
	m_iNumberOfFleetShip(-1),
	m_iNumberOfTheShipInFleet(-1),
	m_iRound(1),
	m_iSelectedView(),
	m_iShowWhichShipInfoInView3(0),
	m_iShowWhichTechInView3(0)
{
	//Init MT with single log file
	CString sLogPath = CIOData::GetInstance()->GetLogPath();
	const CCommandLineParameters* const clp = dynamic_cast<CBotf2App*>(AfxGetApp())->GetCommandLineParameters();
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

	m_ShipArray.RemoveAll();
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
	m_NumberOfTheShipInArray	= -1;
	m_iNumberOfFleetShip		= -1;
	m_iNumberOfTheShipInFleet	= -1;
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
		ar << m_ShipArray.GetSize();
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			m_ShipArray.GetAt(i).Serialize(ar);

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
		ar >> number;
		m_ShipArray.RemoveAll();
		m_ShipArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipArray.GetAt(i).Serialize(ar);
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
			int nCount = 0;
			for (int i = 0; i < m_ShipArray.GetSize(); i++)
				if (m_ShipArray[i].GetKO() == m_ptCurrentCombatSector)
					nCount++;
			ar << nCount;
			// nur Schiffe aus diesem Sektor senden
			for (int i = 0; i < m_ShipArray.GetSize(); i++)
				if (m_ShipArray[i].GetKO() == m_ptCurrentCombatSector)
					m_ShipArray.GetAt(i).Serialize(ar);
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
		ar << m_ShipArray.GetSize();
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			m_ShipArray.GetAt(i).Serialize(ar);

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
			// Es werden nur Schiffe aus dem aktuellen Kampfsektor empfangen
			int nCount;
			ar >> nCount;
			// alle Schiffe aus dem Kampfsektor entfernen
			for (int i = 0; i < m_ShipArray.GetSize(); i++)
				if (m_ShipArray.GetAt(i).GetKO() == m_ptCurrentCombatSector)
					m_ShipArray.RemoveAt(i--);
			int nSize = m_ShipArray.GetSize();
			// empfangene Schiffe wieder hinzufügen
			m_ShipArray.SetSize(nSize + nCount);
			for (int i = nSize; i < m_ShipArray.GetSize(); i++)
				m_ShipArray.GetAt(i).Serialize(ar);

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
		ar >> number;
		m_ShipArray.RemoveAll();
		m_ShipArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ShipArray.GetAt(i).Serialize(ar);
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
		for (int y = 0; y < m_ShipArray.GetSize(); y++) {
			CShip& ship = m_ShipArray.GetAt(y);
			if (ship.GetOwnerOfShip() == pPlayer->GetRaceID()) {
				const CArray<Sector>& path = *ship.GetPath();
				for (int i = 0; i < path.GetSize(); i++)
					GetSector(path.GetAt(i)).AddShipPathPoints(1);
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

		vector<int> vShips;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == pPlayer->GetRaceID())
				vShips.push_back(i);
		ar << vShips.size();
		for (size_t i = 0; i < vShips.size(); i++)
			m_ShipArray.GetAt(vShips[i]).Serialize(ar);

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
			GetSystem(vSystems[i]).Serialize(ar);
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

		int number = 0;
		ar >> number;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == sMajorID)
				m_ShipArray.RemoveAt(i--);
		int oldSize = m_ShipArray.GetSize();
		m_ShipArray.SetSize(oldSize + number);
		for (int i = oldSize; i < m_ShipArray.GetSize(); i++)
		{
			m_ShipArray.GetAt(i).Serialize(ar);
			ASSERT(m_ShipArray.GetAt(i).GetOwnerOfShip() == sMajorID);
		}

		number = 0;
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

	if (GetMainFrame()->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW)
		GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
}

void CBotf2Doc::SetCurrentShipIndex(int NumberOfTheShipInArray)
{
	m_NumberOfTheShipInArray = NumberOfTheShipInArray;
	((CGalaxyMenuView*)GetMainFrame()->GetView(RUNTIME_CLASS(CGalaxyMenuView)))->SetNewShipPath();
	CSmallInfoView::SetShip(&m_ShipArray.GetAt(NumberOfTheShipInArray));
	//re-activation later...MYTRACE("general")(MT::LEVEL_INFO, "m_NumberOfTheShipInArray: %i", m_NumberOfTheShipInArray);
}

void CBotf2Doc::SetNumberOfFleetShip(int NumberOfFleetShip)
{
	m_iNumberOfFleetShip = NumberOfFleetShip;
	CSmallInfoView::SetShip(&m_ShipArray.GetAt(NumberOfFleetShip));
	//re-activation later...MYTRACE("general")(MT::LEVEL_INFO, "m_iNumberOfFleetShip: %i", m_iNumberOfFleetShip);

}

void CBotf2Doc::SetNumberOfTheShipInFleet(int NumberOfTheShipInFleet)
{
	m_iNumberOfTheShipInFleet = NumberOfTheShipInFleet;
	if (NumberOfTheShipInFleet > 0)
		CSmallInfoView::SetShip(m_ShipArray.GetAt(m_iNumberOfFleetShip).GetFleet()->GetShipFromFleet(NumberOfTheShipInFleet - 1));
	else if (NumberOfTheShipInFleet == 0)
		CSmallInfoView::SetShip(&m_ShipArray.GetAt(m_iNumberOfFleetShip));
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
	std::map<CWnd *, UINT>* views = &GetMainFrame()->GetSplitterWindow()->views;
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
		GetMainFrame()->SelectMainView(GALAXY_VIEW, pPlayersRace->GetRaceID());
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
	std::map<CWnd *, UINT>* views = &GetMainFrame()->GetSplitterWindow()->views;
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
		GetMainFrame()->FullScreenMainView(true);
		GetMainFrame()->SelectMainView(EVENT_VIEW, pPlayersRace->GetRaceID());
	}
	else
	{
		GetMainFrame()->FullScreenMainView(false);
		GetMainFrame()->SelectMainView(m_iSelectedView[client], pPlayersRace->GetRaceID());
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
		const CCommandLineParameters* const clp = dynamic_cast<CBotf2App*>(AfxGetApp())->GetCommandLineParameters();
		if(clp->SeeAllOfMap()) {
			map<CString, CRace*>* pmRaces = m_pRaceCtrl->GetRaces();
			for (map<CString, CRace*>::iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
				for (map<CString, CRace*>::const_iterator jt = pmRaces->begin(); jt != pmRaces->end(); ++jt)
					if (it->first != jt->first && it->second->IsMajor() && jt->second->IsMajor())
						it->second->SetIsRaceContacted(jt->first, true);
		}

		m_iRound = 1;

		// Generierungssektornamenklasse wieder neu starten
		m_ShipArray.RemoveAll();
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
		CSector& sector = GetSector(raceKO);
		CSystem& system = GetSystem(raceKO);

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
				if (!GetSector(pt.y).GetSunSystem())
				{
					GetSector(pt).GenerateSector(100, nMinorDensity);
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
					GetSector(pt).SetScanned(it->first);
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
					if (GetSector(pt).GetSunSystem())
					{
						if (GetSector(pt).GetMinorRace())
							minorRaces++;
						sunSystems++;
					}
					else if (GetSector(pt).GetAnomaly())
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
static bool HumanPlayerInCombat(const CArray<CShip,CShip>& ships, const CPoint& CurrentCombatSector,
		const std::map<CString, CMajor*>& majors) {
	for(int i = 0; i < ships.GetSize(); ++i)
	{
		const CShip* pShip = &ships.GetAt(i);
		if (pShip->GetKO() != CurrentCombatSector)
			continue;
		const std::map<CString, CMajor*>::const_iterator major = majors.find(pShip->GetOwnerOfShip());
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
			if (!HumanPlayerInCombat(m_ShipArray, m_ptCurrentCombatSector, *pmMajors))
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
			if (!HumanPlayerInCombat(m_ShipArray, m_ptCurrentCombatSector, *pmMajors))
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
			if (pMinor->GetAgreement(it->first) == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			{
				bMember = true;
				break;
			}
		}
		if (bMember)
			continue;

		CPoint ko = pMinor->GetRaceKO();

		if (ko != CPoint(-1,-1) && GetSystem(ko.x, ko.y).GetOwnerOfSystem() == "" && GetSector(ko).GetOwnerOfSector() == pMinor->GetRaceID())
		{
			// Vielleicht kolonisiert die Minorrace weitere Planeten in ihrem System
			if (pMinor->PerhapsExtend(this))
				// dann sind im System auch weitere Einwohner hinzugekommen
				GetSystem(ko.x, ko.y).SetHabitants(this->GetSector(ko).GetCurrentHabitants());

			// Den Verbrauch der Rohstoffe der kleinen Rassen in jeder Runde berechnen
			pMinor->ConsumeResources(this);
			// Vielleicht baut die Rasse ein Raumschiff
			pMinor->PerhapsBuildShip(this);
		}
	}
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)//Random Events berechenn
	{
		CMajor* pMajor = it->second;
		m_RandomEventManager.CalcEvents(pMajor);
	};

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
				if (ShipInfo.GetRace() == UNKNOWN)
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
				ShipInfo.SetStealthPower(atoi(data[32]));
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
	GetSystem(KO).AddNewBuilding(building);
}

void CBotf2Doc::BuildShip(int nID, const CPoint& KO, const CString& sOwnerID)
{
	CRace* pOwner = m_pRaceCtrl->GetRace(sOwnerID);
	if (!pOwner)
	{
		AfxMessageBox("Error in BuildShip(): RaceID " + sOwnerID + " doesn't exist!");
		return;
	}

	ASSERT(nID >= 10000);
	nID -= 10000;

	// unbedingt Kopie machen, da die Refernz verlorengeht, sobald ein neues Schiff hinzugefügt wurde
	CString sOwner = sOwnerID;
	m_ShipArray.Add((CShip)m_ShipInfoArray.GetAt(nID));
	int n = m_ShipArray.GetUpperBound();
	m_ShipArray[n].SetOwnerOfShip(sOwner);
	m_ShipArray[n].SetKO(KO);

	// Schiffsnamen vergeben
	m_ShipArray.ElementAt(n).SetShipName(m_GenShipName.GenerateShipName(sOwner, m_ShipArray.ElementAt(n).IsStation()));

	// den Rest nur machen, wenn das Schiff durch eine Majorrace gebaut wurde
	if (!pOwner->IsMajor())
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
	ASSERT(pMajor);

	// Spezialforschungsboni dem Schiff hinzufügen
	AddSpecialResearchBoniToShip(&m_ShipArray[n], pMajor);

	for (int i = 0; i < 4; i++)
		m_ShipArray.ElementAt(n).SetTargetKO(CPoint(-1,-1), i, true);
	m_ShipArray.ElementAt(n).SetTerraformingPlanet(-1);

	pMajor->GetShipHistory()->AddShip(&m_ShipArray.GetAt(n), GetSector(KO.x, KO.y).GetName(), m_iRound);
}

void CBotf2Doc::RemoveShip(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_ShipArray.GetSize());

	CShip* pShip = &m_ShipArray[nIndex];
	if (pShip->GetFleet())
	{
		// alten Befehl merken
		SHIP_ORDER::Typ nOldOrder = pShip->GetCurrentOrder();
		// Kopie der Flotte holen
		CFleet* pFleetCopy = pShip->GetFleet();
		// erstes Schiff aus der Flotte holen
		CShip* pFleetShip = pFleetCopy->GetShipFromFleet(0);
		// für dieses eine Flotte erstellen
		pFleetShip->CreateFleet();
		for (USHORT i = 1; i < pFleetCopy->GetFleetSize(); i++)
			pFleetShip->GetFleet()->AddShipToFleet(pFleetCopy->GetShipFromFleet(i));
		pFleetShip->CheckFleet();
		// alten Befehl übergeben
		pFleetShip->SetCurrentOrder(nOldOrder);

		m_ShipArray.Add(*pFleetShip);
		// Schiff nochmal neu holen, da der Vektor verändert wurde und so sich auch der Zeiger ändern kann
		pShip = &m_ShipArray[nIndex];
		// Flotte löschen
		pShip->DeleteFleet();
	}
	m_ShipArray.RemoveAt(nIndex);
}

/// Funktion beachtet die erforschten Spezialforschungen einer Rasse und verbessert die
/// Eigenschaften der übergebenen Schiffes.
/// @param pShip Schiff welches durch Spezialforschungen eventuell verbessert wird
/// @param pShipOwner Zeiger auf den Besitzer des Schiffes
void CBotf2Doc::AddSpecialResearchBoniToShip(CShip* pShip, CMajor* pShipOwner) const
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
			if (pShip->GetRange() == SHIP_RANGE::SHORT)
				pShip->SetRange((SHIP_RANGE::Typ)(pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(1)));
		}
		// erhoehte Geschwindigkeit für Schiffe mit Geschwindigkeit 1
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			if (pShip->GetSpeed() == 1)
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

void CBotf2Doc::AddToLostShipHistory(const CShip* pShip, const CString& sEvent, const CString& sStatus)
{
	CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip()));
	if (pMajor)
	{
		pMajor->GetShipHistory()->ModifyShip(pShip,	GetSector(pShip->GetKO()).GetName(TRUE), m_iRound, sEvent, sStatus);
	}
}

/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
void CBotf2Doc::BuildTroop(BYTE ID, CPoint ko)
{
	// Mal Testweise paar Truppen anlegen
	GetSystem(ko.x, ko.y).AddTroop((CTroop*)&m_TroopInfo.GetAt(ID));
	CString sRace = GetSector(ko).GetOwnerOfSector();
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
		if (!pMajor->IsHumanPlayer())
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
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
		if (m_ShipArray.GetAt(y).GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			BOOLEAN okay = TRUE;
			// Checken dass in diesem System nicht schon ein Angriff durchgeführt wurde
			for (int x = 0; x < fightInSystem.GetSize(); x++)
				if (fightInSystem.GetAt(x) == m_ShipArray.GetAt(y).GetKO())
				{
					okay = FALSE;
					break;
				}

			// nur wenn das Schiff und Schiffe in der Flotte ungetarnt sind
			if (m_ShipArray[y].GetCloak() == TRUE || (m_ShipArray.GetAt(y).GetFleet() != 0 && m_ShipArray.GetAt(y).GetFleet()->CheckOrder(&m_ShipArray.GetAt(y), SHIP_ORDER::ATTACK_SYSTEM) == FALSE))
			{
				m_ShipArray.ElementAt(y).SetCurrentOrder(SHIP_ORDER::ATTACK);
				okay = FALSE;
			}

			// wenn in dem System noch kein Angriff durchgeführt wurde kann angegriffen werden
			if (okay)
			{
				CPoint p = m_ShipArray.GetAt(y).GetKO();
				// Besitzer des Systems (hier Sector wegen Minors) vor dem Systemangriff
				CString sDefender = GetSector(p.x, p.y).GetOwnerOfSector();
				// Angreifer bzw. neuer Besitzer des Systems nach dem Angriff
				set<CString> attackers;
				for (int i = 0; i < m_ShipArray.GetSize(); i++)
					if (m_ShipArray.GetAt(i).GetKO() == p && m_ShipArray.GetAt(i).GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
					{
						CString sOwner = m_ShipArray.GetAt(i).GetOwnerOfShip();
						if (!sOwner.IsEmpty() && m_pRaceCtrl->GetRace(sOwner)->IsMajor())
							attackers.insert(sOwner);
					}

				CAttackSystem* attackSystem = new CAttackSystem();

				CRace* defender = NULL;
				if (!sDefender.IsEmpty())
					defender = m_pRaceCtrl->GetRace(sDefender);
				// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
				if (defender && defender->IsMinor() && GetSector(p.x, p.y).GetTakenSector() == FALSE)
				{
					attackSystem->Init(defender, &GetSystem(p.x, p.y), &m_ShipArray, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn eine Majorrace in dem System lebt
				else if (defender && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
				{
					attackSystem->Init(defender, &GetSystem(p.x, p.y), &m_ShipArray, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn niemand mehr in dem System lebt, z.B. durch Rebellion
				else
				{
					attackSystem->Init(NULL, &GetSystem(p.x, p.y), &m_ShipArray, &GetSector(p.x, p.y), &this->BuildingInfo, CTrade::GetMonopolOwner());
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
						// Die Beziehung zur der Majorrace, die das System vorher besaß verschlechtert sich
						defender->SetRelation(attacker, -rand()%50);
						// Die Beziehung zu der Minorrace verbessert sich auf Seiten des Retters
						CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(p.x, p.y).GetName());
						ASSERT(pMinor);
						pMinor->SetRelation(attacker, rand()%50);
						pMinor->SetSubjugated(false);
						// Eventnachricht an den, der das System verloren hat (erobertes Minorracesystem wieder verloren)
						CString param = GetSector(p.x, p.y).GetName();
						CString eventText = "";

						CMajor* def = dynamic_cast<CMajor*>(defender);
						eventText = def->GetMoralObserver()->AddEvent(17, def->GetRaceMoralNumber(), param);
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
							message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
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
							CString eventText = "";
							eventText = pMajor->GetMoralObserver()->AddEvent(14, pMajor->GetRaceMoralNumber(), param);
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
							CString eventText = "";
							eventText = pDefenderMajor->GetMoralObserver()->AddEvent(15, pDefenderMajor->GetRaceMoralNumber(), param);
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
							eventText = "";
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

							CString eventText = "";
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
							eventText = "";
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
					CString param = GetSector(p.x, p.y).GetName();
					CString eventText = "";
					for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
					{
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
						ASSERT(pMajor);

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
							CMinor* pMinor = m_pRaceCtrl->GetMinorRace(GetSector(p.x, p.y).GetName());
							ASSERT(pMinor);
							GetSector(p.x, p.y).SetMinorRace(FALSE);

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

								// An alle Majors die die Minor kennen die Nachricht schicken, dass diese vernichtet wurde
								// Eventnachricht: #21	Eliminate a Minor Race Entirely
								if (attackers.find(it->first) != attackers.end())
								{
									CString param = pMinor->GetRaceName();
									CString eventText = it->second->GetMoralObserver()->AddEvent(21, it->second->GetRaceMoralNumber(), param);
									CMessage message;
									message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, param, p, 0);
									it->second->GetEmpire()->AddMessage(message);
								}

								// alle anderen Majorrassen, die diese Minor kannten, bekommen eine Nachricht über deren Vernichtung
								if (pMinor->IsRaceContacted(it->first))
								{
									CString news = CResourceManager::GetString("ELIMINATE_MINOR", FALSE, pMinor->GetRaceName());
									CMessage message;
									message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, "", 0, 0);
									it->second->GetEmpire()->AddMessage(message);
									if (it->second->IsHumanPlayer())
									{
										// Event über die Rassenauslöschung einfügen
										CEventRaceKilled* eventScreen = new CEventRaceKilled(it->first, pMinor->GetRaceID(), pMinor->GetRaceName(), pMinor->GetGraphicFileName());
										it->second->GetEmpire()->GetEventMessages()->Add(eventScreen);

										network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
								// alle Majors durchgehen und die vernichtete Minor aus deren Maps entfernen
								CMajor* pMajor = it->second;
								pMajor->SetIsRaceContacted(pMinor->GetRaceID(), false);
								pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
							}
							// Alle Schiffe der Minorrace entfernen
							for (int j = 0; j < m_ShipArray.GetSize(); j++)
								if (m_ShipArray.GetAt(j).GetOwnerOfShip() == pMinor->GetRaceID())
									m_ShipArray.RemoveAt(j--);

							// Rasse zum löschen vormerken
							sKilledMinors.insert(pMinor->GetRaceID());
						}
						// Bei einer Majorrace verringert sich nur die Anzahl der Systeme (auch konnte dies das
						// Minorracesystem von oben gewesen sein, hier verliert es aber die betroffene Majorrace)
						if (defender != NULL && defender->IsMajor() && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
						{
							CMajor* pDefenderMajor = dynamic_cast<CMajor*>(defender);
							eventText = "";
							if (GetSector(p.x, p.y).GetName() == pDefenderMajor->GetHomesystemName())
							{
								// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
								param = GetSector(p.x, p.y).GetName();
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
								ASSERT(pMajor);

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
							for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
								ASSERT(pMajor);

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
								eventText = pDefenderMajor->GetMoralObserver()->AddEvent(22, pDefenderMajor->GetRaceMoralNumber(), param);
								if (pDefenderMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
					}
					// Eventgrafiken hinzufügen
					// für den/die Angreifer
					for (set<CString>::const_iterator it = attackers.begin(); it != attackers.end(); ++it)
					{
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(*it));
						ASSERT(pMajor);

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
						ASSERT(pMajor);

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

	// alle vernichteten Minorraces nun aus dem Feld löschen
	for (set<CString>::const_iterator it = sKilledMinors.begin(); it != sKilledMinors.end(); ++it)
		m_pRaceCtrl->RemoveRace(*it);

	// Schiffsfeld nochmal durchgehen und alle Schiffe ohne Hülle aus dem Feld entfernen.
	// Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte.
	if (fightInSystem.GetSize() > 0)
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
		{
			// Wenn das Schiff eine Flotte hatte, dann erstmal nur die Schiffe in der Flotte beachten
			// Wenn davon welche zerstört wurden diese aus der Flotte nehmen
			if (m_ShipArray.GetAt(i).GetFleet())
			{
				for (int x = 0; x < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); x++)
					if (m_ShipArray.GetAt(i).GetFleet()->GetShipFromFleet(x)->GetHull()->GetCurrentHull() < 1)
					{
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						AddToLostShipHistory(m_ShipArray.GetAt(i).GetFleet()->GetShipFromFleet(x), CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
						// Schiff entfernen
						m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
					}
				m_ShipArray[i].CheckFleet();
			}
			// Wenn das Schiff selbst zerstört wurde
			if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
			{
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				AddToLostShipHistory(&m_ShipArray[i], CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
				// Schiff entfernen
				RemoveShip(i--);
			}
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
		CString *news = 0;
		news = pMajor->GetEmpire()->GetResearch()->CalculateResearch(pMajor->GetEmpire()->GetFP());
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

	for(std::vector<CSector>::iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector) {
		CSystem& system = GetSystemForSector(*sector);
		// Mögliche Is? Variablen für Terraforming und Stationbau erstmal auf FALSE setzen
		sector->ClearAllPoints();
		if(!sector->GetSunSystem()) continue;// Wenn im Sektor ein Sonnensystem existiert
		const CString& system_owner = system.GetOwnerOfSystem();
		if(system_owner == "")
			sector->LetPlanetsGrowth();// Planetenwachstum für andere Sektoren durchführen
		else {
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(system_owner));
			assert(pMajor);
			CEmpire* pEmpire = pMajor->GetEmpire();
			// Jetzt das produzierte Credits im System dem jeweiligen Imperium geben
			COldRoundDataCalculator::CreditsDestructionMoral(pMajor, system,
				this->BuildingInfo, m_fDifficultyLevel);

			// KI Anpassungen (KI bekommt zufälig etwas Deritium geschenkt)
			int diliAdd = COldRoundDataCalculator::DeritiumForTheAI(
				pMajor->IsHumanPlayer(), *sector, system, m_fDifficultyLevel);
			// Das Lager berechnen
			const BOOLEAN bIsRebellion = system.CalculateStorages(pEmpire->GetResearch()->GetResearchInfo(), diliAdd);
			// Wenn wir true zurückbekommen, dann hat sich das System losgesagt
			if (bIsRebellion)
				calc.ExecuteRebellion(*sector, system, pMajor);
			// Hier mit einbeziehen, wenn die Bevölkerung an Nahrungsmangel stirbt
			if (system.GetFoodStore() < 0)
				calc.ExecuteFamine(*sector, system, pMajor);
			 else
				// Planetenwachstum für Spielerrassen durchführen
				sector->LetPlanetsGrowth();
			calc.HandlePopulationEffects(*sector, system, pMajor);
			system.CalculateVariables(&this->BuildingInfo, pEmpire->GetResearch()->GetResearchInfo(), sector->GetPlanets(), pMajor, CTrade::GetMonopolOwner());

			// hier könnte die Energie durch Weltraummonster weggenommen werden!
			// Gebäude die Energie benötigen checken
			if (system.CheckEnergyBuildings(&this->BuildingInfo))
				calc.SystemMessage(*sector, pMajor, "BUILDING_TURN_OFF", MESSAGE_TYPE::SOMETHING, 2);
			// Die Bauaufträge in dem System berechnen. Außerdem wird hier auch die System-KI ausgeführt.
			if (!pMajor->IsHumanPlayer() || system.GetAutoBuild())
			{
				CSystemAI* SAI = new CSystemAI(this);
				SAI->ExecuteSystemAI(sector->GetKO());
				delete SAI;
			}
			calc.Build(*sector, system, pMajor, this->BuildingInfo);
			// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
			system.CalculateNumberOfWorkbuildings(&this->BuildingInfo);
			// freie Arbeiter den Gebäuden zuweisen
			system.SetWorkersIntoBuildings();
		}//if(system_owner == "")
		UpdateGlobalBuildings(system);
	}//for(std::vector<CSector>::const_iterator sector = m_Sectors.begin(); sector != m_Sectors.end(); ++sector)
}

void CBotf2Doc::PutScannedSquareOverSector(const CSector& sector, unsigned range, const int power,
		const CRace& race, bool bBetterScanner, bool patrolship, bool anomaly) {
	const CString& race_id = race.GetRaceID();
	float boni = 1.0f;
	// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet,
	// dann wird die Scanleistung um 20% erhöht.
	if(patrolship) {
		const CString& owner_of_sector = sector.GetOwnerOfSector();
		if(race_id == owner_of_sector || race.GetAgreement(owner_of_sector) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			boni = 1.2f;
	}
	if(bBetterScanner) {
		range *= 1.5;
		boni += 0.5;
	}
	const CPoint& co = sector.GetKO();
	const int intrange = static_cast<int>(range);
	for (int i = -intrange; i <= intrange; ++i) {
		const int x = co.x + i;
		if(0 <= x && x < STARMAP_SECTORS_HCOUNT) {
			for (int j = -intrange; j <= intrange; ++j) {
				const int y = co.y + j;
				if(0 <= y && y < STARMAP_SECTORS_VCOUNT) {
					CSector& scanned_sector = GetSector(x, y);
					// Teiler für die Scanstärke berechnen
					int div = max(abs(i), abs(j));
					if(anomaly)
						div *= 2;
					div = max(div, 1);
					const int old_scan_power = scanned_sector.GetScanPower(race_id, false);
					int new_scan_power = 0;
					if(anomaly) {
						new_scan_power = old_scan_power + power / div;
					} else {
						new_scan_power = (power * boni) / div;
						new_scan_power = max(old_scan_power, new_scan_power);
						if(race.IsMajor())
							scanned_sector.SetScanned(race_id);
					}
					scanned_sector.SetScanPower(new_scan_power, race_id);
				}//if(0 <= y && y < STARMAP_SECTORS_VCOUNT)
			}//for (int j = -range; j <= range; ++j)
		}//if(0 <= x && x < STARMAP_SECTORS_HCOUNT)
	}//for (int i = -range; i <= range; ++i)
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
	// Array beinhaltet die Aussenposten, welche nach dem Bau einer Sternbasis aus dem Feld verschwinden m?ssen.
	// Diese können nicht direkt in der Schleife entfernt werden, da sonst beim der nächsten Iteration die Schleife nicht mehr hinhaut.
	std::vector<CString> vRemoveableOutposts;

	// Hier kommt die Auswertung der Schiffsbefehle
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		CSanity::SanityCheckFleet(m_ShipArray.GetAt(y));

		// Hier wird überprüft, ob der Systemattack-Befehl noch gültig ist
		// Alle Schiffe, welche einen Systemangriffsbefehl haben überprüfen, ob dieser Befehl noch gültig ist
		CSector* pSector = &GetSector(m_ShipArray[y].GetKO());
  		CSystem* pSystem = &GetSystem(m_ShipArray[y].GetKO());
		if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::ATTACK)
			m_ShipArray[y].SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::AVOID)
			m_ShipArray[y].SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
		if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			if (pSector->GetSunSystem())
			{
				// Wenn die Bevölkerung komplett vernichtet wurde
				if (pSystem->GetHabitants() == 0.0f)
					m_ShipArray.ElementAt(y).SetCurrentOrder(SHIP_ORDER::ATTACK);
				// Wenn das System der angreifenden Rasse gehört
				else if (pSystem->GetOwnerOfSystem() == m_ShipArray.GetAt(y).GetOwnerOfShip())
					m_ShipArray.ElementAt(y).SetCurrentOrder(SHIP_ORDER::ATTACK);
				// Wenn eine Rasse in dem System lebt
				else if (pSector->GetOwnerOfSector() != "" && pSector->GetOwnerOfSector() != m_ShipArray.GetAt(y).GetOwnerOfShip())
				{
					CRace* pRace = m_pRaceCtrl->GetRace(pSector->GetOwnerOfSector());
					if (pRace != NULL && pRace->GetAgreement(m_ShipArray.GetAt(y).GetOwnerOfShip()) != DIPLOMATIC_AGREEMENT::WAR)
						m_ShipArray.ElementAt(y).SetCurrentOrder(SHIP_ORDER::ATTACK);
				}
			}
			else
				m_ShipArray.ElementAt(y).SetCurrentOrder(SHIP_ORDER::ATTACK);
		}

		// Flotten checken, falls keine mehr existiert, dann wird der Zeiger auf die Flotte aufgelöst
		// muss nicht unbedingt gemacht werden, hält die Objekte aber sauberer
		m_ShipArray[y].CheckFleet();

		// wenn der Befehl "Terraform" ist und kein Planet ausgew?hlt ist, dann Befehl wieder auf "AVOID"
		// setzen
		if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::TERRAFORM && m_ShipArray[y].GetTerraformingPlanet() == -1)
			m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);

		// Haben wir eine Flotte, den aktuellen Befehl an alle Schiffe in der Flotte weitergeben
		if (m_ShipArray[y].GetFleet() != 0)
			m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);

		 // Planet soll kolonisiert werden
		if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::COLONIZE)
		{
			// Überprüfen das der Sector auch nur mir oder niemandem geh?rt
			if ((pSector->GetOwnerOfSector() == m_ShipArray[y].GetOwnerOfShip() || pSector->GetOwnerOfSector() == ""))
			{
				// Wieviele Einwohner bekommen wir in dem System durch die Kolonisierung?
				float startHabitants = (float)(m_ShipArray[y].GetColonizePoints() * 4);
				// Wenn keine Nummer eines Planeten zum Kolonisieren angegeben ist, dann werden alle geterraformten
				// Planeten kolonisiert. Dazu wird die Bevölkerung, welche bei der Kolonisierung auf das System kommt
				// auf die einzelnen Planeten gleichmäßig aufgeteilt.
				if (m_ShipArray[y].GetTerraformingPlanet() == -1)
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
						m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						if (m_ShipArray[y].GetFleet() != 0)
							m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
						continue;
					}
				}
				else
				{
					if (pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetColonized() == FALSE
						&& pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == TRUE)
					{
						if (startHabitants > pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetMaxHabitant())
							startHabitants = pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetMaxHabitant();
						pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetCurrentHabitant(startHabitants);
						pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetColonisized(TRUE);
					}
					else
					{
						m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						if (m_ShipArray[y].GetFleet() != 0)
							m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
						continue;
					}
				}
				CString s;
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
				ASSERT(pMajor);
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

				// Gebäude bauen, wenn wir das System zum ersten Mal kolonisieren, sprich das System noch niemanden geh?rt
				if (pSystem->GetOwnerOfSystem() == "")
				{
					// Sector- und Systemwerte ändern
					pSector->SetOwned(TRUE);
					pSector->SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
					pSector->SetColonyOwner(m_ShipArray[y].GetOwnerOfShip());
					pSystem->SetOwnerOfSystem(m_ShipArray[y].GetOwnerOfShip());
					// Gebäude nach einer Kolonisierung bauen
					pSystem->BuildBuildingsAfterColonization(pSector,&BuildingInfo,m_ShipArray[y].GetColonizePoints());
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
				AddToLostShipHistory(&m_ShipArray[y], s, CResourceManager::GetString("DESTROYED"));
				// Schiff entfernen
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
				if (m_ShipArray[y].GetFleet() != 0)
					m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
				RemoveShip(y--);
				continue;
			}
			else
			{
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
			}
		}
		// hier wird ein Planet geterraformed
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::TERRAFORM && m_ShipArray[y].GetTerraformingPlanet() != -1)	// Planet soll terraformed werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			if (pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == FALSE)
			{
				if (pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetNeededTerraformPoints(m_ShipArray[y].GetColonizePoints()))
				{
					// Hier wurde ein Planet erfolgreich geterraformt
					m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
					m_ShipArray[y].SetTerraformingPlanet(-1);
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
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
				if (m_ShipArray[y].GetFleet() != 0)
					m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
			}
			// Wenn das Schiff eine Flotte anf?hrt, dann k?nnen auch die Schiffe in der Flotte ihre Terraformpunkte mit
			// einbringen
			if (m_ShipArray[y].GetFleet() != 0 && m_ShipArray[y].GetTerraformingPlanet() != -1)
			{
				unsigned colonize_points_sum = m_ShipArray[y].GetColonizePoints();
				for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
				{
					CShip* pFleetShip = m_ShipArray[y].GetFleet()->GetShipFromFleet(x);
					if (pSector->GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == FALSE)
					{
						const unsigned colonize_points = pFleetShip->GetColonizePoints();
						colonize_points_sum += colonize_points;
						if (pSector->GetPlanet(pFleetShip->GetTerraformingPlanet())->SetNeededTerraformPoints(colonize_points))
						{
							m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
							m_ShipArray[y].SetTerraformingPlanet(-1);
							m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
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
						m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
						break;
					}
				}
				//Gib eine Warnung aus falls Kolonisierungspunkte verschwendet werden würden.
				//Es ist hoffentlich nicht möglich, dass ein Schiff einer Schiffsgruppe einen anderen Planeten
				//terraformt als das die Gruppe anführende Schiff...
				const unsigned terraforming_planet = m_ShipArray[y].GetTerraformingPlanet();
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
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)	// es soll eine Station gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// hier wird vereinfacht angenommen, das an teurerer Aussenposten auch ein besserer ist
			short id = -1;
			// Wenn wir in dem Sektor noch keinen Au?enposten und noch keine Sternbasis stehen haben
			if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
				&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
			{
				USHORT costs = 0;
				const BYTE researchLevels[6] =
				{
					pMajor->GetEmpire()->GetResearch()->GetBioTech(),
					pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
					pMajor->GetEmpire()->GetResearch()->GetCompTech(),
					pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
					pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
					pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
				};
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == SHIP_TYPE::OUTPOST
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(researchLevels))
						{
							costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
							id = m_ShipInfoArray.GetAt(l).GetID();
						}
			}
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, k?nnen wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				{
					if (pSector->GetOutpost(it->first) == TRUE ||
					   (pSector->GetOutpost(it->first) == FALSE && pSector->GetStarbase(it->first) == TRUE))
					{
						buildable = FALSE;
						break;
					}
				}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					pSector->SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (pSector->GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						pSector->SetStartStationPoints(m_ShipInfoArray.GetAt((id-10000)).GetBaseIndustry(),m_ShipArray[y].GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anf?hrt, dann erstmal die Au?enpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					if (m_ShipArray[y].GetFleet() != 0)
					{
						for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							if (pSector->SetNeededStationPoints(m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
							{
								// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
								if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
									&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
								{
									pSector->SetOutpost(TRUE, m_ShipArray[y].GetOwnerOfShip());
									pSector->SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
									pSector->SetScanned(m_ShipArray[y].GetOwnerOfShip());
									pSector->SetOwned(TRUE);
									pSector->SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
									// Nachricht generieren, dass der Aussenpostenbau abgeschlossen wurde
									CMessage message;
									message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									AddToLostShipHistory(m_ShipArray[y].GetFleet()->GetShipFromFleet(x), CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
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
									m_ShipArray[y].GetFleet()->RemoveShipFromFleet(x);
									m_ShipArray[y].CheckFleet();
									BuildShip(id, pSector->GetKO(), m_ShipArray[y].GetOwnerOfShip());
									// Wenn hier ein Au?enposten gebaut wurde den Befehl f?r die Flotte auf Meiden stellen
									m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
									break;
								}
							}
					}
					if (pSector->GetIsStationBuilding(m_ShipArray[y].GetOwnerOfShip()) == TRUE
						&& pSector->SetNeededStationPoints(m_ShipArray[y].GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
							&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
						{
							pSector->SetOutpost(TRUE,m_ShipArray[y].GetOwnerOfShip());
							pSector->SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
							pSector->SetScanned(m_ShipArray[y].GetOwnerOfShip());
							pSector->SetOwned(TRUE);
							pSector->SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
							// Nachricht generieren, dass der Aussenpostenbau abgeschlossen wurde
							CMessage message;
							message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							AddToLostShipHistory(&m_ShipArray[y], CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
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
							BuildShip(id, pSector->GetKO(), m_ShipArray[y].GetOwnerOfShip());

							// Wenn hier ein Aussenposten gebaut wurde den Befehl für die Flotte auf Meiden stellen
							m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
							RemoveShip(y--);
							continue;
						}
					}
				}
				else
					m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
			}
			else
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
		}
		// hier wird eine Sternbasis gebaut
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::BUILD_STARBASE)	// es soll eine Sternbasis gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen k?nnten.
			// um eine Sternbasis bauen zu k?nnen mu? schon ein Aussenposten in dem Sektor stehen
			// hier wird vereinfacht angenommen, das eine teurere Sternbasis auch eine bessere ist
			// oder wir haben einen Aussenposten und wollen diesen zur Sternbasis updaten
			short id = -1;
			if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
				&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
			{
				USHORT costs = 0;
				const BYTE researchLevels[6] =
				{
					pMajor->GetEmpire()->GetResearch()->GetBioTech(),
					pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
					pMajor->GetEmpire()->GetResearch()->GetCompTech(),
					pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
					pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
					pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
				};
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == SHIP_TYPE::STARBASE
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(researchLevels))
						{
							costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
							id = m_ShipInfoArray.GetAt(l).GetID();
						}
			}
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				{
					if (pSector->GetStarbase(it->first) == TRUE
						|| (pSector->GetOutpost(it->first) == TRUE && it->first != m_ShipArray[y].GetOwnerOfShip()))
					{
						buildable = FALSE;
						break;
					}
				}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					pSector->SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (pSector->GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						pSector->SetStartStationPoints(m_ShipInfoArray.GetAt(id-10000).GetBaseIndustry(),m_ShipArray[y].GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anf?hrt, dann erstmal die Au?enpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					if (m_ShipArray[y].GetFleet() != 0)
					{
						for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							if (pSector->SetNeededStationPoints(m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
							{
								// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
								if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
									&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
								{
									pSector->SetOutpost(FALSE,m_ShipArray[y].GetOwnerOfShip());
									pSector->SetStarbase(TRUE,m_ShipArray[y].GetOwnerOfShip());
									pSector->SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
									pSector->SetScanned(m_ShipArray[y].GetOwnerOfShip());
									pSector->SetOwned(TRUE);
									pSector->SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
									// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
									CMessage message;
									message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									AddToLostShipHistory(m_ShipArray[y].GetFleet()->GetShipFromFleet(x), CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
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
									m_ShipArray[y].GetFleet()->RemoveShipFromFleet(x);
									m_ShipArray[y].CheckFleet();
									this->BuildShip(id, pSector->GetKO(), m_ShipArray[y].GetOwnerOfShip());
									// Wenn wir jetzt die Sternbasis gebaut haben, dann müssen wir den alten Aussenposten aus der
									// Schiffsliste nehmen
									for (int k = 0; k <= m_ShipArray.GetSize(); k++)
										if (m_ShipArray[k].GetShipType() == SHIP_TYPE::OUTPOST && m_ShipArray[k].GetKO() == pSector->GetKO())
										{
											// ebenfalls muss der Au?enposten aus der Shiphistory der aktuellen Schiffe entfernt werden
											pMajor->GetShipHistory()->RemoveShip(&m_ShipArray[k]);
											vRemoveableOutposts.push_back(m_ShipArray[k].GetShipName());
											break;
										}
									// Wenn hier eine Station gebaut wurde den Befehl für die Flotte auf Meiden stellen
									m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
									break;
								}
							}
					}
					if (pSector->GetIsStationBuilding(m_ShipArray[y].GetOwnerOfShip()) == TRUE
						&& pSector->SetNeededStationPoints(m_ShipArray[y].GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (pSector->GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
							&& pSector->GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
						{
							pSector->SetOutpost(FALSE,m_ShipArray[y].GetOwnerOfShip());
							pSector->SetStarbase(TRUE,m_ShipArray[y].GetOwnerOfShip());
							pSector->SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
							pSector->SetScanned(m_ShipArray[y].GetOwnerOfShip());
							pSector->SetOwned(TRUE);
							pSector->SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
							// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
							CMessage message;
							message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							AddToLostShipHistory(&m_ShipArray[y], CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
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
							BuildShip(id, pSector->GetKO(), m_ShipArray[y].GetOwnerOfShip());
							// Wenn hier eine Station gebaut wurde den Befehl für die Flotte auf Meiden stellen
							m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
							RemoveShip(y--);

							// Wenn die Sternbasis gebaut haben, dann den alten Au?enposten aus der Schiffsliste nehmen
							for (int k = 0; k < m_ShipArray.GetSize(); k++)
								if (m_ShipArray[k].GetShipType() == SHIP_TYPE::OUTPOST && m_ShipArray[k].GetKO() == pSector->GetKO())
								{
									// ebenfalls muss der Au?enposten aus der Shiphistory der aktuellen Schiffe entfernt werden
									pMajor->GetShipHistory()->RemoveShip(&m_ShipArray[k]);
									vRemoveableOutposts.push_back(m_ShipArray[k].GetShipName());
									break;
								}
							continue;
						}
					}
				}
				else
					m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
			}
			else
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
		}
		// Wenn wir das Schiff abracken/zerst?ren/demontieren wollen
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::DESTROY_SHIP)	// das Schiff wird demontiert
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			/*network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());*/

			// wenn wir in dem Sector wo wir das Schiff demoniteren ein uns gehörendes System haben, dann bekommen wir
			// teilweise Rohstoffe aus der Demontage zurück (vlt. auch ein paar Credits)
			if (pSystem->GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
			{
				USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 ausw?hlen
				// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
				proz += pSystem->GetProduction()->GetShipRecycling();
				USHORT id = m_ShipArray[y].GetID() - 10000;
				pSystem->SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
				pSystem->SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
				pSystem->SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
				pSystem->SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
				pSystem->SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
				pMajor->GetEmpire()->SetCredits((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
			}
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], pSector->GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"),	CResourceManager::GetString("DESTROYED"));

			// Wenn das Schiff eine Flotte anf?hrt, dann auch die Schiffe in der Flotte demontieren
			if (m_ShipArray[y].GetFleet() != 0)
			{
				for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
				{
					if (pSystem->GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
					{
						USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 ausw?hlen
						// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
						proz += pSystem->GetProduction()->GetShipRecycling();
						USHORT id = m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetID() - 10000;
						pSystem->SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
						pSystem->SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
						pSystem->SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
						pSystem->SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
						pSystem->SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
						pMajor->GetEmpire()->SetCredits((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
					}
					// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
					pMajor->GetShipHistory()->ModifyShip(m_ShipArray[y].GetFleet()->GetShipFromFleet(x), pSector->GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"), CResourceManager::GetString("DESTROYED"));
				}
			}

			// Wenn es ein Au?enposten oder eine Sternbasis ist, dann dem Sektor bekanntgeben, dass in ihm keine Station mehr ist
			if (m_ShipArray[y].IsStation())
			{
				pSector->SetOutpost(FALSE, m_ShipArray[y].GetOwnerOfShip());
				pSector->SetStarbase(FALSE, m_ShipArray[y].GetOwnerOfShip());
			}

			m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
			m_ShipArray.RemoveAt(y--);
			continue;	// continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
		}

		// Wenn wir ein Schiff zum Flagschiff ernennen wollen (nur ein Schiff pro Imperium kann ein Flagschiff sein!)
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::ASSIGN_FLAGSHIP && m_ShipArray[y].GetFleet() == 0)
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff finden, diesem den
			// Titel wegnehmen
			for (USHORT n = 0; n < m_ShipArray.GetSize(); n++)
			{
				if (m_ShipArray[n].GetOwnerOfShip() == m_ShipArray[y].GetOwnerOfShip())
				{
					if (m_ShipArray[n].GetIsShipFlagShip() == TRUE)
					{
						m_ShipArray[n].SetIsShipFlagShip(FALSE);
						break;
					}
					// überprüfen ob ein Flagschiff in einer Flotte ist
					else if (m_ShipArray[n].GetFleet() != 0)
					{
						bool bFoundFlagShip = false;
						for (USHORT m = 0; m < m_ShipArray[n].GetFleet()->GetFleetSize(); m++)
						{
							if (m_ShipArray[n].GetFleet()->GetShipFromFleet(m)->GetIsShipFlagShip() == TRUE)
							{
								m_ShipArray[n].GetFleet()->GetShipFromFleet(m)->SetIsShipFlagShip(FALSE);
								bFoundFlagShip = true;
								break;
							}
						}
						if (bFoundFlagShip)
							break;
					}
				}
			}
			// Jetzt das neue Schiff zum Flagschiff ernennen
			m_ShipArray[y].SetIsShipFlagShip(TRUE);
			if (m_ShipArray[y].IsNonCombat())
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::AVOID);
			else
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::ATTACK);
			// Nachricht generieren, dass ein neues Schiff zum Flagschiff ernannt wurde
			CString s = CResourceManager::GetString("ASSIGN_FLAGSHIP_MESSAGE",FALSE,m_ShipArray[y].GetShipName(),m_ShipArray[y].GetShipTypeAsString());
			CMessage message;
			message.GenerateMessage(s,MESSAGE_TYPE::MILITARY,"",pSector->GetKO(),FALSE);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::TRAIN_SHIP)
		{
			// Checken ob der Befehl noch Gültigkeit hat
			if (pSector->GetSunSystem() == TRUE &&
				pSystem->GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
			{
				// Wenn ein Schiff mit Veteranenstatus (Level 4) in der Trainingsflotte ist, dann verdoppelt sich der Erfahrungsgewinn
				// für die niedrigstufigen Schiffe
				int XP = pSystem->GetProduction()->GetShipTraining();
				bool isVeteran = false;
				if (m_ShipArray[y].GetExpLevel() >= 4)
					isVeteran = true;
				else if (m_ShipArray[y].GetFleet() != 0)
				{
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
						if (m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetExpLevel() >= 4)
						{
							isVeteran = true;
							break;
						}
				}

				if (isVeteran == false || m_ShipArray[y].GetExpLevel() >= 4)
					m_ShipArray[y].SetCrewExperiance(XP);
				else
					m_ShipArray[y].SetCrewExperiance(XP * 2);
				// Wenn das Schiff eine Flotte anführt, Schiffstraining auf alle Schiffe in der Flotte anwenden
				if (m_ShipArray[y].GetFleet() != 0)
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
					{
						if (isVeteran == false || m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetExpLevel() >= 4)
							m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->SetCrewExperiance(XP);
						else
							m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->SetCrewExperiance(XP * 2);
					}
			}
		}
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::CLOAK)
		{
			m_ShipArray[y].SetCloak();
			// Wenn das Schiff eine Flotte anführt, checken ob der Tarnenbefehl noch G?ltigkeit hat. Wenn ja, dann
			// alle Schiffe in der Flotte tarnen
			if (m_ShipArray[y].GetCloak() == TRUE)
				if (m_ShipArray[y].GetFleet() != 0)
					if (m_ShipArray[y].GetFleet()->CheckOrder(&m_ShipArray[y], SHIP_ORDER::CLOAK) == TRUE)
						for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							if (m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetCloak() == FALSE)
								m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->SetCloak();

			// Wenn das Schiff enttarnt wurde, dann alle Schiffe in der Flotte entarnen. Dies sollte nicht häufig vorkommen. Selbst
			// kann man es so nicht einstellen, aber die KI enttarnt so die Schiffe in der Flotte
			if (m_ShipArray[y].GetCloak() == FALSE)
				if (m_ShipArray[y].GetFleet() != 0)
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
						if (m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->GetCloak() == TRUE)
							m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->SetCloak();
			// Befehl wieder auf Angreifen stellen
			m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::ATTACK);
		}
		// Blockadebefehl
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::BLOCKADE_SYSTEM)
		{
			BOOLEAN blockadeStillActive = FALSE;
			// Überprüfen ob der Blockadebefehl noch Gültigkeit hat
			if (pSystem->GetOwnerOfSystem() != m_ShipArray[y].GetOwnerOfShip())
				// handelt es sich beim Systembesitzer um eine andere Majorrace
				if (pSystem->GetOwnerOfSystem() != "")
				{
					CString systemOwner = pSystem->GetOwnerOfSystem();
					CString shipOwner   = m_ShipArray[y].GetOwnerOfShip();
					CRace* pShipOwner	= m_pRaceCtrl->GetRace(shipOwner);
					// haben wir einen Vertrag kleiner einem Freundschaftsvertrag mit der Majorrace
					if (pShipOwner->GetAgreement(systemOwner) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						int blockadeValue = pSystem->GetBlockade();
						if (m_ShipArray[y].HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
						{
							blockadeValue += rand()%20 + 1;
							blockadeStillActive = TRUE;
							m_ShipArray[y].CalcExp();
						}
						// Wenn das Schiff eine Flotte anf?hrt, dann erh?hen auch alle Schiffe in der Flotte mit
						// Blockadeeigenschaft den Blockadewert
						if (m_ShipArray[y].GetFleet() != 0)
							for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							{
								if (m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
								{
									blockadeValue += rand()%20 + 1;
									blockadeStillActive = TRUE;
									m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->CalcExp();
								}
								else
									m_ShipArray[y].GetFleet()->GetShipFromFleet(x)->SetCurrentOrder(SHIP_ORDER::ATTACK);
							}
						pSystem->SetBlockade((BYTE)blockadeValue);
						// Die Beziehung zum Systembesitzer verringert sich um bis zu maximal 10 Punkte
						CRace* pSystemOwner	= m_pRaceCtrl->GetRace(systemOwner);
						pSystemOwner->SetRelation(shipOwner, -rand()%(blockadeValue/10 + 1));
					}
				}
			// kann der Blockadebefehl nicht mehr ausgef?hrt werden, so wird der Befehl automatisch gelöscht
			if (!blockadeStillActive)
			{
				m_ShipArray[y].SetCurrentOrder(SHIP_ORDER::ATTACK);
				if (m_ShipArray[y].GetFleet() != 0)
					m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
			}
			// wird das System schlussendlich blockiert, so produzieren die Handelsrouten kein Credits mehr
			if (pSystem->GetBlockade() > NULL)
			{
				// Wird das System blockiert, so generiert die Handelsroute kein Credits
				for (int i = 0; i < pSystem->GetTradeRoutes()->GetSize(); i++)
					pSystem->GetTradeRoutes()->GetAt(i).SetCredits(NULL);

				// Eventscreen für den Angreifer und den Blockierten anlegen
				CRace* pShipOwner = m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip());
				CMajor* pShipOwnerMajor = NULL;
				if (pShipOwner != NULL && pShipOwner->IsMajor() && (pShipOwnerMajor = dynamic_cast<CMajor*>(pShipOwner))->IsHumanPlayer())
				{
					CEventBlockade* eventScreen = new CEventBlockade(m_ShipArray[y].GetOwnerOfShip(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, pSector->GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_" + pShipOwner->GetRaceID(), FALSE, pSector->GetName()));
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
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::WAIT_SHIP_ORDER)
		{
			//Do nothing, but only for this round.
			m_ShipArray[y].UnsetCurrentOrder();
		}
		//else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::SENTRY_SHIP_ORDER)
			//Do nothing for this and all following rounds until an explicit player input.
		else if (m_ShipArray[y].GetCurrentOrder() == SHIP_ORDER::REPAIR)
		{
			//The actual Hull reparing is currenty done in CalcShipMovement(),
			//after the call to this function.
		}

		// Vor der Schiffsbewegung aber nach einer möglichen Demontage dort ?berall einen ShipPort setzen wo
		// eine Sternbasis oder ein Au?enposten steht
		if (m_ShipArray[y].IsStation())
		{
			pSector->SetShipPort(TRUE, m_ShipArray[y].GetOwnerOfShip());
		}
	}

	// jetzt alle durch einen Sternbasisbau verschwundenen Aussenposten aus dem Feld entfernen
	for (unsigned int i = 0; i < vRemoveableOutposts.size(); i++)
	{
		for (int y = 0; y < m_ShipArray.GetSize(); y++)
		{
			if (vRemoveableOutposts[i] == m_ShipArray.GetAt(y).GetShipName())
			{
				m_ShipArray.RemoveAt(y);
				break;
			}
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

	// Hier kommt die Schiffsbewegung (also keine anderen Befehle werden hier noch ausgewertet, lediglich wird überprüft,
	// dass manche Befehle noch ihre Gültigkeit haben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		CShip* pShip = &m_ShipArray[y];

		// Prüfen, dass ein Terraformbefehl noch gültig ist
		if (pShip->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
		{
			CPoint p = pShip->GetKO();
			if (GetSector(pShip->GetKO()).GetPlanet(pShip->GetTerraformingPlanet())->GetTerraformed() == TRUE)
			{
				pShip->SetCurrentOrder(SHIP_ORDER::AVOID);
				pShip->SetTerraformingPlanet(-1);
			}
		}
		// Prüfen, dass ein Aussenpostenbaubefehl noch gültig ist
		else if (pShip->GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)
		{
			if (GetSector(pShip->GetKO()).GetOutpost(pShip->GetOwnerOfShip()) == TRUE)
				pShip->SetCurrentOrder(SHIP_ORDER::AVOID);
		}
		// Prüfen, dass ein Sternbasenbaubefehl noch gültig ist
		else if (pShip->GetCurrentOrder() == SHIP_ORDER::BUILD_STARBASE)
		{
			if (GetSector(pShip->GetKO()).GetStarbase(pShip->GetOwnerOfShip()) == TRUE)
				pShip->SetCurrentOrder(SHIP_ORDER::AVOID);
		}
		// weiter mit Schiffsbewegung
		Sector shipKO((char)pShip->GetKO().x,(char)pShip->GetKO().y);
		Sector targetKO((char)pShip->GetTargetKO().x,(char)pShip->GetTargetKO().y);
		Sector nextKO(-1,-1);

		if (shipKO == targetKO)
			targetKO = Sector(-1,-1);

		// Weltraummonster gesondert behandeln
		if(pShip->GetAlienType()!=ALIEN_TYPE::NONE)
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

					pShip->SetTargetKO(CPoint(targetKO.x, targetKO.y), 0);
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
			char range;
			char speed;
			// Unterscheiden, ob das Schiff eine Flotte anführt oder nicht
			if (pShip->GetFleet() != 0)
			{
				range = (char)(3 - pShip->GetFleet()->GetFleetRange(&m_ShipArray[y]));
				speed = (char)(pShip->GetFleet()->GetFleetSpeed(&m_ShipArray[y]));
			}
			else
			{
				range = (char)(3 - pShip->GetRange());
				speed = (char)(pShip->GetSpeed());
			}

			CRace* pRace = NULL;

			// Weltraummonster gesondert behandeln
			if (pShip->IsAlien())
			{
				CStarmap* pStarmap = new CStarmap(0);
				pStarmap->SetFullRangeMap();

				// Anomalien werden schon beachtet, da dies eine statische Variable ist und in NextRound() schon
				// berechnet wurde.
				nextKO = pStarmap->CalcPath(shipKO,targetKO,range,speed,*pShip->GetPath());

				delete pStarmap;
				pStarmap = NULL;
			}
			else
			{
				pRace = m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip());
				if (pRace != NULL && pRace->IsMajor())
				{
					nextKO = dynamic_cast<CMajor*>(pRace)->GetStarmap()->CalcPath(shipKO,targetKO,range,speed,*pShip->GetPath());
				}
			}

			// Ziel zum Anfliegen vorhanden
			if (nextKO != Sector(-1,-1))
			{
				pShip->SetKO(CPoint((int)nextKO.x,(int)nextKO.y));
				// Die Anzahl speed ersten Felder in Pfad des Schiffes l?schen
				if (nextKO == targetKO)
				{
					pShip->GetPath()->RemoveAll();
					pShip->SetTargetKO(CPoint(-1,-1), 0);
				}
				if (pRace != NULL && pRace->IsMajor() && !(this->GetSector(nextKO.x,nextKO.y).GetFullKnown(pShip->GetOwnerOfShip()))) //Berechnet Zufalls entdeckung in dem Sector den das Schiff anfliegt
					m_RandomEventManager.CalcExploreEvent(CPoint((int)nextKO.x,(int)nextKO.y),dynamic_cast<CMajor*>(pRace),&m_ShipArray);

				int high = speed;
				while (high > 0 && high < pShip->GetPath()->GetSize())
				{
					pShip->GetPath()->RemoveAt(0);
					high--;
				}
			}
		}

		// Gibt es eine Anomalie, wodurch die Schilde schneller aufgeladen werden
		bool bFasterShieldRecharge = false;
		if (GetSector(pShip->GetKO()).GetAnomaly())
			if (GetSector(pShip->GetKO()).GetAnomaly()->GetType() == BINEBULA)
				bFasterShieldRecharge = true;

		// Nach der Bewegung, aber noch vor einem möglichen Kampf werden die Schilde nach ihrem Typ wieder aufgeladen,
		// wenn wir auf einem Shipport sind, dann wird auch die Hülle teilweise wieder repariert
		//FIXME: The shipports are not yet updated for changes due to diplomacy at this spot.
		//If we declared war and are on a shipport of the former friend, the ship is repaired,
		//and a possible repair command isn't unset though it can no longer be set by the player this turn then.
		pShip->Repair(GetSector(pShip->GetKO()).GetShipPort(pShip->GetOwnerOfShip()), bFasterShieldRecharge);
		// Befehle an alle Schiffe in der Flotte weitergeben
		if (pShip->GetFleet())
			pShip->GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);

		// wenn eine Anomalie vorhanden, deren m?gliche Auswirkungen auf das Schiff berechnen
		if (GetSector(pShip->GetKO()).GetAnomaly())
		{
			GetSector(pShip->GetKO()).GetAnomaly()->CalcShipEffects(&m_ShipArray[y]);
			bAnomaly = true;
		}
	}


	if (!bAnomaly)
		return;

	// prüfen ob irgendwelche Schiffe durch eine Anomalie zerstört wurden
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &m_ShipArray.GetAt(i);
		// Wenn das Schiff eine Flotte hatte, dann erstmal nur die Schiffe in der Flotte beachten
		// Wenn davon welche zerstört wurden diese aus der Flotte nehmen
		if (pShip->GetFleet())
		{
			for (int x = 0; x < pShip->GetFleet()->GetFleetSize(); x++)
			{
				if (pShip->GetFleet()->GetShipFromFleet(x)->GetHull()->GetCurrentHull() < 1)
				{
					// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip()));
					if (pMajor)
					{
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						AddToLostShipHistory(pShip->GetFleet()->GetShipFromFleet(x), GetSector(pShip->GetKO()).GetAnomaly()->GetMapName(pShip->GetKO()), CResourceManager::GetString("DESTROYED"));

						CString sShip;
						sShip.Format("%s (%s)", pShip->GetFleet()->GetShipFromFleet(x)->GetShipName(), pShip->GetFleet()->GetShipFromFleet(x)->GetShipTypeAsString());
						CString s = CResourceManager::GetString("ANOMALY_SHIP_LOST", FALSE, sShip, GetSector(pShip->GetKO()).GetAnomaly()->GetMapName(pShip->GetKO()));
						CMessage message;
						message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", 0, 0);
						pMajor->GetEmpire()->AddMessage(message);
						if (pMajor->IsHumanPlayer())
						{
							network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
							m_iSelectedView[client] = EMPIRE_VIEW;
						}
					}
					pShip->GetFleet()->RemoveShipFromFleet(x--);
				}
			}
			pShip->CheckFleet();
		}

		// Wenn das Schiff selbst zerstört wurde
		if (pShip->GetHull()->GetCurrentHull() < 1)
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip()));
			if (pMajor)
			{
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				AddToLostShipHistory(pShip, GetSector(pShip->GetKO()).GetAnomaly()->GetMapName(pShip->GetKO()), CResourceManager::GetString("DESTROYED"));

				CString sShip;
				sShip.Format("%s (%s)", pShip->GetShipName(), pShip->GetShipTypeAsString());
				CString s = CResourceManager::GetString("ANOMALY_SHIP_LOST", FALSE, sShip, GetSector(pShip->GetKO()).GetAnomaly()->GetMapName(pShip->GetKO()));
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", 0, 0);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer())
				{
					network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
					m_iSelectedView[client] = EMPIRE_VIEW;
				}
			}

			RemoveShip(i--);
		}
	}
}

/// Funktion überprüft, ob irgendwo ein Schiffskampf stattfindet. Wenn ja und es sind menschliche Spieler beteiligt,
/// so werden ihnen alle jeweils beteiligten Schiffe geschickt, so dass sie dort Befehle geben können.
bool CBotf2Doc::IsShipCombat()
{
	m_bCombatCalc = false;

	// Jetzt gehen wir nochmal alle Sektoren durch, wenn in einem Sektor Schiffe mehrerer verschiedener Rassen sind,
	// die Schiffe nicht auf Meiden gestellt sind und die Rassen untereinander nicht alle mindst. einen Freundschafts-
	// vertrag haben, dann kommt es in diesem Sektor zum Kampf
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		const CShip* pShip = &m_ShipArray.GetAt(y);
		const CPoint& p = pShip->GetKO();
		const CString& sector = GetSector(p).GetName(TRUE);
		// Wenn unser Schiff auf Angreifen gestellt ist
		// Wenn in dem Sektor des Schiffes schon ein Kampf stattgefunden hat, dann findet hier keiner mehr statt
		if (pShip->GetCombatTactic() != COMBAT_TACTIC::CT_ATTACK
			|| m_sCombatSectors.find(sector) != m_sCombatSectors.end())
			continue;
		// Wenn noch kein Kampf in dem Sektor stattfand, dann kommt es möglicherweise hier zum Kampf
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
		{
			const CShip* pOtherShip = &m_ShipArray.GetAt(i);
			const CString& sOwner1 = pShip->GetOwnerOfShip();
			const CString& sOwner2 = pOtherShip->GetOwnerOfShip();
			// nur weiter, wenn das Schiff nicht unserer Rasse gehört
			// und wenn das Schiff sich im gleichen Sektor befindet
			if (sOwner2 == sOwner1 || pOtherShip->GetKO() != p)
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
	CArray<CShip*> vInvolvedShips;
	CPoint p = m_ptCurrentCombatSector;
	// Jetzt gehen wir nochmal alle Sektoren durch und schauen ob ein Schiff im Kampfsektor ist
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &m_ShipArray[i];
		if (pShip->GetKO() != m_ptCurrentCombatSector)
			continue;

		vInvolvedShips.Add(pShip);

		// Wenn das Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
		if (pShip->GetFleet())
			for (int j = 0; j < pShip->GetFleet()->GetFleetSize(); j++)
				vInvolvedShips.Add(pShip->GetFleet()->GetShipFromFleet(j));
		// CHECK WW:
		// folgendes Zeug kann weg, wenn der Kampf vor der Systemberechnung dramkommt

		// Haben wir einen Outpost oder eine Starbase hinzugefügt, dann entfernen wir erstmal
		// die Auswirkungen dieser Stationen. Diese werden später wieder hinzugefügt, falls
		// die Station nicht zerstört wurde.
		/*if (pShip->GetShipType() == OUTPOST || pShip->GetShipType() == STARBASE)
		{
			GetSector(p.x, p.y).SetShipPort(FALSE, pShip->GetOwnerOfShip());
			GetSector(p.x, p.y).SetOutpost(FALSE,  pShip->GetOwnerOfShip());
			GetSector(p.x, p.y).SetStarbase(FALSE, pShip->GetOwnerOfShip());
		}
		// Haben wir aber in diesem Sektor ein System mit aktiver Werft stehen, dann fügen
		// wir den ShipPort aber gleich jetzt wieder hinzu
		if (GetSystem(p.x, p.y).GetProduction()->GetShipYard() == TRUE && GetSystem(p.x, p.y).GetOwnerOfSystem() == pShip->GetOwnerOfShip())
			GetSector(p.x, p.y).SetShipPort(TRUE, pShip->GetOwnerOfShip());*/
	}

	// es sollten immer Schiffe im Array sein, sonst hätte in diesem Sektor kein Kampf stattfinden dürfen
	ASSERT(vInvolvedShips.GetSize());
	if (vInvolvedShips.GetSize() == 0)
		return;

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
			message.GenerateMessage(CResourceManager::GetString("WIN_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", NULL, FALSE);
			pMajor->GetEmpire()->AddMessage(message);
			// win a minor battle
			CString eventText = pMajor->GetMoralObserver()->AddEvent(3, pMajor->GetRaceMoralNumber());
			message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", 0, 0);
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
				message.GenerateMessage(CResourceManager::GetString("LOSE_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", 0,0);
				pMajor->GetEmpire()->AddMessage(message);
				// lose a minorbattle
				CString eventText = pMajor->GetMoralObserver()->AddEvent(6, pMajor->GetRaceMoralNumber());
				message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", 0, 0);
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
					if (pMajorWin->IsHumanPlayer() == false)
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
			message.GenerateMessage(CResourceManager::GetString("DRAW_COMBAT", false, sSectorName), MESSAGE_TYPE::MILITARY, "", 0,0);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}

	// Nach einem Kampf muß ich das Feld der Schiffe durchgehen und alle Schiffe aus diesem nehmen, die
	// keine Hülle mehr besitzen. Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte
	CStringArray destroyedShips;
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		if (m_ShipArray.GetAt(i).GetKO() != m_ptCurrentCombatSector)
			continue;
		// Wenn das Schiff eine Flotte hatte, dann erstmal nur die Schiffe in der Flotte beachten
		// Wenn davon welche zerstört wurden diese aus der Flotte nehmen
		if (m_ShipArray.GetAt(i).GetFleet())
		{
			for (int x = 0; x < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); x++)
				if (m_ShipArray.GetAt(i).GetFleet()->GetShipFromFleet(x)->GetHull()->GetCurrentHull() < 1)
				{
					// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
					AddToLostShipHistory(m_ShipArray[i].GetFleet()->GetShipFromFleet(x), CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
					destroyedShips.Add(m_ShipArray[i].GetFleet()->GetShipFromFleet(x)->GetShipName()+" ("+m_ShipArray[i].GetFleet()->GetShipFromFleet(x)->GetShipTypeAsString()+")");

					// Wenn es das Flagschiff war, so ein Event über dessen Verlust hinzufügen
					if (m_ShipArray[i].GetFleet()->GetShipFromFleet(x)->GetIsShipFlagShip())
					{
						CRace* pOwner = m_pRaceCtrl->GetRace(m_ShipArray[i].GetFleet()->GetShipFromFleet(x)->GetOwnerOfShip());
						if (pOwner && pOwner->IsMajor())
						{
							CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
							CString eventText = pMajor->GetMoralObserver()->AddEvent(7, pMajor->GetRaceMoralNumber(), m_ShipArray[i].GetFleet()->GetShipFromFleet(x)->GetShipName());
							CMessage message;
							message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", 0, 0);
							pMajor->GetEmpire()->AddMessage(message);
						}
					}

					m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
				}
			m_ShipArray[i].CheckFleet();
		}

		// Wenn das Schiff selbst zerstört wurde
		if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
		{
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			AddToLostShipHistory(&m_ShipArray[i], CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
			destroyedShips.Add(m_ShipArray[i].GetShipName()+" ("+m_ShipArray[i].GetShipTypeAsString()+")");
			// Wenn es das Flagschiff war, so ein Event über dessen Verlust hinzufügen
			if (m_ShipArray[i].GetIsShipFlagShip())
			{
				CRace* pOwner = m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip());
				if (pOwner && pOwner->IsMajor())
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
					CString eventText = pMajor->GetMoralObserver()->AddEvent(7, pMajor->GetRaceMoralNumber(), m_ShipArray[i].GetShipName());
					CMessage message;
					message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", 0, 0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}
			// Wenn es ein Außenposten oder Sternbasis war, so ein Event über dessen Verlust hinzufügen
			if (m_ShipArray[i].IsStation())
			{
				CRace* pOwner = m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip());
				if (pOwner && pOwner->IsMajor())
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
					CString eventText;
					if (m_ShipArray[i].GetShipType() == SHIP_TYPE::OUTPOST)
						eventText = pMajor->GetMoralObserver()->AddEvent(8, pMajor->GetRaceMoralNumber());
					else
						eventText = pMajor->GetMoralObserver()->AddEvent(9, pMajor->GetRaceMoralNumber());
					CMessage message;
					message.GenerateMessage(eventText, MESSAGE_TYPE::MILITARY, "", 0, 0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}

			RemoveShip(i--);
		}
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// Hat die Rasse an dem Kampf teilgenommen, also gewonnen oder verloren oder unentschieden
		if (winner[it->first] != 0)
			for (int j = 0; j < destroyedShips.GetSize(); j++)
			{
				CString s;
				s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,destroyedShips[j]));
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", 0, 0);
				pMajor->GetEmpire()->AddMessage(message);
			}
	}

	// allen Schiffen mit Rückzugsbfehl den aktuellen Befehl zurücknehmen
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &m_ShipArray[i];
		// Hat das Schiff den Rückzugsbefehl
		if (pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
		{
			// Schiff auf Meiden stellen
			pShip->SetCurrentOrderAccordingToType();

			// womögicher Terraformplanet oder Stationsbau zurücknehmen
			pShip->SetTerraformingPlanet(-1);
		}
	}
}

/////BEGIN: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipEffects()
void CBotf2Doc::CalcShipRetreat() {
	// Schiffe mit Rückzugsbefehl auf ein Feld neben dem aktuellen Feld setzen
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &m_ShipArray.GetAt(i);
		const CString& ship_owner = pShip->GetOwnerOfShip();
		// Hat das Schiff den Rückzugsbefehl
		if (pShip->GetCombatTactic() != COMBAT_TACTIC::CT_RETREAT)
			continue;

		// Rückzugssektor für dieses Schiff in diesem Sektor holen
		const std::map<CString, std::map<std::pair<int, int>, CPoint>>::const_iterator&
			SectorRetreatSectorPairs = m_mShipRetreatSectors.find(ship_owner);
		if (SectorRetreatSectorPairs == m_mShipRetreatSectors.end())
			continue;
		const std::map<std::pair<int, int>, CPoint>& mSectorRetreatSectorPairs
			= SectorRetreatSectorPairs->second;
		const CPoint& co = pShip->GetKO();
		const pair<int, int> CurrentSector(co.x, co.y);
		const std::map<std::pair<int, int>, CPoint>::const_iterator& RetreatSector
			= mSectorRetreatSectorPairs.find(CurrentSector);
		if (RetreatSector == mSectorRetreatSectorPairs.end())
			continue;

		pShip->Retreat(RetreatSector->second);

		pShip->CheckFleet();
		CFleet* pFleet = pShip->GetFleet();
		if(!pFleet)
			continue;
		// sind alle Schiffe in einer Flotte im Rückzug, so kann die ganze Flotte
		// in den Rückzugssektor
		const bool bCompleteFleetRetreat = pFleet->GetFleetSpeed(pShip) > 0
			&& pFleet->AllOnTactic(COMBAT_TACTIC::CT_RETREAT);

		// haben alle Schiffe in der Flotte den Rückzugsbefehl oder hat das Schiff keine Flotte
		// -> Rückzugssektor festlegen
		if (bCompleteFleetRetreat)
		{
			// Rückzugsbefehl in Flotte zurücknehmen
			for (int j = 0; j < pFleet->GetFleetSize(); j++)
			{
				CShip* pFleetShip = pFleet->GetShipFromFleet(j);
				pFleetShip->Retreat(RetreatSector->second);
			}
		}
		// Schiffe aus der Flotte nehmen und ans Ende des Schiffsarrays packen. Diese werden
		// dann auch noch behandelt
		else
		{
			// nicht mehr auf pShip arbeiten, da sich der Vektor hier verändern kann und somit
			// der Zeiger nicht mehr gleich ist!
			while (!m_ShipArray[i].GetFleet()->IsEmpty())
			{
				m_ShipArray.Add(*m_ShipArray[i].GetFleet()->GetShipFromFleet(0));
				m_ShipArray[i].GetFleet()->RemoveShipFromFleet(0);
			}
			m_ShipArray[i].DeleteFleet();
		}
	}//	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	m_mShipRetreatSectors.clear();
}

//most of the stuff from CalcShipEffects() for either a ship from the shiparray or a ship of its fleet
void CBotf2Doc::CalcShipEffectsForSingleShip(CShip& ship, CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner, bool fleetship) {
	const CString& sRace = pRace->GetRaceID();
	const bool major = pRace->IsMajor();
	if(!fleetship && major)
		sector.SetFullKnown(sRace);
	if (!bDeactivatedShipScanner) {
		// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
		PutScannedSquareOverSector(sector, ship.GetScanRange(), ship.GetScanPower(),
			*pRace, bBetterScanner, ship.HasSpecial(SHIP_SPECIAL::PATROLSHIP));
		sector.IncrementNumberOfShips(sRace);
	}
	// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
	// sein, ansonsten gilt dort nur Stufe 3.
	if (!ship.IsStation()) {
		// Im Sektor die NeededScanPower setzen, die wir brauchen um dort Schiffe zu sehen. Wir sehen ja keine getarnten
		// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
		// sein, ansonsten gilt dort nur Stufe 3.
		short stealthPower = ship.GetStealthPower();
		if(!ship.GetCloak() && stealthPower > 3)
			stealthPower = 3;
		const short NeededScanPower = stealthPower * 20;
		if (NeededScanPower < sector.GetNeededScanPower(sRace))
			sector.SetNeededScanPower(NeededScanPower, sRace);
	}
	if(!fleetship) {
		// Wenn das Schiff gerade eine Station baut, so dies dem Sektor mitteilen
		const SHIP_ORDER::Typ current_order = ship.GetCurrentOrder();
		if (current_order == SHIP_ORDER::BUILD_OUTPOST || current_order == SHIP_ORDER::BUILD_STARBASE)
			sector.SetIsStationBuilding(TRUE, sRace);
		// Wenn das Schiff gerade Terraform, so dies dem Planeten mitteilen
		else if (current_order == SHIP_ORDER::TERRAFORM) {
			const short nPlanet = ship.GetTerraformingPlanet();
			std::vector<CPlanet>& planets = sector.GetPlanets();
			if (nPlanet != -1 && nPlanet < static_cast<int>(planets.size()))
				planets.at(nPlanet).SetIsTerraforming(TRUE);
			else {
				ship.SetTerraformingPlanet(-1);
				ship.SetCurrentOrder(SHIP_ORDER::AVOID);
			}
		}
	}
	if (major) {
		CMajor* pMajor = dynamic_cast<CMajor*>(pRace);
		// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
		pMajor->GetEmpire()->AddShipCosts(ship.GetMaintenanceCosts());
		// die Schiffe in der Flotte beim modifizieren der Schiffslisten der einzelnen Imperien beachten
		pMajor->GetShipHistory()->ModifyShip(&ship, sector.GetName(TRUE));
	}
	// Erfahrungspunkte der Schiffe anpassen
	ship.CalcExp();
}
/////END: HELPER FUNCTIONS FOR void CBotf2Doc::CalcShipEffects()

/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
void CBotf2Doc::CalcShipEffects()
{
	CalcShipRetreat();

	// Nach einem möglichen Kampf, aber natürlich auch generell die Schiffe und Stationen den Sektoren bekanntgeben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		const CString sRace = m_ShipArray[y].GetOwnerOfShip();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		CShip& ship = m_ShipArray.GetAt(y);
		const CPoint& p = ship.GetKO();
		CSector& sector = GetSector(p);

		// Anomalien beachten
		bool bDeactivatedShipScanner = false;
		bool bBetterScanner = false;
		const CAnomaly* const anomaly = sector.GetAnomaly();
		if(anomaly) {
			bDeactivatedShipScanner = anomaly->IsShipScannerDeactivated();
			bBetterScanner = anomaly->GetType() == QUASAR;
		}

		CalcShipEffectsForSingleShip(ship, sector, pRace, bDeactivatedShipScanner, bBetterScanner, false);
		// wenn das Schiff eine Flotte besitzt, dann die Schiffe in der Flotte auch beachten
		CFleet* fleet = ship.GetFleet();
		if(fleet)
		{
			// Scanstärke der Schiffe in der Flotte auf die Sektoren abhängig von der Scanrange übertragen
			for (int x = 0; x < fleet->GetFleetSize(); x++)
			{
				CShip* fleetship = fleet->GetShipFromFleet(x);
				CalcShipEffectsForSingleShip(*fleetship, sector, pRace,
					bDeactivatedShipScanner, bBetterScanner, true);
			}
		}
		// Dem Sektor nochmal bekanntgeben, dass in ihm eine Sternbasis oder ein Außenposten steht. Weil wenn im Kampf
		// eine Station teilnahm, dann haben wir den Shipport in dem Sektor vorläufig entfernt. Es kann ja passieren,
		// dass die Station zerstört wird. Haben wir jetzt aber immernoch eine Station, dann bleibt der Shipport dort auch
		// bestehen
		if (ship.IsStation()) {
			sector.SetShipPort(TRUE, sRace);
			const SHIP_TYPE::Typ ship_type = ship.GetShipType();
			if (ship_type == SHIP_TYPE::OUTPOST)
				sector.SetOutpost(TRUE, sRace);
			else
				sector.SetStarbase(TRUE, sRace);
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
	const network::RACE contacted_client = m_pRaceCtrl->GetMappedClientID(ContactedRace.GetRaceID());
	m_iSelectedView[client] = EMPIRE_VIEW;
	if(!Major.IsHumanPlayer())
		return;
	// Audiovorstellung der kennengelernten race
	SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_ALIENCONTACT, contacted_client, 1, 1.0f};
	if(ContactedRace.IsMajor()) {
		entry.nMessage = SNDMGR_MSG_FIRSTCONTACT;
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
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		const CShip* pShip = &m_ShipArray.GetAt(y);
		const CString& sRace = pShip->GetOwnerOfShip();
		CRace* pRace = m_pRaceCtrl->GetRace(sRace);
		// kann die Rasse andere Rassen kennenlernen?
		if(pRace->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
			continue;
		const CPoint& p = pShip->GetKO();
		const CSector& sector = GetSector(p);
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
	}//for (int y = 0; y < m_ShipArray.GetSize(); y++)
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
			for (int j = 0; j < m_ShipArray.GetSize(); j++)
			{
				if (m_ShipArray.GetAt(j).GetOwnerOfShip() == pMajor->GetRaceID())
				{
					// Alle noch "lebenden" Schiffe aus der Schiffshistory ebenfalls als zerstört ansehen
					pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[j],
								GetSector(m_ShipArray[j].GetKO()).GetName(TRUE), m_iRound,
								CResourceManager::GetString("UNKNOWN"), CResourceManager::GetString("DESTROYED"));
					m_ShipArray.RemoveAt(j--);
				}
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
				se->SetOutpost(false, ID);
				se->SetOwnerOfShip(false, ID);
				se->SetShipPort(false, ID);
				se->SetStarbase(false, ID);
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
	CSmallInfoView::SetShipInfo(false);
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		if (m_iSelectedView[i] == FLEET_VIEW)
			m_iSelectedView[i] = GALAXY_VIEW;

	m_NumberOfTheShipInArray = -1;
	m_iNumberOfFleetShip = -1;
	m_iNumberOfTheShipInFleet = -1;
}

/// Funktion berechnet, ob zufällig Alienschiffe ins Spiel kommen.
void CBotf2Doc::CalcRandomAlienEntities()
{
	const CIniLoader* pIni = CIniLoader::GetInstance();
	if(!pIni->ReadValueDefault("Special", "GENERATE_ALIEN_ENTITIES", true))
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

			// nur ca. aller 20 + Techmodifikator Runden kommt das Alienschiff ins Spiel
			if (rand()%(20 + nMod) != 0)
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
				if (!GetSector(p).GetAnomaly())
				{
					BuildShip(pShipInfo->GetID(), p, pAlien->GetRaceID());

					CShip* pShip = &m_ShipArray[m_ShipArray.GetUpperBound()];
					// unterschiedliche Aliens unterschieden und Schiffseigenschaften festlegen
					if (pAlien->GetRaceID() == "Ionisierendes Gaswesen")
					{
						pShip->SetAlienType(ALIEN_TYPE::IONISIERENDES_GASWESEN);
						pShip->SetCurrentOrder(SHIP_ORDER::AVOID);
					}
					else if (pAlien->GetRaceID() == "Gaballianer")
					{
						pShip->SetAlienType(ALIEN_TYPE::GABALLIANER_SEUCHENSCHIFF);
						pShip->SetCurrentOrder(SHIP_ORDER::ATTACK);
					}
					else if (pAlien->GetRaceID() == "Blizzard-Plasmawesen")
					{
						pShip->SetAlienType(ALIEN_TYPE::BLIZZARD_PLASMAWESEN);
						pShip->SetCurrentOrder(SHIP_ORDER::ATTACK);
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
	for (int i = 0; i < m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &m_ShipArray.GetAt(i);
		if (!pShip->IsAlien())
			continue;

		// Aliens mit Rückzugsbefehl machen nix
		if (pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
			continue;

		CMinor* pAlien = dynamic_cast<CMinor*>(m_pRaceCtrl->GetRace(pShip->GetOwnerOfShip()));
		if (!pAlien || !pAlien->IsAlienRace())
		{
			ASSERT(FALSE);
			continue;
		}

		// verschiedene Alienrassen unterscheiden
		if ((pShip->GetAlienType() & ALIEN_TYPE::IONISIERENDES_GASWESEN) > 0)
		{
			CString sSystemOwner = GetSystem(pShip->GetKO()).GetOwnerOfSystem();
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner));
			if (!pOwner)
				continue;

			// Energie im System auf 0 setzen
			GetSystem(pShip->GetKO()).SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (GetSystem(pShip->GetKO()).GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CResourceManager::GetString("EVENT_IONISIERENDES_GASWESEN", FALSE, GetSector(pShip->GetKO()).GetName());
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(pShip->GetKO()).GetName(), pShip->GetKO(), 0);
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
		if ((pShip->GetAlienType() & ALIEN_TYPE::GABALLIANER_SEUCHENSCHIFF) > 0)
		{
			CString sSystemOwner = GetSystem(pShip->GetKO()).GetOwnerOfSystem();
			if (CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner)))
			{
				// Nahrung im System auf 0 setzen
				GetSystem(pShip->GetKO()).SetDisabledProduction(WORKER::FOOD_WORKER);
				GetSystem(pShip->GetKO()).SetFoodStore(GetSystem(pShip->GetKO()).GetFoodStore() / 2);

				// Wenn narung produziert oder vorhanden ist, dann die Nachricht bringen über Nahrung verseucht
				if (GetSystem(pShip->GetKO()).GetProduction()->GetMaxFoodProd() > 0 || GetSystem(pShip->GetKO()).GetFoodStore() > 0)
				{
					// Nachricht und Event einfügen
					CString s = CResourceManager::GetString("EVENT_GABALLIANER_SEUCHENSCHIFF", FALSE, GetSector(pShip->GetKO()).GetName());
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(pShip->GetKO()).GetName(), pShip->GetKO(), 0);
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
			if (GetSector(pShip->GetKO()).GetIsShipInSector() && rand()%3 == 0)
			{
				// alle Schiffe im Sektor zu Seuchenschiffen machen
				for (int y = 0; y < m_ShipArray.GetSize(); y++)
				{
					CShip* pOtherShip = &m_ShipArray.GetAt(y);
					// Schiff im gleichen Sektor?
					if (pOtherShip->GetKO() != pShip->GetKO())
						continue;

					// keine anderen Alienschiffe
					if (pOtherShip->IsAlien() || (pOtherShip->GetAlienType() & ALIEN_TYPE::GABALLIANER_SEUCHENSCHIFF) > 0)
						continue;

					// keine Außenposten und Sternenbasen
					if (pOtherShip->IsStation())
						continue;

					vector<CShip*> vShips;
					vShips.push_back(pOtherShip);

					if (pOtherShip->GetFleet())
					{
						for (int x = 0; x < pOtherShip->GetFleet()->GetFleetSize(); x++)
							vShips.push_back(pOtherShip->GetFleet()->GetShipFromFleet(x));
					}

					for (unsigned int n = 0; n < vShips.size(); n++)
					{
						// Schiffe mit Rückzugsbefehl werden nie vom Virus befallen
						if (vShips[n]->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
							continue;

						vShips[n]->SetOwnerOfShip(pAlien->GetRaceID());
						vShips[n]->SetTargetKO(CPoint(-1, -1), 0);
						vShips[n]->SetAlienType(ALIEN_TYPE::GABALLIANER_SEUCHENSCHIFF);
						vShips[n]->SetCurrentOrder(SHIP_ORDER::ATTACK);
						vShips[n]->SetTerraformingPlanet(-1);
						vShips[n]->SetIsShipFlagShip(FALSE);

						// für jedes Schiff eine Meldung über den Verlust machen

						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						if (CMajor* pShipOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(vShips[n]->GetOwnerOfShip())))
						{
							AddToLostShipHistory(vShips[n], CResourceManager::GetString("COMBAT"), CResourceManager::GetString("MISSED"));
							CString s;
							s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,vShips[n]->GetShipName()));
							CMessage message;
							message.GenerateMessage(s, MESSAGE_TYPE::MILITARY, "", 0, 0);
							pShipOwner->GetEmpire()->AddMessage(message);
						}
					}
				}
			}
		}
		if ((pShip->GetAlienType() & ALIEN_TYPE::BLIZZARD_PLASMAWESEN) > 0)
		{
			CString sSystemOwner = GetSystem(pShip->GetKO()).GetOwnerOfSystem();
			CMajor* pOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sSystemOwner));
			if (!pOwner)
				continue;

			// Energie im System auf 0 setzen
			GetSystem(pShip->GetKO()).SetDisabledProduction(WORKER::ENERGY_WORKER);

			// Wenn Energie vorhanden war, dann die Nachricht bringen über Energieausfall
			if (GetSystem(pShip->GetKO()).GetProduction()->GetMaxEnergyProd() > 0)
			{
				// Nachricht und Event einfügen
				CString s = CResourceManager::GetString("EVENT_BLIZZARD_PLASMAWESEN", FALSE, GetSector(pShip->GetKO()).GetName());
				CMessage message;
				message.GenerateMessage(s, MESSAGE_TYPE::SOMETHING, GetSector(pShip->GetKO()).GetName(), pShip->GetKO(), 0);
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

CMainFrame* CBotf2Doc::GetMainFrame(void) const {
	return dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
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