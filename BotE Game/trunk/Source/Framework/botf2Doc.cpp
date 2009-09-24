// botf2Doc.cpp : Implementierung der Klasse CBotf2Doc
//

#include "stdafx.h"
#include "botf2.h"
#include "botf2Doc.h"
#include "GalaxyMenuView.h"
#include "SmallInfoView.h"
#include "MainBaseView.h"
#include "BottomBaseView.h"
#include "PlanetBottomView.h"
#include "MainDlg.h"
#include "NetworkHandler.h"
#include "MainFrm.h"

#include "IniLoader.h"
#include "RaceController.h"
#include "Combat.h"
#include "Fleet.h"
#include "AttackSystem.h"
#include "IntelCalc.h"
#include "DiplomacyController.h"

#include "AIPrios.h"
#include "SectorAI.h"
#include "SystemAI.h"
#include "ShipAI.h"

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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotf2Doc Konstruktion/Destruktion

CBotf2Doc::CBotf2Doc()
{
	//Init MT with single log file
	MYTRACE_INIT("BotE.log");

	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	CResourceManager::Init();
	m_pIniLoader = new CIniLoader();
	
	m_pSoundManager = new CSoundManager();
	m_pSoundManager->Init(!m_pIniLoader->GetValue("HARDWARESOUND"));

	m_pGraphicPool = new CGraphicPool(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics\\");

	m_pRaceCtrl = new CRaceController();
	
	m_pAIPrios = new CAIPrios(this);
	m_pSectorAI= new CSectorAI(this);

	m_pNetworkHandler = new CNetworkHandler(this);
	server.AddServerListener(m_pNetworkHandler);
	client.AddClientListener(m_pNetworkHandler);
}

CBotf2Doc::~CBotf2Doc()
{
	if (m_pIniLoader)
		delete m_pIniLoader;
	if (m_pSoundManager)
		delete m_pSoundManager;
	if (m_pGraphicPool)
		delete m_pGraphicPool;
	if (m_pRaceCtrl)
		delete m_pRaceCtrl;
	if (m_pAIPrios)
		delete m_pAIPrios;
	if (m_pSectorAI)
		delete m_pSectorAI;
	
	m_pIniLoader	= NULL;
	m_pSoundManager = NULL;
	m_pGraphicPool	= NULL;
	m_pRaceCtrl		= NULL;
	m_pAIPrios		= NULL;
	m_pSectorAI		= NULL;

	m_ShipArray.RemoveAll();
	m_ShipInfoArray.RemoveAll();

	if (CSector::m_Font)
		delete CSector::m_Font;
	if (CSector::m_NameGenerator)
		delete CSector::m_NameGenerator;

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

BOOL CBotf2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	srand((unsigned)time(NULL));
	
	m_bDataReceived				= false;
	m_bDontExit					= false;
	m_bGameLoaded				= false;
	m_fStardate					= 121000.0f;
	CString difficulty = m_pIniLoader->GetStringValue("DIFFICULTY");
	difficulty.MakeUpper();
	if (difficulty == "BABY")
		m_fDifficultyLevel			= 1.0f;
	else if (difficulty == "EASY")
		m_fDifficultyLevel			= 0.75f;
	else if (difficulty == "NORMAL")
		m_fDifficultyLevel			= 0.5f;
	else if (difficulty == "HARD")
		m_fDifficultyLevel			= 0.33f;
	else if (difficulty == "IMPOSSIBLE")
		m_fDifficultyLevel			= 0.2f;
	else
		m_fDifficultyLevel			= 0.5f;
	
	CMainDlg mainDlg(this);
	if (mainDlg.DoModal() != ID_WIZFINISH && !m_bDontExit)
		exit(1);
	while (!m_bDataReceived)
		Sleep(50);
		
	// Standardwerte setzen
	m_ptKO = CPoint(0,0);
	m_bRoundEndPressed			= false;
	m_iShowWhichTechInView3		= 0;
	m_iShowWhichShipInfoInView3 = 0;
	m_NumberOfTheShipInArray	= -1;
	m_iNumberOfFleetShip		= -1;
	m_iNumberOfTheShipInFleet	= -1;
	for (int i = HUMAN; i <= DOMINION; i++)
		m_iSelectedView[i] = 1;	// 1 steht für GalaxieView
	
	if (CSector::m_Font)
	{
		delete CSector::m_Font;
		CSector::m_Font = NULL;
	}
	if (CSector::m_NameGenerator)
	{
		delete CSector::m_NameGenerator;
		CSector::m_NameGenerator = NULL;
	}
	CSector::InitNameGenerator();
	
	CMajor* pPlayer = GetPlayersRace();
	ASSERT(pPlayer);
	network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());

	if (m_pIniLoader->GetValue("MUSIC"))
		m_pSoundManager->StartMusic(client, m_pIniLoader->GetFloatValue("MUSICVOLUME"));
	if (!m_pIniLoader->GetValue("SOUND"))
		m_pSoundManager->SetSoundMasterVolume(NULL);
	else
		m_pSoundManager->SetSoundMasterVolume(0.5f);
	MYTRACE(MT::LEVEL_INFO, "Init sound ready...\n");
	// Mal Testweise paar Truppen anlegen
	BYTE techs[6];
	memset(techs, 0, sizeof(*techs)*6);
	USHORT res[5] = {50};	
	CTroopInfo* troopInfo = new CTroopInfo("Sicherheitskräfte","Dies ist eine relativ schwache Einheit der Terranischen Konföderation. Diese Einheiten kämpfen mit modernen Projektielwaffen, sind nicht besonders stark gepanzert und haben keine militärische Ausbildung genossen.",9,10,techs,res,180,0,"MAJOR1",2500,1);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Heckenschützen","Dies ist eine relativ schwache Einheit des Rotharianischen Sternenverbunds. Diese Einheiten bevorzugen eine hinterhältige Taktik, um ihre Gegner auszuschalten. Leider ist ihre militärische Grundausbildung sehr kurz.",8,10,techs,res,160,1,"MAJOR4",2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Khayrin Krieger","Dies ist eine relativ schwache Einheit des Khayrin Imperiums. Diese Einheiten kämpfen mit messerscharfen Nahkampfwaffen. Dies ist aber auch schon der einzige Nachteil in einem Kampf.",12,10,techs,res,240,2,"MAJOR3",2500,2);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Hanuhr Raketenmänner","Ein zusammengewürfelter Haufen von Hanuhr, die mit gefährlichen Raketenabschußanlagen ausgerüstet sind. Leider ist ihre militärische Ausbildung mangelhaft.",5,10,techs,res,100,3,"MAJOR2",1500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Invasionsstoßtrupp","Dies ist eine relativ schwache Einheit der Cartarer Invasoren. Trotzdem sind sie hervorragend für militärische Systeminvasionen geeignet.",15,10,techs,res,250,4,"MAJOR5",2500,3);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Da'unor Sniper","Dies ist eine relativ schwache Einheit der Omega Allianz. Diese Einheiten kämpfen mit Präzisionsgewehren, sind nicht besonders stark gepanzert und haben keine militärische Ausbildung genossen.",10,10,techs,res,200,5,"MAJOR6",2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;

	// Spezialtruppen für den Cartarer anlegen
	memset(techs, 255, sizeof(*techs)*6);
	troopInfo = new CTroopInfo("Alpha Invasoren","Diese Einheit ist eine Spezialeinhait der Cartarer Invasoren. Durch ihre perfekte Ausbildung sind sie sofort in der Lage, feindliche Systeme invasieren zu können.",25,0,techs,res,2500,6,"MAJOR5",2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;

	if (GetRaceKO("MAJOR5") != CPoint(-1,-1))
	{
		// hier werden auch die Spezialtruppen der Cartarer ins System gesteckt
		BuildTroop(6, GetRaceKO("MAJOR5"));
		BuildTroop(6, GetRaceKO("MAJOR5"));
	}
	
	return TRUE;
}

/// Funktion gibt die Rassen-ID der lokalen Spielerrasse zurück.
/// @return Zeiger auf Majorrace-Rassenobjekt
CMajor* CBotf2Doc::GetPlayersRace(void) const
{	
	// so stand es zuvor drin
	// return client.GetClientRace();
	
	// zuerst muss eine Netzwerknummer, also RACE1 bis RACE6 (1-6)
	// auf eine bestimmte Rassen-ID gemappt werden. Dies ist dann
	// die Rassen-ID.
	CString s = m_pRaceCtrl->GetMappedRaceID((network::RACE)client.GetClientRace());
	CMajor* pPlayersRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(s));
/*	ASSERT(pPlayersRace);

	if (!pPlayersRace)
	{
		CString sError;
		sError.Format("Error in CBotf2Doc::GetPlayersRace(): Could not map to players race.\nClient Nr. is %s", s);
		//AfxMessageBox(sError);
		//exit(1);
	}*/
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
		ar << m_ptKO;
								
		// Hauptrassen-Koordinaten speichern
		ar << m_mRaceKO.size();
		for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); it++)
			ar << it->first << it->second.first << it->second.second;
		
		for (int i = 0; i < 7; i++)
		{		
			ar << m_iSelectedView[i];
		}

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
		for (map<CString, short>::const_iterator it = mMoralMap->begin(); it != mMoralMap->end(); it++)
			ar << it->first << it->second;
	}
	else
	{
		int number;
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		ar >> m_iRound;
		ar >> m_fStardate;
		ar >> m_ptKO;
										
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

		for (int i = 0; i < 7; i++)
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
	
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (ar.IsLoading())
				m_System[x][y].ResetSystem();
			m_Sector[x][y].Serialize(ar);
			if (m_Sector[x][y].GetSunSystem())
			{
				if (m_Sector[x][y].GetOwnerOfSector() != "" || m_Sector[x][y].GetColonyOwner() != "" || m_Sector[x][y].GetMinorRace() == TRUE)
					m_System[x][y].Serialize(ar);
			}		
		}
	
	CMoralObserver::SerializeStatics(ar);
	CSector::m_NameGenerator->Serialize(ar);
	
	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	message.Serialize(ar);
	m_Statistics.Serialize(ar);

	m_pRaceCtrl->Serialize(ar);
	
	if (ar.IsLoading())
	{
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			it->second->GetEmpire()->GenerateSystemList(m_System, m_Sector);		
	
		GenerateStarmap();		
	}	
}

/// Serialisiert die Daten, welche am Anfang des Spiels einmal gesendet werden müssen.
void CBotf2Doc::SerializeBeginGameData(CArchive& ar)
{
	// senden auf Serverseite
	if (ar.IsStoring())
	{
		// Hauptrassen-Koordinaten senden
		ar << m_mRaceKO.size();
		for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); it++)
			ar << it->first << it->second.first << it->second.second;

		ar << BuildingInfo.GetSize();
		for (int i = 0; i < BuildingInfo.GetSize(); i++)
			BuildingInfo.GetAt(i).Serialize(ar);
	}
	// Empfangen auf Clientseite
	else
	{
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
	}
	
	CMoralObserver::SerializeStatics(ar);	
}

void CBotf2Doc::SerializeNextRoundData(CArchive &ar)
{
	// TODO Daten der nächsten Runde serialisieren; auf Server-Seite senden, auf Client-Seite empfangen
	if (ar.IsStoring())
	{
		// Server-Dokument
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
		ar << m_iRound;
		ar << m_fStardate;
		for (int i = 0; i < 7; i++)
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
		for (map<CString, short>::const_iterator it = mMoralMap->begin(); it != mMoralMap->end(); it++)
			ar << it->first << it->second;
	}
	else
	{
		// Client-Dokument
		int number;
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		ar >> m_iRound;
		ar >> m_fStardate;
		for (int i = 0; i < 7; i++)
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

	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (ar.IsLoading())
				m_System[x][y].ResetSystem();
			m_Sector[x][y].Serialize(ar);
			if (m_Sector[x][y].GetSunSystem())
			{
				if (m_Sector[x][y].GetOwnerOfSector() != "" || m_Sector[x][y].GetColonyOwner() != "" || m_Sector[x][y].GetMinorRace() == TRUE)
					m_System[x][y].Serialize(ar);
			}
		}

	m_pRaceCtrl->Serialize(ar);	
	for (int i = HUMAN; i <= DOMINION; i++)
		m_SoundMessages[i].Serialize(ar);
	
	m_GlobalBuildings.Serialize(ar);
	message.Serialize(ar);
	m_Statistics.Serialize(ar);

	if (ar.IsLoading())
	{
		CSmallInfoView::SetPlanet(NULL);
		GenerateStarmap();

		CMajor* pPlayer = GetPlayersRace();
		ASSERT(pPlayer);
		network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());
		
		// Ausgehend vom Pfad des Schiffes den Sektoren mitteilen, das durch sie ein Schiff fliegt
		for (int y = 0; y < m_ShipArray.GetSize(); y++)
			if (m_ShipArray.GetAt(y).GetOwnerOfShip() == pPlayer->GetRaceID())
				for (int i = 0; i < m_ShipArray[y].GetPath()->GetSize(); i++)
					m_Sector[m_ShipArray[y].GetPath()->GetAt(i).x][m_ShipArray[y].GetPath()->GetAt(i).y].AddShipPathPoints(1);
		// Sprachmeldungen an den Soundmanager schicken
		m_pSoundManager->ClearMessages();
		for (int i = 0; i < m_SoundMessages[client].GetSize(); i++)
		{
			SNDMGR_MESSAGEENTRY* entry = &m_SoundMessages[client].GetAt(i);
			m_pSoundManager->AddMessage(entry->nMessage, entry->nRace, entry->nPriority, entry->fVolume);
		}

		// Systemliste der Imperien erstellen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			it->second->GetEmpire()->GenerateSystemList(m_System, m_Sector);
	}
}

void CBotf2Doc::SerializeEndOfRoundData(CArchive &ar, network::RACE race)
{
	if (ar.IsStoring())
	{
		CMajor* pPlayer = GetPlayersRace();
		// Client-Dokument
		// Anzahl der eigenen Schiffsinfoobjekte ermitteln
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pPlayer->GetRaceShipNumber())
				m_ShipInfoArray.GetAt(i).Serialize(ar);
		
		int number = 0;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == pPlayer->GetRaceID())
				number++;
		ar << number;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == pPlayer->GetRaceID())
				m_ShipArray.GetAt(i).Serialize(ar);
		
		number = 0;
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if (m_Sector[x][y].GetSunSystem())
				{
					if (m_Sector[x][y].GetOwnerOfSector() == pPlayer->GetRaceID() && m_System[x][y].GetOwnerOfSystem() == pPlayer->GetRaceID())
						number++;
				}
			}		
		ar << number;
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if (m_Sector[x][y].GetSunSystem())
				{
					if (m_System[x][y].GetOwnerOfSystem() == pPlayer->GetRaceID())
					{
						ar << CPoint(x,y);
						m_System[x][y].Serialize(ar);
					}
				}
			}		
		network::RACE client = m_pRaceCtrl->GetMappedClientID(pPlayer->GetRaceID());
		pPlayer->Serialize(ar);
		// aktuelle View mit zum Server senden
		ar << m_iSelectedView[client];
	}
	else
	{
		// Server-Dokument
		CString sMajorID = m_pRaceCtrl->GetMappedRaceID(race);
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
			m_System[p.x][p.y].Serialize(ar);
		}
		
		pMajor->Serialize(ar);
		ar >> m_iSelectedView[race];
	}	
}

/// Funktion gibt die Koordinate des Hauptsystems einer Majorrace zurück.
/// @param sMajor Rassen-ID
/// @return Koordinate auf der Galaxiemap
CPoint CBotf2Doc::GetRaceKO(const CString& sMajorID)
{
	if (m_mRaceKO.find(sMajorID) == m_mRaceKO.end())
		return CPoint(-1,-1);
	else	
		return CPoint(m_mRaceKO[sMajorID].first, m_mRaceKO[sMajorID].second);
}

void CBotf2Doc::SetKO(int m, int n)
{
	m_ptKO = CPoint(m, n);
	CSmallInfoView::SetPlanet(NULL);

	if (GetMainFrame()->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW)
		GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));	
}

void CBotf2Doc::SetNumberOfTheShipInArray(int NumberOfTheShipInArray)
{
	m_NumberOfTheShipInArray = NumberOfTheShipInArray;
	((CGalaxyMenuView*)GetMainFrame()->GetView(RUNTIME_CLASS(CGalaxyMenuView)))->SetNewShipPath();
	CSmallInfoView::SetShip(&m_ShipArray.GetAt(NumberOfTheShipInArray));
}

void CBotf2Doc::SetNumberOfFleetShip(int NumberOfFleetShip)
{
	m_iNumberOfFleetShip = NumberOfFleetShip;
	CSmallInfoView::SetShip(&m_ShipArray.GetAt(NumberOfFleetShip));	
}

void CBotf2Doc::SetNumberOfTheShipInFleet(int NumberOfTheShipInFleet)
{
	m_iNumberOfTheShipInFleet = NumberOfTheShipInFleet;
	if (NumberOfTheShipInFleet > 0)
		CSmallInfoView::SetShip(m_ShipArray.GetAt(m_iNumberOfFleetShip).GetFleet()->GetPointerOfShipFromFleet(NumberOfTheShipInFleet - 1));
	else if (NumberOfTheShipInFleet == 0)
		CSmallInfoView::SetShip(&m_ShipArray.GetAt(m_iNumberOfFleetShip));
}

void CBotf2Doc::DoViewWorkOnNewRound()
{
	std::map<CWnd *, UINT>* views = &GetMainFrame()->GetSplitterWindow()->views;		
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); it++)
	{		
		if (it->second == GALAXY_VIEW)
			((CGalaxyMenuView*)(it->first))->OnNewRound();
		else if (IS_MAIN_VIEW(it->second))
			((CMainBaseView*)(it->first))->OnNewRound();
		else if (IS_BOTTOM_VIEW(it->second))
			((CBottomBaseView*)(it->first))->OnNewRound();		
	}
}

// Generiert ein neues Spiel
void CBotf2Doc::PrepareData()
{
	MYTRACE(MT::LEVEL_INFO, "Begin preparing game data...\n");
	if (CSector::m_NameGenerator)
	{
		delete CSector::m_NameGenerator;
		CSector::m_NameGenerator = NULL;
	}
	
	CSector::InitNameGenerator();
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
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		{
			network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
			// wird das Imperium von einem Menschen oder vom Computer gespielt
			if (client != network::RACE_NONE && server.IsPlayedByClient(client))
				it->second->SetHumanPlayer(true);
			else			
				it->second->SetHumanPlayer(false);
		}

		
		// ALPHA5 alle Rassen untereinander bekanntgeben
		map<CString, CRace*>* pmRaces = m_pRaceCtrl->GetRaces();
		for (map<CString, CRace*>::iterator it = pmRaces->begin(); it != pmRaces->end(); it++)
			for (map<CString, CRace*>::const_iterator jt = pmRaces->begin(); jt != pmRaces->end(); jt++)
				if (it->first != jt->first)
				{
					it->second->SetIsRaceContacted(jt->first, true);
				}
		
		m_iRound = 1;
		
		// Generierungssektornamenklasse wieder neu starten
		m_ShipArray.RemoveAll();
		m_ShipInfoArray.RemoveAll();
		
		ReadBuildingInfosFromFile();	// Gebäude einlesen aus data-Datei
		ReadShipInfosFromFile();		// Schiffe einlesen aus data-Datei
		
		// Schiffsnamen einlesen
		m_GenShipName.Init(this);
		
		// Werte für Moraländerungen auf verschiedene Ereignisse laden
		CMoralObserver::InitMoralMatrix();
		
		// Sektoren generieren
		GenerateGalaxy();
		ApplyShipsAtStartup();
		ApplyBuildingsAtStartup();
		
		MYTRACE(MT::LEVEL_INFO, "Preparing game data ready...\n");
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
				int number=m_Sector[x][y].GetNumberOfPlanets();
				for (int i=0;i<number;i++)
				{
					CPlanet* planet = m_Sector[x][y].GetPlanet(i);
					temp+=planet->GetMaxHabitant();
					hab = temp;
				}
				if (habis < hab)
				{
					habis = hab;
					poi.x = x; poi.y = y;
				}
			}
		s.Format("Größtes System ist %s mit %lf Bevölkerung\nin Sektor %d:%d",m_Sector[poi.x][poi.y].GetName(),habis,poi.x,poi.y);
		AfxMessageBox(s);
		*/		
	}
	// wenn geladen wird
	else
	{
		// Spieler den Majors zuweisen
		map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			// Alle Werte der Systemklasse wieder auf NULL setzen
			m_System[x][y].ResetSystem();
			// Alle Werte der Sektorklasse wieder auf NULL setzen
			m_Sector[x][y].Reset();
			m_Sector[x][y].SetKO(x,y);
		}

	// Die sechs Hauptrassen werden zufällig auf der Karte verteilt. Dabei ist aber zu beachten, dass die Entfernungen
	// zwischen den Systemen groß genug sind. Außerdem müssen um jedes der Hauptsysteme zwei weitere Systeme liegen.
	// Diese werden wenn nötig auch generiert.
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	// minimaler Abstand der Majorraceheimatsysteme
	int nMinDiff = 14 - pmMajors->size();
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
			
			for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); jt++)
			{
				if (it->first != jt->first && GetRaceKO(jt->first) != CPoint(-1,-1) && abs(GetRaceKO(it->first).x - GetRaceKO(jt->first).x) < nMinDiff && abs(GetRaceKO(it->first).y - GetRaceKO(jt->first).y) < nMinDiff)
					m_mRaceKO[it->first] = pair<int,int>(-1,-1);
			}
			// Abbruchbedingung
			if (++nCount > 250)
				bRestart = true;
		}
		while (GetRaceKO(it->first) == CPoint(-1,-1) && bRestart == false);
		
		// nächsten Major auswählen
		it++;		

		// Es konnte kein Sektor nach 250 Durchläufen gefunden werden, welcher der obigen if Bedingung genügt, so
		// wird die for Schleife nochmal neu gestartet.
		if (bRestart)
		{
			m_mRaceKO.clear();
			it = pmMajors->begin();
		}
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		CPoint raceKO = GetRaceKO(it->first);
		
		m_Sector[raceKO.x][raceKO.y].SetSectorsName(pMajor->GetHomesystemName());
		m_Sector[raceKO.x][raceKO.y].SetSunSystem(TRUE);		
		m_Sector[raceKO.x][raceKO.y].SetFullKnown(it->first);
		m_Sector[raceKO.x][raceKO.y].SetOwned(TRUE);
		m_Sector[raceKO.x][raceKO.y].SetOwnerOfSector(it->first);
		m_Sector[raceKO.x][raceKO.y].SetColonyOwner(it->first);
		m_Sector[raceKO.x][raceKO.y].CreatePlanets(it->first);
		m_System[raceKO.x][raceKO.y].SetOwnerOfSystem(it->first);
		m_System[raceKO.x][raceKO.y].SetRessourceStore(TITAN, 1000);
		m_System[raceKO.x][raceKO.y].SetRessourceStore(DILITHIUM, 3);
		
		// Zwei Sonnensysteme in unmittelbarer Umgebung des Heimatsystems anlegen
		BYTE nextSunSystems = 0;
		while (nextSunSystems < 2)
		{
			// Punkt mit Koordinaten zwischen -1 und +1 generieren
			CPoint ko(rand()%3 - 1, rand()%3 - 1);
			if (raceKO.x + ko.x < STARMAP_SECTORS_HCOUNT && raceKO.x + ko.x > -1
				&& raceKO.y + ko.y < STARMAP_SECTORS_VCOUNT && raceKO.y + ko.y > -1)
				if (!m_Sector[raceKO.x + ko.x][raceKO.y + ko.y].GetSunSystem())
				{
					m_Sector[raceKO.x + ko.x][raceKO.y + ko.y].GenerateSector(100,25);
					nextSunSystems++;
				}
		};

		// In einem Radius von einem Feld um das Hauptsystem die Sektoren scannen
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= 1; x++)
				if (raceKO.y + y < STARMAP_SECTORS_VCOUNT && raceKO.y + y > -1
					&& raceKO.x + x < STARMAP_SECTORS_HCOUNT && raceKO.x + x > -1)
						if (raceKO.x + x != raceKO.x || raceKO.y + y != raceKO.y)
							m_Sector[raceKO.x + x][raceKO.y + y].SetScanned(it->first);
	}

	// Vektor der verwendeten Minors, diese nehmen aktiv am Spiel teil.
	vector<CString> vUsedMinors;
	// nun die Sektoren generieren
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			// den Sektor generieren
			// die Wahrscheinlichkeiten sind abhängig von den Systemen in der unmittelbaren Nähe. Ist ein Majorrace-
			// hauptsystem in der Nähe, so wird hier kein System generiert, da diese schon weiter oben angelegt
			// wurden
			int sunSystems = 0;
			int minorRaces = 0;
			for (int j = -1; j <= 1; j++)
				for (int i = -1; i <= 1; i++)
					if (y + j < STARMAP_SECTORS_VCOUNT && y + j > -1 && x + i < STARMAP_SECTORS_HCOUNT && x + i > -1)
						if (m_Sector[x + i][y + j].GetSunSystem())
						{
							if (m_Sector[x + i][y + j].GetMinorRace())
								minorRaces++;
							sunSystems++;
							for (map<CString, pair<int, int> >::const_iterator it = m_mRaceKO.begin(); it != m_mRaceKO.end(); it++)
								if (it->second.first == x + i && it->second.second == y + j)
									sunSystems += 100;
						}
			int sunSystemProb = 35 - sunSystems * 15;
			int minorRaceProb = 32 - minorRaces * 15;
			if (minorRaceProb < 0)
				minorRaceProb = 0;
			if (sunSystemProb > 0)
				m_Sector[x][y].GenerateSector(sunSystemProb, minorRaceProb);
			if (m_Sector[x][y].GetMinorRace())
			{	
				// Nun die Minorrace parametrisieren
				CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[x][y].GetName());
				if (!pMinor)
					AfxMessageBox("Error in function CBotf2Doc::GenerateGalaxy(): Could not create Minorrace");
				else
				{
					pMinor->SetRaceKO(CPoint(x,y));
					m_Sector[x][y].SetOwnerOfSector(pMinor->GetRaceID());
					m_System[x][y].SetOwnerOfSystem("");
					vUsedMinors.push_back(pMinor->GetRaceID());
				}
			}
		}

		// nun können alle nicht verwendeten Minors entfernt werden
		vector<CString> vDelMinors;
		map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();
		for (map<CString, CMinor*>::iterator it = pmMinors->begin(); it != pmMinors->end(); it++)
		{
			bool bInVec = false;
			for (UINT i = 0; i < vUsedMinors.size(); i++)
			{
				if (it->first == vUsedMinors[i])
				{
					bInVec = true;
					break;
				}
			}
			if (!bInVec)
				vDelMinors.push_back(it->first);			
		}
		for (UINT i = 0; i < vDelMinors.size(); i++)
			m_pRaceCtrl->RemoveRace(vDelMinors[i]);
}

void CBotf2Doc::NextRound()
{
	MYTRACE(MT::LEVEL_INFO, "\nNEXT ROUND (round: %d)\n", GetCurrentRound());

	srand((unsigned)time(NULL));
	/* Für später: In den Objekten der Klasse CEmpire, werden die Messages an das Imperium gespeichert.
	   Bei diplomatischen Auswirkungen werden diese Messages unter anderem generiert und auch an andere
	   Imperien versandt. So weit so gut. Nachher werden diese Empireobjekte an die Spieler im Multiplayer
	   versandt. So weit so gut, jeder hat seine Messages. Wenn aber ne neue Runde beginnt muß jeder wieder
	   alle Empireobjekte zurückschicken, auch die der anderen Rassen mit den neuen Messages an das Imperium.
	   Dabei dürfen wir die Objekte nicht einfach überschreiben, sondern alle Messages von den Imperien, zu
	   denen dieses Objekt grad nicht gehört, sammeln und zu den Messages, die in dem Objekt sind, welches
	   auch dem "richtigen" Spieler gehört hinzufügen. Genauso verhält es sich mit den "Answers" von
	   anderen Majorraces an dieses Imperium.
	   So ähnlich gilt dies auch bei der Diplomatieklasse. Wenn ich einer anderen Majorrace ein Angebot
	   mache, dann wird dieser Eintrag auch in der Diplomatieklasse der anderen Majorrace gemacht.
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		m_SoundMessages[i].RemoveAll();
	
	m_pSectorAI->Clear();
	m_pSectorAI->CalculateDangers();
	m_pSectorAI->CalcualteSectorPriorities();
	
	CShipAI* pShipAI = new CShipAI(this);
	pShipAI->CalculateShipOrders(m_pSectorAI);
	
	m_pAIPrios->Clear();
	m_pAIPrios->CalcShipPrios(m_pSectorAI);
	m_pAIPrios->GetIntelAI()->CalcIntelligence(this);

	// alle Statistiken des Spiels berechnen (erst nachdem die Sector-AI berechnet wurde! Nimmt von dort gleich die Schiffsstärken)
	m_Statistics.CalcStats(this);

	this->CalcPreDataForNextRound();
	this->CalcSystemAttack();
	this->CalcIntelligence();
	this->CalcResearch();	
	this->CalcDiplomacy();
	
	// alten Creditbestand festhalten
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	map<CString, long> mOldCredits;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		mOldCredits[it->first] = pMajor->GetEmpire()->GetLatinum();
	}
	
	this->CalcOldRoundData();
	this->CalcNewRoundData();	
	this->CalcTrade();	

	// Hier Schiffsbewegung und alles was dazugehört	
	CSmallInfoView::SetShipInfo(false);
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		if (m_iSelectedView[i] == 8)
			m_iSelectedView[i] = 1;
	
	m_NumberOfTheShipInArray = -1;
	m_iNumberOfFleetShip = -1;
	m_iNumberOfTheShipInFleet = -1;

	// hier einmal das ganze Feld durchgehen und nach Stationen suchen
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		// Zuerst abfragen ob in dem System ein Außenposten/Sternbasis steht, die dem Sector bekanntgeben
		// Die normalen Schiffe werden erst ganz am Ende, nach deren Bewegung dem Sektor bekanntgegeben
		if (m_ShipArray[y].GetShipType() == OUTPOST)
			m_Sector[m_ShipArray[y].GetKO().x][m_ShipArray[y].GetKO().y].SetOutpost(TRUE, m_ShipArray[y].GetOwnerOfShip());
		else if (m_ShipArray[y].GetShipType() == STARBASE)
			m_Sector[m_ShipArray[y].GetKO().x][m_ShipArray[y].GetKO().y].SetStarbase(TRUE, m_ShipArray[y].GetOwnerOfShip());
	}	
	this->CalcShipOrders();
	this->CalcShipMovement();
	this->CalcShipCombat();
	this->CalcShipEffects();
	
	// Nachdem Außenposten und Sternbasen auch den Sektoren wieder bekanntgegeben wurden, können wir die Besitzerpunkte
	// für die Sektoren berechnen.
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CString sID = it->first;
		for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)			
			{
				// Befindet sich ein Außenposten oder ein System in einem der umliegenden Sektoren, so bekommt der
				// Sektor einen Besitzerpunkt. Bei einer Sternbasis sind es sogar zwei Besitzerpunkte.
				BYTE ownerPoints = 0;
				if (m_System[x][y].GetOwnerOfSystem() == sID)
					ownerPoints += 1;
				if (m_Sector[x][y].GetOutpost(sID) == TRUE)
					ownerPoints += 1;
				if (m_Sector[x][y].GetStarbase(sID) == TRUE)
					ownerPoints += 2;
				if (ownerPoints > 0)
				{
					for (int j = -1; j <= 1; j++)
						for (int i = -1; i <= 1; i++)
							if ((y+j < STARMAP_SECTORS_VCOUNT && y+j > -1) && (x+i < STARMAP_SECTORS_HCOUNT && x+i > -1))
									m_Sector[x+i][y+j].AddOwnerPoints(ownerPoints, sID);
					
					// in vertikaler und horizontaler Ausrichtung gibt es sogar 2 Felder vom Sector entfernt noch
					// Besitzerpunkte
					if (x-2 >= 0)
						m_Sector[x-2][y].AddOwnerPoints(ownerPoints, sID);
					if (x+2 < STARMAP_SECTORS_HCOUNT)
						m_Sector[x+2][y].AddOwnerPoints(ownerPoints, sID);
					if (y-2 >= 0)
						m_Sector[x][y-2].AddOwnerPoints(ownerPoints, sID);
					if (y+2 < STARMAP_SECTORS_VCOUNT)
						m_Sector[x][y+2].AddOwnerPoints(ownerPoints, sID);
				}
			}
		}
		// hier das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse des jeweiligen Imperiums
		// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
		// Dies gilt nur für Majorsraces.
		CMajor* pMajor = it->second;
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		{
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				// nur aktuell baubare Schiffe dürfen überprüft werden, wenn wir die Beamwaffen checken
				if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
				{
					// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
					pMajor->GetWeaponObserver()->CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
					pMajor->GetWeaponObserver()->CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
				}
			}
		}
	}
	
	vector<CString> vDelMajors;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		pMajor->GetEmpire()->GenerateSystemList(m_System, m_Sector);
		pMajor->GetEmpire()->SetNumberOfSystems(pMajor->GetEmpire()->GetSystemList()->GetSize());
		
		// Wenn das Imperium keine Systeme mehr besitzt, so wird es für alle anderen Rassen auf unbekannt gestellt.
		// Scheidet somit aus dem Spiel aus
		if (pMajor->GetEmpire()->GetNumberOfSystems() == NULL)
		{
			vDelMajors.push_back(pMajor->GetRaceID());
			// alle anderen Rassen durchgehen und die vernichtete Rasse aus deren Maps entfernen
			map<CString, CRace*>* mRaces = m_pRaceCtrl->GetRaces();
			for (map<CString, CRace*>::const_iterator itt = mRaces->begin(); itt != mRaces->end(); itt++)
			{
				if (pMajor->GetRaceID() == itt->first)
					continue;

				CRace* pLivingRace = itt->second;
				
				pLivingRace->SetIsRaceContacted(pMajor->GetRaceID(), false);				
				pLivingRace->SetAgreement(pMajor->GetRaceID(), NO_AGREEMENT);

				pMajor->SetIsRaceContacted(pLivingRace->GetRaceID(), false);				
				pMajor->SetAgreement(pLivingRace->GetRaceID(), NO_AGREEMENT);

				if (pLivingRace->GetType() == MAJOR)
				{
					((CMajor*)pLivingRace)->SetDefencePact(pMajor->GetRaceID(), false);
					pMajor->SetDefencePact(pLivingRace->GetRaceID(), false);
				}				
			}
			
			// Alle Schiffe entfernen
			for (int j = 0; j < m_ShipArray.GetSize(); j++)
				if (m_ShipArray.GetAt(j).GetOwnerOfShip() == pMajor->GetRaceID())
				{
					// Alle noch "lebenden" Schiffe aus der Schiffshistory ebenfalls als zerstört ansehen
					pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[j],
								m_Sector[m_ShipArray[j].GetKO().x][m_ShipArray[j].GetKO().y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("UNKNOWN"), CResourceManager::GetString("DESTROYED"));
					m_ShipArray.RemoveAt(j--);
				}					
		}
		
		for (UINT i = 0; i < vDelMajors.size(); i++)
			m_pRaceCtrl->RemoveRace(vDelMajors[i]);

		// Moralveränderungen aufgrund möglicher Ereignisse berechnen. Erst nach der Schiffsbewegung und allem anderen
		pMajor->GetMoralObserver()->CalculateEvents(m_System, pMajor->GetRaceID(), pMajor->GetRaceMoralNumber());
		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> kein Abzug beim Stellaren Lager
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetFieldStatus(2) == RESEARCHED)
			pMajor->GetEmpire()->GetGlobalStorage()->SetLosing(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetBonus(2));
		// Ressourcentransfers im globalen Lager vornehmen
		pMajor->GetEmpire()->GetGlobalStorage()->Calculate(m_System);
		pMajor->GetEmpire()->GetGlobalStorage()->SetMaxTakenRessources(1000 * pMajor->GetEmpire()->GetNumberOfSystems());
		// Befindet sich irgendeine Ressource im globalen Lager, bekommt der Spieler eine Imperiumsmeldung
		if (pMajor->GetEmpire()->GetGlobalStorage()->IsFilled())
		{
			CString s = CResourceManager::GetString("RESOURCES_IN_GLOBAL_STORAGE");
			message.GenerateMessage(s, ECONOMY, "", NULL, FALSE, 4);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
			{
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
		}
		// Latinumänderung berechnen
		short costs = pMajor->GetEmpire()->GetPopSupportCosts() - pMajor->GetEmpire()->GetShipCosts();
		if (costs < 0)
			pMajor->GetEmpire()->SetLatinum(costs);
		pMajor->GetEmpire()->SetLatinumChange((int)(pMajor->GetEmpire()->GetLatinum() - mOldCredits[pMajor->GetRaceID()]));

		// Schiffsunterstützungskosten berechnen
		USHORT shipCosts = 0;
		for (int t = 0; t < m_ShipArray.GetSize(); t++)
			if (m_ShipArray.GetAt(t).GetOwnerOfShip() == pMajor->GetRaceID())
				shipCosts += m_ShipArray.GetAt(t).GetMaintenanceCosts();
		pMajor->GetEmpire()->SetShipCosts(shipCosts);
	}

	// Jetzt die Besitzer berechnen und die Variablen, welche nächste Runde auch angezeigt werden sollen.
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			m_Sector[x][y].CalculateOwner(m_System[x][y].GetOwnerOfSystem());
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != "")
			{
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_System[x][y].GetOwnerOfSystem()));
				if (!pMajor || pMajor->GetType() != MAJOR)
					continue;

				m_System[x][y].CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_Sector[x][y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
				// alle produzierten FP und SP der Imperien berechnen und zuweisen
				int currentPoints;
				currentPoints = m_System[x][y].GetProduction()->GetResearchProd();
				pMajor->GetEmpire()->AddFP(currentPoints);
				currentPoints = m_System[x][y].GetProduction()->GetSecurityProd();
				pMajor->GetEmpire()->AddSP(currentPoints);
			}
		}
	
	if (m_pIniLoader->GetValue("AUTOSAVE"))
		DoSave(*((CBotf2App*)AfxGetApp())->GetPath() + "auto.sav", FALSE);
	SetModifiedFlag();
	
	delete pShipAI;	
}

void CBotf2Doc::ApplyShipsAtStartup()
{
	// Name des zu öffnenden Files 
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Ships\\StartShips.data";
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
			BOOLEAN found = FALSE;
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			{
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					// Sektornamen gefunden
					if (m_Sector[x][y].GetName(TRUE) == s)
					{
						// Schiffsklassen durchgehen, die dort stationiert werden sollen
						while (pos < csInput.GetLength())
						{
							s = csInput.Tokenize(",", pos);
							BOOLEAN foundShip = FALSE;
							for (int j = 0; j < m_ShipInfoArray.GetSize(); j++)
							{
								// Wurde eine entsprechende Schiffsklasse gefunden, diese bauen
								if (m_ShipInfoArray.GetAt(j).GetShipClass() == s)
								{
									foundShip = TRUE;
									BuildShip(m_ShipInfoArray.GetAt(j).GetID()-10000, CPoint(x,y), sOwner);
								}
							}
							// Wurde die Schiffsklasse nicht gefunden, sprich es ist ein Fehler in der Datei
							if (!foundShip)
							{
								CString shipClass;
								shipClass.Format("Could not find ship class \"%s\"\nPlease check your StartShips.data file!", s);
								AfxMessageBox(shipClass);
							}
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

void CBotf2Doc::ApplyBuildingsAtStartup()
{
	// Name des zu öffnenden Files 
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Buildings\\StartBuildings.data";
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
					if (m_Sector[x][y].GetName() == s)
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
	//////// kann wieder weg /////////////////////////
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Starmaps berechnen, sofern diese noch nicht existieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		pMajor->CreateStarmap();
	}

	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_Sector[x][y].GetSunSystem() == TRUE)
			{
				m_System[x][y].SetHabitants(m_Sector[x][y].GetCurrentHabitants());
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
					if (m_System[x][y].GetOwnerOfSystem() == it->first)
					{
						CMajor* pMajor = it->second;
						ASSERT(pMajor);
						// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
						m_System[x][y].CalculateNumberOfWorkbuildings(&this->BuildingInfo);						
						m_System[x][y].SetWorkersIntoBuildings();
						m_System[x][y].CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_Sector[x][y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
						// alle produzierten FP und SP der Imperien berechnen und zuweisen
						int currentPoints;
						currentPoints = m_System[x][y].GetProduction()->GetResearchProd();
						pMajor->GetEmpire()->AddFP(currentPoints);
						currentPoints = m_System[x][y].GetProduction()->GetSecurityProd();
						pMajor->GetEmpire()->AddSP(currentPoints);
					}
				for (int i = 0; i < m_System[x][y].GetAllBuildings()->GetSize(); i++)
						m_GlobalBuildings.AddGlobalBuilding(m_System[x][y].GetAllBuildings()->GetAt(i).GetRunningNumber());
						
			}

	this->CalcNewRoundData();
	this->CalcShipEffects();	
	
	// hier das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse des jeweiligen Imperiums
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	// Dies gilt nur für Majorsraces.
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			{
				// nur aktuell baubare Schiffe dürfen überprüft werden, wenn wir die Beamwaffen checken
				if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
				{
					// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
					pMajor->GetWeaponObserver()->CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
					pMajor->GetWeaponObserver()->CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
				}
			}
	}
	///////////////////////////////////////////
}

void CBotf2Doc::ReadBuildingInfosFromFile()
{
	for (int i = 0; i < BuildingInfo.GetSize(); )
		BuildingInfo.RemoveAt(i);
	BuildingInfo.RemoveAll();
	CBuildingInfo info;
	USHORT i = 0;
	CString csInput;
	CString data[133];
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Buildings\\Buildings.data";		// Name des zu Öffnenden Files
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			// Daten lesen
			data[i++] = csInput;
			if (i == 133)
			{
				i = 0;
				info.SetRunningNumber(atoi(data[0]));
				info.SetOwnerOfBuilding(atoi(data[1]));
				info.SetBuildingName(data[2]);
				//info.SetBuildingName(data[3],1);
				info.SetBuildingDescription(data[4]);
				//info.SetBuildingDescription(data[5],1);
				info.SetUpgradeable(atoi(data[6]));
				info.SetGraphikFileName(data[7]);
				info.SetMaxInSystem(atoi(data[8]),atoi(data[9]));
				info.SetMaxInEmpire(atoi(data[10]),atoi(data[11]));
				info.SetOnlyHomePlanet(atoi(data[12]));
				info.SetOnlyOwnColony(atoi(data[13]));
				info.SetOnlyMinorRace(atoi(data[14]));
				info.SetOnlyTakenSystem(atoi(data[15]));
				info.SetOnlyInSystemWithName(data[16]);
				info.SetMinHabitants(atoi(data[17]));
				info.SetMinInSystem(atoi(data[18]),atoi(data[19]));
				info.SetMinInEmpire(atoi(data[20]),atoi(data[21]));
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
				info.SetNeededDilithium(atoi(data[56]));
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
				info.SetDilithiumProd(atoi(data[67]));
				info.SetLatinumProd(atoi(data[68]));
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
				info.SetDilithiumBoni(atoi(data[81]));
				info.SetAllRessourceBoni(atoi(data[82]));
				info.SetLatinumBoni(atoi(data[83]));
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
				info.SetBuildableShipTypes(atoi(data[98]));
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
	CString torpedoData[7];
	CString beamData[10];
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Ships\\Shiplist.data";				// Name des zu Öffnenden Files 
	CStdioFile file;														// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput != "$END_OF_SHIPDATA$")
			{
				if (csInput == "$Torpedo$")
				{
					weapons = 7;	// weil wir 7 Informationen für einen Torpedo brauchen
					torpedo = TRUE;
				}
				else if (csInput == "$Beam$")
				{
					weapons = 10;	// weil wir 10 Informationen für einen Beam brauchen
					torpedo = FALSE;
				}
				else if (torpedo == TRUE && weapons > 0)
				{
					torpedoData[7-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Torpedodaten hinzufügen
						CTorpedoWeapons torpedoWeapon;
						torpedoWeapon.ModifyTorpedoWeapon(atoi(torpedoData[0]),atoi(torpedoData[1]),
							atoi(torpedoData[2]),atoi(torpedoData[3]),torpedoData[4],atoi(torpedoData[5]),atoi(torpedoData[6]));
						ShipInfo.GetTorpedoWeapons()->Add(torpedoWeapon);
					}
				}
				else if (torpedo == FALSE && weapons > 0)
				{
					beamData[10-weapons] = csInput;
					weapons--;
					if (weapons == 0)
					{
						// Beamdaten hinzufügen
						CBeamWeapons beamWeapon;
						beamWeapon.ModifyBeamWeapon(atoi(beamData[0]),atoi(beamData[1]),atoi(beamData[2]),
							beamData[3],atoi(beamData[4]),atoi(beamData[5]),atoi(beamData[6]),atoi(beamData[7]),atoi(beamData[8]),atoi(beamData[9]));
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
				ShipInfo.SetShipType(atoi(data[3]));
				ShipInfo.SetShipSize(atoi(data[4]));
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
				ShipInfo.SetNeededDilithium(atoi(data[18]));
				ShipInfo.SetOnlyInSystem(data[19]);
				ShipInfo.GetHull()->ModifyHull(atoi(data[22]),atoi(data[20]),atoi(data[21]),atoi(data[23]),atoi(data[24]));
				ShipInfo.GetShield()->ModifyShield(atoi(data[25]),atoi(data[26]),atoi(data[27]));
				ShipInfo.SetSpeed(atoi(data[28]));
				ShipInfo.SetRange(atoi(data[29]));
				ShipInfo.SetScanPower(atoi(data[30]));
				ShipInfo.SetScanRange(atoi(data[31]));
				ShipInfo.SetStealthPower(atoi(data[32]));
				ShipInfo.SetStorageRoom(atoi(data[33]));
				ShipInfo.SetColonizePoints(atoi(data[34]));
				ShipInfo.SetStationBuildPoints(atoi(data[35]));
				ShipInfo.SetMaintenanceCosts(atoi(data[36]));
				ShipInfo.SetSpecial(0, atoi(data[37]));
				ShipInfo.SetSpecial(1, atoi(data[38]));
				ShipInfo.SetObsoleteShipClass(data[39]);
				ShipInfo.CalculateFinalCosts();
				ShipInfo.SetStartOrder();
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
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		for (map<CString, CMajor*>::iterator jt = pmMajors->begin(); jt != pmMajors->end(); jt++)
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
void CBotf2Doc::BuildBuilding(USHORT id, CPoint KO)
{
	CBuilding building(id);
	BOOLEAN isOnline = this->GetBuildingInfo(id).GetAllwaysOnline();
	building.SetIsBuildingOnline(isOnline);
	m_System[KO.x][KO.y].AddNewBuilding(building);	
}

void CBotf2Doc::BuildShip(int ID, CPoint KO, const CString& sOwnerID)
{
	CRace* pOwner = m_pRaceCtrl->GetRace(sOwnerID);
	if (!pOwner)
	{
		AfxMessageBox("Error in CBotf2Doc::BuildShip(): ID doesn't exist!");
		return;
	}
	
	CString sOwner = sOwnerID;
	m_ShipArray.Add((CShip)m_ShipInfoArray.GetAt(ID));
	int n = m_ShipArray.GetUpperBound();
	m_ShipArray[n].SetOwnerOfShip(sOwner);
	m_ShipArray[n].SetKO(KO);

	// Schiffsnamen vergeben
	if (m_ShipArray.GetAt(n).GetShipType() != OUTPOST && m_ShipArray.GetAt(n).GetShipType() != STARBASE)
		m_ShipArray.ElementAt(n).SetShipName(m_GenShipName.GenerateShipName(sOwner, FALSE));
	else
		m_ShipArray.ElementAt(n).SetShipName(m_GenShipName.GenerateShipName(sOwner, TRUE));

	// den Rest nur machen, wenn das Schiff durch eine Majorrace gebaut wurde
	if (pOwner->GetType() != MAJOR)
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(pOwner);
	ASSERT(pMajor);

	// mögliche Verbesserungen durch die Spezialforschung werden hier beachtet
	// Spezialforschung #0: "Waffentechnik"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetComplexStatus() == RESEARCHED)
	{
		// 20% erhoehter Phaserschaden
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(1) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetBeamWeapons()->GetSize(); i++)
			{
				USHORT oldPower = m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).GetBeamPower();
				m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).SetBeamPower(oldPower 
					+ (oldPower * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(1) / 100));
			}
		}
		// 20% erhoehte Torpedogenauigkeit
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(2) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldAcc = m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).GetAccuracy();
				m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).SetAccuracy(oldAcc
					+ (oldAcc * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(2) / 100));
			}
		}
		// 20% erhoehte Schussfreuquenz
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(3) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetBeamWeapons()->GetSize(); i++)
			{
				BYTE oldRate = m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).GetRechargeTime();
				m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).SetRechargeTime(oldRate
					- (oldRate * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(3) / 100));
			}
			for (int i = 0; i < m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldRate = m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).GetTupeFirerate();
				m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).SetTubeFirerate(oldRate
					- (oldRate * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(3) / 100));
			}
		}
	}
	// Spezialforschung #1: "Konstruktionstechnik"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetComplexStatus() == RESEARCHED)
	{
		// 20% bessere Schilde
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(1) == RESEARCHED)
		{
			UINT maxShield = m_ShipArray.GetAt(n).GetShield()->GetMaxShield();
			BYTE shieldType = m_ShipArray.GetAt(n).GetShield()->GetShieldType();
			BOOLEAN regenerative = m_ShipArray.GetAt(n).GetShield()->GetRegenerative();
			m_ShipArray.GetAt(n).GetShield()->ModifyShield((maxShield + (maxShield * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(1) / 100)), shieldType, regenerative);			
		}
		// 20% bessere Hülle
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(2) == RESEARCHED)
		{
			BOOLEAN doubleHull = m_ShipArray.GetAt(n).GetHull()->GetDoubleHull();
			BOOLEAN ablative = m_ShipArray.GetAt(n).GetHull()->GetAblative();
			BOOLEAN polarisation = m_ShipArray.GetAt(n).GetHull()->GetPolarisation();
			UINT baseHull = m_ShipArray.GetAt(n).GetHull()->GetBaseHull();
			BYTE hullMaterial = m_ShipArray.GetAt(n).GetHull()->GetHullMaterial();
			m_ShipArray.GetAt(n).GetHull()->ModifyHull(doubleHull,
				(baseHull + (baseHull * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(2) / 100)), hullMaterial,ablative,polarisation);
		}
		// 50% stärkere Scanner
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(3) == RESEARCHED)
		{
			USHORT scanPower = m_ShipArray.GetAt(n).GetScanPower();
			m_ShipArray.GetAt(n).SetScanPower(scanPower + (scanPower * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(3) / 100));
		}
	}
	// Spezialforschung #2: "allgemeine Schiffstechnik"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetComplexStatus() == RESEARCHED)
	{
		// erhoehte Reichweite für Schiffe mit zuvor kurzer Reichweite
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(1) == RESEARCHED)
		{
			if (m_ShipArray.GetAt(n).GetRange() == RANGE_SHORT)
				m_ShipArray.GetAt(n).SetRange((BYTE)(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetBonus(1)));
		}
		// erhoehte Geschwindigkeit für Schiffe mit Geschwindigkeit 1
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(2) == RESEARCHED)
		{
			if (m_ShipArray.GetAt(n).GetSpeed() == 1)
				m_ShipArray.GetAt(n).SetSpeed((BYTE)(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetBonus(2)));
		}
	}
	// Spezialforschung #3: "friedliche Schiffstechnik"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetComplexStatus() == RESEARCHED &&
		m_ShipArray.GetAt(n).GetShipType() <= COLONYSHIP)
	{
		// 25% erhoehte Transportkapazitaet
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetFieldStatus(1) == RESEARCHED)
		{
			USHORT storage = m_ShipArray.GetAt(n).GetStorageRoom();
			m_ShipArray.GetAt(n).SetStorageRoom(storage + (storage * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetBonus(1) / 100));
		}
		// keine Unterhaltskosten
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetFieldStatus(2) == RESEARCHED)
		{
			m_ShipArray.GetAt(n).SetMaintenanceCosts(0);
		}
	}
	
	for (int i = 0; i < 4; i++)
		m_ShipArray.ElementAt(n).SetTargetKO(CPoint(-1,-1), i);
	pMajor->GetShipHistory()->AddShip(&m_ShipArray.GetAt(n), m_Sector[KO.x][KO.y].GetName(), m_iRound);
}

/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
void CBotf2Doc::BuildTroop(BYTE ID, CPoint ko)
{
	// Mal Testweise paar Truppen anlegen
	m_System[ko.x][ko.y].AddTroop((CTroop*)&m_TroopInfo.GetAt(ID));
	CString sRace = m_Sector[ko.x][ko.y].GetOwnerOfSector();
	if (sRace == "")
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sRace));
	ASSERT(pMajor);

	int n = m_System[ko.x][ko.y].GetTroops()->GetUpperBound();

	// Spezialforschung #4: "Truppen"
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetComplexStatus() == RESEARCHED)
	{
		// 20% verbesserte Offensive
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(1) == RESEARCHED)
		{
			BYTE power = m_System[ko.x][ko.y].GetTroops()->GetAt(n).GetPower();
			m_System[ko.x][ko.y].GetTroops()->GetAt(n).SetPower(
				power + (power * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(1) / 100));
		}
		// 500 Erfahrungspunkte dazu -> erste Stufe
		else if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(2) == RESEARCHED)
		{
			m_System[ko.x][ko.y].GetTroops()->GetAt(n).AddExperiancePoints(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(2));
		}
	}

}

// Funktion generiert die Starmaps, so wie sie nach Rundenberechnung auch angezeigt werden können.
void CBotf2Doc::GenerateStarmap(void)
{
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Starmaps aller Majors löschen und neu anlegen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		pMajor->CreateStarmap();
	}	
	
	// Starmaps generieren
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			{
				CMajor* pMajor = it->second;
				// Wenn in dem System eine aktive Werft ist bzw. eine Station/Werft im Sektor ist
				if ((m_System[x][y].GetProduction()->GetShipYard() == TRUE && m_System[x][y].GetOwnerOfSystem() == pMajor->GetRaceID())
					|| m_Sector[x][y].GetShipPort(pMajor->GetRaceID()))
				{
					pMajor->GetStarmap()->AddBase(Sector(x,y), pMajor->GetEmpire()->GetResearch()->GetPropulsionTech());					
				}
			}
			if (m_Sector[x][y].GetSunSystem())
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
					if (m_Sector[x][y].GetOwnerOfSector() == it->first || m_Sector[x][y].GetOwnerOfSector() == "")
					{
						CMajor* pMajor = it->second;
						pMajor->GetStarmap()->AddKnownSystem(Sector(x,y));
					}
		}
	// Jetzt die Starmap abgleichen, das wir nicht auf Gebiete fliegen können, wenn wir einen NAP mit einer Rasse haben
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		map<CString, bool> mNAPRaces;
		for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); itt++)
			if (it->first != itt->first && it->second->GetAgreement(itt->first) == NON_AGGRESSION_PACT)
				mNAPRaces[itt->first] = true;		
		// interne Starmap für KI syncronisieren
		it->second->GetStarmap()->SynchronizeWithMap(m_Sector, &mNAPRaces);		
	}

	// nun die Berechnung für den Außenpostenbau vornehmen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		if (!pMajor->IsHumanPlayer())
			pMajor->GetStarmap()->SetBadAIBaseSectors(m_Sector, it->first);
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

	CMajor* pPlayer = GetPlayersRace();
	ASSERT(pPlayer);

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	// Starmaps aller Majors löschen und neu anlegen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		pMajor->CreateStarmap();
	}	

	// Minors ausbreiten, Akzeptanzpunkte berechnen und Ressourcen verbrauchen
	map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); it++)
	{
		CMinor* pMinor = it->second;
		CPoint ko = pMinor->GetRaceKO();

		if (m_System[ko.x][ko.y].GetOwnerOfSystem() == "" && m_Sector[ko.x][ko.y].GetOwnerOfSector() == pMinor->GetRaceID())
			// Vielleicht kolonisiert die Minorrace weitere Planeten in ihrem System
			if (pMinor->PerhapsExtend(this))
				// dann sind im System auch weitere Einwohner hinzugekommen
				m_System[ko.x][ko.y].SetHabitants(this->GetSector(ko).GetCurrentHabitants());
		
		// Die Punkte für längere gute Beziehungen berechnen
		pMinor->CalcAcceptancePoints(this);
		// Den Verbrauch der Rohstoffe der kleinen Rassen in jeder Runde berechnen
		pMinor->ConsumeResources(this);		
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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

	//f(x):=min(731,max(14,trunc(743-x^3)))
	m_fStardate += (float)(min(731, max(14, 743-pow((float)m_Statistics.GetAverageTechLevel(),3.0f))));
}

/// Diese Funktion berechnet den kompletten Systemangriff.
void CBotf2Doc::CalcSystemAttack()
{
	using namespace network;
	// Systemangriff durchführen
	CArray<CPoint> fightInSystem;
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
		if (m_ShipArray.GetAt(y).GetCurrentOrder() == ATTACK_SYSTEM)
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
			if (m_ShipArray[y].GetCloak() == TRUE || (m_ShipArray.GetAt(y).GetFleet() != 0 && m_ShipArray.GetAt(y).GetFleet()->CheckOrder(&m_ShipArray.GetAt(y), ATTACK_SYSTEM) == FALSE))
			{
				m_ShipArray.ElementAt(y).SetCurrentOrder(ATTACK);
				okay = FALSE;
			}

			// wenn in dem System noch kein Angriff durchgeführt wurde kann angegriffen werden
			if (okay)
			{
				CPoint p = m_ShipArray.GetAt(y).GetKO();
				// Besitzer des Systems (hier Sector wegen Minors) vor dem Systemangriff
				CString sDefender = m_Sector[p.x][p.y].GetOwnerOfSector();
				// Angreifer bzw. neuer Besitzer des Systems nach dem Angriff
				map<CString, bool> attackers;
				for (int i = 0; i < m_ShipArray.GetSize(); i++)
					if (m_ShipArray.GetAt(i).GetKO() == p && m_ShipArray.GetAt(i).GetCurrentOrder() == ATTACK_SYSTEM)
					{
						CString sOwner = m_ShipArray.GetAt(i).GetOwnerOfShip();
						if (!sOwner.IsEmpty() && m_pRaceCtrl->GetRace(sOwner)->GetType() == MAJOR)
							attackers[sOwner] = true;
					}
				
				CAttackSystem* attackSystem = new CAttackSystem();
				
				CRace* defender = NULL;
				if (!sDefender.IsEmpty())
					defender = m_pRaceCtrl->GetRace(sDefender);
				// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
				if (defender && defender->GetType() == MINOR && m_Sector[p.x][p.y].GetTakenSector() == FALSE)
				{
					attackSystem->Init(defender, &m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y], &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn eine Majorrace in dem System lebt
				else if (defender && defender->GetType() == MAJOR && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
				{
					attackSystem->Init(defender, &m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y], &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Wenn niemand mehr in dem System lebt, z.B. durch Rebellion
				else
				{
					attackSystem->Init(NULL, &m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y], &this->BuildingInfo, CTrade::GetMonopolOwner());
				}
				// Ein Systemangriff verringert die Moral in allen System, die von uns schon erobert wurden und zuvor
				// der Majorrace gehörten, deren System hier angegriffen wird
				if (!sDefender.IsEmpty())
					for (int j = 0 ; j < STARMAP_SECTORS_VCOUNT; j++)
						for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
							if (m_Sector[i][j].GetTakenSector() == TRUE && m_Sector[i][j].GetColonyOwner() == sDefender
								&& attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
								m_System[i][j].SetMoral(-rand()%5);

				// Wurde das System mit Truppen erobert, so wechselt es den Besitzer
				if (attackSystem->Calculate())
				{
					CString attacker = m_System[p.x][p.y].GetOwnerOfSystem();
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(attacker));
					ASSERT(pMajor);
					//* Der Besitzer des Systems wurde in der Calculate() Funktion schon auf den neuen Besitzer
					//* umgestellt. Der Besitzer des Sektors ist aber noch der alte, wird hier dann auf einen
					//* eventuell neuen Besitzer umgestellt.
						
						// Wenn in dem System eine Minorrace beheimatet ist und das System nicht vorher schon von jemand
						// anderem militärisch erobert wurde oder die Minorace bei einem anderen Imperium schon vermitgliedelt
						// wurde, dann muss diese zuerst die Gebäude bauen
					if (m_Sector[p.x][p.y].GetMinorRace() == TRUE && m_Sector[p.x][p.y].GetTakenSector() == FALSE && defender != NULL && defender->GetType() == MINOR)
					{
						CMinor* pMinor = dynamic_cast<CMinor*>(defender);
						ASSERT(pMinor);
						pMinor->SetSubjugated(true);
						// Wenn das System noch keiner Majorrace gehört, dann Gebäude bauen
						m_System[p.x][p.y].BuildBuildingsForMinorRace(&m_Sector[p.x][p.y], &BuildingInfo, m_Statistics.GetAverageTechLevel(), pMinor);								
						// Sektor gilt ab jetzt als erobert.
						m_Sector[p.x][p.y].SetTakenSector(TRUE);
						m_Sector[p.x][p.y].SetOwned(TRUE);
						m_Sector[p.x][p.y].SetOwnerOfSector(attacker);
						// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
						pMinor->SetRelation(attacker, -100);
						// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte						
						m_System[p.x][p.y].SetMoral(-rand()%25-10);
						// ist die Moral unter 50, so wird sie auf 50 gesetzt
						if (m_System[p.x][p.y].GetMoral() < 50)
							m_System[p.x][p.y].SetMoral(50 - m_System[p.x][p.y].GetMoral());
						CString param = m_Sector[p.x][p.y].GetName();
						CString eventText = "";
						
						// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
						// bekannte Imperien die Nachricht der Unterwerfung senden
						pMinor->GetIncomingDiplomacyNews()->clear();
						pMinor->GetOutgoingDiplomacyNews()->clear();
						map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
								message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetRaceName()), MILITARY, param, p, 0);
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
							message.GenerateMessage(eventText, MILITARY, param, p, 0, 1);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
						}
					}
					// Wenn das System einer Minorrace gehört, eingenommen wurde und somit befreit wird
					else if (m_Sector[p.x][p.y].GetMinorRace() == TRUE && m_Sector[p.x][p.y].GetTakenSector() == TRUE && defender != NULL && defender->GetType() == MAJOR)
					{
						// Die Beziehung zur der Majorrace, die das System vorher besaß verschlechtert sich
						defender->SetRelation(attacker, -rand()%50);
						// Die Beziehung zu der Minorrace verbessert sich auf Seiten des Retters
						CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[p.x][p.y].GetName());
						ASSERT(pMinor);
						pMinor->SetRelation(attacker, rand()%50);
						pMinor->SetSubjugated(false);
						// Eventnachricht an den, der das System verloren hat (erobertes Minorracesystem wieder verloren)
						CString param = m_Sector[p.x][p.y].GetName();
						CString eventText = "";

						eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(17, ((CMajor*)defender)->GetRaceMoralNumber(), param);
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, MILITARY, param, p, 0);
							((CMajor*)defender)->GetEmpire()->AddMessage(message);
							if (((CMajor*)defender)->IsHumanPlayer())
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
							message.GenerateMessage(eventText, MILITARY, param, p, 0);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
						}
						// Sektor gilt ab jetzt als nicht mehr erobert.
						m_Sector[p.x][p.y].SetTakenSector(FALSE);
						m_Sector[p.x][p.y].SetOwned(FALSE);
						m_Sector[p.x][p.y].SetOwnerOfSector(pMinor->GetRaceID());
						m_System[p.x][p.y].SetOwnerOfSystem("");
						// Moral in dem System um rand()%50+25 erhöhen
						m_System[p.x][p.y].SetMoral(rand()%50+25);
					}
					// Eine andere Majorrace besaß das System
					else
					{
						// Beziehung zum ehemaligen Besitzer verschlechtert sich
						if (defender != NULL && defender->GetRaceID() != attacker)
							defender->SetRelation(attacker, -rand()%50);
						// Wenn das System zurückerobert wird, dann gilt es als befreit
						if (m_Sector[p.x][p.y].GetColonyOwner() == attacker)
						{
							m_Sector[p.x][p.y].SetTakenSector(FALSE);
							CString param = m_Sector[p.x][p.y].GetName();
							// Eventnachricht an den Eroberer (unser ehemaliges System wieder zurückerobert)
							CString eventText = "";
							eventText = pMajor->GetMoralObserver()->AddEvent(14, pMajor->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText, MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							if (defender != NULL && defender->GetRaceID() != attacker && defender->GetType() == MAJOR)
							{
								// Eventnachricht an den, der das System verloren hat (unser erobertes System verloren)
								eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(17, ((CMajor*)defender)->GetRaceMoralNumber(), param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, MILITARY, param, p, 0);
									((CMajor*)defender)->GetEmpire()->AddMessage(message);
									if (((CMajor*)defender)->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
							}
							// Moral in dem System um rand()%25+10 erhöhen
							m_System[p.x][p.y].SetMoral(rand()%25+10);
						}
						// Handelte es sich dabei um das Heimatsystem einer Rasse
						else if (defender != NULL && defender->GetType() == MAJOR && m_Sector[p.x][p.y].GetOwnerOfSector() == defender->GetRaceID() && m_Sector[p.x][p.y].GetName() == ((CMajor*)defender)->GetHomesystemName())
						{
							// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
							CString param = m_Sector[p.x][p.y].GetName();
							CString eventText = "";
							eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(15, ((CMajor*)defender)->GetRaceMoralNumber(), param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText, MILITARY, param, p, 0);
								((CMajor*)defender)->GetEmpire()->AddMessage(message);
								if (((CMajor*)defender)->IsHumanPlayer())
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
								message.GenerateMessage(eventText, MILITARY, param, p, 0, 1);
								pMajor->GetEmpire()->AddMessage(message);
								if (pMajor->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							// Sektor gilt ab jetzt als erobert.
							m_Sector[p.x][p.y].SetTakenSector(TRUE);
							// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
							m_System[p.x][p.y].SetMoral(-rand()%25-10);
							// ist die Moral unter 50, so wird sie auf 50 gesetzt
							if (m_System[p.x][p.y].GetMoral() < 50)
								m_System[p.x][p.y].SetMoral(50 - m_System[p.x][p.y].GetMoral());
						}
						// wurde das System erobert und obere Bedingungen traten nicht ein
						else
						{
							CString param = m_Sector[p.x][p.y].GetName();
							// Hat eine andere Majorrace die Minorrace vermitgliedelt, so unterwerfen wir auch diese Minorrace
							if (m_Sector[p.x][p.y].GetMinorRace())
							{
								CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[p.x][p.y].GetName());
								ASSERT(pMinor);
								pMinor->SetSubjugated(true);
								// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
								pMinor->SetRelation(attacker, -100);
								// Sektor gilt ab jetzt als erobert.
								m_Sector[p.x][p.y].SetTakenSector(TRUE);
								// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte						
								m_System[p.x][p.y].SetMoral(-rand()%25-10);
								// ist die Moral unter 50, so wird sie auf 50 gesetzt
								if (m_System[p.x][p.y].GetMoral() < 50)
									m_System[p.x][p.y].SetMoral(50 - m_System[p.x][p.y].GetMoral());
								
								CString param = m_Sector[p.x][p.y].GetName();
								CString eventText = "";
								
								// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
								// bekannte Imperien die Nachricht der Unterwerfung senden
								pMinor->GetIncomingDiplomacyNews()->clear();
								pMinor->GetOutgoingDiplomacyNews()->clear();
								map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
								for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
										message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED", FALSE, pMinor->GetRaceName()), MILITARY, param, p, 0);
										it->second->GetEmpire()->AddMessage(message);
										if (it->second->IsHumanPlayer())
										{
											network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
											m_iSelectedView[client] = EMPIRE_VIEW;
										}
									}	
								}
								eventText = "";
								// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
								if (defender != NULL && defender->GetRaceID() != attacker && defender->GetType() == MAJOR)
									eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(16, ((CMajor*)defender)->GetRaceMoralNumber(), param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, MILITARY, param, p, 0);
									((CMajor*)defender)->GetEmpire()->AddMessage(message);
									if (((CMajor*)defender)->IsHumanPlayer())
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
									message.GenerateMessage(eventText, MILITARY, param, p, 0, 1);
									pMajor->GetEmpire()->AddMessage(message);
									if (pMajor->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(attacker);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}								
							}
						}
						m_Sector[p.x][p.y].SetOwnerOfSector(attacker);
					}
					// Variablen berechnen und Gebäude besetzen
					m_System[p.x][p.y].CalculateNumberOfWorkbuildings(&this->BuildingInfo);
					m_System[p.x][p.y].SetWorkersIntoBuildings();


					// erfolgreiches Invasionsevent für den Angreifer einfügen (sollte immer ein Major sein)
					if (!attacker.IsEmpty() && pMajor && pMajor->IsHumanPlayer())
						pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(attacker, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
			
					// Invasionsevent für den Verteidiger einfügen
					if (defender != NULL && defender->GetType() == MAJOR && ((CMajor*)defender)->IsHumanPlayer() && attackSystem->IsDefenderNotAttacker(sDefender, &attackers))
						((CMajor*)defender)->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(sDefender, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
				}
				// Wurde nur bombardiert, nicht erobert
				else
				{
					CString param = m_Sector[p.x][p.y].GetName();
					CString eventText = "";
					for (map<CString, bool>::const_iterator it = attackers.begin(); it != attackers.end(); it++)
					{
						if (!it->second)
							continue;

						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(it->first));
						ASSERT(pMajor);
						
						// Erstmal die Beziehung zu der Rasse verschlechtern, der das System gehört
						if (defender != NULL && defender->GetRaceID() != pMajor->GetRaceID())
							defender->SetRelation(pMajor->GetRaceID(), -rand()%10);
					}
					// Wenn die Bevölkerung des Systems auf NULL geschrumpft ist, dann ist dieses System verloren
					if (m_System[p.x][p.y].GetHabitants() == 0.0f)
					{
						// Bei einer Minorrace wird es komplizierter. Wenn diese keine Systeme mehr hat, dann ist diese
						// aus dem Spiel verschwunden. Alle Einträge in der Diplomatie müssen daher gelöscht werden
						if (m_Sector[p.x][p.y].GetMinorRace() && defender != NULL && defender->GetType() == MINOR)
						{
							CMinor* pMinor = dynamic_cast<CMinor*>(defender);
							ASSERT(pMinor);
							m_Sector[p.x][p.y].SetMinorRace(FALSE);
														
							defender->GetIncomingDiplomacyNews()->clear();
							defender->GetOutgoingDiplomacyNews()->clear();
							map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
									CString param = defender->GetRaceName();
									CString eventText = it->second->GetMoralObserver()->AddEvent(21, it->second->GetRaceMoralNumber(), param);
									message.GenerateMessage(eventText, MILITARY, param, p, 0);
									it->second->GetEmpire()->AddMessage(message);
									if (it->second->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
								// alle anderen Majorrassen, die diese Minor kannten, bekommen eine Nachricht über deren Vernichtung
								else if (defender->IsRaceContacted(it->first))
								{
									CString news = CResourceManager::GetString("ELIMINATE_MINOR", FALSE, defender->GetRaceName());
									message.GenerateMessage(news, SOMETHING, "", 0, 0);
									it->second->GetEmpire()->AddMessage(message);
									if (it->second->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
							}
							// Rasse aus dem Feld löschen
							m_pRaceCtrl->RemoveRace(defender->GetRaceID());
						}
						// Bei einer Majorrace verringert sich nur die Anzahl der Systeme (auch konnte dies das
						// Minorracesystem von oben gewesen sein, hier verliert es aber die betroffene Majorrace)
						if (defender != NULL && defender->GetType() == MAJOR && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
						{
							eventText = "";
							if (m_Sector[p.x][p.y].GetName() == ((CMajor*)defender)->GetHomesystemName())
							{
								// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
								param = m_Sector[p.x][p.y].GetName();
								eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(15, ((CMajor*)defender)->GetRaceMoralNumber(), param);								
							}
							else
							{
								// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
								eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(16, ((CMajor*)defender)->GetRaceMoralNumber(), param);								
							}
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText, MILITARY, param, p, 0);
								((CMajor*)defender)->GetEmpire()->AddMessage(message);
								if (((CMajor*)defender)->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
						m_System[p.x][p.y].SetOwnerOfSystem("");
						m_Sector[p.x][p.y].SetOwnerOfSector("");
						m_Sector[p.x][p.y].SetColonyOwner("");
						m_Sector[p.x][p.y].SetTakenSector(FALSE);
						m_Sector[p.x][p.y].SetOwned(FALSE);
					}
					// Bombardierung hat die Rasse nicht komplett ausgelöscht
					else
					{
						// Eventnachrichten nicht jedesmal, sondern nur wenn Gebäude vernichtet wurden oder
						// mindst. 3% der Bevölkerung vernichtet wurden
						if (attackSystem->GetDestroyedBuildings() > 0 || attackSystem->GetKilledPop() >= m_System[p.x][p.y].GetHabitants() * 0.03)
						{
							for (map<CString, bool>::const_iterator it = attackers.begin(); it != attackers.end(); it++)
							{
								if (!it->second)
									continue;

								CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(it->first));
								ASSERT(pMajor);
							
								// Wenn das System nicht durch eine Rebellion verloren ging, sondern noch irgendwem gehört
								if (defender != NULL)
									eventText = pMajor->GetMoralObserver()->AddEvent(19, pMajor->GetRaceMoralNumber(), param);
								// Wenn das System mal uns gehört hatte und durch eine Rebellion verloren ging
								else if (m_Sector[p.x][p.y].GetColonyOwner() == pMajor->GetRaceID() && defender == NULL)
									eventText = pMajor->GetMoralObserver()->AddEvent(20, pMajor->GetRaceMoralNumber(), param);
								// Eventnachricht für Agressor hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, MILITARY, param, p, 0);
									pMajor->GetEmpire()->AddMessage(message);
									if (pMajor->IsHumanPlayer())
									{
										network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}							
							}
							// Eventnachricht über Bombardierung für Verteidiger erstellen und hinzufügen
							if (defender != NULL && defender->GetType() == MAJOR && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
							{
								eventText = ((CMajor*)defender)->GetMoralObserver()->AddEvent(22, ((CMajor*)defender)->GetRaceMoralNumber(), param);
								if (((CMajor*)defender)->IsHumanPlayer())
								{
									network::RACE client = m_pRaceCtrl->GetMappedClientID(defender->GetRaceID());
									m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
						}
					}
					// Eventsgrafiken hinzufügen
					// für den/die Angreifer
					for (map<CString, bool>::const_iterator it = attackers.begin(); it != attackers.end(); it++)
					{
						if (!it->second)
							continue;

						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(it->first));
						ASSERT(pMajor);
					
						// reine Bombardierung
						if (pMajor->IsHumanPlayer())
						{
							if (!attackSystem->IsTroopsInvolved())
								pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(pMajor->GetRaceID(), "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
							// gescheitere Invasion
							else
								pMajor->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(pMajor->GetRaceID(), "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
						}

					}
					// für den Verteidiger
					if (defender != NULL && defender->GetType() == MAJOR && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						if (((CMajor*)defender)->IsHumanPlayer())
						{
							// reine Bombardierung
							if (!attackSystem->IsTroopsInvolved())
								((CMajor*)defender)->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(defender->GetRaceID(), "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
							// gescheitere Invasion
							else
								((CMajor*)defender)->GetEmpire()->GetEventMessages()->Add(new CEventBombardment(defender->GetRaceID(), "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
						}
					}					
				}

				// Nachrichten hinzufügen
				for (int i = 0; i < attackSystem->GetNews()->GetSize(); )
				{
					for (map<CString, bool>::const_iterator it = attackers.begin(); it != attackers.end(); it++)
					{
						if (!it->second)
							continue;

						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(it->first));
						ASSERT(pMajor);

						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), MILITARY, m_Sector[p.x][p.y].GetName(), p, 0);
						pMajor->GetEmpire()->AddMessage(message);
						if (pMajor->IsHumanPlayer())
						{
							network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
							m_iSelectedView[client] = EMPIRE_VIEW;
						}
					}
					if (defender != NULL && defender->GetType() == MAJOR && attackSystem->IsDefenderNotAttacker(defender->GetRaceID(), &attackers))
					{
						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), MILITARY, m_Sector[p.x][p.y].GetName(), p, 0);
						((CMajor*)defender)->GetEmpire()->AddMessage(message);
						if (((CMajor*)defender)->IsHumanPlayer())
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
					if (m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(x)->GetHull()->GetCurrentHull() < 1)
					{
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip()));
						if (pMajor)
						{
							pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[i].GetFleet()->GetShipFromFleet(x),
								m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound, 
								CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
						}
						m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
					}
				m_ShipArray[i].CheckFleet();
			}
			// Wenn das Schiff selbst zerstört wurde
			if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
			{
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip()));
				if (pMajor)
				{
					pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[i],
					m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound,
					CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
				}
				// Wenn es keine Flotte hatte, einfach rausnehmen
				if (m_ShipArray.GetAt(i).GetFleet() == 0)
					m_ShipArray.RemoveAt(i--);
				// Wenn es eine Flotte hatte, dann die Flotte auf das nächste Schiff übergeben
				else
				{
					BYTE oldOrder = m_ShipArray.GetAt(i).GetCurrentOrder();
					CFleet f = *(m_ShipArray[i].GetFleet());
					// Flotte löschen
					m_ShipArray[i].GetFleet()->DeleteFleet();
					m_ShipArray[i].DeleteFleet();
					// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
					m_ShipArray.SetAt(i, f.RemoveShipFromFleet(0));
					// für dieses eine Flotte erstellen
					m_ShipArray[i].CreateFleet();
					for (USHORT x = 0; x < f.GetFleetSize(); x++)
						m_ShipArray[i].GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
					m_ShipArray[i].CheckFleet();
					// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
					m_ShipArray[i].SetCurrentOrder(oldOrder);
				}
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
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			intel->StartCalc(it->second);
		// danach werden die Punkte dazuaddiert und zum Schluss werden die einzelnen Depotpunkte etwas reduziert.
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
				message.GenerateMessage(CResourceManager::GetString("WE_HAVE_NEW_INTELREPORTS"), SECURITY, "", NULL, FALSE, 4);
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
							message.GenerateMessage(eventText, SECURITY, "", NULL, FALSE, 4);
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

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
	using namespace network;

	// Forschungsboni, die die Systeme machen holen. Wir benötigen diese dann für die CalculateResearch Funktion
	struct RESEARCHBONI { short nBoni[6]; };
	map<CString, RESEARCHBONI> researchBonis;
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_System[x][y].GetOwnerOfSystem() != "")
			{
				// hier Forschungsboni besorgen
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[0] += m_System[x][y].GetProduction()->GetBioTechBoni();				
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[1] += m_System[x][y].GetProduction()->GetEnergyTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[2] += m_System[x][y].GetProduction()->GetCompTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[3] += m_System[x][y].GetProduction()->GetPropulsionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[4] += m_System[x][y].GetProduction()->GetConstructionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[5] += m_System[x][y].GetProduction()->GetWeaponTechBoni();
			}

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;

		pMajor->GetEmpire()->GetResearch()->SetResearchBoni(researchBonis[it->first].nBoni);
		CString *news = 0;
		news = pMajor->GetEmpire()->GetResearch()->CalculateResearch(pMajor->GetEmpire()->GetFP());
		network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

		for (int j = 0; j < 8; j++)		// aktuell 8 verschiedene Nachrichten mgl, siehe CResearch Klasse
		{
			// Wenn irgendwas erforscht wurde, wir also eine Nachricht erstellen wollen
			if (news[j] != "")
			{
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
					message.GenerateMessage(news[j], RESEARCH, "", 0, FALSE, 1);
				}
				else
				{
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_NEWTECHNOLOGY, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
					message.GenerateMessage(news[j], RESEARCH, "", 0, FALSE);
				}
				pMajor->GetEmpire()->AddMessage(message);
			}
		}		
	}
}

/// Diese Funktion berechnet die Auswirkungen von diplomatischen Angeboten und ob Minorraces Angebote an
/// Majorraces abgeben
void CBotf2Doc::CalcDiplomacy()
{		
	using namespace network;
	/*
		Der Start des Feldes ist immer zufällig, Berechnung geht also nicht immer mit der Föd los.
		Das ist besser, weil wenn bei der Diplomatie was gleichzeitig eintreffen sollte, aber es nur
		auf eine Hauptrasse zutreffen kann, dann trifft das auf die zu, die im Feld eher bearbeitet wurde.
		Da die Föd (normalerweise an erster Stelle) nicht einen Vorteil daraus erhalten soll, gehen wir
		diese Schleife hier zufällig durch
	*/

	CDiplomacyController* pDiplomacyController = new CDiplomacyController();
	pDiplomacyController->CalcDiplomaticFallouts();
	delete pDiplomacyController;
	pDiplomacyController = NULL;

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();

	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); it++)
	{
		CMinor* pMinor = it->second;
		
		// Die diplomatische Konsistenz überprüfen! z.B kann niemand mehr eine Freundschaft haben, wenn jemand eine
		// Mitgliedschaft mit dieser Rasse hat. Verträge werden gekündigt, wenn die Minorrace unterworfen wurde
		pMinor->CheckDiplomaticConsistence(this);
		pMinor->PerhapsCancelAgreement(this);

		for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); jt++)
		{
			CMajor* pMajor = jt->second;

			// Wenn wir mit der Minorrace mindst. einen Handelsvertrag abgeschlossen haben, dann wird deren Sector gescannt/gesehen
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= TRADE_AGREEMENT)
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetScanned(pMajor->GetRaceID());
			// Wenn wir mit der Minorrace mindst. einen Freundschaftsvertrag abgeschlossen haben, dann wird deren Sector bekannt
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= FRIENDSHIP_AGREEMENT)
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetKnown(pMajor->GetRaceID());
			// Wenn wir eine Mitgliedschaft mit der kleinen Rasse haben und das System noch der kleinen Rasse gehört, dann bekommen wir das
			if (pMinor->GetAgreement(pMajor->GetRaceID()) == MEMBERSHIP && m_System[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].GetOwnerOfSystem() == "")
			{
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetFullKnown(pMajor->GetRaceID());
				m_System[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetOwnerOfSystem(pMajor->GetRaceID());
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetOwnerOfSector(pMajor->GetRaceID());
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetOwned(TRUE);
				// Der Sector gilt jetzt als nicht eingenommen
				m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetTakenSector(FALSE);
				// Nun Gebäude in neuen System bauen
				m_System[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].BuildBuildingsForMinorRace(&m_Sector[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y], &BuildingInfo, m_Statistics.GetAverageTechLevel(), pMinor);
				// Gebäude so weit wie möglich mit Arbeitern besetzen
				m_System[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].SetWorkersIntoBuildings();				
			}
		}
	}

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		if (pMajor->GetIncomingDiplomacyNews()->size() > 0)
		{
			network::RACE client = m_pRaceCtrl->GetMappedClientID(it->first);
			if (pMajor->IsHumanPlayer())
			{
				SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_DIPLOMATICNEWS, client, 0, 1.0f};
				m_SoundMessages[client].Add(entry);
				m_iSelectedView[client] = EMPIRE_VIEW;
			}
		}
	}		

	// Hat die Rasse die Mitgliedschaft gekündigt, wurde Bestochen oder irgendein anderer Grund ist dafür
	// verantwortlich, warum eine Major plötzlich nicht mehr Zugriff auf das System der Minor hat.
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); it++)
	{
		CMinor* pMinor = it->second;
	
		CString sOwner = m_System[pMinor->GetRaceKO().x][pMinor->GetRaceKO().y].GetOwnerOfSystem();
		if (sOwner != "")
		{
			// Wenn wir eine Mitgliedschaft bei der kleinen Rasse hatten, sprich uns das System noch gehört, wir aber
			// der kleinen Rasse den Krieg erklären bzw. den Vertrag aufheben (warum auch immer?!?) und das System nicht
			// gewaltätig erobert wurde, dann gehört uns das System auch nicht mehr
			if (pMinor->GetAgreement(sOwner) != MEMBERSHIP && GetSector(pMinor->GetRaceKO()).GetMinorRace() == TRUE && GetSector(pMinor->GetRaceKO()).GetTakenSector() == FALSE)
			{
				GetSector(pMinor->GetRaceKO()).SetOwned(false);
				GetSector(pMinor->GetRaceKO()).SetOwnerOfSector("");
				GetSystem(pMinor->GetRaceKO()).SetOwnerOfSystem("");
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sOwner));
				if (pMajor)
				{
					int n = pMajor->GetEmpire()->GetNumberOfSystems();
					pMajor->GetEmpire()->SetNumberOfSystems(n - 1);
				}
			}
		}
	}
}

/// Diese Funktion berechnet das Planetenwachstum, die Aufträge in der Bauliste und sonstige Einstellungen aus der
/// alten Runde.
void CBotf2Doc::CalcOldRoundData()
{
	using namespace network;
	m_GlobalBuildings.Reset();

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		// hier setzen wir wieder die gesamten FP, SP und die gesamten Lager auf 0
		it->second->GetEmpire()->ClearAllPoints();
	}

	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			// Mögliche Is? Variablen für Terraforming und Stationbau erstmal auf FALSE setzen
			m_Sector[x][y].ClearAllPoints();
			
			// Wenn im Sektor ein Sonnensystem existiert
			if (m_Sector[x][y].GetSunSystem() == TRUE)
			{
				// Jetzt das produzierte Latinum im System dem jeweiligen Imperium geben
				if (m_System[x][y].GetOwnerOfSystem() != "")
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_System[x][y].GetOwnerOfSystem()));
					if (pMajor)
					{
						network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

						// spielt es der Computer, so bekommt er etwas mehr Credits
						if (pMajor->IsHumanPlayer() == false)
							pMajor->GetEmpire()->SetLatinum((int)(m_System[x][y].GetProduction()->GetLatinumProd() / m_fDifficultyLevel));
						else
							pMajor->GetEmpire()->SetLatinum(m_System[x][y].GetProduction()->GetLatinumProd());
					
						// Hier die Gebäude abreißen, die angeklickt wurden
						if (m_System[x][y].DestroyBuildings())
							m_System[x][y].CalculateNumberOfWorkbuildings(&this->BuildingInfo);
					
						// Variablen berechnen lassen, bevor der Planet wächst -> diese ins Lager
						// nur berechnen, wenn das System auch jemandem gehört, ansonsten würde auch die Mind.Prod. ins Lager kommen
						// In CalculateStorages wird auch die Systemmoral berechnet. Wenn wir einen Auftrag mit
						// NeverReady (z.B. Kriegsrecht) in der Bauliste haben, und dieser Moral produziert, dann diese
						// auf die Moral anrechnen. Das wird dann in CalculateStorages gemacht.
						int list = m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(0);
						if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() && m_System[x][y].GetMoral() <= 85)
							m_System[x][y].GetProduction()->AddMoralProd(BuildingInfo[list-1].GetMoralProd());
						
						// KI Anpassungen (KI bekommt zufälig etwas Deritium geschenkt)
						int diliAdd = 0;
						if (pMajor->IsHumanPlayer() == false && m_System[x][y].GetProduction()->GetDilithiumProd() == 0)
						{
							// umso höher der Schwierigkeitsgrad, desto höher die Wahrscheinlichkeit, das die KI
							// Dilithium auf ihrem Systemen geschenkt bekommt
							int temp = rand()%((int)(m_fDifficultyLevel * 7.5));
							// TRACE("KI: System: %s - DiliAddProb: %d (NULL for adding Dili) - Difficulty: %.2lf\n",m_Sector[x][y].GetName(), temp, m_fDifficultyLevel);
							if (temp == NULL)
								diliAdd = 1;
						}

						// Das Lager berechnen
						BOOLEAN bIsRebellion = m_System[x][y].CalculateStorages(pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), diliAdd);
						// Wenn wir true zurückbekommen, dann hat sich das System losgesagt
						if (bIsRebellion)	
						{
							m_Sector[x][y].SetOwned(FALSE);
							m_Sector[x][y].SetShipPort(FALSE, pMajor->GetRaceID());
							
							CString news = CResourceManager::GetString("REBELLION_IN_SYSTEM", FALSE, m_Sector[x][y].GetName());
							message.GenerateMessage(news, SOMETHING, "", CPoint(x,y), FALSE);							
							pMajor->GetEmpire()->AddMessage(message);

							// zusätzliche Eventnachricht (Lose a System to Rebellion #18) wegen der Moral an das Imperium
							message.GenerateMessage(pMajor->GetMoralObserver()->AddEvent(18, pMajor->GetRaceMoralNumber(), m_Sector[x][y].GetName()), SOMETHING, "", CPoint(x,y), FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;
													
							if (m_Sector[x][y].GetMinorRace() == TRUE && m_Sector[x][y].GetTakenSector() == FALSE)
							{
								CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[x][y].GetName());
								if (pMinor)
								{
									pMinor->SetAgreement(pMajor->GetRaceID(), NO_AGREEMENT);
									pMajor->SetAgreement(pMinor->GetRaceID(), NO_AGREEMENT);
									
									pMinor->SetRelation(pMajor->GetRaceID(), (-(rand()%50+20)));
									news = CResourceManager::GetString("MINOR_CANCELS_MEMBERSHIP", FALSE, pMinor->GetRaceName());
									message.GenerateMessage(news, DIPLOMACY, "", CPoint(x,y), FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									m_Sector[x][y].SetOwnerOfSector(pMinor->GetRaceID());
								}
							}
							else
								m_Sector[x][y].SetOwnerOfSector("");
							m_System[x][y].SetOwnerOfSystem("");
							m_Sector[x][y].SetTakenSector(FALSE);
						}
								
						// Hier mit einbeziehen, wenn die Bevölkerung an Nahrungsmangel stirbt						
						if (m_System[x][y].GetFoodStore() < 0)
						{
							m_Sector[x][y].LetPlanetsShrink((float)(m_System[x][y].GetFoodStore()) * 0.01f);
							m_System[x][y].SetMoral((short)(m_System[x][y].GetFoodStore() / (m_System[x][y].GetHabitants() + 1))); // +1, wegen Division durch NULL umgehen
							m_System[x][y].SetFoodStore(0);
							
							CString news = CResourceManager::GetString("FAMINE", FALSE, m_Sector[x][y].GetName());
							message.GenerateMessage(news, SOMETHING, "", CPoint(x,y), FALSE, 1);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;							
						}
						else
							// Planetenwachstum für Spielerrassen durchführen
							m_Sector[x][y].LetPlanetsGrowth();
					}
				}
				else
					// Planetenwachstum für andere Sektoren durchführen
					m_Sector[x][y].LetPlanetsGrowth();

				if (m_System[x][y].GetOwnerOfSystem() != "")
				{
					CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_System[x][y].GetOwnerOfSystem()));
					if (pMajor)
					{
						network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

						float fCurrentHabitants = m_Sector[x][y].GetCurrentHabitants();
						pMajor->GetEmpire()->AddPopSupportCosts((USHORT)fCurrentHabitants * 10);
						// Funktion gibt TRUE zurück, wenn wir durch die Bevölkerung eine neue Handelsroute anlegen können
						if (m_System[x][y].SetHabitants(fCurrentHabitants))
						{
							CString news = CResourceManager::GetString("ENOUGH_HABITANTS_FOR_NEW_TRADEROUTE", FALSE, m_Sector[x][y].GetName());
							message.GenerateMessage(news, ECONOMY, "", CPoint(x,y), FALSE, 4);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;							
						}					
						m_System[x][y].CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_Sector[x][y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
						
						// Gebäude die Energie benötigen checken
						if (m_System[x][y].CheckEnergyBuildings(&this->BuildingInfo) == 1)
						{
							CString news = CResourceManager::GetString("BUILDING_TURN_OFF",FALSE,m_Sector[x][y].GetName());
							message.GenerateMessage(news, SOMETHING, "", CPoint(x,y), FALSE, 2);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;							
						}
					
						// Die Bauaufträge in dem System berechnen. Außerdem wird hier auch die System-KI ausgeführt.
						if (pMajor->IsHumanPlayer() == false || m_System[x][y].GetAutoBuild())
						{
							CSystemAI* SAI = new CSystemAI(this);
							SAI->ExecuteSystemAI(CPoint(x,y));
							delete SAI;
						}
						// Hier berechnen, wenn was in der Bauliste ist, und ob es fertig wird usw.
						if (m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(0) != 0)	// wenn was drin ist
						{
							int list = m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(0);
							int IPProd = m_System[x][y].GetProduction()->GetIndustryProd();
							// Wenn ein Auftrag in der Bauliste ist, der niemals fertig ist z.B. Kriegsrecht, dann dies
							// einzeln beachten
							if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() == TRUE)
							{
								// dann zählen die industriellen Baukosten des Auftrags als Anzahl Runden, wie lange
								// es in der Liste stehen bleibt und den Bonus gibt
								// Also wird immer nur ein IP abgezogen, außer wenn die Moral größer gleich 80 ist,
								// dann wird der Auftrag gleich abgebrochen
								if (m_System[x][y].GetMoral() >= 85)
									IPProd = (int)m_System[x][y].GetAssemblyList()->GetNeededIndustryForBuild();
								else
									IPProd = 1;
							}
							// Wenn ein Update in der Liste ist die mögliche UpdateBuildSpeed-Boni beachten
							else if (list < 0)
								IPProd = (int)floor((float)IPProd * (100 + m_System[x][y].GetProduction()->GetUpdateBuildSpeed()) / 100);
							// Wenn ein Gebäude in der Liste ist die mögliche BuildingBuildSpeed-Boni beachten
							else if (list < 10000)
								IPProd = (int)floor((float)IPProd * (100 + m_System[x][y].GetProduction()->GetBuildingBuildSpeed()) / 100);
							// Wenn es ein Schiff ist, dann die Effiziens der Werft und möglichen ShipBuildSpeed-Boni beachten
							else if (list < 20000)
								IPProd = (int)floor((float)IPProd * m_System[x][y].GetProduction()->GetShipYardEfficiency() / 100
											* (100 + m_System[x][y].GetProduction()->GetShipBuildSpeed()) / 100);
							// Wenn es eine Truppe ist, dann Effiziens der Werft und möglichen TroopBuildSpeed-Boni beachten
							else
								IPProd = (int)floor((float)IPProd * m_System[x][y].GetProduction()->GetBarrackEfficiency() / 100
											* (100 + m_System[x][y].GetProduction()->GetTroopBuildSpeed()) / 100);
							// Ein Bauauftrag ist fertig gestellt worden
							if (m_System[x][y].GetAssemblyList()->CalculateBuildInAssemblyList(IPProd))
							{
								// Wenn Gebäude gekauft wurde, dann die in der letzten Runde noch erbrachte IP-Leistung
								// den Credits des Imperiums gutschreiben, IP-Leistung darf aber nicht größer der Baukosten sein
								if (m_System[x][y].GetAssemblyList()->GetWasBuildingBought() == TRUE)
								{
									int goods = IPProd;
									if (goods > m_System[x][y].GetAssemblyList()->GetBuildCosts())
										goods = m_System[x][y].GetAssemblyList()->GetBuildCosts();
									pMajor->GetEmpire()->SetLatinum(goods);
									m_System[x][y].GetAssemblyList()->SetWasBuildingBought(FALSE);
								}
								// Ab jetzt die Abfrage ob Gebäude oder ein Update fertig wurde
								if (list > 0 && list < 10000 && !BuildingInfo[list-1].GetNeverReady())	// Es wird ein Gebäude gebaut
								{
									// Die Nachricht, dass neues Gebäude fertig ist mit allen Daten generieren
									message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(), ECONOMY, m_Sector[x][y].GetName(), CPoint(x,y), FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									// Gebäude bauen
									BuildBuilding(list, CPoint(x,y));
									// und Gebäude (welches letztes im Feld) ist auch gleich online setzen, wenn 
									// genügend Arbeiter da sind
									unsigned short CheckValue = m_System[x][y].SetNewBuildingOnline(&this->BuildingInfo);
									// Nachricht generierenm das das Gebäude nicht online genommen werden konnte
									if (CheckValue == 1)
									{
										CString news = CResourceManager::GetString("NOT_ENOUGH_WORKER", FALSE, m_Sector[x][y].GetName());
										message.GenerateMessage(news, SOMETHING, "",CPoint(x,y), FALSE, 1);
										pMajor->GetEmpire()->AddMessage(message);
										if (pMajor->IsHumanPlayer())
											m_iSelectedView[client] = EMPIRE_VIEW;
									}
									else if (CheckValue == 2)
									{
										CString news = CResourceManager::GetString("NOT_ENOUGH_ENERGY", FALSE, m_Sector[x][y].GetName());
										message.GenerateMessage(news, SOMETHING, "",CPoint(x,y), FALSE, 2);
										pMajor->GetEmpire()->AddMessage(message);
										if (pMajor->IsHumanPlayer())
											m_iSelectedView[client] = EMPIRE_VIEW;
									}
								}
								else if (list < 0)	// Es wird ein Update gemacht
								{
									list *= (-1);
									// Die Nachricht, dass neues Gebäudeupdate fertig wurde, mit allen Daten generieren
									message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(),ECONOMY,m_Sector[x][y].GetName(),CPoint(x,y),TRUE);
									pMajor->GetEmpire()->AddMessage(message);
									// Vorgänger von "list" holen
									// Gebäude mit RunningNumbner == pre werden durch UpdateBuilding() gelöscht und 
									// deren Anzahl wird zurückgegeben.
									USHORT pre = BuildingInfo[list-1].GetPredecessorID();
									int NumberOfNewBuildings = m_System[x][y].UpdateBuildings(pre);
									// So, nun bauen wir so viel mal das nächste
									for (int z = 0; z < NumberOfNewBuildings; z++)
									{
										BuildBuilding(list,CPoint(x,y));
										// falls das geupgradete Gebäude Energie benötigt wird versucht es gleich online zu setzen
										if (GetBuildingInfo(list).GetNeededEnergy() > NULL && m_System[x][y].SetNewBuildingOnline(&this->BuildingInfo) == 2)
										{
											CString news = CResourceManager::GetString("NOT_ENOUGH_ENERGY",FALSE,m_Sector[x][y].GetName());
											message.GenerateMessage(news,SOMETHING,"",CPoint(x,y),FALSE,2);
											pMajor->GetEmpire()->AddMessage(message);
											if (pMajor->IsHumanPlayer())
												m_iSelectedView[client] = EMPIRE_VIEW;
										}
									}									
								}
								else if (list >= 10000 && list < 20000)	// Es wird ein Schiff gebaut
								{
									BuildShip(list-10000, CPoint(x,y), pMajor->GetRaceID());
									CString s = CResourceManager::GetString("SHIP_BUILDING_FINISHED",FALSE,
										m_ShipInfoArray[list-10000].GetShipTypeAsString(),m_Sector[x][y].GetName());
									message.GenerateMessage(s,MILITARY,m_Sector[x][y].GetName(),CPoint(x,y),FALSE);
									pMajor->GetEmpire()->AddMessage(message);
								}
								else if (list >= 20000)					// Es wird eine Truppe gebaut
								{
									BuildTroop(list-20000, CPoint(x,y));
									CString s = CResourceManager::GetString("TROOP_BUILDING_FINISHED",FALSE,
										m_TroopInfo[list-20000].GetName(),m_Sector[x][y].GetName());
									message.GenerateMessage(s,MILITARY,m_Sector[x][y].GetName(),CPoint(x,y),FALSE);
									pMajor->GetEmpire()->AddMessage(message);									
								}
								// Nach CalculateBuildInAssemblyList wird ClearAssemblyList() aufgerufen, wenn der Auftrag fertig wurde.
								// Normalerweise wird nach ClearAssemblyList() die Funktion CalculateVariables() aufgerufen, wegen Geld durch
								// Handelsgüter wenn nix mehr drin steht. Hier mal testweise weggelassen, weil diese Funktion
								// später eh für das System aufgerufen wird und wir bis jetzt glaub ich keine Notwendigkeit
								// haben die Funktion CalculateVariables() aufzurufen.
								m_System[x][y].GetAssemblyList()->ClearAssemblyList(CPoint(x,y), m_System);
								// Wenn die Bauliste nach dem letzten gebauten Gebäude leer ist, eine Nachricht generieren
								if (m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(0) == 0)
								{
									CString news = CResourceManager::GetString("EMPTY_BUILDLIST",FALSE,m_Sector[x][y].GetName());
									message.GenerateMessage(news,SOMETHING,"",CPoint(x,y),FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									if (pMajor->IsHumanPlayer())
										m_iSelectedView[client] = EMPIRE_VIEW;
								}
							}
							m_System[x][y].GetAssemblyList()->CalculateNeededRessourcesForUpdate(&BuildingInfo, m_System[x][y].GetAllBuildings(), pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
						}
						// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
						m_System[x][y].CalculateNumberOfWorkbuildings(&this->BuildingInfo);
						// freie Arbeiter den Gebäuden zuweisen
						m_System[x][y].SetWorkersIntoBuildings();
					}
				}
			
				// Jedes Sonnensystem wird durchgegangen und alle Gebäude des Systems werden in die Variable
				// m_GlobalBuildings geschrieben. Damit wissen welche Gebäude in der Galaxie stehen. Benötigt wird
				// dies z.B. um zu Überprüfen, ob max. X Gebäude einer bestimmten ID in einem Imperium stehen.
				for (int i = 0; i < m_System[x][y].GetAllBuildings()->GetSize(); i++)
					m_GlobalBuildings.AddGlobalBuilding(m_System[x][y].GetAllBuildings()->GetAt(i).GetRunningNumber());
				// Alle Gebäude und Updates, die sich aktuell auch in der Bauliste befinden, werden dem Feld hinzugefügt
				for (int i = 0; i < ALE; i++)
					if (m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(i) != 0 && m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
					{
						int entry = m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(i);
						if (entry < 0)
							entry *= (-1);
						m_GlobalBuildings.AddGlobalBuilding(BuildingInfo.GetAt(entry - 1).GetRunningNumber());
					}
				}
		}
}

/// Diese Funktion berechnet die Produktion der Systeme, was in den Baulisten gebaut werden soll und sonstige
/// Daten für die neue Runde.
void CBotf2Doc::CalcNewRoundData()
{
	using namespace network;
	CSystemProd::ResetMoralEmpireWide();
	// Hier müssen wir nochmal die Systeme durchgehen und die imperienweite Moralproduktion auf die anderen System
	// übertragen
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != "")
			{
				// imperiumsweite Moralproduktion aus diesem System berechnen
				m_System[x][y].CalculateEmpireWideMoralProd(&this->BuildingInfo);
				// Blockadewert des Systems zurücksetzen
				m_System[x][y].SetBlockade(NULL);
			}
	struct RESEARCHBONI { short nBoni[6]; };
	map<CString, RESEARCHBONI> researchBonis;

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();

	// Hier werden jetzt die baubaren Gebäude für die nächste Runde und auch die Produktion in den einzelnen
	// Systemen berechnet. Können das nicht in obiger Hauptschleife machen, weil dort es alle globalen Gebäude
	// gesammelt werden müssen und ich deswegen alle Systeme mit den fertigen Bauaufträgen in dieser Runde einmal
	// durchgegangen sein muß.
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{		
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != "")
			{
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_System[x][y].GetOwnerOfSystem()));
				ASSERT(pMajor);
				
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

				// Hier die Credits durch Handelsrouten berechnen
				BYTE deletetTradeRoutes = 0;
				for (int i = 0; i < m_System[x][y].GetTradeRoutes()->GetSize(); i++)
				{
					CPoint dest = m_System[x][y].GetTradeRoutes()->GetAt(i).GetDestKO();
					// Wenn die Handelsroute aus diplomatischen Gründen nicht mehr vorhanden sein kann
					if (m_System[x][y].GetTradeRoutes()->ElementAt(i).CheckTradeRoute(CPoint(x,y), CPoint(dest.x, dest.y), this) == FALSE)
					{
						// dann müssen wir diese Route löschen
						m_System[x][y].GetTradeRoutes()->RemoveAt(i--);
						deletetTradeRoutes++;
					}
					// Ansonsten könnte sich die Beziehung zu der Minorrace verbessern
					else
						m_System[x][y].GetTradeRoutes()->ElementAt(i).PerhapsChangeRelationship(CPoint(x,y), CPoint(dest.x, dest.y), this);							

				}
				deletetTradeRoutes += m_System[x][y].CheckTradeRoutes(pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				if (deletetTradeRoutes > 0)
				{
					CString news;
					if (deletetTradeRoutes == 1)
						news = CResourceManager::GetString("LOST_ONE_TRADEROUTE",FALSE,m_Sector[x][y].GetName());
					else
					{
						CString lost; lost.Format("%d",deletetTradeRoutes);
						news = CResourceManager::GetString("LOST_TRADEROUTES",FALSE,lost,m_Sector[x][y].GetName());
					}
					message.GenerateMessage(news, ECONOMY, "", CPoint(x,y), FALSE, 4);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
						m_iSelectedView[client] = EMPIRE_VIEW;
				}
				// Ressourcenrouten checken
				BYTE deletedResourceRoutes = 0;
				// checken ob das System noch der Rasse gehört, welcher auch das Startsystem der Route gehört
				for (int i = 0; i < m_System[x][y].GetResourceRoutes()->GetSize(); i++)
				{
					CPoint dest = m_System[x][y].GetResourceRoutes()->GetAt(i).GetKO();
					if (!m_System[x][y].GetResourceRoutes()->GetAt(i).CheckResourceRoute(m_System[x][y].GetOwnerOfSystem(), &m_Sector[dest.x][dest.y]))
					{
						m_System[x][y].GetResourceRoutes()->RemoveAt(i--);
						deletedResourceRoutes++;
					}
				}
				deletedResourceRoutes += m_System[x][y].CheckResourceRoutes(pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				if (deletedResourceRoutes > 0)
				{
					CString news;
					if (deletedResourceRoutes == 1)
						news = CResourceManager::GetString("LOST_ONE_RESOURCEROUTE",FALSE,m_Sector[x][y].GetName());
					else
					{
						CString lost; lost.Format("%d", deletedResourceRoutes);
						news = CResourceManager::GetString("LOST_RESOURCEROUTES",FALSE,lost,m_Sector[x][y].GetName());
					}
					message.GenerateMessage(news, ECONOMY, "", CPoint(x,y), FALSE, 4);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
						m_iSelectedView[client] = EMPIRE_VIEW;
				}
				
				// baubare Gebäude, Schiffe und Truppen berechnen
				m_System[x][y].CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_Sector[x][y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
				m_System[x][y].CalculateBuildableBuildings(&m_Sector[x][y], &BuildingInfo, pMajor, &m_GlobalBuildings);
				m_System[x][y].CalculateBuildableShips(this, CPoint(x,y));
				m_System[x][y].CalculateBuildableTroops(&m_TroopInfo, pMajor->GetEmpire()->GetResearch());
			
				// Haben wir eine online Schiffswerft im System, dann ShipPort in dem Sektor setzen
				if (m_System[x][y].GetProduction()->GetShipYard() == TRUE)
				{
					m_Sector[x][y].SetShipPort(TRUE, m_System[x][y].GetOwnerOfSystem());
					CStarmap* pStarmap = pMajor->GetStarmap();
					pStarmap->AddBase(Sector((char)x,(char)y), pMajor->GetEmpire()->GetResearch()->GetPropulsionTech());
				}
				// Haben wir einen Scanner in dem System, dann Umgebung scannen
				if (m_System[x][y].GetProduction()->GetScanPower() > 0)
				{
					int power = m_System[x][y].GetProduction()->GetScanPower();
					int range = m_System[x][y].GetProduction()->GetScanRange();
					for (int j = -range; j <= range; j++)
						for (int i = -range; i <= range; i++)
							if (y+j < STARMAP_SECTORS_VCOUNT && y+j > -1 && x+i < STARMAP_SECTORS_HCOUNT && x+i > -1)
							{
								m_Sector[x+i][y+j].SetScanned(m_System[x][y].GetOwnerOfSystem());
								// Teiler für die Scanstärke berechnen
								int div = max(abs(j),abs(i));
								if (div == 0)
									div = 1;
								if (power / div > m_Sector[x+i][y+j].GetScanPower(m_System[x][y].GetOwnerOfSystem()))
									m_Sector[x+i][y+j].SetScanPower(power / div, m_System[x][y].GetOwnerOfSystem());
							}
				}
				// Wurde das System militärisch erobert, so verringert sich die Moral pro Runde etwas
				if (m_Sector[x][y].GetTakenSector() == TRUE && m_System[x][y].GetMoral() > 70)
					m_System[x][y].SetMoral(-rand()%2);
				// möglicherweise wird die Moral durch stationierte Truppen etwas stabilisiert
				m_System[x][y].IncludeTroopMoralValue(&m_TroopInfo);
				
				// Hier die gesamten Forschungsboni der Imperien berechnen
				// Forschungsboni, die die Systeme machen holen. Wir benötigen diese dann für die CalculateResearch Funktion
				// hier Forschungsboni besorgen
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[0] += m_System[x][y].GetProduction()->GetBioTechBoni();				
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[1] += m_System[x][y].GetProduction()->GetEnergyTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[2] += m_System[x][y].GetProduction()->GetCompTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[3] += m_System[x][y].GetProduction()->GetPropulsionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[4] += m_System[x][y].GetProduction()->GetConstructionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()].nBoni[5] += m_System[x][y].GetProduction()->GetWeaponTechBoni();
							
				// Hier die gesamten Sicherheitsboni der Imperien berechnen
				pMajor->GetEmpire()->GetIntelligence()->AddInnerSecurityBonus(m_System[x][y].GetProduction()->GetInnerSecurityBoni());
				pMajor->GetEmpire()->GetIntelligence()->AddEconomyBonus(m_System[x][y].GetProduction()->GetEconomySpyBoni(), 0);
				pMajor->GetEmpire()->GetIntelligence()->AddEconomyBonus(m_System[x][y].GetProduction()->GetEconomySabotageBoni(), 1);
				pMajor->GetEmpire()->GetIntelligence()->AddScienceBonus(m_System[x][y].GetProduction()->GetScienceSpyBoni(), 0);
				pMajor->GetEmpire()->GetIntelligence()->AddScienceBonus(m_System[x][y].GetProduction()->GetScienceSabotageBoni(), 1);
				pMajor->GetEmpire()->GetIntelligence()->AddMilitaryBonus(m_System[x][y].GetProduction()->GetMilitarySpyBoni(), 0);
				pMajor->GetEmpire()->GetIntelligence()->AddMilitaryBonus(m_System[x][y].GetProduction()->GetMilitarySabotageBoni(), 1);

				// Anzahl aller Ressourcen in allen eigenen Systemen berechnen
				for (int res = TITAN; res <= DILITHIUM; res++)
					pMajor->GetEmpire()->SetStorageAdd(res, m_System[x][y].GetRessourceStore(res));				
			}
			
			// für jede Rasse Sektorsachen berechnen

			// Hier wird berechnet, was wir von der Karte alles sehen, welche Sektoren wir durchfliegen können
			// alles abhängig von unseren diplomatischen Beziehungen
			map<CString, bool> mShipPort;
			for (map<CString, CMajor*>::const_iterator i = pmMajors->begin(); i != pmMajors->end(); i++)
				for (map<CString, CMajor*>::const_iterator j = pmMajors->begin(); j != pmMajors->end(); j++)
				{
					if (m_Sector[x][y].GetScanned(i->first) == TRUE && i->first != j->first)
						if (i->second->GetAgreement(j->first) >= COOPERATION)
							m_Sector[x][y].SetScanned(j->first);
					if (m_Sector[x][y].GetKnown(i->first) == TRUE && i->first != j->first)
					{
						if (i->second->GetAgreement(j->first) >= FRIENDSHIP_AGREEMENT)
							m_Sector[x][y].SetScanned(j->first);
						if (i->second->GetAgreement(j->first) >= COOPERATION)
							m_Sector[x][y].SetKnown(j->first);
					}
					if (m_Sector[x][y].GetOwnerOfSector() == i->first && i->first != j->first)
					{
						if (i->second->GetAgreement(j->first) >= TRADE_AGREEMENT)
							m_Sector[x][y].SetScanned(j->first);
						if (i->second->GetAgreement(j->first) >= FRIENDSHIP_AGREEMENT)
							m_Sector[x][y].SetKnown(j->first);
					}
					if (m_Sector[x][y].GetShipPort(i->first) == TRUE && i->first != j->first)
						if (i->second->GetAgreement(j->first) >= COOPERATION)
							mShipPort[j->first] = TRUE;
				}

			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
				if (mShipPort[it->first] == true)
				{
					m_Sector[x][y].SetShipPort(TRUE, it->first);
					it->second->GetStarmap()->AddBase(Sector(x,y), it->second->GetEmpire()->GetResearch()->GetPropulsionTech());
				}
		}		
		
		// Forschungsboni aus Spezialforschungen setzen, nachdem wir diese aus allen Systemen geholt haben
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		{
			CMajor* pMajor = it->second;
			CResearchInfo* pInfo = pMajor->GetEmpire()->GetResearch()->GetResearchInfo();
			// Die Boni auf die einzelnen Forschungsgebiete durch Spezialforschungen addieren
			if (pInfo->GetResearchComplex(8)->GetFieldStatus(1) == RESEARCHED)
			{
				researchBonis[it->first].nBoni[0] += pInfo->GetResearchComplex(8)->GetBonus(1);
				researchBonis[it->first].nBoni[1] += pInfo->GetResearchComplex(8)->GetBonus(1);
			}
			else if (pInfo->GetResearchComplex(8)->GetFieldStatus(2) == RESEARCHED)
			{
				researchBonis[it->first].nBoni[2] += pInfo->GetResearchComplex(8)->GetBonus(2);
				researchBonis[it->first].nBoni[3] += pInfo->GetResearchComplex(8)->GetBonus(2);
			}
			else if (pInfo->GetResearchComplex(8)->GetFieldStatus(3) == RESEARCHED)
			{
				researchBonis[it->first].nBoni[4] += pInfo->GetResearchComplex(8)->GetBonus(3);
				researchBonis[it->first].nBoni[5] += pInfo->GetResearchComplex(8)->GetBonus(3);
			}
			pMajor->GetEmpire()->GetResearch()->SetResearchBoni(researchBonis[it->first].nBoni);
		}
		
		// Geheimdienstboni aus Spezialforschungen holen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		{
			CMajor* pMajor = it->second;
			CResearchInfo* pInfo = pMajor->GetEmpire()->GetResearch()->GetResearchInfo();

			// Die Boni auf die einzelnen Geheimdienstgebiete berechnen
			if (pInfo->GetResearchComplex(9)->GetFieldStatus(1) == RESEARCHED)
				pMajor->GetEmpire()->GetIntelligence()->AddInnerSecurityBonus(pInfo->GetResearchComplex(9)->GetBonus(1));
			else if (pInfo->GetResearchComplex(9)->GetFieldStatus(2) == RESEARCHED)
			{
				pMajor->GetEmpire()->GetIntelligence()->AddEconomyBonus(pInfo->GetResearchComplex(9)->GetBonus(2), 1);
				pMajor->GetEmpire()->GetIntelligence()->AddMilitaryBonus(pInfo->GetResearchComplex(9)->GetBonus(2), 1);
				pMajor->GetEmpire()->GetIntelligence()->AddScienceBonus(pInfo->GetResearchComplex(9)->GetBonus(2), 1);
			}
			else if (pInfo->GetResearchComplex(9)->GetFieldStatus(3) == RESEARCHED)
			{
				pMajor->GetEmpire()->GetIntelligence()->AddEconomyBonus(pInfo->GetResearchComplex(9)->GetBonus(3), 0);
				pMajor->GetEmpire()->GetIntelligence()->AddMilitaryBonus(pInfo->GetResearchComplex(9)->GetBonus(3), 0);
				pMajor->GetEmpire()->GetIntelligence()->AddScienceBonus(pInfo->GetResearchComplex(9)->GetBonus(3), 0);
			}
		}		

		// Nun überprüfen, ob sich unsere Grenzen erweitern, wenn die MinorRasse eine Spaceflight-Rasse ist und wir mit
		// ihr eine Kooperations oder ein Bündnis haben
		map<CString, CMinor*>* pmMinors = m_pRaceCtrl->GetMinors();
		for (map<CString, CMajor*>::const_iterator i = pmMajors->begin(); i != pmMajors->end(); i++)
		{
			CMajor* pMajor = i->second;
			for (map<CString, CMinor*>::const_iterator j = pmMinors->begin(); j != pmMinors->end(); j++)
			{
				CMinor* pMinor = j->second;
				if (pMinor->GetSpaceflightNation() == TRUE && (pMinor->GetAgreement(pMajor->GetRaceID()) == COOPERATION || pMinor->GetAgreement(pMajor->GetRaceID()) == AFFILIATION))
				{
					CPoint p = pMinor->GetRaceKO();
					m_Sector[p.x][p.y].SetShipPort(TRUE, pMajor->GetRaceID());
					pMajor->GetStarmap()->AddBase(Sector((char)p.x,(char)p.y), pMajor->GetEmpire()->GetResearch()->GetPropulsionTech());
				}
			}
		}
}

/// Diese Funktion berechnet die kompletten Handelsaktivitäten.
void CBotf2Doc::CalcTrade()
{
	using namespace network;
	// Hier berechnen wir alle Handelsaktionen
	USHORT taxMoney[] = {0,0,0,0,0};	// alle Steuern auf eine Ressource
		
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	if (!pmMajors->size())
		AfxMessageBox("Error in CBotf2Doc::CalcTrade(): Could not get any major from race controller!");

	for (map<CString, CMajor*>::iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetFieldStatus(1) == RESEARCHED)
		{
			float newTax = (float)pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(1);
			newTax = 1.0f + newTax / 100;
			pMajor->GetTrade()->SetTax(newTax);
		}
		pMajor->GetTrade()->CalculateTradeActions(pMajor, m_System, m_Sector, taxMoney);
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
			//hh.Format("Steuern auf %d: %d Latinum",i,taxMoney[i]);
			//AfxMessageBox(hh);
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(CTrade::GetMonopolOwner(i)));
			ASSERT(pMajor);
			if (pMajor)
				pMajor->GetEmpire()->SetLatinum(taxMoney[i]);
		}
		
		// Hier die gekauften Monopole den Rassen zuschreiben. Wer am meisten bezahlt hat (falls mehrere Rassen
		// in der selben Runde ein Monopol kaufen möchten) bekommt das Monopol. Die anderen bekommen ihr Latinum zurück
		double max = 0.0f;				// meiste Latinum was für ein Monopol gegeben wurde
		CString sMonopolRace = "";		// Rasse die das Monopol erlangt hat
		

		for (map<CString, CMajor*>::iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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

		for (map<CString, CMajor*>::iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		{
			CMajor* pMajor = it->second;
			ASSERT(pMajor);
		
			CString sNews = "";
			// Die anderen Rassen bekommen ihr Geld zurück
			if (pMajor->GetRaceID() != sMonopolRace && pMajor->GetTrade()->GetMonopolBuying()[i] != 0)
			{
				pMajor->GetEmpire()->SetLatinum((long)pMajor->GetTrade()->GetMonopolBuying()[i]);
				// Nachricht generieren, dass wir es nicht geschafft haben ein Monopol zu kaufen
				sNews = CResourceManager::GetString("WE_DONT_GET_MONOPOLY",FALSE,resName);				
			}
			// Nachricht an unser Imperium, dass wir ein Monopol erlangt haben
			else if (pMajor->GetRaceID() == sMonopolRace)
				sNews = CResourceManager::GetString("WE_GET_MONOPOLY",FALSE,resName);

			if (!sNews.IsEmpty())
			{
				message.GenerateMessage(sNews,SOMETHING,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				RACE clientID = (RACE)m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
				if (pMajor->IsHumanPlayer())
					m_iSelectedView[clientID] = EMPIRE_VIEW;
				pMajor->GetTrade()->SetMonopolBuying(i,0.0f);
			}
			
			
			// Nachrichten an die einzelnen Imperien verschicken, das eine Rasse das Monopol erlangt hat
			if (sMonopolRace.IsEmpty() == false && sMonopolRace != pMajor->GetRaceID() && pMajor->IsRaceContacted(sMonopolRace))
			{
				CMajor* pMonopolRace = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sMonopolRace));
				ASSERT(pMonopolRace);

				CString sRace = pMonopolRace->GetRaceNameWithArticle();
								
				CString news = CResourceManager::GetString("SOMEBODY_GET_MONOPOLY",TRUE,sRace,resName);
				message.GenerateMessage(news,SOMETHING,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				RACE clientID = (RACE)m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
				if (server.IsPlayedByClient(clientID))
					m_iSelectedView[clientID] = EMPIRE_VIEW;
			}
		}
	}
	// Hier den neuen Kurs der Waren an den Handelsbörsen berechnen, dürfen wir erst machen, wenn wir für alle Tradeobjekte
	// die einzelnen Kurse berechnet haben
	for (map<CString, CMajor*>::iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
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
	using namespace network;
	// Array beinhaltet die Außenposten, welche nach dem Bau einer Sternbasis aus dem Feld verschwinden müssen.
	// Diese können nicht direkt in der Schleife entfernt werden, da sonst beim der nächsten Iterierung die Schleife nicht mehr hinhaut.
	CArray<CShip> removeableOutposts;
	// Hier kommt die Auswertung der Schiffsbefehle
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		// Hier wird überprüft, ob der Systemattack-Befehl noch gültig ist
		// Alle Schiffe, welche einen Systemangriffsbefehl haben überprüfen, ob dieser Befehl noch gültig ist
		CPoint p = m_ShipArray.GetAt(y).GetKO();		
		if (m_ShipArray[y].GetCurrentOrder() == ATTACK_SYSTEM && m_Sector[p.x][p.y].GetSunSystem())
		{
			// Wenn die Bevölkerung komplett vernichtet wurde
			if (m_System[p.x][p.y].GetHabitants() == 0.0f)
				m_ShipArray.ElementAt(y).SetCurrentOrder(ATTACK);
			// Wenn das System der angreifenden Rasse gehört
			else if (m_System[p.x][p.y].GetOwnerOfSystem() == m_ShipArray.GetAt(y).GetOwnerOfShip())
				m_ShipArray.ElementAt(y).SetCurrentOrder(ATTACK);
			// Wenn eine Rasse in dem System lebt
			else if (m_Sector[p.x][p.y].GetOwnerOfSector() != "" && m_Sector[p.x][p.y].GetOwnerOfSector() != m_ShipArray.GetAt(y).GetOwnerOfShip())
			{
				CRace* pRace = m_pRaceCtrl->GetRace(m_Sector[p.x][p.y].GetOwnerOfSector());
				if (pRace != NULL && pRace->GetAgreement(m_ShipArray.GetAt(y).GetOwnerOfShip()) != WAR)
					m_ShipArray.ElementAt(y).SetCurrentOrder(ATTACK);
			}
		}

		CPoint ShipKO = m_ShipArray[y].GetKO();
		// Flotten checken, falls keine mehr existiert, dann wird der Zeiger auf die Flotte aufgelöst
		// muß nicht unbedingt gemacht werden, hält die Objekte aber sauberer
		m_ShipArray[y].CheckFleet();

		// Haben wir eine Flotte, den aktuellen Befehl an alle Schiffe in der Flotte weitergeben
		if (m_ShipArray[y].GetFleet() != 0)
			m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
				
		// wenn der Befehl "Terraform" ist und kein Planet ausgewählt ist, dann Befehl wieder auf "AVOID"
		// setzen
		if (m_ShipArray[y].GetCurrentOrder() == TERRAFORM && m_ShipArray[y].GetTerraformingPlanet() == -1)
			m_ShipArray[y].SetCurrentOrder(AVOID);
		 // Planet soll kolonisiert werden
		if (m_ShipArray[y].GetCurrentOrder() == COLONIZE)
		{
			// Überprüfen das der Sector auch nur mir oder niemandem gehört
			if ((m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == m_ShipArray[y].GetOwnerOfShip() || m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == ""))
			{
				// Wieviele Einwohner bekommen wir in dem System durch die Kolonisierung?
				float startHabitants = (float)(m_ShipArray[y].GetColonizePoints() * 4);
				// Wenn keine Nummer eines Planeten zum Kolonisieren angegeben ist, dann werden alle geterraformten
				// Planeten kolonisiert. Dazu wird die Bevölkerung, welche bei der Kolonisierung auf das System kommt
				// auf die einzelnen Planeten gleichmäßig aufgeteilt.
				if (m_ShipArray[y].GetTerraformingPlanet() == -1)
				{
					BYTE terraformedPlanets = 0;
					for (int i = 0; i < m_Sector[ShipKO.x][ShipKO.y].GetNumberOfPlanets(); i++)
						if (m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetTerraformed() == TRUE
							&& m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetColonized() == FALSE)
							terraformedPlanets++;
					if (terraformedPlanets > 0)
					{
						float tmpHab = startHabitants /= terraformedPlanets;
						float tmpHab2 = 0.0f;
						float oddHab = 0.0f;	// überschüssige Kolonisten, wenn ein Planet zu klein ist
						// Geterraformte Planeten durchgehen und die Bevölkerung auf diese verschieben
						for (int i = 0; i < m_Sector[ShipKO.x][ShipKO.y].GetNumberOfPlanets(); i++)
							if (m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetTerraformed() == TRUE
								&& m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetColonized() == FALSE)
							{
								if (startHabitants > m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant())
								{
									oddHab += (startHabitants - m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant());
									startHabitants = m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant();
								}
								tmpHab2 += startHabitants;
								m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->SetCurrentHabitant(startHabitants);
								m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->SetColonisized(TRUE);
								startHabitants = tmpHab;
							}
						startHabitants = tmpHab2;
						// die übrigen Kolonisten auf die Planeten verteilen
						if (oddHab > 0.0f)
							for (int i = 0; i < m_Sector[ShipKO.x][ShipKO.y].GetNumberOfPlanets(); i++)
								if (m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetTerraformed() == TRUE
									&& m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetCurrentHabitant() > 0.0f)
								{
									if ((oddHab + m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetCurrentHabitant())
										> m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant())
									{
										oddHab -= (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant()
											- m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetCurrentHabitant());
										m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->SetCurrentHabitant(m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->GetMaxHabitant());
									}
									else
									{
										m_Sector[ShipKO.x][ShipKO.y].GetPlanet(i)->SetCurrentHabitant(
											m_Sector[ShipKO.x][ShipKO.y].GetPlanets()->GetAt(i).GetCurrentHabitant() + oddHab);
										break;
									}
								}								
					}
					else
					{
						m_ShipArray[y].SetCurrentOrder(AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						continue;
					}
				}			
				else
				{
					if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetColonized() == FALSE
						&& m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == TRUE)
					{
						if (startHabitants > m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetMaxHabitant())
							startHabitants = m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetMaxHabitant();
						m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetCurrentHabitant(startHabitants);
						m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetColonisized(TRUE);
					}
					else
					{
						m_ShipArray[y].SetCurrentOrder(AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						continue;
					}
				}
				CString s;
				CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
				ASSERT(pMajor);
				network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

				// Gebäude bauen, wenn wir das System zum ersten Mal kolonisieren, sprich das System noch niemanden gehört
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == "")
				{
					// Sector- und Systemwerte ändern
					m_Sector[ShipKO.x][ShipKO.y].SetOwned(TRUE);
					m_Sector[ShipKO.x][ShipKO.y].SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
					m_Sector[ShipKO.x][ShipKO.y].SetColonyOwner(m_ShipArray[y].GetOwnerOfShip());
					m_System[ShipKO.x][ShipKO.y].SetOwnerOfSystem(m_ShipArray[y].GetOwnerOfShip());
					// Gebäude nach einer Kolonisierung bauen
					m_System[ShipKO.x][ShipKO.y].BuildBuildingsAfterColonization(&m_Sector[ShipKO.x][ShipKO.y],&BuildingInfo,m_ShipArray[y].GetColonizePoints());
					// Nachricht an das Imperium senden, das ein System neu kolonisiert wurde
					s = CResourceManager::GetString("FOUND_COLONY_MESSAGE",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
					message.GenerateMessage(s,SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					
					// zusätzliche Eventnachricht (Colonize a system #12) wegen der Moral an das Imperium
					message.GenerateMessage(pMajor->GetMoralObserver()->AddEvent(12, pMajor->GetRaceMoralNumber(), m_Sector[ShipKO.x][ShipKO.y].GetName()), SOMETHING, "", ShipKO, FALSE); 
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
					{	
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_CLAIMSYSTEM, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;						
					
						CEventColonization* eventScreen = new CEventColonization(pMajor->GetRaceID(), CResourceManager::GetString("COLOEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("COLOEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));
						pMajor->GetEmpire()->GetEventMessages()->Add(eventScreen);
						s.Format("Added Colonization-Eventscreen for Race %s in System %s", pMajor->GetRaceName(), m_Sector[ShipKO.x][ShipKO.y].GetName());
						MYTRACE(MT::LEVEL_INFO, s);
					}
				}
				else
				{					
					// Nachricht an das Imperium senden, das ein Planet kolonisiert wurde
					s = CResourceManager::GetString("NEW_PLANET_COLONIZED",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
					message.GenerateMessage(s,SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
						m_iSelectedView[client] = EMPIRE_VIEW;					
				}
				m_System[ShipKO.x][ShipKO.y].SetHabitants(m_Sector[ShipKO.x][ShipKO.y].GetCurrentHabitants());
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableBuildings(&m_Sector[ShipKO.x][ShipKO.y],&BuildingInfo,pMajor,&m_GlobalBuildings);
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableShips(this, ShipKO);
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableTroops(&m_TroopInfo,pMajor->GetEmpire()->GetResearch());
				m_System[ShipKO.x][ShipKO.y].CalculateNumberOfWorkbuildings(&this->BuildingInfo);
				m_System[ShipKO.x][ShipKO.y].CalculateVariables(&this->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_Sector[ShipKO.x][ShipKO.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				s.Format("%s %s",CResourceManager::GetString("COLONIZATION"), m_Sector[ShipKO.x][ShipKO.y].GetName());
				pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, s, CResourceManager::GetString("DESTROYED"));
				// Wenn das Schiff, welches hier kolonisiert hat eine Flotte anführt, dann müssen wir die Flotte auf
				// das nächste Schiff in der Flotte übergeben, weil dieses Schiff hier ja zerstört wird
				if (m_ShipArray[y].GetFleet() != 0)
				{
					CFleet f = *(m_ShipArray[y].GetFleet());
					// Flotte löschen
					m_ShipArray[y].GetFleet()->DeleteFleet();
					m_ShipArray[y].DeleteFleet();
					// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
					m_ShipArray.SetAt(y, f.RemoveShipFromFleet(0));
					// für dieses eine Flotte erstellen
					m_ShipArray[y].CreateFleet();
					for (USHORT x = 0; x < f.GetFleetSize(); x++)
						m_ShipArray[y].GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
					m_ShipArray[y].CheckFleet();
					// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
					// Somit können wir in in der for-Schleife mit dem nächsten Schiff fortfahren
					continue;
				}
				// Wenn das Schiff keine Flotte anführte, dann ganz normal weitermachen
				else
				{
					m_ShipArray.RemoveAt(y--);
					continue;	// continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
				}
			}
			else
			{
				m_ShipArray[y].SetCurrentOrder(AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
			}
		}
		// hier wird ein Planet geterraformed
		else if (m_ShipArray[y].GetCurrentOrder() == TERRAFORM && m_ShipArray[y].GetTerraformingPlanet() != -1)	// Planet soll terraformed werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == FALSE)
			{
				if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetNeededTerraformPoints(m_ShipArray[y].GetColonizePoints()))
				{
					// Hier wurde ein Planet erfolgreich geterraformt
					m_ShipArray[y].SetCurrentOrder(AVOID);
					m_ShipArray[y].SetTerraformingPlanet(-1);
					// Nachricht generieren, dass Terraforming abgeschlossen wurde
					CString s = CResourceManager::GetString("TERRAFORMING_FINISHED",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
					message.GenerateMessage(s,SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					pMajor->GetEmpire()->AddMessage(message);
					if (pMajor->IsHumanPlayer())
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, client, 0, 1.0f};
						m_SoundMessages[client].Add(entry);
						m_iSelectedView[client] = EMPIRE_VIEW;
					}
					// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
					if (m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() != "" && m_Sector[ShipKO.x][ShipKO.y].GetMinorRace() == TRUE && m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == "")
					{
						CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[ShipKO.x][ShipKO.y].GetName());
						if (pMinor)
							pMinor->SetRelation(pMajor->GetRaceID(), +rand()%11);
					}
				}
			}
			else	// wenn der Plani aus irgendeinen Grund schon geterraformed ist
			{
				m_ShipArray[y].SetCurrentOrder(AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
			}
			// Wenn das Schiff eine Flotte anführt, dann können auch die Schiffe in der Flotte ihre Terraformpunkte mit
			// einbringen
			if (m_ShipArray[y].GetFleet() != 0 && m_ShipArray[y].GetTerraformingPlanet() != -1)
				for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
				{
					if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == FALSE)
					{
						if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetNeededTerraformPoints(m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetColonizePoints()))
						{
							m_ShipArray[y].SetCurrentOrder(AVOID);
							m_ShipArray[y].SetTerraformingPlanet(-1);
							// Nachricht generieren, dass Terraforming abgeschlossen wurde
							CString s = CResourceManager::GetString("TERRAFORMING_FINISHED",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
							message.GenerateMessage(s,SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
							if (m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() != "" && m_Sector[ShipKO.x][ShipKO.y].GetMinorRace() == TRUE && m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == "")
							{
								CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[ShipKO.x][ShipKO.y].GetName());
								if (pMinor)
									pMinor->SetRelation(pMajor->GetRaceID(), +rand()%11);
							}
							break;
						}
					}
					else	// wenn der Plani aus irgendeinen Grund schon geterraformed ist
					{
						m_ShipArray[y].SetCurrentOrder(AVOID);
						m_ShipArray[y].SetTerraformingPlanet(-1);
						break;
					}
				}
		}
		// hier wird ein Außenposten gebaut
		else if (m_ShipArray[y].GetCurrentOrder() == BUILD_OUTPOST)	// es soll eine Station gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// hier wird vereinfacht angenommen, das an teurerer Außenposten auch ein besserer ist
			USHORT costs = 0;
			short id = -1;
			// Wenn wir in dem Sektor noch keinen Außenposten und noch keine Sternbasis stehen haben
			if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
				&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == OUTPOST
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
					{
						costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
						id = m_ShipInfoArray.GetAt(l).GetID();
					}
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
				{
					if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(it->first) == TRUE || 
					   (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(it->first) == FALSE && m_Sector[ShipKO.x][ShipKO.y].GetStarbase(it->first) == TRUE))
					{
						buildable = FALSE;
						break;
					}
				}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (m_Sector[ShipKO.x][ShipKO.y].GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(m_ShipInfoArray.GetAt((id-10000)).GetBaseIndustry(),m_ShipArray[y].GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anführt, dann erstmal die Außenpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					if (m_ShipArray[y].GetFleet() != 0)
					{
						for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							if (m_Sector[ShipKO.x][ShipKO.y].SetNeededStationPoints(m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
							{
								// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
								if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
									&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
								{
									m_Sector[ShipKO.x][ShipKO.y].SetOutpost(TRUE, m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetScanned(m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetOwned(TRUE);
									m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
									// Nachricht generieren, dass der Außenpostenbau abgeschlossen wurde
									message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MILITARY,"",ShipKO,FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x), m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,	CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
									if (pMajor->IsHumanPlayer())
									{
										SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, client, 0, 1.0f};
										m_SoundMessages[client].Add(entry);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
									// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
									for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
									{
										m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, it->first);
										m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(0, it->first);
									}
									// Das Schiff, welches die Station fertiggestellt hat aus der Flotte entfernen
									m_ShipArray[y].GetFleet()->RemoveShipFromFleet(x);
									m_ShipArray[y].CheckFleet();
									this->BuildShip(id-10000, ShipKO, m_ShipArray[y].GetOwnerOfShip());
									// Wenn hier ein Außenposten gebaut wurde den Befehl für die Flotte auf Meiden stellen
									m_ShipArray[y].SetCurrentOrder(AVOID);
									break;
								}
							}
					}
					if (m_Sector[ShipKO.x][ShipKO.y].GetIsStationBuilding(m_ShipArray[y].GetOwnerOfShip()) == TRUE
						&& m_Sector[ShipKO.x][ShipKO.y].SetNeededStationPoints(m_ShipArray[y].GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
							&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
						{
							m_Sector[ShipKO.x][ShipKO.y].SetOutpost(TRUE,m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetScanned(m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetOwned(TRUE);
							m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
							// Nachricht generieren, dass der Außenpostenbau abgeschlossen wurde
							message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),MILITARY,"",ShipKO,FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
							{
								m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, it->first);
								m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(0, it->first);
							}
							// Hier den Außenposten bauen
							this->BuildShip(id-10000, ShipKO, m_ShipArray[y].GetOwnerOfShip());
							// Wenn das Schiff, welches hier den Außenposten baut eine Flotte anführt, dann müssen wir die Flotte auf
							// das nächste Schiff in der Flotte übergeben, weil dieses Schiff hier ja zerstört wird
							if (m_ShipArray[y].GetFleet() != 0)
							{
								CFleet f = *(m_ShipArray[y].GetFleet());
								// Flotte löschen
								m_ShipArray[y].GetFleet()->DeleteFleet();
								m_ShipArray[y].DeleteFleet();
								// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
								m_ShipArray.SetAt(y, f.RemoveShipFromFleet(0));
								// für dieses eine Flotte erstellen
								m_ShipArray[y].CreateFleet();
								for (USHORT x = 0; x < f.GetFleetSize(); x++)
									m_ShipArray[y].GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
								m_ShipArray[y].CheckFleet();
								// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
								// Somit können wir in in der for-Schleife mit dem nächsten Schiff fortfahren
								// Wenn hier ein Außenposten gebeut wurde den Befehl für die Flotte auf Meiden stellen
								m_ShipArray[y].SetCurrentOrder(AVOID);
							}
							// Wenn das Schiff keine Flotte anführte, dann ganz normal weitermachen
							else
							{
								m_ShipArray.RemoveAt(y--);
								continue; // continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
							}
						}
					}
				}
				else
					m_ShipArray[y].SetCurrentOrder(AVOID);
			}
			else
				m_ShipArray[y].SetCurrentOrder(AVOID);
		}
		// hier wird eine Sternbasis gebaut
		else if (m_ShipArray[y].GetCurrentOrder() == BUILD_STARBASE)	// es soll eine Sternbasis gebaut werden
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// um eine Sternbasis bauen zu können muß schon ein Außenposten in dem Sektor stehen
			// hier wird vereinfacht angenommen, das eine teurere Sternbasis auch eine bessere ist
			// oder wir haben einen Außenposten und wollen diesen zur Sternbasis updaten
			USHORT costs = 0;
			short id = -1;
			if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
				&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == STARBASE
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
					{
						costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
						id = m_ShipInfoArray.GetAt(l).GetID();
					}

			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
				{
					if (m_Sector[ShipKO.x][ShipKO.y].GetStarbase(it->first) == TRUE
						|| (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(it->first) == TRUE && it->first != m_ShipArray[y].GetOwnerOfShip()))
					{
						buildable = FALSE;
						break;
					}
				}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (m_Sector[ShipKO.x][ShipKO.y].GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == 0)
						// dann Industriekosten der Station als StationBuildPoints nehmen
						m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(m_ShipInfoArray.GetAt(id-10000).GetBaseIndustry(),m_ShipArray[y].GetOwnerOfShip());
					// Wenn das Schiff eine Flotte anführt, dann erstmal die Außenpostenbaupunkte der Schiffe in der Flotte
					// beachten und gegebenfalls das Schiff aus der Flotte entfernen
					if (m_ShipArray[y].GetFleet() != 0)
					{
						for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							if (m_Sector[ShipKO.x][ShipKO.y].SetNeededStationPoints(m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
							{
								// Station ist fertig, also bauen (wurde durch ein Schiff in der Flotte fertiggestellt)
								if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
									&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
								{
									m_Sector[ShipKO.x][ShipKO.y].SetOutpost(FALSE,m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetStarbase(TRUE,m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetScanned(m_ShipArray[y].GetOwnerOfShip());
									m_Sector[ShipKO.x][ShipKO.y].SetOwned(TRUE);
									m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
									// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
									message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MILITARY,"",ShipKO,FALSE);
									pMajor->GetEmpire()->AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x), m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
									if (pMajor->IsHumanPlayer())
									{
										SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, client, 0, 1.0f};
										m_SoundMessages[client].Add(entry);
										m_iSelectedView[client] = EMPIRE_VIEW;
									}
									// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
									for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
									{
										m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, it->first);
										m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(0, it->first);
									}
									// Das Schiff, welches die Station fertiggestellt hat aus der Flotte entfernen
									m_ShipArray[y].GetFleet()->RemoveShipFromFleet(x);
									m_ShipArray[y].CheckFleet();
									this->BuildShip(id-10000, ShipKO, m_ShipArray[y].GetOwnerOfShip());
									// Wenn wir jetzt die Sternbasis gebaut haben, dann müssen wir den alten Außenposten aus der
									// Schiffsliste nehmen
									for (int k = 0; k <= m_ShipArray.GetSize(); k++)
										if (m_ShipArray[k].GetShipType() == OUTPOST && m_ShipArray[k].GetKO() == ShipKO)
										{
											// ebenfalls muss der Außenposten aus der Shiphistory der aktuellen Schiffe entfernt werden
											pMajor->GetShipHistory()->RemoveShip(&m_ShipArray[k]);
											removeableOutposts.Add(m_ShipArray[k]);
											break;
										}
									// Wenn hier ein Außenposten gebaut wurde den Befehl für die Flotte auf Meiden stellen
									m_ShipArray[y].SetCurrentOrder(AVOID);
									break;
								}
							}
					}
					if (m_Sector[ShipKO.x][ShipKO.y].GetIsStationBuilding(m_ShipArray[y].GetOwnerOfShip()) == TRUE
						&& m_Sector[ShipKO.x][ShipKO.y].SetNeededStationPoints(m_ShipArray[y].GetStationBuildPoints(),m_ShipArray[y].GetOwnerOfShip()))
					{
						// Station ist fertig, also bauen (wurde NICHT!!! durch ein Schiff in der Flotte fertiggestellt)
						if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
							&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
						{
							m_Sector[ShipKO.x][ShipKO.y].SetOutpost(FALSE,m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetStarbase(TRUE,m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetOwnerOfSector(m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetScanned(m_ShipArray[y].GetOwnerOfShip());
							m_Sector[ShipKO.x][ShipKO.y].SetOwned(TRUE);
							m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE,m_ShipArray[y].GetOwnerOfShip());
							// Nachricht generieren, dass der Sternbasisbau abgeschlossen wurde
							message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),MILITARY,"",ShipKO,FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,	CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
							if (pMajor->IsHumanPlayer())
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, client, 0, 1.0f};
								m_SoundMessages[client].Add(entry);
								m_iSelectedView[client] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
							{
								m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, it->first);
								m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(0, it->first);
							}
							// Sternbasis bauen
							this->BuildShip(id-10000, ShipKO, m_ShipArray[y].GetOwnerOfShip());
							// Wenn das Schiff, welches hier die Sternbasis baut eine Flotte anführt, dann müssen wir die Flotte auf
							// das nächste Schiff in der Flotte übergeben, weil dieses Schiff hier ja zerstört wird
							if (m_ShipArray[y].GetFleet() != 0)
							{
								CFleet f = *(m_ShipArray[y].GetFleet());
								// Flotte löschen
								m_ShipArray[y].GetFleet()->DeleteFleet();
								m_ShipArray[y].DeleteFleet();
								// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
								m_ShipArray.SetAt(y, f.RemoveShipFromFleet(0));
								// für dieses eine Flotte erstellen
								m_ShipArray[y].CreateFleet();
								for (USHORT x = 0; x < f.GetFleetSize(); x++)
									m_ShipArray[y].GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
								m_ShipArray[y].CheckFleet();
								// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
								// Somit können wir in in der for-Schleife mit dem nächsten Schiff fortfahren
								// Wenn hier ein Außenposten gebeut wurde den Befehl für die Flotte auf Meiden stellen
								m_ShipArray[y].SetCurrentOrder(AVOID);
								// Wenn wir jetzt die Sternbasis gebaut haben, dann müssen wir den alten Außenposten aus der
								// Schiffsliste nehmen
								for (int k = 0; k < m_ShipArray.GetSize(); k++)
									if (m_ShipArray[k].GetShipType() == OUTPOST && m_ShipArray[k].GetKO() == ShipKO)
									{
										// ebenfalls muss der Außenposten aus der Shiphistory der aktuellen Schiffe entfernt werden
										pMajor->GetShipHistory()->RemoveShip(&m_ShipArray[k]);
										removeableOutposts.Add(m_ShipArray[k]);
										break;
									}
							}
							// Wenn das Schiff keine Flotte anführte, dann ganz normal weitermachen
							else
							{
								m_ShipArray.RemoveAt(y--);
								// Wenn wir jetzt die Sternbasis gebaut haben, dann müssen wir den alten Außenposten aus der
								// Schiffsliste nehmen
								for (int k = 0; k < m_ShipArray.GetSize(); k++)
									if (m_ShipArray[k].GetShipType() == OUTPOST && m_ShipArray[k].GetKO() == ShipKO)
									{
										// ebenfalls muss der Außenposten aus der Shiphistory der aktuellen Schiffe entfernt werden
										pMajor->GetShipHistory()->RemoveShip(&m_ShipArray[k]);
										removeableOutposts.Add(m_ShipArray[k]);
										break;
									}
								continue; // continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
							}
						}
					}
				}
				else
					m_ShipArray[y].SetCurrentOrder(AVOID);
			}
			else
				m_ShipArray[y].SetCurrentOrder(AVOID);
		}
		// Wenn wir das Schiff abracken/zerstören/demontieren wollen
		else if (m_ShipArray[y].GetCurrentOrder() == DESTROY_SHIP)	// das Schiff wird demontiert
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// wenn wir in dem Sector wo wir das Schiff demoniteren ein uns gehörendes System haben, dann bekommen wir
			// teilweise Rohstoffe aus der Demontage zurück (vlt. auch ein paar Credits)
			if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
			{
				USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 auswählen
				// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
				proz += m_System[ShipKO.x][ShipKO.y].GetProduction()->GetShipRecycling();
				USHORT id = m_ShipArray[y].GetID() - 10000;
				m_System[ShipKO.x][ShipKO.y].SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
				m_System[ShipKO.x][ShipKO.y].SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
				m_System[ShipKO.x][ShipKO.y].SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
				m_System[ShipKO.x][ShipKO.y].SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
				m_System[ShipKO.x][ShipKO.y].SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
				pMajor->GetEmpire()->SetLatinum((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
			}
			// Wenn es ein Außenposten oder eine Sternbasis ist, dann dem Sektor bekanntgeben, dass in ihm keine Station mehr ist
			if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
			{
				m_Sector[ShipKO.x][ShipKO.y].SetOutpost(FALSE, m_ShipArray[y].GetOwnerOfShip());
				m_Sector[ShipKO.x][ShipKO.y].SetStarbase(FALSE, m_ShipArray[y].GetOwnerOfShip());
			}
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"),	CResourceManager::GetString("DESTROYED"));
			// Wenn das Schiff eine Flotte anführt, dann auch die Schiffe in der Flotte demontieren
			if (m_ShipArray[y].GetFleet() != 0)
				for (USHORT x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
					if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
					{
						USHORT proz = rand()%26 + 50;	// Wert zwischen 50 und 75 auswählen
						// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
						proz += m_System[ShipKO.x][ShipKO.y].GetProduction()->GetShipRecycling();
						USHORT id = m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetID() - 10000;
						m_System[ShipKO.x][ShipKO.y].SetTitanStore((int)(m_ShipInfoArray.GetAt(id).GetNeededTitan() * proz / 100));
						m_System[ShipKO.x][ShipKO.y].SetDeuteriumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDeuterium() * proz / 100));
						m_System[ShipKO.x][ShipKO.y].SetDuraniumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededDuranium() * proz / 100));
						m_System[ShipKO.x][ShipKO.y].SetCrystalStore((int)(m_ShipInfoArray.GetAt(id).GetNeededCrystal() * proz / 100));
						m_System[ShipKO.x][ShipKO.y].SetIridiumStore((int)(m_ShipInfoArray.GetAt(id).GetNeededIridium() * proz / 100));
						pMajor->GetEmpire()->SetLatinum((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x),
							m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
							CResourceManager::GetString("DISASSEMBLY"), CResourceManager::GetString("DESTROYED"));
					}
			m_ShipArray.RemoveAt(y--);
			continue;	// continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
		}
		
		// Wenn wir ein Schiff zum Flagschiff ernennen wollen (nur ein Schiff pro Imperium kann ein Flagschiff sein!)
		else if (m_ShipArray[y].GetCurrentOrder() == ASSIGN_FLAGSHIP && m_ShipArray[y].GetFleet() == 0)
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip()));
			ASSERT(pMajor);
			network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());

			// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff finden, diesem den
			// Titel wegnehmen
			for (USHORT n = 0; n < m_ShipArray.GetSize(); n++)
				if (m_ShipArray[n].GetOwnerOfShip() == m_ShipArray[y].GetOwnerOfShip())
				{
					if (m_ShipArray[n].GetIsShipFlagShip() == TRUE)
					{
						m_ShipArray[n].SetIsShipFlagShip(FALSE);
						m_ShipArray[n].SetCurrentOrder(ATTACK);
						break;
					}
					// überprüfen ob ein Flagschiff in einer Flotte ist
					else if (m_ShipArray[n].GetFleet() != 0)
					{
						for (USHORT m = 0; m < m_ShipArray[n].GetFleet()->GetFleetSize(); m++)
							if (m_ShipArray[n].GetFleet()->GetShipFromFleet(m).GetIsShipFlagShip() == TRUE)
							{
								m_ShipArray[n].GetFleet()->GetPointerOfShipFromFleet(m)->SetIsShipFlagShip(FALSE);
								m_ShipArray[n].GetFleet()->GetPointerOfShipFromFleet(m)->SetCurrentOrder(m_ShipArray[n].GetCurrentOrder());
								break;
							}
					}
				}
			// Jetzt das neue Schiff zum Flagschiff ernennen
			m_ShipArray[y].SetIsShipFlagShip(TRUE);
			m_ShipArray[y].SetCurrentOrder(ATTACK);
			// Nachricht generieren, dass ein neues Schiff zum Flagschiff ernannt wurde
			CString s = CResourceManager::GetString("ASSIGN_FLAGSHIP_MESSAGE",FALSE,m_ShipArray[y].GetShipName(),m_ShipArray[y].GetShipTypeAsString());
			message.GenerateMessage(s,MILITARY,"",ShipKO,FALSE);
			pMajor->GetEmpire()->AddMessage(message);
			if (pMajor->IsHumanPlayer())
				m_iSelectedView[client] = EMPIRE_VIEW;
		}
		else if (m_ShipArray[y].GetCurrentOrder() == TRAIN_SHIP)
		{
			// Checken ob der Befehl noch Gültigkeit hat
			if (m_Sector[ShipKO.x][ShipKO.y].GetSunSystem() == TRUE &&
				m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == m_ShipArray[y].GetOwnerOfShip())
			{
				// Wenn ein Schiff mit Veteranenstatus (Level 4) in der Trainingsflotte ist, dann verdoppelt sich der Erfahrungsgewinn
				// für die niedrigstufigen Schiffe
				int XP = m_System[ShipKO.x][ShipKO.y].GetProduction()->GetShipTraining();
				bool isVeteran = false;
				if (m_ShipArray[y].GetExpLevel() >= 4)
					isVeteran = true;
				else if (m_ShipArray[y].GetFleet() != 0)
				{
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
						if (m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->GetExpLevel() >= 4)
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
						if (isVeteran == false || m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->GetExpLevel() >= 4)
							m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->SetCrewExperiance(XP);
						else
							m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->SetCrewExperiance(XP * 2);
					}
			}
		}
		else if (m_ShipArray[y].GetCurrentOrder() == CLOAK)
		{
			m_ShipArray[y].SetCloak();
			// Wenn das Schiff eine Flotte anführt, checken ob der Tarnenbefehl noch Gültigkeit hat. Wenn ja, dann
			// alle Schiffe in der Flotte tarnen
			if (m_ShipArray[y].GetFleet() != 0)
				if (m_ShipArray[y].GetFleet()->CheckOrder(&m_ShipArray[y], CLOAK) == TRUE)
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
						m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->SetCloak();
			// Wenn das Schiff enttarnt wurde, dann alle Schiffe in der Flotte entarnen. Dies sollte nicht häufig vorkommen. Selbst
			// kann man es so nicht einstellen, aber die KI enttarnt so die Schiffe in der Flotte
			if (m_ShipArray[y].GetCloak() == FALSE)
				if (m_ShipArray[y].GetFleet() != 0)
					for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
						if (m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->GetCloak() == TRUE)
							m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->SetCloak();
			// Befehl wieder auf Angreifen stellen
			m_ShipArray[y].SetCurrentOrder(ATTACK);
		}
		// Blockadebefehl
		else if (m_ShipArray[y].GetCurrentOrder() == BLOCKADE_SYSTEM)
		{
			BOOLEAN blockadeStillActive = FALSE;
			// überprüfen ob der Blockadebefehl noch Gültigkeit hat
			if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() != m_ShipArray[y].GetOwnerOfShip())
				// handelt es sich beim Systembesitzer um eine andere Majorrace
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() != "")
				{
					CString systemOwner = m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem();
					CString shipOwner   = m_ShipArray[y].GetOwnerOfShip();
					CRace* pShipOwner	= m_pRaceCtrl->GetRace(shipOwner);
					// haben wir einen Vertrag kleiner einem Freundschaftsvertrag mit der Majorrace
					if (pShipOwner->GetAgreement(systemOwner) < FRIENDSHIP_AGREEMENT)
					{
						int blockadeValue = m_System[ShipKO.x][ShipKO.y].GetBlockade();
						if (m_ShipArray[y].HasSpecial(BLOCKADESHIP))
						{
							blockadeValue += rand()%20 + 1;
							blockadeStillActive = TRUE;
							CalcShipExp(&m_ShipArray[y]);
						}
						// Wenn das Schiff eine Flotte anführt, dann erhöhen auch alle Schiffe in der Flotte mit
						// Blockadeeigenschaft den Blockadewert
						if (m_ShipArray[y].GetFleet() != 0)
							for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
							{
								if (m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->HasSpecial(BLOCKADESHIP))
								{
									blockadeValue += rand()%20 + 1;
									blockadeStillActive = TRUE;
									CalcShipExp(m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x));
								}
								else
									m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->SetCurrentOrder(ATTACK);
							}
						m_System[ShipKO.x][ShipKO.y].SetBlockade((BYTE)blockadeValue);
						// Die Beziehung zum Systembesitzer verringert sich um bis zu maximal 10 Punkte
						CRace* pSystemOwner	= m_pRaceCtrl->GetRace(systemOwner);
						pSystemOwner->SetRelation(shipOwner, -rand()%(blockadeValue/10 + 1));
					}
				}
			// kann der Blockadebefehl nicht mehr ausgeführt werden, so wird der Befehl automatisch gelöscht
			if (!blockadeStillActive)
			{
				m_ShipArray[y].SetCurrentOrder(ATTACK);
				if (m_ShipArray[y].GetFleet() != 0)
					m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
			}
			// wird das System schlußendlich blockiert, so produzieren die Handelsrouten kein Latinum mehr
			if (m_System[ShipKO.x][ShipKO.y].GetBlockade() > NULL)
			{
				// Wird das System blockiert, so generiert die Handelsroute kein Latinum
				for (int i = 0; i < m_System[ShipKO.x][ShipKO.y].GetTradeRoutes()->GetSize(); i++)
					m_System[ShipKO.x][ShipKO.y].GetTradeRoutes()->GetAt(i).SetLatinum(NULL);

				// Eventscreen für den Angreifer und den Blockierten anlegen
				CRace* pShipOwner = m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip());
				if (pShipOwner != NULL && pShipOwner->GetType() == MAJOR && ((CMajor*)pShipOwner)->IsHumanPlayer())
				{					
					CEventBlockade* eventScreen = new CEventBlockade(m_ShipArray[y].GetOwnerOfShip(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));
					((CMajor*)pShipOwner)->GetEmpire()->GetEventMessages()->Add(eventScreen);
				}
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() != "")
				{
					CRace* pSystemOwner = m_pRaceCtrl->GetRace(m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem());
					if (pSystemOwner != NULL && pSystemOwner->GetType() == MAJOR && ((CMajor*)pSystemOwner)->IsHumanPlayer())
					{
						CEventBlockade* eventScreen = new CEventBlockade(m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));					
						((CMajor*)pSystemOwner)->GetEmpire()->GetEventMessages()->Add(eventScreen);
					}
				}
			}
		}

		// Vor der Schiffsbewegung aber nach einer möglichen Demontage dort überall einen ShipPort setzen wo
		// eine Sternbasis oder ein Außenposten steht
		if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
		{
			m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE, m_ShipArray[y].GetOwnerOfShip());
			CRace* pShipOwner = m_pRaceCtrl->GetRace(m_ShipArray[y].GetOwnerOfShip());
			if (pShipOwner != NULL && pShipOwner->GetType() == MAJOR)
				((CMajor*)pShipOwner)->GetStarmap()->AddBase(Sector((char)m_ShipArray[y].GetKO().x,(char)m_ShipArray[y].GetKO().y),
				((CMajor*)pShipOwner)->GetEmpire()->GetResearch()->GetPropulsionTech());
		}
	}

	// jetzt alle durch einen Sternbasisbau verschwundenen Außenposten aus dem Feld entfernen
	for (int i = 0; i < removeableOutposts.GetSize(); i++)
		for (int y = 0; y < m_ShipArray.GetSize(); y++)
			if (removeableOutposts.GetAt(i).GetShipName() == m_ShipArray.GetAt(y).GetShipName())
			{
				m_ShipArray.RemoveAt(y);
				break;
			}
}

/// Diese Funktion berechnet die Schiffsbewegung und noch weitere kleine Sachen im Zusammenhang mit Schiffen.
void CBotf2Doc::CalcShipMovement()
{
	// Jetzt die Starmap abgleichen, das wir nicht auf Gebiete fliegen können, wenn wir einen NAP mit einer Rasse haben
	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CMajor* pMajor = it->second;
		// Schiffunterstützungskosten auf NULL setzen
		pMajor->GetEmpire()->SetShipCosts(0);
		map<CString, bool> races;
		for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); itt++)
			if (it->first != itt->first && pMajor->GetAgreement(itt->first) == NON_AGGRESSION_PACT)
				races[itt->first] = true;
		pMajor->GetStarmap()->SynchronizeWithMap(m_Sector, &races);
	}
	
	// Hier kommt die Schiffsbewegung (also keine anderen Befehle werden hier noch ausgewertet, lediglich wird überprüft,
	// dass manche Befehle noch ihre Gültigkeit haben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		CString sRace = m_ShipArray[y].GetOwnerOfShip();
		// Prüfen, dass ein Terraformbefehl noch gültig ist
		if (m_ShipArray[y].GetCurrentOrder() == TERRAFORM)
		{
			CPoint p = m_ShipArray[y].GetKO();
			if (m_Sector[p.x][p.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == TRUE)
			{
				m_ShipArray[y].SetCurrentOrder(AVOID);
				m_ShipArray[y].SetTerraformingPlanet(-1);
			}
		}
		// Prüfen, dass ein Aussenpostenbaubefehl noch gültig ist
		else if (m_ShipArray[y].GetCurrentOrder() == BUILD_OUTPOST)
		{
			CPoint p = m_ShipArray[y].GetKO();
			if (m_Sector[p.x][p.y].GetOutpost(sRace) == TRUE)
				m_ShipArray[y].SetCurrentOrder(AVOID);
		}
		// Prüfen, dass ein Sternbasenbaubefehl noch gültig ist
		else if (m_ShipArray[y].GetCurrentOrder() == BUILD_STARBASE)
		{
			CPoint p = m_ShipArray[y].GetKO();
			if (m_Sector[p.x][p.y].GetStarbase(sRace) == TRUE)
				m_ShipArray[y].SetCurrentOrder(AVOID);
		}
		// weiter mit Schiffsbewegung
		Sector shipKO((char)m_ShipArray[y].GetKO().x,(char)m_ShipArray[y].GetKO().y);
		Sector targetKO((char)m_ShipArray[y].GetTargetKO().x,(char)m_ShipArray[y].GetTargetKO().y);
		Sector nextKO(-1,-1);
		if (targetKO.x != -1)
		{
			char range;
			char speed;
			// Unterscheiden, ob das Schiff eine Flotte anführt oder nicht
			if (m_ShipArray[y].GetFleet() != 0)
			{
				range = (char)(3 - m_ShipArray[y].GetFleet()->GetFleetRange(&m_ShipArray[y]));
				speed = (char)(m_ShipArray[y].GetFleet()->GetFleetSpeed(&m_ShipArray[y]));
			}
			else
			{
				range = (char)(3 - m_ShipArray[y].GetRange());
				speed = (char)(m_ShipArray[y].GetSpeed());
			}
			CRace* pRace = m_pRaceCtrl->GetRace(sRace);
			if (pRace != NULL && pRace->GetType() == MAJOR)
				nextKO = ((CMajor*)pRace)->GetStarmap()->CalcPath(shipKO,targetKO,range,speed,*m_ShipArray[y].GetPath());
			if (nextKO != Sector(-1,-1))
			{
				m_ShipArray[y].SetKO(CPoint((int)nextKO.x,(int)nextKO.y));
				// Die Anzahl speed ersten Felder in Pfad des Schiffes löschen
				if (nextKO == targetKO)
					m_ShipArray[y].GetPath()->RemoveAll();
				int high = speed;
				while (high > 0 && high < m_ShipArray[y].GetPath()->GetSize())
				{
					m_ShipArray[y].GetPath()->RemoveAt(0);
					high--;
				}
			}
		}
		
// !!!	// hier stand vorher, das die Sektoren bekannt gegeben wurden und auch der umliegende Bereich gescannt wurde.
		// dies ist jetzt erst nach dem Kampf -> überprüfen das keine Bugs auftreten
		// -> BUG: Schilde und Hülle der Schiffe in Flotte müssen hier repariert werden

		// Nach der Bewegung, aber noch vor einem möglichen Kampf werden die Schilde nach ihrem Typ wieder aufgeladen,
		// wenn wir auf einem Shipport sind, dann wird auch die Hülle teilweise wieder repariert
		// Die Schilde werden ähnlich wie im Kampf selbst um multi * Schildtyp / 400 aufgeladen.
		m_ShipArray[y].GetShield()->RechargeShields(20);
		// Wenn wir in diesem Sektor einen Shipport haben, dann wird die Hülle repariert
		if (m_Sector[m_ShipArray[y].GetKO().x][m_ShipArray[y].GetKO().y].GetShipPort(m_ShipArray[y].GetOwnerOfShip()) == TRUE)
			m_ShipArray[y].GetHull()->RepairHull();
		// Befehle an alle Schiffe in der Flotte weitergeben
		if (m_ShipArray[y].GetFleet() != 0)
		{
			m_ShipArray[y].GetFleet()->AdoptCurrentOrders(&m_ShipArray[y]);
			for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
			{
				m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->GetShield()->RechargeShields(20);			
				if (m_Sector[m_ShipArray[y].GetKO().x][m_ShipArray[y].GetKO().y].GetShipPort(m_ShipArray[y].GetOwnerOfShip()) == TRUE)
					m_ShipArray[y].GetFleet()->GetPointerOfShipFromFleet(x)->GetHull()->RepairHull();
			}
		}
	}
}

/// Diese Funktion berechnet einen möglichen Weltraumkampf und dessen Auswirkungen.
void CBotf2Doc::CalcShipCombat()
{	
	using namespace network;
	CPoint p;
	// Sektoren in denen diese Runde schon ein Kampf stattgefunden hat
	CArray<CPoint> combatSectors;


	// Jetzt gehen wir nochmal alle Sektoren durch, wenn in einem Sektor Schiffe mehrerer verschiedener Rassen sind, 
	// die Schiffe nicht auf Meiden gestellt sind und die Rassen untereinander nicht alle mindst. einen Freundschafts-
	// vertrag haben, dann kommt es in diesem Sektor zum Kampf
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
		// Wenn unser Schiff auf Angreifen gestellt ist
		if (m_ShipArray.GetAt(y).GetCurrentOrder() == ATTACK)
		{
			map<CString, BYTE> winner;
			CArray<CShip*,CShip*> pShipArray;
			pShipArray.RemoveAll();
			// Wenn in dem Sektor des Schiffes schon ein Kampf stattgefunden hat, dann findet hier keiner mehr statt
			BOOLEAN combatFind = FALSE;
			for (int i = 0; i < combatSectors.GetSize(); i++)
				if (combatSectors.GetAt(i) == m_ShipArray.GetAt(y).GetKO())
				{
					combatFind = TRUE;
					break;
				}
			if (combatFind)
				continue;
			// Wenn noch kein Kampf in dem Sektor stattfand, dann kommt es möglicherweise hier zum Kampf
			for (int i = 0; i < m_ShipArray.GetSize(); i++)
			{
				// Wenn das Schiff nicht unserer Rasse gehört
				if (m_ShipArray.GetAt(i).GetOwnerOfShip() != m_ShipArray.GetAt(y).GetOwnerOfShip())
					// Wenn das Schiff sich im gleichen Sektor befindet
					if (m_ShipArray.GetAt(i).GetKO() == m_ShipArray.GetAt(y).GetKO())
					{
						// Dann können wir diese Schiffe an einem Kampf teilnehmen lassen
						pShipArray.Add(&m_ShipArray.ElementAt(i));
						// Wenn das Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
						if (m_ShipArray.GetAt(i).GetFleet() != 0)
							for (int j = 0; j < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); j++)
								pShipArray.Add(m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j));
						// Haben wir einen Outpost oder eine Starbase hinzugefügt, dann entfernen wir erstmal
						// die Auswirkungen dieser Stationen. Diese werden später wieder hinzugefügt, falls
						// die Station nicht zerstört wurde.
						if (m_ShipArray.GetAt(i).GetShipType() == OUTPOST || m_ShipArray.GetAt(i).GetShipType() == STARBASE)
						{
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetShipPort(FALSE,m_ShipArray[i].GetOwnerOfShip());
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetOutpost(FALSE, m_ShipArray[i].GetOwnerOfShip());
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetStarbase(FALSE,m_ShipArray[i].GetOwnerOfShip());
						}
						// Haben wir aber in diesem Sektor ein System mit aktiver Werft stehen, dann fügen
						// wir den ShipPort aber gleich jetzt wieder hinzu
						if (m_System[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetProduction()->GetShipYard() == TRUE
							&& m_System[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetOwnerOfSystem() == m_ShipArray[i].GetOwnerOfShip())
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetShipPort(TRUE,m_ShipArray[i].GetOwnerOfShip());
					}
			}
			// Jetzt haben wir das Feld mit den Zeigern auf die Schiffe gefüllt
			// Nun müssen wir noch die Schiffe unserer Rasse mit eventueller Flotte hinzufügen			
			if (!pShipArray.IsEmpty())
			{
				p = m_ShipArray.GetAt(y).GetKO();
				combatSectors.Add(p);
				for (int i = 0; i < m_ShipArray.GetSize(); i++)
					if (m_ShipArray.GetAt(i).GetOwnerOfShip() == m_ShipArray.GetAt(y).GetOwnerOfShip() &&
						m_ShipArray.GetAt(i).GetKO() == m_ShipArray.GetAt(y).GetKO())
					{
						// Wenn unser Schiff eine Flotte anführt, dann auch die Zeiger auf die Schiffe in der Flotte reingeben
						if (m_ShipArray.GetAt(i).GetFleet() != 0)
							for (int j = 0; j < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); j++)
								pShipArray.Add(m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j));
						pShipArray.Add(&m_ShipArray.GetAt(i));
						// Haben wir einen Outpost oder eine Starbase hinzugefügt, dann entfernen wir erstmal
						// die Auswirkungen dieser Stationen. Diese werden später wieder hinzugefügt, falls
						// die Station nicht zerstört wurde.
						if (m_ShipArray.GetAt(i).GetShipType() == OUTPOST || m_ShipArray.GetAt(i).GetShipType() == STARBASE)
						{
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetShipPort(FALSE,m_ShipArray[i].GetOwnerOfShip());
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetOutpost(FALSE, m_ShipArray[i].GetOwnerOfShip());
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetStarbase(FALSE,m_ShipArray[i].GetOwnerOfShip());
						}
						// Haben wir aber in diesem Sektor ein System mit aktiver Werft stehen, dann fügen
						// wir den ShipPort aber gleich jetzt wieder hinzu
						if (m_System[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetProduction()->GetShipYard() == TRUE
							&& m_System[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetOwnerOfSystem() == m_ShipArray[i].GetOwnerOfShip())
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].SetShipPort(TRUE,m_ShipArray[i].GetOwnerOfShip());
				
					}

				CCombat* pCombat = new CCombat();
				// Jetzt können wir einen Kampf stattfinden lassen				
				pCombat->SetInvolvedShips(&pShipArray, m_pRaceCtrl->GetRaces());
				if (pCombat->GetReadyForCombat())
				{
					pCombat->PreCombatCalculation();
				/*	CString fg;
					fg.Format("Calculation in Sector %s\n%d",m_Sector[pShipArray[0]->GetKO().x][pShipArray[0]->GetKO().y].GetName(1),pShipArray.GetSize());
					AfxMessageBox(fg);
				*/
					pCombat->CalculateCombat(winner);				
					// Nach einem Kampf muß ich das Feld der Schiffe durchgehen und alle Schiffe aus diesem nehmen, die
					// keine Hülle mehr besitzen. Aufpassen muß ich dabei, wenn das Schiff eine Flotte anführte
					CStringArray destroyedShips;
					for (int i = 0; i < m_ShipArray.GetSize(); i++)
					{
						// Wenn das Schiff eine Flotte hatte, dann erstmal nur die Schiffe in der Flotte beachten
						// Wenn davon welche zerstört wurden diese aus der Flotte nehmen
						if (m_ShipArray.GetAt(i).GetFleet())
						{
							for (int x = 0; x < m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); x++)
								if (m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(x)->GetHull()->GetCurrentHull() < 1)
								{
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip()));
									if (pMajor)
									{
										pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[i].GetFleet()->GetShipFromFleet(x),
										m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound, 
										CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
									}
									destroyedShips.Add(m_ShipArray[i].GetFleet()->GetShipFromFleet(x).GetShipName()+" ("+m_ShipArray[i].GetFleet()->GetShipFromFleet(x).GetShipTypeAsString()+")");
									m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
								}
							m_ShipArray[i].CheckFleet();
						}
						// Wenn das Schiff selbst zerstört wurde
						if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
						{
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_ShipArray[i].GetOwnerOfShip()));
							if (pMajor)
							{
								pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[i],
								m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
							}
							destroyedShips.Add(m_ShipArray[i].GetShipName()+" ("+m_ShipArray[i].GetShipTypeAsString()+")");							
							// Wenn es keine Flotte hatte, einfach rausnehmen
							if (m_ShipArray.GetAt(i).GetFleet() == 0)
								m_ShipArray.RemoveAt(i--);
							// Wenn es eine Flotte hatte, dann die Flotte auf das nächste Schiff übergeben
							else
							{
								BYTE oldOrder = m_ShipArray.GetAt(i).GetCurrentOrder();
								CFleet f = *(m_ShipArray[i].GetFleet());
								// Flotte löschen
								m_ShipArray[i].GetFleet()->DeleteFleet();
								m_ShipArray[i].DeleteFleet();
								// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
								m_ShipArray.SetAt(i, f.RemoveShipFromFleet(0));
								// für dieses eine Flotte erstellen
								m_ShipArray[i].CreateFleet();
								for (USHORT x = 0; x < f.GetFleetSize(); x++)
									m_ShipArray[i].GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
								m_ShipArray[i].CheckFleet();
								// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
								m_ShipArray[i].SetCurrentOrder(oldOrder);
							}
						}
					}
					
					map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
					for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
					{
						CMajor* pMajor = it->second;
						network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
						// gewonnen
						if (winner[it->first] == 1)
						{
							message.GenerateMessage(CResourceManager::GetString("WIN_COMBAT", false, m_Sector[p.x][p.y].GetName(TRUE)),	MILITARY, "", NULL, FALSE);
							pMajor->GetEmpire()->AddMessage(message);
							// win a minor battle
							CString eventText = pMajor->GetMoralObserver()->AddEvent(3, pMajor->GetRaceMoralNumber());
							message.GenerateMessage(eventText, MILITARY, "", 0, 0);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;
						}
						// verloren
						else if (winner[it->first] == 2)
						{
							message.GenerateMessage(CResourceManager::GetString("LOSE_COMBAT", false, m_Sector[p.x][p.y].GetName(TRUE)), MILITARY, "", 0,0);
							pMajor->GetEmpire()->AddMessage(message);
							// lose a minorbattle
							CString eventText = pMajor->GetMoralObserver()->AddEvent(6, pMajor->GetRaceMoralNumber());
							message.GenerateMessage(eventText, MILITARY, "", 0, 0);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;
							// Die Beziehung zum Gewinner verschlechtert sich dabei. Treffen zwei computergesteuerte Rassen
							// aufeinander, so ist die Beziehungsveringerung geringer
							for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); itt++)
							{
								if (it->first != itt->first && winner[itt->first] == 1)
								{
									CMajor* pMajorLoose = itt->second;
									if (pMajor->IsHumanPlayer() == false && pMajorLoose->IsHumanPlayer() == false)
										pMajor->SetRelation(pMajorLoose->GetRaceID(), -(rand()%4));
									else
										pMajor->SetRelation(pMajorLoose->GetRaceID(), -(rand()%6 + 5));
								}
							}
						}
						// unentschieden
						else if (winner[it->first] == 3)
						{
							message.GenerateMessage(CResourceManager::GetString("DRAW_COMBAT", false, m_Sector[p.x][p.y].GetName(TRUE)), MILITARY, "", 0,0);
							pMajor->GetEmpire()->AddMessage(message);
							if (pMajor->IsHumanPlayer())
								m_iSelectedView[client] = EMPIRE_VIEW;
						}
					}
					
					for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
					{
						CMajor* pMajor = it->second;
						// Hat die Rasse an dem Kampf teilgenommen, also gewonnen oder verloren oder unentschieden
						if (winner[it->first] != 0)
							for (int j = 0; j < destroyedShips.GetSize(); j++)
							{
								CString s;
								s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,destroyedShips[j]));
								message.GenerateMessage(s, MILITARY, "", 0, 0);
								pMajor->GetEmpire()->AddMessage(message);
							}
					}
				}
				y = 0;
				pCombat->Reset();
				delete pCombat;
			}
		}
}

/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
void CBotf2Doc::CalcShipEffects()
{
	using namespace network;
	map<CString, bool> getContactWithMinor;

	map<CString, CMajor*>* pmMajors = m_pRaceCtrl->GetMajors();
	
	// Nach einem möglichen Kampf, aber natürlich auch generell die Schiffe und Stationen den Sektoren bekanntgeben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		// Hier die Sektoren "bekannt machen", wenn ein Schiff dort ist (stand vorher noch vor dem Kampf)
		// Bis jetzt wirds bekannt bei jedem Schiff, später nach von dem Schiffstyp abhängig machen
		CString sRace = m_ShipArray[y].GetOwnerOfShip();
		CPoint NewShipKO = m_ShipArray[y].GetKO();
		m_Sector[NewShipKO.x][NewShipKO.y].SetScanned(sRace);
		m_Sector[NewShipKO.x][NewShipKO.y].SetFullKnown(sRace);
		USHORT scanPower = m_Sector[NewShipKO.x][NewShipKO.y].GetScanPower(sRace);
		float boni = 1.0f;
		// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet, dann
		// wird die Scanleistung um 20% erhöht
		if (sRace == m_Sector[NewShipKO.x][NewShipKO.y].GetOwnerOfSector() && m_ShipArray[y].HasSpecial(PATROLSHIP))
			boni = 1.2f;
		if ((USHORT)(m_ShipArray[y].GetScanPower() * boni) > scanPower)
			m_Sector[NewShipKO.x][NewShipKO.y].SetScanPower((short)(m_ShipArray[y].GetScanPower() * boni), sRace);
				
		// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
		for (int j = -m_ShipArray[y].GetScanRange(); j <= m_ShipArray[y].GetScanRange(); j++)
			for (int i = -m_ShipArray[y].GetScanRange(); i <= m_ShipArray[y].GetScanRange(); i++)
				if (NewShipKO.y+j < STARMAP_SECTORS_VCOUNT && NewShipKO.y+j > -1
					&& NewShipKO.x+i < STARMAP_SECTORS_HCOUNT && NewShipKO.x+i > -1)
					if (NewShipKO.x+i != NewShipKO.x || NewShipKO.y+j != NewShipKO.y)
					{
						m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanned(sRace);
						// Teiler für die Scanstärke berechnen
						int div = max(abs(j),abs(i));
						if (div > 0)
						{
							scanPower = m_Sector[NewShipKO.x+i][NewShipKO.y+j].GetScanPower(sRace);
							if ((USHORT)(m_ShipArray[y].GetScanPower() * boni) / div > scanPower)
								m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanPower((short)(m_ShipArray[y].GetScanPower() *boni / div), sRace);
						}
					}
		// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
		CMajor* pMajor = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(sRace));
		if (pMajor)
			pMajor->GetEmpire()->AddShipCosts(m_ShipArray[y].GetMaintenanceCosts());
		// Scanstärke der Schiffe in der Flotte auf die Sektoren abhängig von der Scanrange übertragen
		if (m_ShipArray[y].GetFleet() != 0)
			for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
			{
				// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
				if (pMajor)
					pMajor->GetEmpire()->AddShipCosts(m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetMaintenanceCosts());
				// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
				CShip ship = m_ShipArray[y].GetFleet()->GetShipFromFleet(x);
				for (int j = -ship.GetScanRange(); j <= ship.GetScanRange(); j++)
					for (int i = -ship.GetScanRange(); i <= ship.GetScanRange(); i++)
						if (NewShipKO.y+j < STARMAP_SECTORS_VCOUNT && NewShipKO.y+j > -1
							&& NewShipKO.x+i < STARMAP_SECTORS_HCOUNT && NewShipKO.x+i > -1)
							{
								m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanned(sRace);
								// Teiler für die Scanstärke berechnen
								int div = max(abs(j),abs(i));
								if (div == 0)
									div = 1;
								scanPower = m_Sector[NewShipKO.x+i][NewShipKO.y+j].GetScanPower(sRace);
								// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor
								// befindet, dann wird die Scanleistung um 20% erhöht
								if (sRace == m_Sector[NewShipKO.x][NewShipKO.y].GetOwnerOfSector() && ship.HasSpecial(PATROLSHIP))
									boni = 1.2f;
								else
									boni = 1.0f;								
								if ((ship.GetScanPower() * boni / div) > scanPower)
									m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanPower((short)(ship.GetScanPower() * boni / div), sRace);
							}
			}

		CPoint p = m_ShipArray[y].GetKO();
		// Erfahrunspunkte der Schiffe anpassen
		this->CalcShipExp(&m_ShipArray[y]);

		// Die Schiffslisten der einzelnen Imperien modifizieren
		if (pMajor)
			pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y], m_Sector[p.x][p.y].GetName(TRUE));
		
		// Dem Sektor nochmal bekanntgeben, dass in ihm eine Sternbasis oder ein Außenposten steht. Weil wenn im Kampf
		// eine Station teilnahm, dann haben wir den Shipport in dem Sektor vorläufig entfernt. Es kann ja passieren,
		// dass die Station zerstört wird. Haben wir jetzt aber immernoch eine Station, dann bleibt der Shipport dort auch
		// bestehen
		if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
		{
			m_Sector[p.x][p.y].SetShipPort(TRUE, sRace);
			if (m_ShipArray[y].GetShipType() == OUTPOST)
				m_Sector[p.x][p.y].SetOutpost(TRUE, sRace);
			else
				m_Sector[p.x][p.y].SetStarbase(TRUE, sRace);
		}
		// Dem Sektor bekanntgeben, das in ihm ein Schiff ist
		if (m_ShipArray[y].GetShipType() != OUTPOST && m_ShipArray[y].GetShipType() != STARBASE) 
			m_Sector[p.x][p.y].SetOwnerOfShip(TRUE, sRace);
		// Im Sektor die NeededScanPower setzen, die wir brauchen um dort Schiffe zu sehen. Wir sehen ja keine getarneten
		// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
		// sein, ansonsten gilt dort nur Stufe 3.
		USHORT stealthPower = m_ShipArray[y].GetStealthPower() * 20;
		if (m_ShipArray[y].GetStealthPower() > 3 && m_ShipArray[y].GetCloak() == FALSE)
			stealthPower = 3 * 20;
		if (stealthPower < m_Sector[p.x][p.y].GetNeededScanPower(sRace))
			m_Sector[p.x][p.y].SetNeededScanPower(stealthPower, sRace);
		
		// Jedes Schiff erhöht auf seinem Sektor zusätzlich die Scanpower um 1
		m_Sector[p.x][p.y].SetScanPower(m_Sector[p.x][p.y].GetScanPower(sRace) + 1, sRace);
		// auch die Schiffe in einer Flotte erhöhen die Scankraft
		if (m_ShipArray[y].GetFleet())
			for (int i = 0; i < m_ShipArray.GetAt(y).GetFleet()->GetFleetSize(); i++)
			{
				m_Sector[p.x][p.y].SetScanPower(m_Sector[p.x][p.y].GetScanPower(sRace) + 1, sRace);
				// Auch die Schiffe in der Flotte beim modifizieren der Schiffslisten der einzelnen Imperien beachten
				if (pMajor)
					pMajor->GetShipHistory()->ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(i), m_Sector[p.x][p.y].GetName(TRUE));
			}

		// Wenn in dem Sektor eine Minorrace lebt, diese dem jeweiligen Imperium bekanntgeben
		if (m_Sector[p.x][p.y].GetMinorRace())
		{
			CMinor* pMinor = m_pRaceCtrl->GetMinorRace(m_Sector[p.x][p.y].GetName());
			if (pMinor != NULL && pMajor != NULL && pMinor->IsRaceContacted(sRace) == false)
			{
				pMinor->SetIsRaceContacted(sRace, true);
				pMajor->SetIsRaceContacted(pMinor->GetRaceID(), true);
				// Nachricht generieren, dass wir eine Minorrace kennengelernt haben
				CString s = CResourceManager::GetString("GET_CONTACT_TO_MINOR", FALSE, pMinor->GetRaceName());
				message.GenerateMessage(s,DIPLOMACY,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer())
				{
					network::RACE client = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
					if (getContactWithMinor[sRace] == false)
					{						
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_ALIENCONTACT, client, 1, 1.0f};
						m_SoundMessages[client].Add(entry);
						getContactWithMinor[sRace] = true;
					}
					m_iSelectedView[client] = EMPIRE_VIEW;
				}
			}
		}
		// Wenn diesem System einer anderen Majorrace gehört, wir die noch nicht kannten, dann bekanntgeben
		if (m_System[p.x][p.y].GetOwnerOfSystem() != "" &&
			sRace != "" && pMajor != NULL &&
			m_System[p.x][p.y].GetOwnerOfSystem() != sRace &&			
			pMajor->IsRaceContacted(m_System[p.x][p.y].GetOwnerOfSystem()) == FALSE)
		{
			CMajor* pSystemOwner = dynamic_cast<CMajor*>(m_pRaceCtrl->GetRace(m_System[p.x][p.y].GetOwnerOfSystem()));
			if (pSystemOwner)
			{
				pMajor->SetIsRaceContacted(pSystemOwner->GetRaceID(), true);
				pSystemOwner->SetIsRaceContacted(pMajor->GetRaceID(), true);
				// Nachricht generieren, dass wir eine Majorrace kennengelernt haben
				CString s;
				CString sect; sect.Format("%c%i",(char)(p.y+97),p.x+1);
				// der Major, der der Sektor gehört
				s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR",FALSE, pMajor->GetRaceName(),sect);
				message.GenerateMessage(s,DIPLOMACY,"",0,FALSE);
				pSystemOwner->GetEmpire()->AddMessage(message);
				// der Major, der das Schiff gehört
				s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR",FALSE, pSystemOwner->GetRaceName(),sect);
				message.GenerateMessage(s,DIPLOMACY,"",0,FALSE);
				pMajor->GetEmpire()->AddMessage(message);
				if (pMajor->IsHumanPlayer() || pSystemOwner->IsHumanPlayer())
				{
					network::RACE clientShip = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
					network::RACE clientSystem = m_pRaceCtrl->GetMappedClientID(pSystemOwner->GetRaceID());
					// Audiovorstellung der kennengelernten Majorrace
					SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_FIRSTCONTACT, clientShip, 2, 1.0f};
					m_SoundMessages[clientSystem].Add(entry);
					SNDMGR_MESSAGEENTRY entry2 = {SNDMGR_MSG_FIRSTCONTACT, clientSystem, 2, 1.0f};
					m_SoundMessages[clientShip].Add(entry2);
					m_iSelectedView[clientShip] = EMPIRE_VIEW;
					m_iSelectedView[clientSystem] = EMPIRE_VIEW;
				}
			}
		}
		// wenn zwei Schiffe einer Rasse in diesem Sektor stationiert sind, so können sich die Besitzer auch kennenlernen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			if (pMajor != NULL && pMajor->GetRaceID() != it->first)
				if (m_Sector[p.x][p.y].GetOwnerOfShip(it->first) == TRUE)
					if (pMajor->IsRaceContacted(it->first) == false)
					{
						CMajor* pOtherMajor = it->second;
						
						pMajor->SetIsRaceContacted(pOtherMajor->GetRaceID(), true);
						pOtherMajor->SetIsRaceContacted(pMajor->GetRaceID(), true);
						// Nachricht generieren, dass wir eine Majorrace kennengelernt haben
						CString s;
						CString sect; sect.Format("%c%i",(char)(p.y+97),p.x+1);
						// der Major, der das erste Schiff gehört
						s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR", FALSE, pMajor->GetRaceName() ,sect);
						message.GenerateMessage(s, DIPLOMACY, "", 0, FALSE);
						pOtherMajor->GetEmpire()->AddMessage(message);
						// der Major, der das zweite Schiff gehört
						s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR", FALSE, pOtherMajor->GetRaceName(), sect);
						message.GenerateMessage(s, DIPLOMACY, "", 0, FALSE);
						pMajor->GetEmpire()->AddMessage(message);
						if (pMajor->IsHumanPlayer() || pOtherMajor->IsHumanPlayer())
						{
							network::RACE clientShip = m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
							network::RACE clientOther = m_pRaceCtrl->GetMappedClientID(pOtherMajor->GetRaceID());
							// Audiovorstellung der kennengelernten Majorrace
							SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_FIRSTCONTACT, clientShip, 2, 1.0f};
							m_SoundMessages[clientOther].Add(entry);
							SNDMGR_MESSAGEENTRY entry2 = {SNDMGR_MSG_FIRSTCONTACT, clientOther, 2, 1.0f};
							m_SoundMessages[clientShip].Add(entry2);
							m_iSelectedView[clientShip] = EMPIRE_VIEW;
							m_iSelectedView[clientOther] = EMPIRE_VIEW;
						}
					}
	}
}

/// Diese Funktion berechnet die Schiffserfahrung in einer neuen Runde. Außer Erfahrung im Kampf, diese werden nach einem
/// Kampf direkt verteilt.
void CBotf2Doc::CalcShipExp(CShip* ship)
{
/*
Starterfahrung 0 (bis auf Schiffe, die in Systemen mit Akademien gebaut werden, die fangen mit 500 Erfahrung bereits an), Bezeichnung Unerfahren. Erfahrung steigt mit 10 pro Runde kontinuierlich an bis 500.
Erfahrung 500 = Normal. Erfahrung steigt nun mit 5 pro Runde bis 1000.
Erfahrung 1000 = Veteran. Erfahrung steigt nicht mehr.
Erfahrung 2500 = Elite.
Erfahrung 5000 = Legendär.

Für den Erfahrungsgewinn gibt es mehrere Möglichkeiten:

1. random events die pauschal jeweils immer EP geben (Ionensturm überstanden, diplomatische Eskortmission, Anomalie erforscht)
2. Forschungsmissionen (Pulsar, Neutronenstern, schwarzes Loch, Wurmloch, Nebel scannen)
3. Akademien: Bei Anwesenheit eines Veteranen- oder höher -schiffes in der Crewtrainingflotte werden die Erfahrungsgewinne von unerfahrenen und normalen Crews pro Runde verdoppelt. Das Veteranenschiff bekommt die normalen EP-Gewinne.
4. Invasionen: Schiffe bekommen (Bevölkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
5. Systemblockaden, Überfälle: Verdoppeln pro Runde Gewinn für unerfahrene und normale Crews
6. Systemangriff: (Bevölkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
7. Schiffskampf: ((Durchschnittscrewerfahrung aller gegnerischen Schiffe)/((Durchschnittscrewerfahrung aller gegnerischen Schiffe)+(Durchschnittscrewerfahrung aller eigenen Schiffe)))x(totalen Hüllenschaden am Gegner)/100. Letzteres sorgt dafür dass Schäden an erfahrenen Schiffen höher gewichtet werden (da sie ja auch seltener sind, weil erfahrenere Schiffe seltener getroffen werden). Distributiert wird dann gleichmäßig auf alle Schiffe.
*/
	
	int expAdd = 0;
	switch (ship->GetExpLevel())
	{
		case 0:	expAdd = 15;	break;
		case 1: expAdd = 10;	break;
		case 2: expAdd = 5;		break;
	}
	ship->SetCrewExperiance(expAdd);
	if (ship->GetFleet() != NULL)
		for (int x = 0; x < ship->GetFleet()->GetFleetSize(); x++)
		{
			switch (ship->GetFleet()->GetPointerOfShipFromFleet(x)->GetExpLevel())
			{
				case 0:	expAdd = 15;	break;
				case 1: expAdd = 10;	break;
				case 2: expAdd = 5;		break;
			}
			ship->GetFleet()->GetPointerOfShipFromFleet(x)->SetCrewExperiance(expAdd);
		}
}

void CBotf2Doc::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: Fügen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->Enable(FALSE);
}

BOOL CBotf2Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
//	return __super::OnOpenDocument(lpszPathName);

	MYTRACE(MT::LEVEL_INFO, "loading savegame \"%s\"\n", lpszPathName);

	CFile file;
	BYTE *lpBuf = NULL;

	{
	CFileException ex;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		MYTRACE(MT::LEVEL_ERROR, "savegame: could not open file (%s)\n", cause);
		return FALSE;
	}

	// Header lesen
	UINT nSize = 4 + 2 * sizeof(UINT);
	lpBuf = new BYTE[nSize];
	UINT nDone = file.Read(lpBuf, nSize);
	if (nDone < nSize)
	{
		MYTRACE(MT::LEVEL_ERROR, "savegame: unexpected end of file\n");
		goto error;
	}

	// Magic Number prüfen
	BYTE *p = lpBuf;
	if (memcmp(p, "BotE", 4) != 0)
	{
		MYTRACE(MT::LEVEL_ERROR, "savegame: invalid magic number\n");
		goto error;
	}
	p += 4;

	// Versionsnummer prüfen
	UINT nVersion = 0;
	memcpy(&nVersion, p, sizeof(UINT));
	p += sizeof(UINT);
	if (nVersion != DOCUMENT_VERSION)
	{
		MYTRACE(MT::LEVEL_ERROR, "savegame: wrong version\n");
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
		MYTRACE(MT::LEVEL_ERROR, "savegame: unexpected end of file\n");
		goto error;
	}

	// Dekomprimieren
	CMemFile memFile;
	if (!BotE_LzmaDecompress(lpBuf, nSize, memFile))
	{
		MYTRACE(MT::LEVEL_ERROR, "savegame: error during decompression\n");
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
	
	MYTRACE(MT::LEVEL_INFO, "storing savegame \"%s\"\n", lpszPathName);

	// Savegame schreiben
	CFileException ex;
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeBinary,
		&ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		MYTRACE(MT::LEVEL_ERROR, "savegame: could not open file (%s)\n", cause);
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
	if (!BotE_LzmaCompress(lpBuf, nSize, file))
	{
		MYTRACE(MT::LEVEL_ERROR, "savegame: error during compression\n");
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
			MYTRACE(MT::LEVEL_ERROR, "savegame: could not delete corrupted savegame (%s)\n", cause);
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