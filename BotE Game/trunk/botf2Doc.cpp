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
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	CResourceManager::Init();
	m_pIniLoader = new CIniLoader();
	
	CRect r;
	AfxGetMainWnd()->GetDesktopWindow()->GetClientRect(r);
	if (r.right != 1280 || r.bottom != 1024)
	{
		if (m_pIniLoader->GetValue("SMOOTHSCALING") == FALSE)
		{
			CString s;
			s.Format("BotE looks and runs best with a displayresolution of 1280x1024 pixel.\nIf possible you should change your displayresolution.\n\nYour current resolution is %dx%d pixel.\nPlease activate SMOOTHSCALING in the BotE.ini file to improve display quality.", r.right, r.bottom);
			AfxMessageBox(s);
		}
	}
	else
		m_pIniLoader->WriteIniString("SMOOTHSCALING", "OFF");
	
	m_pSoundManager = new CSoundManager();
	m_pSoundManager->Init(!m_pIniLoader->GetValue("HARDWARESOUND"));

	m_pGraphicPool = new CGraphicPool(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics\\");
	
	m_pStarmap = new CStarmap();
	for (int i = 0; i <= DOMINION; i++)
		starmap[i] = NULL;
	m_pAIPrios = new CAIPrios(this);

	m_pNetworkHandler = new CNetworkHandler(this);
	server.AddServerListener(m_pNetworkHandler);
	client.AddClientListener(m_pNetworkHandler);
}

CBotf2Doc::~CBotf2Doc()
{
	delete m_pIniLoader;
	delete m_pSoundManager;
	delete m_pGraphicPool;

	delete m_pAIPrios;
	if (m_pStarmap)
	{
		delete m_pStarmap;
		m_pStarmap = NULL;
	}
	for (int i = 0; i <= DOMINION; i++)
		if (starmap[i])
		{
			delete starmap[i];
			starmap[i] = NULL;
		}

	m_ShipArray.RemoveAll();
	m_ShipInfoArray.RemoveAll();
	m_MinorRaceArray.RemoveAll();

	if (CSector::m_Font)
		delete CSector::m_Font;
	if (CSector::m_NameGenerator)
		delete CSector::m_NameGenerator;

	if (m_pNetworkHandler)
	{
		server.RemoveServerListener(m_pNetworkHandler);
		client.RemoveClientListener(m_pNetworkHandler);
		delete m_pNetworkHandler;
	}
}

BOOL CBotf2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	//AfxMessageBox(ALPHA_NEWS);
	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	srand((unsigned)time(NULL));
	
	m_bDataReceived				= FALSE;
	m_bDontExit					= FALSE;
	m_bGameLoaded				= FALSE;
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
	
	m_iPlayersRace = client.GetClientRace();
	
	// Standardwerte setzen
	KO.x = 0;
	KO.y = 0;
	m_bRoundEndPressed			= FALSE;
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
	if (m_pIniLoader->GetValue("MUSIC"))
		m_pSoundManager->StartMusic((network::RACE)GetPlayersRace(), m_pIniLoader->GetFloatValue("MUSICVOLUME"));
	if (!m_pIniLoader->GetValue("SOUND"))
		m_pSoundManager->SetSoundMasterVolume(NULL);
	else
		m_pSoundManager->SetSoundMasterVolume(0.5f);

	// Mal Testweise paar Truppen anlegen
	BYTE techs[6];
	memset(techs, 0, sizeof(*techs)*6);
	USHORT res[5] = {50};	
	CTroopInfo* troopInfo = new CTroopInfo("Sicherheitskräfte","Dies ist eine relativ schwache Einheit der Terranischen Konföderation. Diese Einheiten kämpfen mit modernen Projektielwaffen, sind nicht besonders stark gepanzert und haben keine militärische Ausbildung genossen.",9,10,techs,res,180,0,HUMAN,2500,1);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Heckenschützen","Dies ist eine relativ schwache Einheit des Rotharianischen Sternenverbunds. Diese Einheiten bevorzugen eine hinterhältige Taktik, um ihre Gegner auszuschalten. Leider ist ihre militärische Grundausbildung sehr kurz.",8,10,techs,res,160,1,ROMULAN,2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Khayrin Krieger","Dies ist eine relativ schwache Einheit des Khayrin Imperiums. Diese Einheiten kämpfen mit messerscharfen Nahkampfwaffen. Dies ist aber auch schon der einzige Nachteil in einem Kampf.",12,10,techs,res,240,2,KLINGON,2500,2);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Hanuhr Raketenmänner","Ein zusammengewürfelter Haufen von Hanuhr, die mit gefährlichen Raketenabschußanlagen ausgerüstet sind. Leider ist ihre militärische Ausbildung mangelhaft.",5,10,techs,res,100,3,FERENGI,1500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Invasionsstoßtrupp","Dies ist eine relativ schwache Einheit der Cartarer Invasoren. Trotzdem sind sie hervorragend für militärische Systeminvasionen geeignet.",15,10,techs,res,250,4,CARDASSIAN,2500,3);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;
	troopInfo = new CTroopInfo("Da'unor Sniper","Dies ist eine relativ schwache Einheit der Omega Allianz. Diese Einheiten kämpfen mit Präzisionsgewehren, sind nicht besonders stark gepanzert und haben keine militärische Ausbildung genossen.",10,10,techs,res,200,5,DOMINION,2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;

	// Spezialtruppen für den Cartarer anlegen
	memset(techs, 255, sizeof(*techs)*6);
	troopInfo = new CTroopInfo("Alpha Invasoren","Diese Einheit ist eine Spezialeinhait der Cartarer Invasoren. Durch ihre perfekte Ausbildung sind sie sofort in der Lage, feindliche Systeme invasieren zu können.",25,0,techs,res,2500,6,CARDASSIAN,2500,0);
	m_TroopInfo.Add(*troopInfo);
	delete troopInfo;

	// hier werden auch die Spezialtruppen der Cartarer ins System gesteckt
	BuildTroop(6, raceKO[CARDASSIAN]);
	BuildTroop(6, raceKO[CARDASSIAN]);

	return TRUE;
}

BYTE CBotf2Doc::GetPlayersRace() const
{
	return client.GetClientRace();
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
		ar << KO;
		ar << m_iPlayersRace;
		for (int i = 0; i < 7; i++)
		{
			ar << raceKO[i];
			ar << m_iSelectedView[i];
		}
		ar << m_MinorRaceArray.GetSize();
		for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
			m_MinorRaceArray.GetAt(i).Serialize(ar);
		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		ar << m_ShipArray.GetSize();
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			m_ShipArray.GetAt(i).Serialize(ar);

		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
			ar << CTrade::GetMonopolOwner(j);
		for (int j = HUMAN; j <= DOMINION; j++)
			ar << CSystemProd::GetMoralProdEmpireWide(j);
	}
	else
	{
		int number;
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
		ar >> m_iRound;
		ar >> m_fStardate;
		ar >> KO;
		ar >> m_iPlayersRace;
		for (int i = 0; i < 7; i++)
		{
			ar >> raceKO[i];
			ar >> m_iSelectedView[i];
		}
		ar >> number;
		m_MinorRaceArray.RemoveAll();
		m_MinorRaceArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_MinorRaceArray.GetAt(i).Serialize(ar);
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
			USHORT owner;
			ar >> owner;
			CTrade::SetMonopolOwner(j,owner);
		}
		for (int j = HUMAN; j <= DOMINION; j++)
		{
			short value;
			ar >> value;
			CSystemProd::SetMoralProdEmpireWide(j, value);
		}
	}
	
	for (int y = 0; y < 20; y++)
		for (int x = 0; x < 30; x++)
		{
			if (ar.IsLoading())
				m_System[x][y].ResetSystem();
			m_Sector[x][y].Serialize(ar);
			if (m_Sector[x][y].GetSunSystem())
			{
				if (m_Sector[x][y].GetOwnerOfSector() != NOBODY || m_Sector[x][y].GetColonyOwner() != NOBODY
					|| m_Sector[x][y].GetMinorRace() == TRUE)
				m_System[x][y].Serialize(ar);
			}		
		}
	CMoralObserver::SerializeStatics(ar);
	CSector::m_NameGenerator->Serialize(ar);
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_Empire[i].Serialize(ar);
		if (ar.IsLoading())
			m_Empire[i].GenerateSystemList(m_System, m_Sector);
		m_Diplomacy[i].Serialize(ar);
		m_Trade[i].Serialize(ar);
		m_TradeHistory[i].Serialize(ar);
		m_ShipHistory[i].Serialize(ar);
		m_MajorRace[i].Serialize(ar);
		m_WeaponObserver[i].Serialize(ar);
		m_GlobalStorage[i].Serialize(ar);
	}
	m_GenShipName.Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	message.Serialize(ar);
	m_Statistics.Serialize(ar);

	if (ar.IsLoading())
	{
		GenerateStarmap();		
	}
}

/// Serialisiert die Daten, welche am Anfang des Spiels einmal gesendet werden müssen.
void CBotf2Doc::SerializeBeginGameData(CArchive& ar)
{
	// senden auf Serverseite
	if (ar.IsStoring())
	{
		for (int i = 0; i < 7; i++)
			ar << raceKO[i];
		ar << BuildingInfo.GetSize();
		for (int i = 0; i < BuildingInfo.GetSize(); i++)
			BuildingInfo.GetAt(i).Serialize(ar);
	}
	// Empfangen auf Clientseite
	else
	{
		for (int i = 0; i < 7; i++)
		{
			ar >> raceKO[i];
			//m_Empire[i].GenerateSystemList(m_System, m_Sector);
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
		ar << m_MinorRaceArray.GetSize();
		for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
			m_MinorRaceArray.GetAt(i).Serialize(ar);
		ar << m_ShipInfoArray.GetSize();
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			m_ShipInfoArray.GetAt(i).Serialize(ar);
		ar << m_ShipArray.GetSize();
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			m_ShipArray.GetAt(i).Serialize(ar);
		// statische Variablen serialisieren
		for (int j = TITAN; j <= IRIDIUM; j++)
			ar << CTrade::GetMonopolOwner(j);
		for (int j = HUMAN; j <= DOMINION; j++)
			ar << CSystemProd::GetMoralProdEmpireWide(j);
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
		m_MinorRaceArray.RemoveAll();
		m_MinorRaceArray.SetSize(number);
		for (int i = 0; i < number; i++)
			m_MinorRaceArray.GetAt(i).Serialize(ar);
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
			USHORT owner;
			ar >> owner;
			CTrade::SetMonopolOwner(j,owner);
		}
		for (int j = HUMAN; j <= DOMINION; j++)
		{
			short value;
			ar >> value;
			CSystemProd::SetMoralProdEmpireWide(j, value);
		}
	}

	for (int y = 0; y < 20; y++)
		for (int x = 0; x < 30; x++)
		{
			if (ar.IsLoading())
				m_System[x][y].ResetSystem();
			m_Sector[x][y].Serialize(ar);
			if (m_Sector[x][y].GetSunSystem())
			{
				if (m_Sector[x][y].GetOwnerOfSector() != NOBODY || m_Sector[x][y].GetColonyOwner() != NOBODY
					|| m_Sector[x][y].GetMinorRace() == TRUE)
				m_System[x][y].Serialize(ar);
			}
		}

	for (int i = HUMAN; i <= DOMINION; i++)
	{	
		m_Empire[i].Serialize(ar);		
		m_Diplomacy[i].Serialize(ar);
		m_Trade[i].Serialize(ar);
		m_TradeHistory[i].Serialize(ar);
		m_ShipHistory[i].Serialize(ar);
		m_MajorRace[i].Serialize(ar);
		m_WeaponObserver[i].Serialize(ar);
		m_GlobalStorage[i].Serialize(ar);
		m_SoundMessages[i].Serialize(ar);
	}		
	m_GlobalBuildings.Serialize(ar);
	message.Serialize(ar);
	m_Statistics.Serialize(ar);

	if (ar.IsLoading())
	{
		CSmallInfoView::SetPlanet(NULL);
		GenerateStarmap();
		// Ausgehend vom Pfad des Schiffes den Sektoren mitteilen, das durch sie ein Schiff fliegt
		for (int y = 0; y < m_ShipArray.GetSize(); y++)
			if (m_ShipArray.GetAt(y).GetOwnerOfShip() == GetPlayersRace())
				for (int i = 0; i < m_ShipArray[y].GetPath()->GetSize(); i++)
					m_Sector[m_ShipArray[y].GetPath()->GetAt(i).x][m_ShipArray[y].GetPath()->GetAt(i).y].AddShipPathPoints(1);
		// Sprachmeldungen an den Soundmanager schicken
		m_pSoundManager->ClearMessages();
		for (int i = 0; i < m_SoundMessages[GetPlayersRace()].GetSize(); i++)
		{
			SNDMGR_MESSAGEENTRY* entry = &m_SoundMessages[GetPlayersRace()].GetAt(i);
			m_pSoundManager->AddMessage(entry->nMessage, entry->nRace, entry->nPriority, entry->fVolume);
		}
		// Systemliste der Imperien erstellen
		for (int i = HUMAN; i <= DOMINION; i++)
			m_Empire[i].GenerateSystemList(m_System, m_Sector);
	}
}

void CBotf2Doc::SerializeEndOfRoundData(CArchive &ar, network::RACE race)
{
	if (ar.IsStoring())
	{
		// Client-Dokument
		// Anzahl der eigenen Schiffsinfoobjekte ermitteln
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == GetPlayersRace())
				m_ShipInfoArray.GetAt(i).Serialize(ar);
		
		int number = 0;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == GetPlayersRace())
				number++;
		ar << number;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == GetPlayersRace())
				m_ShipArray.GetAt(i).Serialize(ar);
		
		number = 0;
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if (m_Sector[x][y].GetSunSystem())
				{
					if (m_Sector[x][y].GetOwnerOfSector() == GetPlayersRace() && m_System[x][y].GetOwnerOfSystem() == GetPlayersRace())
						number++;
				}
			}		
		ar << number;
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if (m_Sector[x][y].GetSunSystem())
				{
					if (m_Sector[x][y].GetOwnerOfSector() == GetPlayersRace() && m_System[x][y].GetOwnerOfSystem() == GetPlayersRace())
					{
						ar << CPoint(x,y);
						m_System[x][y].Serialize(ar);
					}
				}
			}
		m_Empire[GetPlayersRace()].Serialize(ar);
		m_Diplomacy[GetPlayersRace()].Serialize(ar);
		m_Trade[GetPlayersRace()].Serialize(ar);
		m_GlobalStorage[GetPlayersRace()].Serialize(ar);
		// aktuelle View mit zum Server senden
		ar << m_iSelectedView[GetPlayersRace()];
	}
	else
	{
		// Server-Dokument
		for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
			if (m_ShipInfoArray.GetAt(i).GetRace() == race)
			{
				m_ShipInfoArray.GetAt(i).Serialize(ar);
				ASSERT(m_ShipInfoArray.GetAt(i).GetRace() == race);
			}
		
		int number = 0;
		ar >> number;
		for (int i = 0; i < m_ShipArray.GetSize(); i++)
			if (m_ShipArray.GetAt(i).GetOwnerOfShip() == race)
				m_ShipArray.RemoveAt(i--);
		int oldSize = m_ShipArray.GetSize();
		m_ShipArray.SetSize(oldSize + number);
		for (int i = oldSize; i < m_ShipArray.GetSize(); i++)
		{
			m_ShipArray.GetAt(i).Serialize(ar);
			ASSERT(m_ShipArray.GetAt(i).GetOwnerOfShip() == race);
		}
		
		number = 0;
		ar >> number;		
		for (int i = 0; i < number; i++)
		{
			CPoint p;
			ar >> p;
			m_System[p.x][p.y].Serialize(ar);
		}
		m_Empire[race].Serialize(ar);
		m_Diplomacy[race].Serialize(ar);
		m_Trade[race].Serialize(ar);
		m_GlobalStorage[race].Serialize(ar);
		ar >> m_iSelectedView[race];
	}	
}

void CBotf2Doc::SetKO(int m, int n)
{
	KO.x = m;
	KO.y = n;
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
	if (CSector::m_NameGenerator)
	{
		delete CSector::m_NameGenerator;
		CSector::m_NameGenerator = NULL;
	}
	CSector::InitNameGenerator();
	if (!m_bGameLoaded)
	{
		m_iRound = 1;
		// Generierungssektornamenklasse wieder neu starten
		m_ShipArray.RemoveAll();
		m_ShipInfoArray.RemoveAll();
		m_MinorRaceArray.RemoveAll();

		ReadBuildingInfosFromFile();	// Gebäude einlesen aus data-Datei
		ReadShipInfosFromFile();		// Schiffe einlesen aus data-Datei
		
		m_pStarmap->ClearAll();
		
		// Werte für Moraländerungen auf verschiedene Ereignisse laden
		CMoralObserver::InitMoralMatrix();
		// Alles über die Imperien sowie Sektoren und alte Rassenkoordinaten zurücksetzen
		for (int i = HUMAN; i <= DOMINION; i++)
		{
			raceKO[i] = CPoint(-1,-1);
			
			m_Empire[i].Reset();					// Alle Werte vorher wieder auf Standard setzen
			m_Empire[i].SetEmpireName(i);			// Namen des Imperiums setzten (HUMAN == 1, FERENGI == 2 usw.)
			m_Diplomacy[i].Reset();					// Alle Werte vorher wieder auf Standard setzen
			m_Diplomacy[i].SetDiplomacyNumber(i);	// Damit wir im Objekt wissen, zu welcher Rasse dieses Objekt gehört
			m_MajorRace[i].Reset();					// Alle Werte vorher wieder auf Standard setzen
			m_MajorRace[i].SetRaceNumber(i);		// Nummer der Majorrace setzten (HUMAN == 1, FERENGI == 2 usw.)
			m_GlobalStorage[i].Reset();				// Alle Werte vorher wieder auf Standard setzen
			// wird das Imperium von einem Menschen oder vom Computer gespielt
			if (server.IsPlayedByClient((network::RACE)i))
				m_Empire[i].SetPlayerOfEmpire(PLAYER);
			else
			
				m_Empire[i].SetPlayerOfEmpire(COMPUTER);
			//////////////////////////////////////////////////////////////////////////////////////////
			m_WeaponObserver[i].Reset();			// Alle Werte der baubaren Waffen wieder auf Ausgangswerte setzen
			m_Trade[i].Reset();						// Alle Werte des Handelsobjektes auf Standard setzen
			m_Trade[i].SetRaceNumber(i);			// Nummer der Majorrace setzten (HUMAN == 1, FERENGI == 2 usw.)
			m_TradeHistory[i].Reset();				// Alle Werte aus der Kurshistory werden gelöscht
			m_ShipHistory[i].Reset();				// Alle Werte aus der Schiffshistory werden gelöscht
			// Hier die Variablen für die Handelsklasse rassenspezifisch setzen
			// die Handelssteuerrate festsetzen (alle 15% außer bis jetzt der Ferengi, der hat 5%)
			switch(m_MajorRace[i].GetKind())
			{
			case	FINANCIAL: m_Trade[i].SetTax(1.05f); break;
			default: m_Trade[i].SetTax(1.15f);
			}
			// Hier die aktuellen Kursdaten in die History schreiben
			USHORT* resPrices = m_Trade[i].GetRessourcePrice();
			m_TradeHistory[i].SaveCurrentPrices(resPrices, m_Trade[i].GetTax());
		}		

		// Sektoren generieren
		GenerateGalaxy();
		
		ApplyShipsAtStartup();
		ApplyBuildingsAtStartup();
		
/*		double habis = 0;
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
		AfxMessageBox(s);*/
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
	// die Schleife zufällig initialisieren
	int start = rand()%DOMINION + 1;
	int t = HUMAN;
	for (int i = start; t <= DOMINION;)
	{
		// Variable, welche die Quadranten der einzelnen Rassen beinhaltet. Die zufällig erste ausgewählte Rasse
		// kommt in den ersten Quadranten. Die Letzte in den letzten Quadranten.
		CPoint quadrant;
		switch (t)
		{
		case 1: quadrant.SetPoint(0, 0); break;
		case 2: quadrant.SetPoint(STARMAP_SECTORS_HCOUNT/3, 0); break;
		case 3: quadrant.SetPoint(STARMAP_SECTORS_HCOUNT/1.5, 0); break;
		case 4: quadrant.SetPoint(0, STARMAP_SECTORS_VCOUNT/2); break;
		case 5: quadrant.SetPoint(STARMAP_SECTORS_HCOUNT/3, STARMAP_SECTORS_VCOUNT/2); break;
		case 6: quadrant.SetPoint(STARMAP_SECTORS_HCOUNT/1.5, STARMAP_SECTORS_VCOUNT/2); break;
		}
		// Zufällig einen Sektor ermitteln. Dies wird solange getan, solange der ermittelte Sektor noch nicht die
		// if Bedingung erfüllt oder die Abbruchbedingung erreicht ist.
		BOOLEAN restart = FALSE;
		int count = 0;
		do {
			raceKO[i].SetPoint(rand()%(STARMAP_SECTORS_HCOUNT/3) + quadrant.x, rand()%(STARMAP_SECTORS_VCOUNT/2) + quadrant.y);
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				if (i != j && raceKO[j] != CPoint(-1,-1) && abs(raceKO[i].x - raceKO[j].x) < 8 && abs(raceKO[i].y - raceKO[j].y) < 8)
					raceKO[i] = CPoint(-1,-1);
			}
			// Abbruchbedingung
			if (++count > 250)
				restart = TRUE;
		} while (raceKO[i] == CPoint(-1,-1) && restart == FALSE);
		// Variablen der Zählschleife aktuallisieren
		if (i == DOMINION)
			i = HUMAN;
		else
			i++;
		t++;
		// Es konnte kein Sektor nach 250 Durchläufen gefunden werden, welcher der obigen if Bedingung genügt, so
		// wird die for Schleife nochmal neu gestartet.
		if (restart)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
				raceKO[j] = CPoint(-1,-1);
			i = start;
			t = HUMAN;
		}
	}

	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_Sector[raceKO[i].x][raceKO[i].y].SetSectorsName(m_MajorRace[i].GetHomeSystemName());
		m_Sector[raceKO[i].x][raceKO[i].y].SetSunSystem(TRUE);
		m_Sector[raceKO[i].x][raceKO[i].y].SetFullKnown(i);
		m_Sector[raceKO[i].x][raceKO[i].y].SetOwned(TRUE);
		m_Sector[raceKO[i].x][raceKO[i].y].SetOwnerOfSector(i);
		m_Sector[raceKO[i].x][raceKO[i].y].SetColonyOwner(i);
		m_Sector[raceKO[i].x][raceKO[i].y].CreatePlanets(i);
		m_System[raceKO[i].x][raceKO[i].y].SetOwnerOfSystem(i);
		m_System[raceKO[i].x][raceKO[i].y].SetRessourceStore(TITAN, 1000);
		m_System[raceKO[i].x][raceKO[i].y].SetRessourceStore(DILITHIUM, 3);
		
		// 2 Sonnensystem in unmittelbarer Umgebung des Heimatsystems anlegen
		BYTE nextSunSystems = 0;
		while (nextSunSystems < 2)
		{
			// Punkt mit Koordinaten zwischen -1 und +1 generieren
			CPoint ko(rand()%3 - 1, rand()%3 - 1);
			if (raceKO[i].x + ko.x < STARMAP_SECTORS_HCOUNT && raceKO[i].x + ko.x > -1
				&& raceKO[i].y + ko.y < STARMAP_SECTORS_VCOUNT && raceKO[i].y + ko.y > -1)
				if (!m_Sector[raceKO[i].x + ko.x][raceKO[i].y + ko.y].GetSunSystem())
				{
					m_Sector[raceKO[i].x + ko.x][raceKO[i].y + ko.y].GenerateSector(100,25);
					nextSunSystems++;
				}
		};

		// In einem Radius von einem Feld um das Hauptsystem die Sektoren scannen
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= 1; x++)
				if (raceKO[i].y + y < STARMAP_SECTORS_VCOUNT && raceKO[i].y + y > -1
					&& raceKO[i].x + x < STARMAP_SECTORS_HCOUNT && raceKO[i].x + x > -1)
						if (raceKO[i].x + x != raceKO[i].x || raceKO[i].y + y != raceKO[i].y)
							m_Sector[raceKO[i].x + x][raceKO[i].y + y].SetScanned(i);
	}

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
							for (int t = HUMAN; t <= DOMINION; t++)
								if (raceKO[t] == CPoint(x + i, y + j))
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
				// Nun die Minorrace generieren und in das dynamische Feld m_MinorRaceArray schreiben
				CMinorRace minorRace;
				minorRace.GenerateMinorRace(m_Sector[x][y].GetName());
				minorRace.SetRaceKO(CPoint(x,y));
				m_MinorRaceArray.Add(minorRace);
				m_System[x][y].SetOwnerOfSystem(NOBODY);	// bedeutet irgendeine kleine Rasse ist in dem System
			}
		}
	// Das MinorRaceArray alphabetisch nach dem Namen der Minorrace sortieren lassen
	c_arraysort<CArray<CMinorRace,CMinorRace>,CMinorRace>(m_MinorRaceArray,sort_asc);
}

void CBotf2Doc::NextRound()
{
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
	{
	CMyTimer timer(m_iRound);
	for (int i = HUMAN; i <= DOMINION; i++)
		m_SoundMessages[i].RemoveAll();
	
	CSectorAI* pSectorAI = new CSectorAI(this);
	pSectorAI->CalculateDangers();
	for (int i = HUMAN; i <= DOMINION; i++)
		m_iCombatShipPower[i] = pSectorAI->GetCompleteDanger(i);
	pSectorAI->CalcualteSectorPriorities();
	CShipAI* pShipAI = new CShipAI(this);
	pShipAI->CalculateShipOrders(pSectorAI);
	m_pAIPrios->Clear();
	m_pAIPrios->CalcShipPrios(pSectorAI);
	m_pAIPrios->GetIntelAI()->CalcIntelligence(this);
	this->CalcPreDataForNextRound();
	this->CalcSystemAttack();
	this->CalcMajorOffers();
	this->CalcIntelligence();
	this->CalcResearch();	
	this->CalcDiplomacy();
	long oldLatinum[7];
	for (int i = HUMAN; i <= DOMINION; i++)
		oldLatinum[i] = m_Empire[i].GetLatinum();
	this->CalcOldRoundData();
	this->CalcNewRoundData();	
	this->CalcTrade();	
	// Hier Schiffsbewegung und alles was dazugehört
	CSmallInfoView::SetShipInfo(false);
	for (int i = HUMAN; i <= DOMINION; i++)
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
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			for (int t = HUMAN; t <= DOMINION; t++)
			{	
				// Befindet sich ein Außenposten oder ein System in einem der umliegenden Sektoren, so bekommt der
				// Sektor einen Besitzerpunkt. Bei einer Sternbasis sind es sogar zwei Besitzerpunkte.
				if (m_System[x][y].GetOwnerOfSystem() == t || m_Sector[x][y].GetOutpost(t) == TRUE || m_Sector[x][y].GetStarbase(t) == TRUE)
				{
					BYTE ownerPoints = 0;
					if (m_Sector[x][y].GetStarbase(t) == TRUE)
						ownerPoints += 2;
					if (m_Sector[x][y].GetOutpost(t) == TRUE)
						ownerPoints += 1;
					if (m_System[x][y].GetOwnerOfSystem() == t)
						ownerPoints += 1;
					for (int j = -1; j <= 1; j++)
						for (int i = -1; i <= 1; i++)
							if ((y+j < STARMAP_SECTORS_VCOUNT && y+j > -1) && (x+i < STARMAP_SECTORS_HCOUNT && x+i > -1))
									m_Sector[x+i][y+j].AddOwnerPoints(ownerPoints,t);
					// in vertikaler und horizontaler Ausrichtung gibt es sogar 2 Felder vom Sector entfernt noch
					// Besitzerpunkte
					if (x-2 >= 0)
						m_Sector[x-2][y].AddOwnerPoints(ownerPoints,t);
					if (x+2 < STARMAP_SECTORS_HCOUNT)
						m_Sector[x+2][y].AddOwnerPoints(ownerPoints,t);
					if (y-2 >= 0)
						m_Sector[x][y-2].AddOwnerPoints(ownerPoints,t);
					if (y+2 < STARMAP_SECTORS_VCOUNT)
						m_Sector[x][y+2].AddOwnerPoints(ownerPoints,t);
				}
			}
	
	// hier das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse des jeweiligen Imperiums
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		if (m_ShipInfoArray.GetAt(i).GetRace() != NOBODY && m_ShipInfoArray.GetAt(i).GetRace() != UNKNOWN)
		{
			// nur aktuell baubare Schiffe dürfen überprüft werden, wenn wir die Beamwaffen checken
			if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(m_Empire[m_ShipInfoArray.GetAt(i).GetRace()].GetResearch()))
			{
				// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
				m_WeaponObserver[m_ShipInfoArray.GetAt(i).GetRace()].CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
				m_WeaponObserver[m_ShipInfoArray.GetAt(i).GetRace()].CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
			}
		}
	
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_Empire[i].GenerateSystemList(m_System, m_Sector);
		m_Empire[i].SetNumberOfSystems(m_Empire[i].GetSystemList()->GetSize());
		// Wenn das Imperium keine Systeme mehr besitzt, so wird es für alle anderen Rassen auf unbekannt gestellt.
		// Scheidet somit aus dem Spiel aus
		if (m_Empire[i].GetNumberOfSystems() == NULL)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				m_MajorRace[j].SetKnownMajorRace(i, FALSE);
				m_MajorRace[j].SetDefencePact(i, FALSE);
				m_MajorRace[j].SetDurationOfDefencePact(i, 0);
				m_MajorRace[j].SetDiplomaticStatus(i, NO_AGREEMENT);
				m_MajorRace[j].SetDurationOfAgreement(i, 0);
				m_MajorRace[i].SetKnownMajorRace(j, FALSE);
				m_MajorRace[i].SetDefencePact(j, FALSE);
				m_MajorRace[i].SetDurationOfDefencePact(j, 0);
				m_MajorRace[i].SetDiplomaticStatus(j, NO_AGREEMENT);
				m_MajorRace[i].SetDurationOfAgreement(j, 0);
			}
			for (int j = 0; j < m_MinorRaceArray.GetSize(); j++)
			{
				m_MinorRaceArray.GetAt(j).SetDiplomacyStatus(i, NO_AGREEMENT);
				m_MinorRaceArray.GetAt(j).SetKnownByMajorRace(i, FALSE);
			}
			// Alle Schiffe entfernen
			for (int j = 0; j < m_ShipArray.GetSize(); j++)
				if (m_ShipArray.GetAt(j).GetOwnerOfShip() == i)
				{
					// Alle noch "lebenden" Schiffe aus der Schiffshistory ebenfalls als zerstört ansehen
					m_ShipHistory[i].ModifyShip(&m_ShipArray[j],
								m_Sector[m_ShipArray[j].GetKO().x][m_ShipArray[j].GetKO().y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("UNKNOWN"), CResourceManager::GetString("DESTROYED"));
					m_ShipArray.RemoveAt(j--);
				}
					
		}

		// Moralveränderungen aufgrund möglicher Ereignisse berechnen. Erst nach der Schiffsbewegung und allem anderen
		m_MajorRace[i].GetMoralObserver()->CalculateEvents(m_System, i);
		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> kein Abzug beim Stellaren Lager
		if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetFieldStatus(2) == RESEARCHED)
			m_GlobalStorage[i].SetLosing(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetBonus(2));
		// Ressourcentransfers im globalen Lager vornehmen
		m_GlobalStorage[i].Calculate(m_System);
		m_GlobalStorage[i].SetMaxTakenRessources(1000 * m_Empire[i].GetNumberOfSystems());
		// Befindet sich irgendeine Ressource im globalen Lager, bekommt der Spieler eine Imperiumsmeldung
		if (m_GlobalStorage[i].IsFilled())
		{
			CString s = CResourceManager::GetString("RESOURCES_IN_GLOBAL_STORAGE");
			message.GenerateMessage(s, i, ECONOMY, "", NULL, FALSE, 4);
			m_Empire[i].AddMessage(message);
			if (server.IsPlayedByClient((network::RACE)i))
				m_iSelectedView[i] = EMPIRE_VIEW;
		}
		// Latinumänderung berechnen
		short costs = m_Empire[i].GetPopSupportCosts() - m_Empire[i].GetShipCosts();
		if (costs < 0)
			m_Empire[i].SetLatinum(costs);
		m_Empire[i].SetLatinumChange((int)(m_Empire[i].GetLatinum() - oldLatinum[i]));

		// Schiffsunterstützungskosten berechnen
		USHORT shipCosts = 0;
		for (int t = 0; t < m_ShipArray.GetSize(); t++)
			if (m_ShipArray.GetAt(t).GetOwnerOfShip() == i)
				shipCosts += m_ShipArray.GetAt(t).GetMaintenanceCosts();
		m_Empire[i].SetShipCosts(shipCosts);
	}

	// Jetzt die Besitzer berechnen und die Variablen, welche nächste Runde auch angezeigt werden sollen.
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			m_Sector[x][y].CalculateOwner(m_System[x][y].GetOwnerOfSystem());
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != NOBODY && m_System[x][y].GetOwnerOfSystem() != UNKNOWN)
			{
				m_System[x][y].CalculateVariables(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),	m_Sector[x][y].GetPlanets(), CTrade::GetMonopolOwner());
				// alle produzierten FP und SP der Imperien berechnen und zuweisen
				int currentPoints;
				currentPoints = m_System[x][y].GetProduction()->GetResearchProd();
				m_Empire[m_System[x][y].GetOwnerOfSystem()].AddFP(currentPoints);
				currentPoints = m_System[x][y].GetProduction()->GetSecurityProd();
				m_Empire[m_System[x][y].GetOwnerOfSystem()].AddSP(currentPoints);
			}
		}	

	if (m_pIniLoader->GetValue("AUTOSAVE"))
		DoSave(*((CBotf2App*)AfxGetApp())->GetPath() + "auto.sav", FALSE);
	SetModifiedFlag();

	delete pShipAI;
	delete pSectorAI;
	}	
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
			BYTE owner = atoi(s);
			VERIFY(owner >= HUMAN && owner <= DOMINION);
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
									BuildShip(m_ShipInfoArray.GetAt(j).GetID()-10000, CPoint(x,y), owner);
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

	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (!starmap[i])
			starmap[i] = new CStarmap(server.IsPlayedByServer((network::RACE)i), 3 - RANGE_MIDDLE);
	}

	// testweise mal in allen Systemen alles berechnen
	//////// kann wieder weg /////////////////////////
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_Sector[x][y].GetSunSystem() == TRUE)
			{
				m_System[x][y].SetHabitants(m_Sector[x][y].GetCurrentHabitants());
				for (int i = HUMAN; i <= DOMINION; i++)
					if (m_System[x][y].GetOwnerOfSystem() == i)
					{
						// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
						m_System[x][y].CalculateNumberOfWorkbuildings();
						m_System[x][y].SetWorkersIntoBuildings();
						m_System[x][y].CalculateVariables(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(), m_Sector[x][y].GetPlanets(), CTrade::GetMonopolOwner());
						// alle produzierten FP und SP der Imperien berechnen und zuweisen
						int currentPoints;
						currentPoints = m_System[x][y].GetProduction()->GetResearchProd();
						m_Empire[i].AddFP(currentPoints);
						currentPoints = m_System[x][y].GetProduction()->GetSecurityProd();
						m_Empire[i].AddSP(currentPoints);
					}
				for (int i = 0; i < m_System[x][y].GetAllBuildings()->GetSize(); i++)
						m_GlobalBuildings.AddGlobalBuilding(m_System[x][y].GetAllBuildings()->GetAt(i).GetRunningNumber());
						
			}

	this->CalcNewRoundData();
	this->CalcShipEffects();	
	// hier das Schiffinformationsfeld durchgehen und in die WeaponObserver-Klasse des jeweiligen Imperiums
	// die baubaren Waffen eintragen. Wir brauchen dies um selbst Schiffe designen zu können
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		if (m_ShipInfoArray.GetAt(i).GetRace() != NOBODY && m_ShipInfoArray.GetAt(i).GetRace() != UNKNOWN)
			// nur aktuell baubare Schiffe dürfen überprüft werden
			if (m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(m_Empire[m_ShipInfoArray.GetAt(i).GetRace()].GetResearch()))
			{
				// Wenn die jeweilige Rasse dieses technologisch bauen könnte, dann Waffen des Schiffes checken
				m_WeaponObserver[m_ShipInfoArray.GetAt(i).GetRace()].CheckBeamWeapons(&m_ShipInfoArray.GetAt(i));
				m_WeaponObserver[m_ShipInfoArray.GetAt(i).GetRace()].CheckTorpedoWeapons(&m_ShipInfoArray.GetAt(i));
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
				info.SetIsBuildingOnline(atoi(data[124]));
				info.SetWorker(atoi(data[125]));
				info.SetNeverReady(atoi(data[126]));
				info.SetEquivalent(NOBODY,0);		// niemand-index immer auf NULL setzen
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
	file.Close();							// Datei wird geschlossen
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
	file.Close();							// Datei wird geschlossen
}

// Später noch hinzufügen, dass auch andere Rassen bauen können
void CBotf2Doc::BuildBuilding(int z, CPoint KO)
{
	m_System[KO.x][KO.y].AddNewBuilding((CBuilding)BuildingInfo[z-1]);
}

void CBotf2Doc::BuildShip(int ID, CPoint KO, BYTE owner)
{
	if (owner < HUMAN || owner > DOMINION)
	{
		MessageBox(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), "No minorrace ships allowed in this version!\n\nPlease check your StartShips.data file.", "Error", MB_ICONERROR);
		return;
	}
	//VERIFY(owner >= HUMAN && owner <= DOMINION);


	BYTE race = owner;
	m_ShipArray.Add((CShip)m_ShipInfoArray.GetAt(ID));
	int n = m_ShipArray.GetUpperBound();
	// Schiffsnamen vergeben
	if (m_ShipArray.GetAt(n).GetShipType() != OUTPOST && m_ShipArray.GetAt(n).GetShipType() != STARBASE)
		m_ShipArray.ElementAt(n).SetShipName(m_GenShipName.GenerateShipName(race, FALSE));
	else
		m_ShipArray.ElementAt(n).SetShipName(m_GenShipName.GenerateShipName(race, TRUE));
	m_ShipArray.ElementAt(n).SetOwnerOfShip(race);
	m_ShipArray.ElementAt(n).SetKO(KO);

	// bei Minorraceschiffen braucht der Rest nicht mehr beachtet werden
	if (owner == UNKNOWN)
		return;

	// mögliche Verbesserungen durch die Spezialforschung werden hier beachtet
	// Spezialforschung #0: "Waffentechnik"
	if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetComplexStatus() == RESEARCHED)
	{
		// 20% erhoehter Phaserschaden
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(1) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetBeamWeapons()->GetSize(); i++)
			{
				USHORT oldPower = m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).GetBeamPower();
				m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).SetBeamPower(oldPower 
					+ (oldPower * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(1) / 100));
			}
		}
		// 20% erhoehte Torpedogenauigkeit
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(2) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldAcc = m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).GetAccuracy();
				m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).SetAccuracy(oldAcc
					+ (oldAcc * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(2) / 100));
			}
		}
		// 20% erhoehte Schussfreuquenz
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetFieldStatus(3) == RESEARCHED)
		{
			for (int i = 0; i < m_ShipArray.GetAt(n).GetBeamWeapons()->GetSize(); i++)
			{
				BYTE oldRate = m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).GetRechargeTime();
				m_ShipArray.GetAt(n).GetBeamWeapons()->GetAt(i).SetRechargeTime(oldRate
					- (oldRate * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(3) / 100));
			}
			for (int i = 0; i < m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldRate = m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).GetTupeFirerate();
				m_ShipArray.GetAt(n).GetTorpedoWeapons()->GetAt(i).SetTubeFirerate(oldRate
					- (oldRate * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(0)->GetBonus(3) / 100));
			}
		}
	}
	// Spezialforschung #1: "Konstruktionstechnik"
	if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetComplexStatus() == RESEARCHED)
	{
		// 20% bessere Schilde
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(1) == RESEARCHED)
		{
			UINT maxShield = m_ShipArray.GetAt(n).GetShield()->GetMaxShield();
			BYTE shieldType = m_ShipArray.GetAt(n).GetShield()->GetShieldType();
			BOOLEAN regenerative = m_ShipArray.GetAt(n).GetShield()->GetRegenerative();
			m_ShipArray.GetAt(n).GetShield()->ModifyShield((maxShield + (maxShield * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(1) / 100)), shieldType, regenerative);			
		}
		// 20% bessere Hülle
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(2) == RESEARCHED)
		{
			BOOLEAN doubleHull = m_ShipArray.GetAt(n).GetHull()->GetDoubleHull();
			BOOLEAN ablative = m_ShipArray.GetAt(n).GetHull()->GetAblative();
			BOOLEAN polarisation = m_ShipArray.GetAt(n).GetHull()->GetPolarisation();
			UINT baseHull = m_ShipArray.GetAt(n).GetHull()->GetBaseHull();
			BYTE hullMaterial = m_ShipArray.GetAt(n).GetHull()->GetHullMaterial();
			m_ShipArray.GetAt(n).GetHull()->ModifyHull(doubleHull,
				(baseHull + (baseHull * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(2) / 100)), hullMaterial,ablative,polarisation);
		}
		// 50% stärkere Scanner
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(3) == RESEARCHED)
		{
			USHORT scanPower = m_ShipArray.GetAt(n).GetScanPower();
			m_ShipArray.GetAt(n).SetScanPower(scanPower + (scanPower * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(1)->GetBonus(3) / 100));
		}
	}
	// Spezialforschung #2: "allgemeine Schiffstechnik"
	if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetComplexStatus() == RESEARCHED)
	{
		// erhoehte Reichweite für Schiffe mit zuvor kurzer Reichweite
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(1) == RESEARCHED)
		{
			if (m_ShipArray.GetAt(n).GetRange() == RANGE_SHORT)
				m_ShipArray.GetAt(n).SetRange((BYTE)(m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetBonus(1)));
		}
		// erhoehte Geschwindigkeit für Schiffe mit Geschwindigkeit 1
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(2) == RESEARCHED)
		{
			if (m_ShipArray.GetAt(n).GetSpeed() == 1)
				m_ShipArray.GetAt(n).SetSpeed((BYTE)(m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(2)->GetBonus(2)));
		}
	}
	// Spezialforschung #3: "friedliche Schiffstechnik"
	if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetComplexStatus() == RESEARCHED &&
		m_ShipArray.GetAt(n).GetShipType() <= COLONYSHIP)
	{
		// 25% erhoehte Transportkapazitaet
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetFieldStatus(1) == RESEARCHED)
		{
			USHORT storage = m_ShipArray.GetAt(n).GetStorageRoom();
			m_ShipArray.GetAt(n).SetStorageRoom(storage + (storage * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetBonus(1) / 100));
		}
		// keine Unterhaltskosten
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(3)->GetFieldStatus(2) == RESEARCHED)
		{
			m_ShipArray.GetAt(n).SetMaintenanceCosts(0);
		}
	}
	
	for (int i = 0; i < 4; i++)
		m_ShipArray.ElementAt(n).SetTargetKO(CPoint(-1,-1), i);
	m_ShipHistory[race].AddShip(&m_ShipArray.GetAt(n), m_Sector[KO.x][KO.y].GetName(), m_iRound);
}

/// Die Truppe mit der ID <code>ID</code> wird im System mit der Koordinate <code>ko</code> gebaut.
void CBotf2Doc::BuildTroop(BYTE ID, CPoint ko)
{
	// Mal Testweise paar Truppen anlegen
	m_System[ko.x][ko.y].AddTroop((CTroop*)&m_TroopInfo.GetAt(ID));
	BYTE race = m_Sector[ko.x][ko.y].GetOwnerOfSector();
	int n = m_System[ko.x][ko.y].GetTroops()->GetUpperBound();

	// Spezialforschung #4: "Truppen"
	if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetComplexStatus() == RESEARCHED)
	{
		// 20% verbesserte Offensive
		if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(1) == RESEARCHED)
		{
			BYTE power = m_System[ko.x][ko.y].GetTroops()->GetAt(n).GetPower();
			m_System[ko.x][ko.y].GetTroops()->GetAt(n).SetPower(
				power + (power * m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(1) / 100));
		}
		// 500 Erfahrungspunkte dazu -> erste Stufe
		else if (m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(2) == RESEARCHED)
		{
			m_System[ko.x][ko.y].GetTroops()->GetAt(n).AddExperiancePoints(m_Empire[race].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(2));
		}
	}

}

// Funktion generiert die Starmaps, so wie sie nach Rundenberechnung auch angezeigt werden können.
void CBotf2Doc::GenerateStarmap(void)
{
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (starmap[i])
			delete starmap[i];
		starmap[i] = new CStarmap(client.IsPlayedByServer((network::RACE)i), 3 - RANGE_MIDDLE);
	}
	// Starmap generieren
	m_pStarmap->ClearAll();
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if ((m_System[x][y].GetProduction()->GetShipYard() == TRUE && m_System[x][y].GetOwnerOfSystem() == this->GetPlayersRace())
				|| m_Sector[x][y].GetShipPort(this->GetPlayersRace()))
				m_pStarmap->AddBase(Sector(x,y), m_Empire[GetPlayersRace()].GetResearch()->GetPropulsionTech());
			for (int i = HUMAN; i <= DOMINION; i++)
				if (((m_System[x][y].GetProduction()->GetShipYard() == TRUE && m_System[x][y].GetOwnerOfSystem() == i)
					|| m_Sector[x][y].GetShipPort(i)))
					starmap[i]->AddBase(Sector(x,y), m_Empire[i].GetResearch()->GetPropulsionTech());
			if (m_Sector[x][y].GetSunSystem())
				for (int i = HUMAN; i <= DOMINION; i++)
					 if (m_Sector[x][y].GetOwnerOfSector() == i || m_Sector[x][y].GetOwnerOfSector() == NOBODY)
						starmap[i]->AddKnownSystem(Sector(x,y));
		}
	// Jetzt die Starmap abgleichen, das wir nicht auf Gebiete fliegen können, wenn wir einen NAP mit einer Rasse haben
	BOOLEAN races[7] = {FALSE};
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != GetPlayersRace() && m_MajorRace[GetPlayersRace()].GetDiplomacyStatus(i) == NON_AGGRESSION_PACT)
			races[i] = TRUE;
	m_pStarmap->SynchronizeWithMap(m_Sector, races);

	for (int j = HUMAN; j <= DOMINION; j++)
	{
		BOOLEAN races[7] = {FALSE};
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != j && m_MajorRace[j].GetDiplomacyStatus(i) == NON_AGGRESSION_PACT)
				races[i] = TRUE;
		starmap[j]->SynchronizeWithMap(m_Sector, races);
	}

	// nun die Berechnung für den Außenpostenbau vornehmen
	for (int i = HUMAN; i <= DOMINION; i++)
		if (client.IsPlayedByServer((network::RACE)i))
			starmap[i]->SetBadAIBaseSectors(m_Sector, i);
}

CMinorRace* CBotf2Doc::GetMinorRace(const CString& homePlanetName)
{
	for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
		if (m_MinorRaceArray.GetAt(i).GetHomeplanetName() == homePlanetName)
			return &m_MinorRaceArray.ElementAt(i);
	return 0;
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
	
	// Statistiken des Spiels berechnen, hier die durchschnittliche Forschungstufe (wie sie letzte Runde war)
	m_Statistics.CalculateAverageTechLevel(m_Empire);			// Forschung
	m_Statistics.CalculateAverageResourceStorages(m_Empire);	// Ressourcenlager
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (starmap[i])
			delete starmap[i];
		starmap[i] = new CStarmap(client.IsPlayedByServer((network::RACE)i), 3 - RANGE_MIDDLE);
	}
	m_pStarmap->ClearAll();
	for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
	{
		CPoint ko(m_MinorRaceArray.GetAt(i).GetRaceKO());
		if (this->GetSystem(ko).GetOwnerOfSystem() == NOBODY || this->GetSystem(ko).GetOwnerOfSystem() == UNKNOWN)
		{			
			// Vielleicht kolonisiert die Minorrace weitere Planeten in ihrem System
			if (m_MinorRaceArray.ElementAt(i).PerhapsExtend(this->GetSector(ko).GetPlanets()))
			{
				// dann sind im System auch weitere Einwohner hinzugekommen
				m_System[ko.x][ko.y].SetHabitants(this->GetSector(ko).GetCurrentHabitants());
			}
		}
		// Die Punkte für längere gute Beziehungen berechnen
		m_MinorRaceArray.ElementAt(i).CalculateRelationPoints();
		// Den Verbrauch der Rohstoffe der kleinen Rassen in jeder Runde berechnen
		m_MinorRaceArray.ElementAt(i).ConsumeRessources(&m_Sector[m_MinorRaceArray.GetAt(i).GetRaceKO().x][m_MinorRaceArray.GetAt(i).GetRaceKO().y]);
	}

	CArray<DiplomaticActionStruct,DiplomaticActionStruct> madeOffers[7];
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		// Alle alten Nachrichten und Antworten von anderen Majorraces aus letzter Runde löschen
		m_Empire[i].ClearMessagesAndAnswers();
		// Bevölkerungsunterstützungskosten auf NULL setzen
		m_Empire[i].SetPopSupportCosts(0);
		// verbleibende Vertragsdauer mit anderen Majorraces berechnen und gegebenenfalls Nachrichten und diplomatische
		// Auswirkungen anwenden
		if (m_MajorRace[i].DekrementDurationOfAllAgreements(&m_Empire[i]) && i == server.IsPlayedByClient((network::RACE)i))
			m_iSelectedView[i] = EMPIRE_VIEW;
		// wird das Imperium von einem Menschen oder vom Computer gespielt
		if (server.IsPlayedByClient((network::RACE)i))
			m_Empire[i].SetPlayerOfEmpire(PLAYER);
		else
			m_Empire[i].SetPlayerOfEmpire(COMPUTER);
		// Angebote aus der Diplomatieklasse an andere Imperien in deren eigene Diplomatieobjekte schreiben
		// dazu erst alle Angebote in der Variable "madeOffers" sammeln. Nach dieser for-Schleife werden diese dann
		// in ihre jeweiligen Diplomatieobjekte geschrieben.
		for (int j = 0; j < m_Diplomacy[i].GetMadeOffers()->GetSize(); j++)
			if (m_Diplomacy[i].GetMadeOffers()->GetAt(j).majorRace != i
				&& m_Diplomacy[i].GetMadeOffers()->GetAt(j).minorRace == 0
				&& m_Diplomacy[i].GetMadeOffers()->GetAt(j).round == (m_iRound-1))
			{
				madeOffers[i].Add(m_Diplomacy[i].GetMadeOffers()->GetAt(j));
			}
	}
	// Jetzt werden die diplomatischen Angebote in ihrer eigenen Objekte geschrieben
	for (int j = HUMAN; j <= DOMINION; j++)
	{
		for (int i = 0; i < madeOffers[j].GetSize(); i++)
		{
			m_Diplomacy[madeOffers[j].GetAt(i).majorRace].GetOfferFromMajorRace(
				j,
				madeOffers[j].GetAt(i).latinum,
				madeOffers[j].GetAt(i).ressource,
				madeOffers[j].GetAt(i).round,
				madeOffers[j].GetAt(i).shownText,
				madeOffers[j].GetAt(i).type,
				madeOffers[j].GetAt(i).KO,
				madeOffers[j].GetAt(i).corruptedMajor);
			if (server.IsPlayedByClient((network::RACE)madeOffers[j].GetAt(i).majorRace))
				m_iSelectedView[madeOffers[j].GetAt(i).majorRace] = 7;
		}
		madeOffers[j].RemoveAll();
	}
	// Schiffe, welche nur auf einem bestimmten System baubar sind, z.B. Schiffe von Minorraces, den Besitzer wieder
	// auf UNKNOWN setzen. In der Funktion, welche in einem System die baubaren Schiffe berechnet, wird dieser
	// Wert dann auf die richtige Rasse gesetzt. Jeder der das System dann besitzt, kann dieses Schiff bauen
	for (int i = 0; i < m_ShipInfoArray.GetSize(); i++)
		if (!m_ShipInfoArray.GetAt(i).GetOnlyInSystem().IsEmpty())
			m_ShipInfoArray.GetAt(i).SetRace(UNKNOWN);

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
				BYTE defender = m_Sector[p.x][p.y].GetOwnerOfSector();
				// Angreifer bzw. neuer Besitzer des Systems nach dem Angriff
				CArray<BYTE> attackers;
				BOOLEAN att[DOMINION] = {FALSE};
				for (int i = 0; i < m_ShipArray.GetSize(); i++)
					if (m_ShipArray.GetAt(i).GetKO() == p && m_ShipArray.GetAt(i).GetCurrentOrder() == ATTACK_SYSTEM)
						if (m_ShipArray.GetAt(i).GetOwnerOfShip() != NOBODY && m_ShipArray.GetAt(i).GetOwnerOfShip() != UNKNOWN)
							att[m_ShipArray.GetAt(i).GetOwnerOfShip()-1] = TRUE;
				for (int i = HUMAN; i <= DOMINION; i++)
					if (att[i-1])
						attackers.Add(i);			
				
				CAttackSystem* attackSystem = new CAttackSystem();
				// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
				if (defender == UNKNOWN && m_Sector[p.x][p.y].GetTakenSector() == FALSE)
				{
					attackSystem->Init(&m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y],
					m_Empire[defender].GetResearch()->GetResearchInfo(), p, this->GetMinorRace(m_Sector[p.x][p.y].GetName())->GetKind(), CTrade::GetMonopolOwner());
				}
				// Wenn eine Majorrace in dem System lebt
				else if (defender != NOBODY && attackSystem->IsDefenderNotAttacker(defender, attackers))
				{
					attackSystem->Init(&m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y],
					m_Empire[defender].GetResearch()->GetResearchInfo(), p, m_MajorRace[m_Sector[p.x][p.y].GetOwnerOfSector()].GetKind(), CTrade::GetMonopolOwner());
				}
				// Wenn niemand mehr in dem System lebt, z.B. durch Rebellion
				else
				{
					attackSystem->Init(&m_System[p.x][p.y], &m_ShipArray, &m_Sector[p.x][p.y],
					m_Empire[m_Sector[p.x][p.y].GetColonyOwner()].GetResearch()->GetResearchInfo(), p, m_MajorRace[m_Sector[p.x][p.y].GetColonyOwner()].GetKind(), CTrade::GetMonopolOwner());
				}
				// Ein Systemangriff verringert die Moral in allen System, die von uns schon erobert wurden und zuvor
				// der Majorrace gehörten, deren System hier angegriffen wird
				for (int j = 0 ; j < STARMAP_SECTORS_VCOUNT; j++)
					for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
						if (m_Sector[i][j].GetTakenSector() == TRUE && m_Sector[i][j].GetColonyOwner() == defender
							&& m_Sector[i][j].GetColonyOwner() != NOBODY && attackSystem->IsDefenderNotAttacker(defender, attackers))
							m_System[i][j].SetMoral(-rand()%5);

				// Wurde das System mit Truppen erobert, so wechselt es den Besitzer
				if (attackSystem->Calculate())
				{
					attackers.RemoveAll();
					attackers.Add(m_System[p.x][p.y].GetOwnerOfSystem());
					for (int i = 0; i < attackers.GetSize(); i++)
					{
						BYTE attacker = attackers.GetAt(i);
						/* Der Besitzer des Systems wurde in der Calculate() Funktion schon auf den neuen Besitzer
						 * umgestellt. Der Besitzer des Sektors ist aber noch der alte, wird hier dann auf einen
						 * eventuell neuen Besitzer umgestellt.
						 */
						
						// Wenn in dem System eine Minorrace beheimatet ist und das System nicht vorher schon von jemand
						// anderem militärisch erobert wurde oder die Minorace bei einem anderen Imperium schon vermitgliedelt
						// wurde, dann muss diese zuerst die Gebäude bauen
						if (m_Sector[p.x][p.y].GetMinorRace() == TRUE && m_Sector[p.x][p.y].GetTakenSector() == FALSE
							&& defender == UNKNOWN)
						{
							CMinorRace* minor = this->GetMinorRace(m_Sector[p.x][p.y].GetName());
							minor->SetSubjugated(TRUE);
							// Wenn das System noch keiner Majorrace gehört, dann Gebäude bauen
							m_System[p.x][p.y].BuildBuildingsForMinorRace(&m_Sector[p.x][p.y], &BuildingInfo, m_Statistics.GetAverageTechLevel(), minor);								
							// Sektor gilt ab jetzt als erobert.
							m_Sector[p.x][p.y].SetTakenSector(TRUE);
							m_Sector[p.x][p.y].SetOwned(TRUE);
							m_Sector[p.x][p.y].SetOwnerOfSector(attacker);
							// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
							minor->SetRelationshipToMajorRace(attacker, -100);
							// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte						
							m_System[p.x][p.y].SetMoral(-rand()%25-10);
							// ist die Moral unter 50, so wird sie auf 50 gesetzt
							if (m_System[p.x][p.y].GetMoral() < 50)
								m_System[p.x][p.y].SetMoral(50 - m_System[p.x][p.y].GetMoral());
							CString param = m_Sector[p.x][p.y].GetName();
							CString eventText = "";
							// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
							// bekannte Imperien die Nachricht der Unterwerfung senden
							for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
								if (m_MinorRaceArray.GetAt(i).GetRaceKO() == p)
								{
									for (int j = HUMAN; j <= DOMINION; j++)
									{
										m_Diplomacy[j].ClearArraysFromMinor(i);
										// An alle Majors die die Minor kennen die Nachricht schicken, dass diese unterworden wurde
										if (m_MinorRaceArray.GetAt(i).GetKnownByMajorRace(j))
										{
											message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED",0,m_MinorRaceArray.GetAt(i).GetRaceName()),j,MILITARY,param,p,0);
											m_Empire[j].AddMessage(message);
											if (server.IsPlayedByClient((RACE)j))
												m_iSelectedView[j] = EMPIRE_VIEW;
										}
									}
									break;
								}
							// Eventnachricht an den Eroberer (System erobert)
							eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(11, attacker, param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText,attacker, MILITARY, param, p, 0, 1);
								m_Empire[attacker].AddMessage(message);
								if (server.IsPlayedByClient((RACE)attacker))
									m_iSelectedView[attacker] = EMPIRE_VIEW;
							}
						}
						// Wenn das System einer Minorrace gehört, eingenommen wurde und somit befreit wird
						else if (m_Sector[p.x][p.y].GetMinorRace() == TRUE && m_Sector[p.x][p.y].GetTakenSector() == TRUE)
						{
							// Die Beziehung zur der Majorrace, die das System vorher besaß verschlechtert sich
							m_MajorRace[defender].SetRelationshipToMajorRace(attacker, -rand()%50);
							// Die Beziehung zu der Minorrace verbessert sich auf Seiten des Retters
							this->GetMinorRace(m_Sector[p.x][p.y].GetName())->SetRelationshipToMajorRace(attacker, rand()%50);
							this->GetMinorRace(m_Sector[p.x][p.y].GetName())->SetSubjugated(FALSE);
							// Eventnachricht an den, der das System verloren hat (erobertes Minorracesystem wieder verloren)
							CString param = m_Sector[p.x][p.y].GetName();
							CString eventText = "";
							eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(17, defender, param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
								m_Empire[defender].AddMessage(message);
								if (server.IsPlayedByClient((RACE)defender))
									m_iSelectedView[defender] = EMPIRE_VIEW;
							}
							// Eventnachricht an den Eroberer (Minorracesystem befreit)
							param = this->GetMinorRace(m_Sector[p.x][p.y].GetName())->GetRaceName();
							eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(13, attacker, param);
							// Eventnachricht hinzufügen
							if (!eventText.IsEmpty())
							{
								message.GenerateMessage(eventText,attacker, MILITARY, param, p, 0);
								m_Empire[attacker].AddMessage(message);
								if (server.IsPlayedByClient((RACE)attacker))
									m_iSelectedView[attacker] = EMPIRE_VIEW;
							}
							// Sektor gilt ab jetzt als nicht mehr erobert.
							m_Sector[p.x][p.y].SetTakenSector(FALSE);
							m_Sector[p.x][p.y].SetOwned(FALSE);
							m_Sector[p.x][p.y].SetOwnerOfSector(UNKNOWN);
							m_System[p.x][p.y].SetOwnerOfSystem(NOBODY);
							// Moral in dem System um rand()%50+25 erhöhen
							m_System[p.x][p.y].SetMoral(rand()%50+25);
						}
						// Eine andere Majorrace besaß das System
						else
						{
							// Beziehung zum ehemaligen Besitzer verschlechtert sich
							if (defender != NOBODY && defender != attacker)
								m_MajorRace[defender].SetRelationshipToMajorRace(attacker, -rand()%50);
							// Wenn das System zurückerobert wird, dann gilt es als befreit
							if (m_Sector[p.x][p.y].GetColonyOwner() == attacker)
							{
								m_Sector[p.x][p.y].SetTakenSector(FALSE);
								CString param = m_Sector[p.x][p.y].GetName();
								// Eventnachricht an den Eroberer (unser ehemaliges System wieder zurückerobert)
								CString eventText = "";
								eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(14, attacker, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, attacker, MILITARY, param, p, 0, 1);
									m_Empire[attacker].AddMessage(message);
									if (server.IsPlayedByClient((RACE)attacker))
										m_iSelectedView[attacker] = EMPIRE_VIEW;
								}
								if (defender != attacker)
								{
									// Eventnachricht an den, der das System verloren hat (unser erobertes System verloren)
									eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(17, defender, param);
									// Eventnachricht hinzufügen
									if (!eventText.IsEmpty())
									{
										message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
										m_Empire[defender].AddMessage(message);
										if (server.IsPlayedByClient((RACE)defender))
											m_iSelectedView[defender] = EMPIRE_VIEW;
									}
								}
								// Moral in dem System um rand()%25+10 erhöhen
								m_System[p.x][p.y].SetMoral(rand()%25+10);
							}
							// Handelte es sich dabei um das Heimatsystem einer Rasse
							else if (p == raceKO[defender])
							{
								// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
								CString param = m_Sector[p.x][p.y].GetName();
								CString eventText = "";
								eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(15, defender, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
									m_Empire[defender].AddMessage(message);
									if (server.IsPlayedByClient((RACE)defender))
										m_iSelectedView[defender] = EMPIRE_VIEW;
								}
								// Eventnachricht an den Eroberer (System erobert)
								eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(11, attacker, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, attacker, MILITARY, param, p, 0, 1);
									m_Empire[attacker].AddMessage(message);
									if (server.IsPlayedByClient((RACE)attacker))
										m_iSelectedView[attacker] = EMPIRE_VIEW;
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
									CMinorRace* minor = this->GetMinorRace(m_Sector[p.x][p.y].GetName());
									minor->SetSubjugated(TRUE);
									// Beziehung zu dieser Minorrace verschlechtert sich auf 0 Punkte
									minor->SetRelationshipToMajorRace(attacker, -100);
									// Alle diplomatischen Nachrichten der Minorrace aus den Feldern löschen und an der Minorrace
									// bekannte Imperien die Nachricht der Unterwerdung senden
									for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
										if (m_MinorRaceArray.GetAt(i).GetRaceKO() == p)
										{
											for (int j = HUMAN; j <= DOMINION; j++)
											{
												m_Diplomacy[j].ClearArraysFromMinor(i);
												// An alle Majors die die Minor kennen die Nachricht schicken, dass diese unterworden wurde
												if (m_MinorRaceArray.GetAt(i).GetKnownByMajorRace(j))
												{
													message.GenerateMessage(CResourceManager::GetString("MINOR_SUBJUGATED",0,m_MinorRaceArray.GetAt(i).GetRaceName()),j,MILITARY,param,p,0);
													m_Empire[j].AddMessage(message);
													if (server.IsPlayedByClient((RACE)j))
														m_iSelectedView[j] = EMPIRE_VIEW;
												}
											}
											break;
										}
								}
								CString eventText = "";
								// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
								if (defender != NOBODY && defender != attacker)
									eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(16, defender, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
									m_Empire[defender].AddMessage(message);
									if (server.IsPlayedByClient((RACE)defender))
										m_iSelectedView[defender] = EMPIRE_VIEW;
								}
								// Eventnachricht an den Eroberer (System erobert)
								eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(11, attacker, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, attacker, MILITARY, param, p, 0, 1);
									m_Empire[attacker].AddMessage(message);
									if (server.IsPlayedByClient((RACE)attacker))
										m_iSelectedView[attacker] = EMPIRE_VIEW;
								}
								// Sektor gilt ab jetzt als erobert.
								m_Sector[p.x][p.y].SetTakenSector(TRUE);
								// Moral in diesem System verschlechtert sich um rand()%25+10 Punkte
								m_System[p.x][p.y].SetMoral(-rand()%25-10);
								// ist die Moral unter 50, so wird sie auf 50 gesetzt
								if (m_System[p.x][p.y].GetMoral() < 50)
									m_System[p.x][p.y].SetMoral(50 - m_System[p.x][p.y].GetMoral());
							}
							m_Sector[p.x][p.y].SetOwnerOfSector(attacker);
						}
						// Variablen berechnen und Gebäude besetzen
						m_System[p.x][p.y].CalculateNumberOfWorkbuildings();
						m_System[p.x][p.y].SetWorkersIntoBuildings();

						// erfolgreiches Invasionsevent für den Angreifer einfügen
						if (attacker >= HUMAN && attacker <= DOMINION)
							m_Empire[attacker].GetEventMessages()->Add(new CEventBombardment(attacker, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
					}
					// Invasionsevent für den Verteidiger einfügen
					if (defender != UNKNOWN && attackSystem->IsDefenderNotAttacker(defender, attackers))
						m_Empire[defender].GetEventMessages()->Add(new CEventBombardment(defender, "InvasionSuccess", CResourceManager::GetString("INVASIONSUCCESSEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONSUCCESSEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
				}
				// Wurde nur bombardiert, nicht erobert
				else
				{
					CString param = m_Sector[p.x][p.y].GetName();
					CString eventText = "";
					for (int i = 0; i < attackers.GetSize(); i++)
					{
						BYTE attacker = attackers.GetAt(i);
						// Erstmal die Beziehung zu der Rasse verschlechtern, der das System gehört
						if (m_Sector[p.x][p.y].GetMinorRace() == TRUE && m_Sector[p.x][p.y].GetTakenSector() == FALSE
							&& defender == UNKNOWN)
							// Wenn mehrere Majors gleichzeitig bombardieren, wird leider nur zur ersten dieser Majors die
							// Beziehung verschlechtert. Will aber nicht viel mehr Code Schreiben, da dies so gut wie nie
							// im Spiel eintreten wird
							this->GetMinorRace(m_Sector[p.x][p.y].GetName())->SetRelationshipToMajorRace(attacker, -rand()%10);
						// Wenn das System einer Majorrace gehört (egal ob erobertes oder nicht)
						else if (defender != NOBODY && defender != attacker)
							// Das selbe wie oben bei der Minor, nur wird hier die Beziehung zur Major verschlechtert
							m_MajorRace[defender].SetRelationshipToMajorRace(attacker, -rand()%10);
					}
					// Wenn die Bevölkerung des Systems auf NULL geschrumpft ist, dann ist dieses System verloren
					if (m_System[p.x][p.y].GetHabitants() == 0.0f)
					{
						// Bei einer Minorrace wird es komplizierter. Wenn diese keine Systeme mehr hat, dann ist diese
						// aus dem Spiel verschwunden. Alle Einträge in der Diplomatie müssen daher gelöscht werden
						if (m_Sector[p.x][p.y].GetMinorRace())
						{
							m_Sector[p.x][p.y].SetMinorRace(FALSE);
							USHORT n = 0;
							for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
								if (m_MinorRaceArray.GetAt(i).GetRaceKO() == p)
								{
									n = i;
									break;
								}
							for (int i = HUMAN; i <= DOMINION; i++)
								m_Diplomacy[i].ClearArraysFromMinor(n, TRUE);
							// Eventnachricht: #21	Eliminate a Minor Race Entirely
							for (int i = 0; i < attackers.GetSize(); i++)
							{
								BYTE attacker = attackers.GetAt(i);
								CString param = m_MinorRaceArray.GetAt(n).GetRaceName();
								CString eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(21, attacker, param);
								message.GenerateMessage(eventText, attacker, MILITARY, param, p, 0);
								m_Empire[attacker].AddMessage(message);
								if (server.IsPlayedByClient((RACE)attacker))
									m_iSelectedView[attacker] = EMPIRE_VIEW;
							}
							// alle anderen Majorrassen, die diese Minor kannten, bekommen eine Nachricht über deren Vernichtung
							for (int i = HUMAN; i <= DOMINION; i++)
								if (attackSystem->IsDefenderNotAttacker(i, attackers) && m_MinorRaceArray.GetAt(n).GetKnownByMajorRace(i) == TRUE)
								{
									CString news = CResourceManager::GetString("ELIMINATE_MINOR",0,m_MinorRaceArray.GetAt(n).GetRaceName());
									message.GenerateMessage(news,i,SOMETHING,"",0,0);
									m_Empire[i].AddMessage(message);
									if (server.IsPlayedByClient((RACE)i))
										m_iSelectedView[i] = EMPIRE_VIEW;
								}
							// Minor wird aus dem Feld der Minors gelöscht
							m_MinorRaceArray.RemoveAt(n);
						}
						// Bei einer Majorrace verringert sich nur die Anzahl der Systeme (auch konnte dies das
						// Minorracesystem von oben gewesen sein, hier verliert es aber die betroffene Majorrace)
						if (defender != NOBODY && defender != UNKNOWN && attackSystem->IsDefenderNotAttacker(defender, attackers))
						{
							if (p == raceKO[defender])
							{
								// Eventnachricht an den ehemaligen Heimatsystembesitzer (Heimatsystem verloren)
								param = m_Sector[p.x][p.y].GetName();
								eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(15, defender, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
									m_Empire[defender].AddMessage(message);
									if (server.IsPlayedByClient((RACE)defender))
										m_iSelectedView[defender] = EMPIRE_VIEW;
								}
							}
							else
							{
								// Eventnachricht an den ehemaligen Besitzer (eigenes System verloren)
								eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(16, defender, param);
								// Eventnachricht hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, defender, MILITARY, param, p, 0);
									m_Empire[defender].AddMessage(message);
									if (server.IsPlayedByClient((RACE)defender))
										m_iSelectedView[defender] = EMPIRE_VIEW;
								}
							}
						}
						m_System[p.x][p.y].SetOwnerOfSystem(NOBODY);
						m_Sector[p.x][p.y].SetOwnerOfSector(NOBODY);
						m_Sector[p.x][p.y].SetColonyOwner(NOBODY);
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
							for (int i = 0; i < attackers.GetSize(); i++)
							{
								BYTE attacker = attackers.GetAt(i);
								// Wenn das System nicht durch eine Rebellion verloren ging, sondern noch irgendwem gehört
								if (defender != NOBODY)
									eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(19, attacker, param);
								// Wenn das System mal uns gehört hatte und durch eine Rebellion verloren ging
								else if (m_Sector[p.x][p.y].GetColonyOwner() == attacker && defender == NOBODY)
									eventText = m_MajorRace[attacker].GetMoralObserver()->AddEvent(20, attacker, param);
								// Eventnachricht für Agressor hinzufügen
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, attacker, MILITARY, param, p, 0);
									m_Empire[attacker].AddMessage(message);
									if (server.IsPlayedByClient((RACE)attacker))
										m_iSelectedView[attacker] = EMPIRE_VIEW;
								}							
							}
							// Eventnachricht über Bombardierung für Verteidiger erstellen und hinzufügen
							if (defender != NOBODY && defender != UNKNOWN && attackSystem->IsDefenderNotAttacker(defender, attackers))
							{
								eventText = m_MajorRace[defender].GetMoralObserver()->AddEvent(22, defender, param);
								if (!eventText.IsEmpty())
								{
									message.GenerateMessage(eventText, defender, MILITARY, param, p, 0, 1);
									m_Empire[defender].AddMessage(message);
									if (server.IsPlayedByClient((RACE)defender))
										m_iSelectedView[defender] = EMPIRE_VIEW;
								}
							}
						}
					}
					// Eventsgrafiken hinzufügen
					// für den/die Angreifer
					for (int j = 0; j < attackers.GetSize(); j++)
					{
						BYTE attacker = attackers.GetAt(j);
						if (attacker >= HUMAN && attacker <= DOMINION)
						{
							// reine Bombardierung
							if (!attackSystem->IsTroopsInvolved())
								m_Empire[attacker].GetEventMessages()->Add(new CEventBombardment(attacker, "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
							// gescheitere Invasion
							else
								m_Empire[attacker].GetEventMessages()->Add(new CEventBombardment(attacker, "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
						}
						// für den Verteidiger
						if (defender != UNKNOWN && attackSystem->IsDefenderNotAttacker(defender, attackers))
						{
							// reine Bombardierung
							if (!attackSystem->IsTroopsInvolved())
								m_Empire[defender].GetEventMessages()->Add(new CEventBombardment(defender, "Bombardment", CResourceManager::GetString("BOMBARDEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("BOMBARDEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
							// gescheitere Invasion
							else
								m_Empire[defender].GetEventMessages()->Add(new CEventBombardment(defender, "InvasionFailed", CResourceManager::GetString("INVASIONFAILUREEVENT_HEADLINE", FALSE, m_Sector[p.x][p.y].GetName()), CResourceManager::GetString("INVASIONFAILUREEVENT_TEXT_RACE1", FALSE, m_Sector[p.x][p.y].GetName())));
						}
					}
				}

				// Nachrichten hinzufügen
				for (int i = 0; i < attackSystem->GetNews()->GetSize(); )
				{
					for (int j = 0; j < attackers.GetSize(); j++)
					{
						BYTE attacker = attackers.GetAt(j);
						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), attacker, MILITARY, m_Sector[p.x][p.y].GetName(), p, 0);
						m_Empire[attacker].AddMessage(message);
						if  (server.IsPlayedByClient((RACE)attacker))
							m_iSelectedView[attacker] = EMPIRE_VIEW;
					}
					if (defender != UNKNOWN && attackSystem->IsDefenderNotAttacker(defender, attackers))
					{
						message.GenerateMessage(attackSystem->GetNews()->GetAt(i), defender, MILITARY, m_Sector[p.x][p.y].GetName(), p, 0);
						m_Empire[defender].AddMessage(message);
						if (server.IsPlayedByClient((RACE)defender))
							m_iSelectedView[defender] = EMPIRE_VIEW;
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
						m_ShipHistory[m_ShipArray[i].GetOwnerOfShip()].ModifyShip(&m_ShipArray[i].GetFleet()->GetShipFromFleet(x),
							m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound, 
							CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
						m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
					}
				m_ShipArray[i].CheckFleet();
			}
			// Wenn das Schiff selbst zerstört wurde
			if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
			{
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				m_ShipHistory[m_ShipArray[i].GetOwnerOfShip()].ModifyShip(&m_ShipArray[i],
					m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound,
					CResourceManager::GetString("SYSTEMATTACK"), CResourceManager::GetString("DESTROYED"));
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

/// Diese Funktion berechnet die Angebote von Majorraces an andere Majorraces.
void CBotf2Doc::CalcMajorOffers()
{
	using namespace network;
	// Matrix speichert, ob eine Rasse in dieser Runde einer anderen schon ein Angebot gemacht hat
	// weil bei WAR_PACT kann es passieren, das wir in der selben Runde noch ein anderes Angebot machen
	BOOLEAN madeOffer[7][7];	
	for (int i = HUMAN; i <= DOMINION; i++)
		for (int j = HUMAN; j <= DOMINION; j++)
			madeOffer[i][j] = FALSE;	// Matrix löschen
	// Berechnen, ob eine computergesteuerte Majorrace einer anderen Majorrace etwas anbietet
	for (int i = HUMAN; i <= DOMINION; i++)
		for (int j = HUMAN; j <= DOMINION; j++)
			if (j != i && m_Empire[i].GetPlayerOfEmpire() == COMPUTER && m_Empire[i].GetNumberOfSystems() > 0 && m_Empire[j].GetNumberOfSystems() > 0)
			{
				// Wir bekommen eine Struktur zurück
				ComputerOfferStruct status = m_MajorRace[i].PerhapsMakeOfferToMajorRace(j, m_MajorRace[j].GetRelationshipToMajorRace(i), 
					m_Empire, m_System, m_Statistics.GetAverageTechLevel(), m_Statistics.GetAverageResourceStorages(), m_iCombatShipPower);
				// wenn wir ein Angebot zurückbekommen, außer bei Kriegspakt
				if (status.type != NO_AGREEMENT && status.type != WAR_PACT && madeOffer[i][j] == FALSE)
				{
					// hier wird ein (round-1) übergeben, damit wir in der selben Runde noch die Nachricht bekommen
					m_Diplomacy[j].GetOfferFromMajorRace(i,status.latinum,status.ressource,(m_iRound-1),status.shownText,status.type,status.KO,status.duration);
					m_Diplomacy[i].MakeOfferToMajorRace(j,status.latinum,status.ressource,(m_iRound-1),status.shownText,status.type,status.KO,status.duration);
					if (server.IsPlayedByClient((RACE)j))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_DIPLOMATICNEWS, (RACE)j, 0, 1.0f};
						m_SoundMessages[j].Add(entry);
						m_iSelectedView[j] = EMPIRE_VIEW;
					}
					madeOffer[i][j] = TRUE;
				}
				// wenn wir ein Kriegspaktangebot zurückbekommen
				else if (status.type == WAR_PACT && madeOffer[i][status.duration] == FALSE)
				{
					// hier wird ein (round-1) übergeben, damit wir in der selben Runde noch die Nachricht bekommen
					m_Diplomacy[status.duration].GetOfferFromMajorRace(i,status.latinum,status.ressource,(m_iRound-1),status.shownText,status.type,status.KO,j);
					m_Diplomacy[i].MakeOfferToMajorRace(status.duration,status.latinum,status.ressource,(m_iRound-1),status.shownText,status.type,status.KO,j);
					if (server.IsPlayedByClient((RACE)j))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_DIPLOMATICNEWS, (RACE)j, 0, 1.0f};
						m_SoundMessages[j].Add(entry);
						m_iSelectedView[j] = EMPIRE_VIEW;
					}
					madeOffer[i][status.duration] = TRUE;
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

	// das Objekt für alle Berechnung mit Geheimdienstkontext anlegen
	CIntelCalc* intel = new CIntelCalc(this);
	if (intel)
	{
		// zuerst werden die ganzen Berechnungen durchgeführt, ohne das Punkte vorher von irgendeiner Rasse dazugerechnet werden.
		// Dadurch haben alle Rassen die selbe Chance.
		for (int i = HUMAN; i <= DOMINION; i++)
			intel->StartCalc(i);
		// danach werden die Punkte dazuaddiert und zum Schluss werden die einzelnen Depotpunkte etwas reduziert.
		for (int i = HUMAN; i <= DOMINION; i++)
		{
			intel->AddPoints(i);
			// die Intelpunkte in den Lagern werden ganz am Ende abgezogen.
			intel->ReduceDepotPoints(i);
			// Boni wieder löschen, damit für die neue und nächste Runde neue berechnet werden können
			m_Empire[i].GetIntelligence()->ClearBoni();	
			// wenn neue Geheimdienstnachrichten vorhanden sind die Meldung im Impieriumsmenü erstellen
			if (m_Empire[i].GetIntelligence()->GetIntelReports()->IsReportAdded())
			{
				// die Sortierung der Geheimdienstberichte
				m_Empire[i].GetIntelligence()->GetIntelReports()->SortAllReports();
			
				CMessage message;
				message.GenerateMessage(CResourceManager::GetString("WE_HAVE_NEW_INTELREPORTS"), i, SECURITY, "", NULL, FALSE, 4);
				m_Empire[i].AddMessage(message);
				
				SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_INTELNEWS, (network::RACE)i, 0, 1.0f};
				m_SoundMessages[i].Add(entry);
				
				BOOLEAN addSpy = FALSE;
				BOOLEAN addSab = FALSE;
				for (int j = m_Empire[i].GetIntelligence()->GetIntelReports()->GetAllReports()->GetUpperBound(); j >= 0; j--)
				{
					CIntelObject* intelObj = m_Empire[i].GetIntelligence()->GetIntelReports()->GetReport(j);
					if (intelObj->GetEnemy() != i && intelObj->GetRound() == this->GetCurrentRound())
					{
						CString eventText = "";
						if (intelObj->GetIsSpy() && !addSpy)
						{
							eventText = m_MajorRace[i].GetMoralObserver()->AddEvent(59, i);
							addSpy = TRUE;
						}
						else if (intelObj->GetIsSabotage() && !addSab)
						{
							eventText = m_MajorRace[i].GetMoralObserver()->AddEvent(60, i);
							addSab = TRUE;
						}
						// Eventnachricht hinzufügen
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, i, SECURITY, "", NULL, FALSE, 4);
							m_Empire[i].AddMessage(message);
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

	for (int i = HUMAN; i <= DOMINION; i++)
		if (server.IsPlayedByClient((network::RACE)i))
			if (m_Empire[i].GetIntelligence()->GetIntelReports()->IsReportAdded())
				m_iSelectedView[i] = EMPIRE_VIEW;
}

/// Diese Funktion berechnet die Forschung eines Imperiums
void CBotf2Doc::CalcResearch()
{
	using namespace network;

	// Forschungsboni, die die Systeme machen holen. Wir benötigen diese dann für die CalculateResearch Funktion
	short researchBonis[7][6] = {0};
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_System[x][y].GetOwnerOfSystem() != NOBODY && m_System[x][y].GetOwnerOfSystem() != UNKNOWN)
			{
				// hier Forschungsboni besorgen
				researchBonis[m_System[x][y].GetOwnerOfSystem()][0] += m_System[x][y].GetProduction()->GetBioTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][1] += m_System[x][y].GetProduction()->GetEnergyTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][2] += m_System[x][y].GetProduction()->GetCompTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][3] += m_System[x][y].GetProduction()->GetPropulsionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][4] += m_System[x][y].GetProduction()->GetConstructionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][5] += m_System[x][y].GetProduction()->GetWeaponTechBoni();
			}
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_Empire[i].GetResearch()->SetResearchBoni(researchBonis[i]);
		CString *news = 0;
		news = m_Empire[i].GetResearch()->CalculateResearch(m_Empire[i].GetFP());
		for (int j = 0; j < 8; j++)		// aktuell 8 verschiedene Nachrichten mgl, siehe CResearch Klasse
		{
			// Wenn irgendwas erforscht wurde, wir also eine Nachricht erstellen wollen
			if (news[j] != "")
			{
				// flag setzen, wenn wir eine Spezialforschung erforschen dürfen
				if (j == 7)
				{
					// Spezialforschung kann erforscht werden
					if (server.IsPlayedByClient((RACE)i))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_SCIENTISTNEWS, (RACE)i, 0, 1.0f};
						m_SoundMessages[i].Add(entry);
						m_iSelectedView[i] = EMPIRE_VIEW;
					}
					message.GenerateMessage(news[j],i,RESEARCH,"",0,FALSE,1);
				}
				else
				{
					if (server.IsPlayedByClient((RACE)i))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_NEWTECHNOLOGY, (RACE)i, 0, 1.0f};
						m_SoundMessages[i].Add(entry);
						m_iSelectedView[i] = EMPIRE_VIEW;
					}
					message.GenerateMessage(news[j],i,RESEARCH,"",0,FALSE);
				}
				m_Empire[i].AddMessage(message);
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
	int number = 0;
	for (int i = rand()%DOMINION+1; number < DOMINION; i++)
	{
		// alle diplomatischen Ereignisse berechnen
		m_Diplomacy[i].CalculateDiplomaticFallouts(&m_MinorRaceArray,m_MajorRace,m_Empire,m_iRound,m_System,
			m_Statistics.GetAverageResourceStorages(), m_GlobalStorage, m_iCombatShipPower);
		// die "Lieblingsminorrace" der KI-gesteuerten Majorraces berechnen, also der ich Geschenk bzw. Mitgift gebe
		// das sollte nämlich nicht jedesmal ne andere sein, sondern schon fast immer die selbe, solange es sich lohnt
		// ihr was anzubieten
		short favoritMinor = -1;
		if (m_Empire[i].GetPlayerOfEmpire() == COMPUTER && m_Empire[i].GetNumberOfSystems() > 0)
			favoritMinor = m_MajorRace[i].CalculateFavoritMinor(&m_MinorRaceArray);
/*		CString fg;
		if (favoritMinor != -1)
			fg.Format("%s -> Favorit: %s",CMajorRace::GetRaceName(i),m_MinorRaceArray.GetAt(favoritMinor).GetRaceName());
		else
			fg.Format("%s -> niemand",CMajorRace::GetRaceName(i));
		AfxMessageBox(fg);
*/		// Berechnen, ob die Minorraces einer MajorRace etwas anbietet bzw. eine MajorRace einer MinorRace was anbietet
		for (int j = 0; j < m_MinorRaceArray.GetSize(); j++)
		{
			// Angebote an die Minorrace oder von der Minorrace macht die KI nur, wenn die Minorrace nicht
			// militärisch unterworfen wurde
			if (m_MinorRaceArray.GetAt(j).GetSubjugated() == FALSE)
			{
				short status = m_MinorRaceArray.ElementAt(j).PerhapsMakeOfferToMajorRace(i);
				if (status != NO_AGREEMENT)
				{
					// hier wird ein (round-1) übergeben, damit wir in der selben Runde noch die Nachricht bekommen
					m_Diplomacy[i].MinorMakesOfferToMajor(j, i, (m_iRound-1), status, &m_MinorRaceArray, m_MajorRace, &m_Empire[i]);
					if (server.IsPlayedByClient((RACE)i))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_DIPLOMATICNEWS, (RACE)i, 0, 1.0f};
						m_SoundMessages[i].Add(entry);
						m_iSelectedView[i] = EMPIRE_VIEW;
					}
				}
			
				// Bietet eine KI gesteuerte Majorrace einer Minorrace mal was an?
				ComputerOfferStruct offer;
				if (m_Empire[i].GetPlayerOfEmpire() == COMPUTER && m_MinorRaceArray.GetAt(j).GetKnownByMajorRace(i) == TRUE
					&& m_Empire[i].GetNumberOfSystems() > 0)
				{
					if (j == favoritMinor)
						offer =	m_MajorRace[i].PerhapsMakeOfferToMinorRace(&m_MinorRaceArray.ElementAt(j), TRUE,
								m_Empire, m_System, m_Statistics.GetAverageTechLevel(), m_Statistics.GetAverageResourceStorages()); 
					else
						offer =	m_MajorRace[i].PerhapsMakeOfferToMinorRace(&m_MinorRaceArray.ElementAt(j));
					if (offer.type != NO_AGREEMENT)
						m_Diplomacy[i].MakeOfferToMinorRace(i, j, offer.latinum, offer.ressource, m_iRound, offer.shownText, offer.type, offer.KO, offer.duration);
				}
			}
			// Die diplomatische Konsistenz überprüfen! z.B kann niemand mehr eine Freundschaft haben, wenn jemand eine
			// Mitgliedschaft mit dieser Rasse hat. Verträge werden gekündigt, wenn die Minorrace unterworfen wurde
			m_MinorRaceArray.ElementAt(j).CheckDiplomaticConsistence(m_Empire);
			m_MinorRaceArray.ElementAt(j).PerhapsCancelAgreement(m_Empire);
			// Wenn wir mit der Minorrace mindst. einen Handelsvertrag abgeschlossen haben, dann wird deren Sector gescannt/gesehen
			if (m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(i) >= TRADE_AGREEMENT)
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetScanned(i);
			// Wenn wir mit der Minorrace mindst. einen Freundschaftsvertrag abgeschlossen haben, dann wird deren Sector bekannt
			if (m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(i) >= FRIENDSHIP_AGREEMENT)
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetKnown(i);
			// Wenn wir eine Mitgliedschaft mit der kleinen Rasse haben und das System noch der kleinen Rasse gehört, dann bekommen wir das
			if (m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(i) == MEMBERSHIP && m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].GetOwnerOfSystem() == NOBODY)
			{
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetFullKnown(i);
				m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwnerOfSystem(i);
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwnerOfSector(i);
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwned(TRUE);
// ! CHECK !	// Der Sector gilt jetzt als nicht eingenommen
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetTakenSector(FALSE);
				// Nun Gebäude in neuen System bauen
				m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].BuildBuildingsForMinorRace(&m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y],&BuildingInfo,m_Statistics.GetAverageTechLevel(),	&m_MinorRaceArray.GetAt(j));
				// Gebäude so weit wie möglich mit Arbeitern besetzen
				m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetWorkersIntoBuildings();				
			}
		}
		// hier setzen wir wieder die gesamten FP, SP und die gesamten Lager auf 0
		m_Empire[i].ClearAllPoints();
		
		if (i == 6)
			i = 0;
		number++;
	}

	// Hat die Rasse die Mitgliedschaft gekündigt, wurde Bestochen oder irgendein anderer Grund ist dafür
	// verantwortlich, warum eine Major plötzlich nicht mehr Zugriff auf das System der Minor hat.
	for (int j = 0; j < m_MinorRaceArray.GetSize(); j++)
	{
		BYTE owner = m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].GetOwnerOfSystem();
		if (owner != NOBODY && owner != UNKNOWN)
		{
			// Wenn wir eine Mitgliedschaft bei der kleinen Rasse hatten, sprich uns das System noch gehört, wir aber
			// der kleinen Rasse den Krieg erklären bzw. den Vertrag aufheben (warum auch immer?!?) und das System nicht
			// gewaltätig erobert wurde, dann gehört uns das System auch nicht mehr
			if (m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(owner) != MEMBERSHIP && 
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].GetMinorRace() == TRUE &&
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].GetTakenSector() == FALSE)
			{
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwned(FALSE);
				m_System[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwnerOfSystem(NOBODY);
				m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetOwnerOfSector(UNKNOWN);
				m_Empire[owner].SetNumberOfSystems(-1);
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
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			// Mögliche Is? Variablen für Terraforming und Stationbau erstmal auf FALSE setzen
			m_Sector[x][y].ClearAllPoints();
			for (int i = NOBODY; i <= DOMINION; i++)
			{
				// Alle Is? Variablen der Rassen, das dort ein Schiff im Sektor ist auf FALSE setzen
				m_Sector[x][y].SetOwnerOfShip(FALSE,i);
				// Die benötigte Scanpower um Schiffe sehen zu können wieder auf NULL setzen
				m_Sector[x][y].SetNeededScanPower(MAXSHORT, i);
				// Sagen das erstmal kein Außenposten und keine Sternbasis in dem Sektor steht
				m_Sector[x][y].SetOutpost(FALSE,i);
				m_Sector[x][y].SetStarbase(FALSE,i);
				m_Sector[x][y].SetShipPort(FALSE,i);
				m_Sector[x][y].SetScanPower(0,i);
			}
			if (m_Sector[x][y].GetSunSystem() == TRUE)	// Wenn wir ein Sonnensystem gefunden haben
			{
				// Jetzt das produzierte Latinum im System dem jeweiligen Imperium geben
				if (m_System[x][y].GetOwnerOfSystem() != NOBODY && m_System[x][y].GetOwnerOfSystem() != UNKNOWN)
				{
					if (m_Empire[m_System[x][y].GetOwnerOfSystem()].GetPlayerOfEmpire() == COMPUTER)
						m_Empire[m_System[x][y].GetOwnerOfSystem()].SetLatinum((int)(m_System[x][y].GetProduction()->GetLatinumProd() / 0.5f));
					else
						m_Empire[m_System[x][y].GetOwnerOfSystem()].SetLatinum(m_System[x][y].GetProduction()->GetLatinumProd());
				}
				// Hier die Gebäude abreißen, die angeklickt wurden
				m_System[x][y].DestroyBuildings();
				
				// Variablen berechnen lassen, bevor der Planet wächst -> diese ins Lager
				// nur berechnen, wenn das System auch jemandem gehört, ansonsten würde auch die Mind.Prod. ins Lager kommen
				if (m_System[x][y].GetOwnerOfSystem() != NOBODY)
				{
					// In CalculateStorages wird auch die Systemmoral berechnet. Wenn wir einen Auftrag mit
					// NeverReady (z.B. Kriegsrecht) in der Bauliste haben, und dieser Moral produziert, dann diese
					// auf die Moral anrechnen. Das wird dann in CalculateStorages gemacht.
					int list = m_System[x][y].GetAssemblyList()->GetAssemblyListEntry(0);
					if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() && m_System[x][y].GetMoral() <= 85)
						m_System[x][y].GetProduction()->AddMoralProd(BuildingInfo[list-1].GetMoralProd());
					
					/// ALPHA3 Modifikation
					int diliAdd = 0;
					if (m_Empire[m_System[x][y].GetOwnerOfSystem()].GetPlayerOfEmpire() == COMPUTER && m_System[x][y].GetProduction()->GetDilithiumProd() == 0)
					{
						// umso höher der Schwierigkeitsgrad, desto höher die Wahrscheinlichkeit, das die KI
						// Dilithium auf ihrem Systemen geschenkt bekommt
						int temp = rand()%((int)(m_fDifficultyLevel * 7.5));
					//	TRACE("KI: System: %s - DiliAddProb: %d (NULL for adding Dili) - Difficulty: %.2lf\n",m_Sector[x][y].GetName(), temp, m_fDifficultyLevel);
						if (temp == NULL)
							diliAdd = 1;
					}
					USHORT rebellion = m_System[x][y].CalculateStorages(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(), diliAdd);	// Das Lager berechnen
					// Wenn wir ne 1 zurückbekommen, dann hat sich das System losgesagt, bei 0 alles i.O.
					if (rebellion == 1)	
					{
						m_Sector[x][y].SetOwned(FALSE);
						m_Sector[x][y].SetShipPort(FALSE, m_Sector[x][y].GetOwnerOfSector());
						CPoint p; p.x = x; p.y = y;
						CString news = CResourceManager::GetString("REBELLION_IN_SYSTEM",FALSE,m_Sector[x][y].GetName());
						message.GenerateMessage(news,m_Sector[x][y].GetOwnerOfSector(),SOMETHING,"",p,FALSE);
						m_Empire[m_Sector[x][y].GetOwnerOfSector()].AddMessage(message);
						// zusätzliche Eventnachricht (Lose a System to Rebellion #18) wegen der Moral an das Imperium
						message.GenerateMessage(m_MajorRace[m_Sector[x][y].GetOwnerOfSector()].GetMoralObserver()->AddEvent(18,
							m_Sector[x][y].GetOwnerOfSector(), m_Sector[x][y].GetName()), m_Sector[x][y].GetOwnerOfSector(),
							SOMETHING, "", p, FALSE);
						m_Empire[m_Sector[x][y].GetOwnerOfSector()].AddMessage(message);
						if (server.IsPlayedByClient((RACE)m_Sector[x][y].GetOwnerOfSector()))
							m_iSelectedView[m_Sector[x][y].GetOwnerOfSector()] = EMPIRE_VIEW;							
												
						if (m_Sector[x][y].GetMinorRace() == TRUE && m_Sector[x][y].GetTakenSector() == FALSE)
						{
							this->GetMinorRace(m_Sector[x][y].GetName())->SetDiplomacyStatus(m_Sector[x][y].GetOwnerOfSector(), NO_AGREEMENT);
							this->GetMinorRace(m_Sector[x][y].GetName())->SetRelationshipToMajorRace(m_Sector[x][y].GetOwnerOfSector(),(-(rand()%50+20)));
							news = CResourceManager::GetString("MINOR_CANCELS_MEMBERSHIP",FALSE,this->GetMinorRace(m_Sector[x][y].GetName())->GetRaceName());
							message.GenerateMessage(news,m_Sector[x][y].GetOwnerOfSector(),DIPLOMACY,"",p,FALSE);
							m_Empire[m_Sector[x][y].GetOwnerOfSector()].AddMessage(message);
							m_Sector[x][y].SetOwnerOfSector(UNKNOWN);
						}
						else
							m_Sector[x][y].SetOwnerOfSector(NOBODY);
						m_System[x][y].SetOwnerOfSystem(NOBODY);
						m_Sector[x][y].SetTakenSector(FALSE);
					}
				}
				// Hier mit einbeziehen, wenn die Bevölkerung an Nahrungsmangel stirbt
				if (m_System[x][y].GetOwnerOfSystem() != NOBODY)
				{
					if (m_System[x][y].GetFoodStore() < 0)
					{
						m_Sector[x][y].LetPlanetsShrink((float)(m_System[x][y].GetFoodStore())*0.01f);
						m_System[x][y].SetMoral((short)(m_System[x][y].GetFoodStore()/(m_System[x][y].GetHabitants()+1))); // +1, wegen Division durch NULL umgehen
						m_System[x][y].SetFoodStore(0);
						CPoint p; p.x = x; p.y = y;
						CString news = CResourceManager::GetString("FAMINE",FALSE,m_Sector[x][y].GetName());
						message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE,1);
						m_Empire[m_System[x][y].GetOwnerOfSystem()].AddMessage(message);
						if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
							m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
					}
					else
						m_Sector[x][y].LetPlanetsGrowth();	// Planetenwachstum für Spielerrassen durchführen
				}
				else
					m_Sector[x][y].LetPlanetsGrowth();		// Planetenwachstum für andere Sektoren durchführen
				float currentHabitants = m_Sector[x][y].GetCurrentHabitants();
				m_Empire[m_System[x][y].GetOwnerOfSystem()].AddPopSupportCosts((USHORT)currentHabitants*10);
				// Funktion gibt TRUE zurück, wenn wir durch die Bevölkerung eine neue Handelsroute anlegen können
				if (m_System[x][y].SetHabitants(currentHabitants))
				{
					CString news = CResourceManager::GetString("ENOUGH_HABITANTS_FOR_NEW_TRADEROUTE",FALSE,m_Sector[x][y].GetName());
					message.GenerateMessage(news, m_System[x][y].GetOwnerOfSystem(), ECONOMY, "", CPoint(x,y), FALSE, 4);
					m_Empire[m_System[x][y].GetOwnerOfSystem()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
						m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
				}
				if (m_System[x][y].GetOwnerOfSystem() != NOBODY)
				{
					m_System[x][y].CalculateVariables(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),
						m_Sector[x][y].GetPlanets(), CTrade::GetMonopolOwner());
					// Gebäude die Energie benötigen checken
					if (m_System[x][y].CheckEnergyBuildings() == 1)
					{
						CPoint p; p.x = x; p.y = y;
						CString news = CResourceManager::GetString("BUILDING_TURN_OFF",FALSE,m_Sector[x][y].GetName());
						message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE,2);
						m_Empire[m_System[x][y].GetOwnerOfSystem()].AddMessage(message);
						if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
							m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
					}
				}								
				// Diese for-Schleife berechnet die Bauaufträge in dem System. Außerdem wird hier auch die System-KI
				// ausgeführt.
				for (int i = HUMAN; i <= DOMINION; i++)
					if (m_System[x][y].GetOwnerOfSystem() == i)
					{
						if (m_Empire[m_System[x][y].GetOwnerOfSystem()].GetPlayerOfEmpire() == COMPUTER || m_System[x][y].GetAutoBuild())
						{
							CSystemAI* SAI = new CSystemAI(this);
							SAI->ExecuteSystemAI(CPoint(x,y));
							delete SAI;
						}
						// Hier berechnen, wenn was in der Bauliste ist, und obs fertig wird usw.
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
									m_Empire[i].SetLatinum(goods);
									m_System[x][y].GetAssemblyList()->SetWasBuildingBought(FALSE);
								}
								// Ab jetzt die Abfrage ob Gebäude oder ein Update fertig wurde
								if (list > 0 && list < 10000 && !BuildingInfo[list-1].GetNeverReady())	// Es wird ein Gebäude gebaut
								{
									// Die Nachricht, dass neues Gebäude fertig ist mit allen Daten generieren
									CPoint p(x,y);
									message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(),i,ECONOMY,m_Sector[x][y].GetName(),p,FALSE);
									m_Empire[i].AddMessage(message);
									BuildBuilding(list,CPoint(x,y));	// Gebäude bauen
									// und Gebäude (welches letztes im Feld) ist auch gleich online setzen, wenn 
									// genügend Arbeiter da sind
									unsigned short CheckValue = m_System[x][y].SetNewBuildingOnline();
									// Nachricht generierenm das das Gebäude nicht online genommen werden konnte
									if (CheckValue == 1)
									{
										CString news = CResourceManager::GetString("NOT_ENOUGH_WORKER",FALSE,m_Sector[x][y].GetName());
										message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE,1);
										m_Empire[i].AddMessage(message);
										if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
											m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
									}
									if (CheckValue == 2)
									{
										CString news = CResourceManager::GetString("NOT_ENOUGH_ENERGY",FALSE,m_Sector[x][y].GetName());
										message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE,2);
										m_Empire[i].AddMessage(message);
										if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
											m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
									}
								}
								else if (list < 0)	// Es wird ein Update gemacht
								{
									CPoint p(x,y);
									list *= (-1);
									// Die Nachricht, dass neues Gebäudeupdate fertig wurde, mit allen Daten generieren
									message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(),i,ECONOMY,m_Sector[x][y].GetName(),p,TRUE);
									m_Empire[i].AddMessage(message);
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
										if (GetBuildingInfo(list).GetNeededEnergy() > NULL && m_System[x][y].SetNewBuildingOnline() == 2)
										{
											CString news = CResourceManager::GetString("NOT_ENOUGH_ENERGY",FALSE,m_Sector[x][y].GetName());
											message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE,2);
											m_Empire[i].AddMessage(message);
											if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
												m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
										}
									}									
								}
								else if (list >= 10000 && list < 20000)	// Es wird ein Schiff gebaut
								{
									CPoint p; p.x = x; p.y = y;
									BuildShip(list-10000, p, i);
									CString s = CResourceManager::GetString("SHIP_BUILDING_FINISHED",FALSE,
										m_ShipInfoArray[list-10000].GetShipTypeAsString(),m_Sector[p.x][p.y].GetName());
									message.GenerateMessage(s,i,MILITARY,m_Sector[p.x][p.y].GetName(),p,FALSE);
									m_Empire[i].AddMessage(message);
								}
								else if (list >= 20000)					// Es wird eine Truppe gebaut
								{
									CPoint p(x,y);
									BuildTroop(list-20000,p);
									CString s = CResourceManager::GetString("TROOP_BUILDING_FINISHED",FALSE,
										m_TroopInfo[list-20000].GetName(),m_Sector[p.x][p.y].GetName());
									message.GenerateMessage(s,i,MILITARY,m_Sector[p.x][p.y].GetName(),p,FALSE);
									m_Empire[i].AddMessage(message);									
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
									CPoint p; p.x = x; p.y = y;
									CString news = CResourceManager::GetString("EMPTY_BUILDLIST",FALSE,m_Sector[x][y].GetName());
									message.GenerateMessage(news,m_System[x][y].GetOwnerOfSystem(),SOMETHING,"",p,FALSE);
									m_Empire[i].AddMessage(message);
									if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
										m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
								}
							}
							m_System[x][y].GetAssemblyList()->CalculateNeededRessourcesForUpdate(&BuildingInfo, m_System[x][y].GetAllBuildings(), m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo());
						}
						// Anzahl aller Farmen, Bauhöfe usw. im System berechnen
						m_System[x][y].CalculateNumberOfWorkbuildings();
						// freie Arbeiter den Gebäuden zuweisen
						m_System[x][y].SetWorkersIntoBuildings();
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
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != NOBODY &&	m_System[x][y].GetOwnerOfSystem() != UNKNOWN)
			{
				// imperiumsweite Moralproduktion aus diesem System berechnen
				m_System[x][y].CalculateEmpireWideMoralProd();
				// Blockadewert des Systems zurücksetzen
				m_System[x][y].SetBlockade(NULL);
			}
	// Hier werden jetzt die baubaren Gebäude für die nächste Runde und auch die Produktion in den einzelnen
	// Systemen berechnet. Können das nicht in obiger Hauptschleife machen, weil dort es alle globalen Gebäude
	// gesammelt werden müssen und ich deswegen alle Systeme mit den fertigen Bauaufträgen in dieser Runde einmal
	// durchgegangen sein muß.
	short researchBonis[7][6] = {0};
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{		
			if (m_Sector[x][y].GetSunSystem() == TRUE && m_System[x][y].GetOwnerOfSystem() != NOBODY &&
				m_System[x][y].GetOwnerOfSystem() != UNKNOWN)	// Wenn wir ein Sonnensystem gefunden haben
			{
				// Hier die Credits durch Handelsrouten berechnen
				BYTE deletetTradeRoutes = 0;
				for (int i = 0; i < m_System[x][y].GetTradeRoutes()->GetSize(); i++)
				{
					CPoint dest = m_System[x][y].GetTradeRoutes()->GetAt(i).GetDestKO();
					// Wenn die Handelsroute aus diplomatischen Gründen nicht mehr vorhanden sein kann
					if (m_System[x][y].GetTradeRoutes()->ElementAt(i).CheckTradeRoute(m_System[x][y].GetOwnerOfSystem(), 
						&m_Sector[dest.x][dest.y], m_MajorRace, &m_MinorRaceArray) == FALSE)
					{
						// dann müssen wir diese Route löschen
						m_System[x][y].GetTradeRoutes()->RemoveAt(i--);
						deletetTradeRoutes++;
					}
					// Ansonsten könnte sich die Beziehung zu der Minorrace verbessern
					else
						m_System[x][y].GetTradeRoutes()->ElementAt(i).PerhapsChangeRelationship(
							m_System[x][y].GetOwnerOfSystem(),&m_Sector[dest.x][dest.y], &m_MinorRaceArray);

				}
				deletetTradeRoutes += m_System[x][y].CheckTradeRoutes(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo());
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
					message.GenerateMessage(news, m_System[x][y].GetOwnerOfSystem(), ECONOMY, "", CPoint(x,y), FALSE, 4);
					m_Empire[m_System[x][y].GetOwnerOfSystem()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
						m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
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
				deletedResourceRoutes += m_System[x][y].CheckResourceRoutes(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo());
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
					message.GenerateMessage(news, m_System[x][y].GetOwnerOfSystem(), ECONOMY, "", CPoint(x,y), FALSE, 4);
					m_Empire[m_System[x][y].GetOwnerOfSystem()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_System[x][y].GetOwnerOfSystem()))
						m_iSelectedView[m_System[x][y].GetOwnerOfSystem()] = EMPIRE_VIEW;
				}
				
				// baubare Gebäude, Schiffe und Truppen berechnen
				m_System[x][y].CalculateVariables(m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),	m_Sector[x][y].GetPlanets(), CTrade::GetMonopolOwner());
				m_System[x][y].CalculateBuildableBuildings(&m_Sector[x][y],&BuildingInfo,&m_Empire[m_System[x][y].GetOwnerOfSystem()],&m_GlobalBuildings,m_MajorRace);
				m_System[x][y].CalculateBuildableShips(&m_ShipInfoArray,m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch(),m_Sector[x][y].GetName(),m_Sector[x][y].GetMinorRace());
				m_System[x][y].CalculateBuildableTroops(&m_TroopInfo,m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch());
			
				// Haben wir eine online Schiffswerft im System, dann ShipPort in dem Sektor setzen
				if (m_System[x][y].GetProduction()->GetShipYard() == TRUE)
				{
					m_Sector[x][y].SetShipPort(TRUE,m_System[x][y].GetOwnerOfSystem());
					starmap[m_System[x][y].GetOwnerOfSystem()]->AddBase(Sector((char)x,(char)y),
						m_Empire[m_System[x][y].GetOwnerOfSystem()].GetResearch()->GetPropulsionTech());
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
				researchBonis[m_System[x][y].GetOwnerOfSystem()][0] += m_System[x][y].GetProduction()->GetBioTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][1] += m_System[x][y].GetProduction()->GetEnergyTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][2] += m_System[x][y].GetProduction()->GetCompTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][3] += m_System[x][y].GetProduction()->GetPropulsionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][4] += m_System[x][y].GetProduction()->GetConstructionTechBoni();
				researchBonis[m_System[x][y].GetOwnerOfSystem()][5] += m_System[x][y].GetProduction()->GetWeaponTechBoni();
								
				// Hier die gesamten Sicherheitsboni der Imperien berechnen
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddInnerSecurityBonus(m_System[x][y].GetProduction()->GetInnerSecurityBoni());
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddEconomyBonus(m_System[x][y].GetProduction()->GetEconomySpyBoni(), 0);
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddEconomyBonus(m_System[x][y].GetProduction()->GetEconomySabotageBoni(), 1);
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddScienceBonus(m_System[x][y].GetProduction()->GetScienceSpyBoni(), 0);
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddScienceBonus(m_System[x][y].GetProduction()->GetScienceSabotageBoni(), 1);
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddMilitaryBonus(m_System[x][y].GetProduction()->GetMilitarySpyBoni(), 0);
				m_Empire[m_System[x][y].GetOwnerOfSystem()].GetIntelligence()->AddMilitaryBonus(m_System[x][y].GetProduction()->GetMilitarySabotageBoni(), 1);

				// Anzahl aller Ressourcen in allen eigenen Systemen berechnen
				m_Empire[m_System[x][y].GetOwnerOfSystem()].SetStorageAdd(TITAN,m_System[x][y].GetTitanStore());
				m_Empire[m_System[x][y].GetOwnerOfSystem()].SetStorageAdd(DEUTERIUM,m_System[x][y].GetDeuteriumStore());
				m_Empire[m_System[x][y].GetOwnerOfSystem()].SetStorageAdd(DURANIUM,m_System[x][y].GetDuraniumStore());
				m_Empire[m_System[x][y].GetOwnerOfSystem()].SetStorageAdd(CRYSTAL,m_System[x][y].GetCrystalStore());
				m_Empire[m_System[x][y].GetOwnerOfSystem()].SetStorageAdd(IRIDIUM,m_System[x][y].GetIridiumStore());
			}
			// Hier wird berechnet, was wir von der Karte alles sehen, welche Sektoren wir durchfliegen können
			// alles abhängig von unseren diplomatischen Beziehungen
			BOOLEAN shipPort[DOMINION] = {FALSE};
			for (int i = HUMAN; i <= DOMINION; i++)
				for (int j = HUMAN; j <= DOMINION; j++)
				{
					if (m_Sector[x][y].GetScanned(i) == TRUE && i != j)
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= COOPERATION)
							m_Sector[x][y].SetScanned(j);
					if (m_Sector[x][y].GetKnown(i) == TRUE && i != j)
					{
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= FRIENDSHIP_AGREEMENT)
							m_Sector[x][y].SetScanned(j);
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= COOPERATION)
							m_Sector[x][y].SetKnown(j);
					}
					if (m_Sector[x][y].GetOwnerOfSector() == i && i != j)
					{
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= TRADE_AGREEMENT)
							m_Sector[x][y].SetScanned(j);
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= FRIENDSHIP_AGREEMENT)
							m_Sector[x][y].SetKnown(j);
					}
					if (m_Sector[x][y].GetShipPort(i) == TRUE && i != j)
						if (m_MajorRace[i].GetDiplomacyStatus(j) >= COOPERATION)
							shipPort[j-1] = TRUE;
				}
			for (int i = HUMAN; i <= DOMINION; i++)
				if (shipPort[i-1])
				{
					m_Sector[x][y].SetShipPort(TRUE, i);
					starmap[i]->AddBase(Sector(x,y), m_Empire[i].GetResearch()->GetPropulsionTech());
				}
		}		
		// Forschungsboni aus Spezialforschungen setzen, nachdem wir diese aus allen Systemen geholt haben
		for (int i = HUMAN; i <= DOMINION; i++)
		{
			// Die Boni auf die einzelnen Forschungsgebiete durch Spezialforschungen addieren
			if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetFieldStatus(1) == RESEARCHED)
			{
				researchBonis[i][0] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(1);
				researchBonis[i][1] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(1);
			}
			else if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetFieldStatus(2) == RESEARCHED)
			{
				researchBonis[i][2] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(2);
				researchBonis[i][3] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(2);
			}
			else if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetFieldStatus(3) == RESEARCHED)
			{
				researchBonis[i][4] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(3);
				researchBonis[i][5] += m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(8)->GetBonus(3);
			}
			m_Empire[i].GetResearch()->SetResearchBoni(researchBonis[i]);
		}
		// Geheimdienstboni aus Spezialforschungen holen
		for (int i = HUMAN; i <= DOMINION; i++)
		{
			// Die Boni auf die einzelnen Geheimdienstgebiete berechnen
			if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetFieldStatus(1) == RESEARCHED)
				m_Empire[i].GetIntelligence()->AddInnerSecurityBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(1));
			else if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetFieldStatus(2) == RESEARCHED)
			{
				m_Empire[i].GetIntelligence()->AddEconomyBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(2), 1);
				m_Empire[i].GetIntelligence()->AddMilitaryBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(2), 1);
				m_Empire[i].GetIntelligence()->AddScienceBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(2), 1);
			}
			else if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetFieldStatus(3) == RESEARCHED)
			{
				m_Empire[i].GetIntelligence()->AddEconomyBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(3), 0);
				m_Empire[i].GetIntelligence()->AddMilitaryBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(3), 0);
				m_Empire[i].GetIntelligence()->AddScienceBonus(m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(9)->GetBonus(3), 0);
			}
		}		

		// Nun überprüfen, ob sich unsere Grenzen erweitern, wenn die MinorRasse eine Spaceflight-Rasse ist und wir mit
		// ihr eine Kooperations oder ein Bündnis haben
		for (int i = HUMAN; i <= DOMINION; i++)
			for (int j = 0; j < m_MinorRaceArray.GetSize(); j++)
				if (m_MinorRaceArray.GetAt(j).GetSpaceflightNation() == TRUE && (m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(i) == COOPERATION ||
					m_MinorRaceArray.GetAt(j).GetDiplomacyStatus(i) == AFFILIATION))
				{
					m_Sector[m_MinorRaceArray.GetAt(j).GetRaceKO().x][m_MinorRaceArray.GetAt(j).GetRaceKO().y].SetShipPort(TRUE, i);
					starmap[i]->AddBase(Sector((char)m_MinorRaceArray.GetAt(j).GetRaceKO().x,(char)m_MinorRaceArray.GetAt(j).GetRaceKO().y),
						m_Empire[i].GetResearch()->GetPropulsionTech());
				}
}

/// Diese Funktion berechnet die kompletten Handelsaktivitäten.
void CBotf2Doc::CalcTrade()
{
	using namespace network;
	// Hier berechnen wir alle Handelsaktionen
	USHORT oldPrices[7][5];				// Matrix der jetzigen Preise
	USHORT taxMoney[] = {0,0,0,0,0};	// alle Steuern auf eine Ressource
	double monopolBuy[7][5];			// hier drin steht wer wieviel für eine Monopol ausgeben möchte
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
		// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
		if (m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetFieldStatus(1) == RESEARCHED)
		{
			float newTax = (float)m_Empire[i].GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(1);
			newTax = 1.0f + newTax / 100;
			m_Trade[i].SetTax(newTax);
		}
		m_Trade[i].CalculateTradeActions(m_Empire, m_System, m_Sector, m_MajorRace, taxMoney);
		for (int j = TITAN; j <= IRIDIUM; j++)
		{
			// plus Steuern, die durch Sofortkäufe von Bauaufträgen entstanden sind holen
			if (CTrade::GetMonopolOwner(j) != NOBODY)
				if (i == CTrade::GetMonopolOwner(j) || m_MajorRace[i].GetKnownMajorRace(CTrade::GetMonopolOwner(j)) == TRUE)
					taxMoney[j] += m_Trade[i].GetTaxesFromBuying()[j];
			// jetzigen Preise speichern
			oldPrices[i][j] = m_Trade[i].GetRessourcePrice()[j];
			// schauen ob jemand ein Monopol kaufen möchte
			monopolBuy[i][j] = m_Trade[i].GetMonopolBuying()[j];
		}
	}
	// die Steuern durch den Handel den Monopolbesitzern gutschreiben und nach Monopolkäufen Ausschau halten
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		CString resName;
		switch(i)
		{
		case TITAN: resName = CResourceManager::GetString("TITAN"); break;
		case DEUTERIUM: resName = CResourceManager::GetString("DEUTERIUM"); break;
		case DURANIUM: resName = CResourceManager::GetString("DURANIUM"); break;
		case CRYSTAL: resName = CResourceManager::GetString("CRYSTAL"); break;
		case IRIDIUM: resName = CResourceManager::GetString("IRIDIUM"); break;
		}

		if (CTrade::GetMonopolOwner(i) != NOBODY)
		{/*
		 CString hh;
		 hh.Format("Steuern auf %d: %d Latinum",i,taxMoney[i]);
		 AfxMessageBox(hh);*/
			m_Empire[CTrade::GetMonopolOwner(i)].SetLatinum(taxMoney[i]);
		}
		// Hier die gekauften Monopole den Rassen zuschreiben. Wer am meisten bezahlt hat (falls mehrere Rassen
		// in der selben Runde ein Monopol kaufen möchten) bekommt das Monopol. Die anderen bekommen ihr Latinum zurück
		double max = 0.0f;				// meiste Latinum was für ein Monopol gegeben wurde
		USHORT monopolRace = NOBODY;	// Rasse die das Monopol erlangt hat
		for (int j = HUMAN; j <= DOMINION; j++)
			if (monopolBuy[j][i] > max)
			{
				max = monopolBuy[j][i];
				monopolRace = j;
				CTrade::SetMonopolOwner(i,j);
			}
		for (int j = HUMAN; j <= DOMINION; j++)
		{
			// Die anderen Rassen bekommen ihr Geld zurück
			if (j != monopolRace && monopolBuy[j][i] != 0)
			{
				m_Empire[j].SetLatinum((long)(monopolBuy[j][i]));
				// Nachricht generieren, dass wir es nicht geschafft haben ein Monopol zu kaufen
				CString news = CResourceManager::GetString("WE_DONT_GET_MONOPOLY",FALSE,resName);
				message.GenerateMessage(news,j,SOMETHING,"",0,FALSE);
				m_Empire[j].AddMessage(message);
				if (server.IsPlayedByClient((RACE)j))
					m_iSelectedView[j] = EMPIRE_VIEW;
				m_Trade[j].SetMonopolBuying(i,0.0f);
			}
			// Nachricht an unser Imperium, dass wir ein Monopol erlangt haben
			if (j == monopolRace)
			{
				CString news = CResourceManager::GetString("WE_GET_MONOPOLY",FALSE,resName);
				message.GenerateMessage(news,j,SOMETHING,"",0,FALSE);
				m_Empire[j].AddMessage(message);
				if (server.IsPlayedByClient((RACE)j))
					m_iSelectedView[j] = EMPIRE_VIEW;
				m_Trade[j].SetMonopolBuying(i,0.0f);
			}
			// Nachrichten an die einzelnen Imperien verschicken, das eine Rasse das Monopol erlangt hat
			else if (monopolRace != NOBODY && m_MajorRace[j].GetKnownMajorRace(monopolRace) == TRUE)
			{
				CString race;
				switch (monopolRace)
				{
					case 1:	{race = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE", TRUE); break;}
					case 2:	{race = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE", TRUE); break;}
					case 3:	{race = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE", TRUE); break;}
					case 4:	{race = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE", TRUE); break;}
					case 5:	{race = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE", TRUE); break;}
					case 6:	{race = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE", TRUE); break;}
				}
				CString news = CResourceManager::GetString("SOMEBODY_GET_MONOPOLY",FALSE,race,resName);
				message.GenerateMessage(news,j,SOMETHING,"",0,FALSE);
				m_Empire[j].AddMessage(message);
				if (server.IsPlayedByClient((RACE)j))
					m_iSelectedView[j] = EMPIRE_VIEW;
			}
		}
	}
	// Hier den neuen Kurs der Waren an den Handelsbörsen berechnen, dürfen wir erst machen, wenn wir für alle Tradeobjekte
	// die einzelnen Kurse berechnet haben
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_Trade[i].CalculatePrices(oldPrices, m_MajorRace);
		// Hier die aktuellen Kursdaten in die History schreiben
		USHORT* resPrices = m_Trade[i].GetRessourcePrice();
		m_TradeHistory[i].SaveCurrentPrices(resPrices,m_Trade[i].GetTax());
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
			// Wenn eine Minorrace in dem System lebt und dieser nicht schon erobert wurde
			else if (m_Sector[p.x][p.y].GetOwnerOfSector() == UNKNOWN)
			{
				if (this->GetMinorRace(m_Sector[p.x][p.y].GetName())->GetDiplomacyStatus(m_ShipArray.GetAt(y).GetOwnerOfShip()) != WAR)
					m_ShipArray.ElementAt(y).SetCurrentOrder(ATTACK);
			}
			// Wenn eine Majorrace in dem System lebt
			else if (m_Sector[p.x][p.y].GetOwnerOfSector() != NOBODY)
			{
				if (m_Sector[p.x][p.y].GetOwnerOfSector() != m_ShipArray.GetAt(y).GetOwnerOfShip())
					if (m_MajorRace[m_Sector[p.x][p.y].GetOwnerOfSector()].GetDiplomacyStatus(m_ShipArray.GetAt(y).GetOwnerOfShip()) != WAR)
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
			if ((m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == m_ShipArray[y].GetOwnerOfShip() || m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == NOBODY))
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
				// Gebäude bauen, wenn wir das System zum ersten Mal kolonisieren, sprich das System noch niemanden gehört
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() == NOBODY)
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
					message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
					
					// zusätzliche Eventnachricht (Colonize a system #12) wegen der Moral an das Imperium
					message.GenerateMessage(m_MajorRace[m_ShipArray[y].GetOwnerOfShip()].GetMoralObserver()->AddEvent(12,
						m_ShipArray[y].GetOwnerOfShip(), m_Sector[ShipKO.x][ShipKO.y].GetName()),
						m_ShipArray[y].GetOwnerOfShip(), SOMETHING, "", ShipKO, FALSE); 
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_CLAIMSYSTEM, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
						m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
						m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;

						CEventColonization* eventScreen = new CEventColonization(m_ShipArray[y].GetOwnerOfShip(), CResourceManager::GetString("COLOEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("COLOEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));
						m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetEventMessages()->Add(eventScreen);
					}
				}
				else
				{
					// Nachricht an das Imperium senden, das ein Planet kolonisiert wurde
					s = CResourceManager::GetString("NEW_PLANET_COLONIZED",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
					message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
						m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
				}
				m_System[ShipKO.x][ShipKO.y].SetHabitants(m_Sector[ShipKO.x][ShipKO.y].GetCurrentHabitants());
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableBuildings(&m_Sector[ShipKO.x][ShipKO.y],&BuildingInfo,&m_Empire[m_ShipArray[y].GetOwnerOfShip()],&m_GlobalBuildings,m_MajorRace);
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableShips(&m_ShipInfoArray,m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetResearch(),m_Sector[ShipKO.x][ShipKO.y].GetName(),m_Sector[ShipKO.x][ShipKO.y].GetMinorRace());
				m_System[ShipKO.x][ShipKO.y].CalculateBuildableTroops(&m_TroopInfo,m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetResearch());
				m_System[ShipKO.x][ShipKO.y].CalculateNumberOfWorkbuildings();
				m_System[ShipKO.x][ShipKO.y].CalculateVariables(m_Empire[m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(), m_Sector[ShipKO.x][ShipKO.y].GetPlanets(), CTrade::GetMonopolOwner());
				// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
				s.Format("%s %s",CResourceManager::GetString("COLONIZATION"), m_Sector[ShipKO.x][ShipKO.y].GetName());
				m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y],
					m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, s, CResourceManager::GetString("DESTROYED"));
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
			if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->GetTerraformed() == FALSE)
			{
				if (m_Sector[ShipKO.x][ShipKO.y].GetPlanet(m_ShipArray[y].GetTerraformingPlanet())->SetNeededTerraformPoints(m_ShipArray[y].GetColonizePoints()))
				{
					// Hier wurde ein Planet erfolgreich geterraformt
					m_ShipArray[y].SetCurrentOrder(AVOID);
					m_ShipArray[y].SetTerraformingPlanet(-1);
					// Nachricht generieren, dass Terraforming abgeschlossen wurde
					CString s = CResourceManager::GetString("TERRAFORMING_FINISHED",FALSE,m_Sector[ShipKO.x][ShipKO.y].GetName());
					message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
					{
						SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
						m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
						m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
					}
					// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
					if (m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == UNKNOWN && m_Sector[ShipKO.x][ShipKO.y].GetMinorRace() == TRUE)
						GetMinorRace(m_Sector[ShipKO.x][ShipKO.y].GetName())->SetRelationshipToMajorRace(m_ShipArray[y].GetOwnerOfShip(), +rand()%11);
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
							message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),SOMETHING,m_Sector[ShipKO.x][ShipKO.y].GetName(),ShipKO,FALSE);
							m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
							if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_TERRAFORM_COMPLETE, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
								m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
								m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
							}
							// Wenn wir einer Rasse beim Terraformen helfen, so gibt es einen Beziehungsboost
							if (m_Sector[ShipKO.x][ShipKO.y].GetOwnerOfSector() == UNKNOWN && m_Sector[ShipKO.x][ShipKO.y].GetMinorRace() == TRUE)
								GetMinorRace(m_Sector[ShipKO.x][ShipKO.y].GetName())->SetRelationshipToMajorRace(m_ShipArray[y].GetOwnerOfShip(), +rand()%11);
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
			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// hier wird vereinfacht angenommen, das an teurerer Außenposten auch ein besserer ist
			USHORT costs = 0;
			short id = -1;
			// Wenn wir in dem Sektor noch keinen Außenposten und noch keine Sternbasis stehen haben
			if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == FALSE
				&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == m_ShipArray[y].GetOwnerOfShip()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == OUTPOST
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetResearch()))
					{
						costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
						id = m_ShipInfoArray.GetAt(l).GetID();
					}
			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				for (int l = HUMAN; l <= DOMINION; l++)
					if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(l) == TRUE || 
					   (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(l) == FALSE && m_Sector[ShipKO.x][ShipKO.y].GetStarbase(l) == TRUE))
					{
						buildable = FALSE;
						break;
					}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (m_Sector[ShipKO.x][ShipKO.y].GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == -1)
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
									message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),m_ShipArray[y].GetOwnerOfShip(),MILITARY,"",ShipKO,FALSE);
									m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x),
										m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
										CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
									if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
									{
										SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
										m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
										m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
									}
									// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
									for (int k = 0; k <= DOMINION; k++)
									{
										m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, k);
										m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(-1, k);
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
							message.GenerateMessage(CResourceManager::GetString("OUTPOST_FINISHED"),m_ShipArray[y].GetOwnerOfShip(),MILITARY,"",ShipKO,FALSE);
							m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y],
								m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("OUTPOST_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
							if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_OUTPOST_READY, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
								m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
								m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (int k = 0; k <= DOMINION; k++)
							{
								m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, k);
								m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(-1, k);
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
			// jetzt müssen wir die Schiffsinfos durchgehen und schauen, welche Station wir technologisch bauen könnten.
			// um eine Sternbasis bauen zu können muß schon ein Außenposten in dem Sektor stehen
			// hier wird vereinfacht angenommen, das eine teurere Sternbasis auch eine bessere ist
			// oder wir haben einen Außenposten und wollen diesen zur Sternbasis updaten
			USHORT costs = 0;
			short id = -1;
			if (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(m_ShipArray[y].GetOwnerOfShip()) == TRUE
				&& m_Sector[ShipKO.x][ShipKO.y].GetStarbase(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
				for (int l = 0; l < m_ShipInfoArray.GetSize(); l++)
					if (m_ShipInfoArray.GetAt(l).GetRace() == m_ShipArray[y].GetOwnerOfShip()
						&& m_ShipInfoArray.GetAt(l).GetShipType() == STARBASE
						&& m_ShipInfoArray.GetAt(l).GetBaseIndustry() > costs
						&& m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetResearch()))
					{
						costs = m_ShipInfoArray.GetAt(l).GetBaseIndustry();
						id = m_ShipInfoArray.GetAt(l).GetID();
					}

			// Wenn wir eine baubare Station gefunden haben und in dem Sektor nicht gerade eine andere (durch andere Rasse)
			// Station fertig wurde, können wir diese dort auch errichten
			if (id != -1)
			{
				BOOL buildable = TRUE;
				for (int l = HUMAN; l <= DOMINION; l++)
					if (m_Sector[ShipKO.x][ShipKO.y].GetStarbase(l) == TRUE
						|| (m_Sector[ShipKO.x][ShipKO.y].GetOutpost(l) == TRUE && l != m_ShipArray[y].GetOwnerOfShip()))
					{
						buildable = FALSE;
						break;
					}
				if (buildable == TRUE)
				{
					// Wenn wir also an einer Station gerade bauen -> Variable auf TRUE setzen
					m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(TRUE, m_ShipArray[y].GetOwnerOfShip());
					// Wenn wir gerade anfangen an einer Station zu bauen, also noch keine BuildPoints zusammenhaben
					if (m_Sector[ShipKO.x][ShipKO.y].GetStartStationPoints(m_ShipArray[y].GetOwnerOfShip()) == -1)
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
									message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),m_ShipArray[y].GetOwnerOfShip(),MILITARY,"",ShipKO,FALSE);
									m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
									// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
									m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x),
										m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
										CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
									if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
									{
										SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
										m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
										m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
									}
									// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
									for (int k = 0; k <= DOMINION; k++)
									{
										m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, k);
										m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(-1, k);
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
											m_ShipHistory[m_ShipArray[k].GetOwnerOfShip()].RemoveShip(&m_ShipArray[k]);
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
							message.GenerateMessage(CResourceManager::GetString("STARBASE_FINISHED"),m_ShipArray[y].GetOwnerOfShip(),MILITARY,"",ShipKO,FALSE);
							m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y],
								m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("STARBASE_CONSTRUCTION"), CResourceManager::GetString("DESTROYED"));
							if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
							{
								SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_STARBASE_READY, (RACE)m_ShipArray[y].GetOwnerOfShip(), 0, 1.0f};
								m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
								m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
							}
							// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
							for (int k = 0; k <= DOMINION; k++)
							{
								m_Sector[ShipKO.x][ShipKO.y].SetIsStationBuilding(FALSE, k);
								m_Sector[ShipKO.x][ShipKO.y].SetStartStationPoints(-1, k);
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
										m_ShipHistory[m_ShipArray[k].GetOwnerOfShip()].RemoveShip(&m_ShipArray[k]);
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
										m_ShipHistory[m_ShipArray[k].GetOwnerOfShip()].RemoveShip(&m_ShipArray[k]);
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
				m_Empire[m_ShipArray[y].GetOwnerOfShip()].SetLatinum((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
			}
			// Wenn es ein Außenposten oder eine Sternbasis ist, dann dem Sektor bekanntgeben, dass in ihm keine Station mehr ist
			if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
			{
				m_Sector[ShipKO.x][ShipKO.y].SetOutpost(FALSE, m_ShipArray[y].GetOwnerOfShip());
				m_Sector[ShipKO.x][ShipKO.y].SetStarbase(FALSE, m_ShipArray[y].GetOwnerOfShip());
			}
			// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
			m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y],
				m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound, CResourceManager::GetString("DISASSEMBLY"),
				CResourceManager::GetString("DESTROYED"));
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
						m_Empire[m_ShipArray[y].GetOwnerOfShip()].SetLatinum((int)(m_ShipInfoArray.GetAt(id).GetNeededIndustry() * proz / 100));
						// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
						m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(x),
							m_Sector[ShipKO.x][ShipKO.y].GetName(TRUE), m_iRound,
							CResourceManager::GetString("DISASSEMBLY"), CResourceManager::GetString("DESTROYED"));
					}
			m_ShipArray.RemoveAt(y--);
			continue;	// continue, damit wir am Ende der Schleife nicht sagen, dass ein Schiff im Sektor ist
		}
		
		// Wenn wir ein Schiff zum Flagschiff ernennen wollen (nur ein Schiff pro Imperium kann ein Flagschiff sein!)
		else if (m_ShipArray[y].GetCurrentOrder() == ASSIGN_FLAGSHIP && m_ShipArray[y].GetFleet() == 0)
		{
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
			message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),MILITARY,"",ShipKO,FALSE);
			m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
			if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
				m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
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
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() >= HUMAN && m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() <= DOMINION)
				{
					BYTE systemOwner = m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem();
					BYTE shipOwner   = m_ShipArray[y].GetOwnerOfShip();
					// haben wir einen Vertrag kleiner einem Freundschaftsvertrag mit der Majorrace
					if (m_MajorRace[shipOwner].GetDiplomacyStatus(systemOwner) < FRIENDSHIP_AGREEMENT)
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
						m_MajorRace[systemOwner].SetRelationshipToMajorRace(shipOwner, -rand()%(blockadeValue/10 + 1));
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
				if (m_ShipArray[y].GetOwnerOfShip() >= HUMAN && m_ShipArray[y].GetOwnerOfShip() <= DOMINION)
				{					
					CEventBlockade* eventScreen = new CEventBlockade(m_ShipArray[y].GetOwnerOfShip(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetEventMessages()->Add(eventScreen);
				}
				if (m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() >= HUMAN && m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem() <= DOMINION)
				{
					CEventBlockade* eventScreen = new CEventBlockade(m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem(), CResourceManager::GetString("BLOCKADEEVENT_HEADLINE", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()), CResourceManager::GetString("BLOCKADEEVENT_TEXT_RACE1", FALSE, m_Sector[ShipKO.x][ShipKO.y].GetName()));
					m_Empire[m_System[ShipKO.x][ShipKO.y].GetOwnerOfSystem()].GetEventMessages()->Add(eventScreen);
				}
			}
		}

		// Vor der Schiffsbewegung aber nach einer möglichen Demontage dort überall einen ShipPort setzen wo
		// eine Sternbasis oder ein Außenposten steht
		if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
		{
			m_Sector[ShipKO.x][ShipKO.y].SetShipPort(TRUE, m_ShipArray[y].GetOwnerOfShip());
			starmap[m_ShipArray[y].GetOwnerOfShip()]->AddBase(Sector((char)m_ShipArray[y].GetKO().x,(char)m_ShipArray[y].GetKO().y),
				m_Empire[m_ShipArray[y].GetOwnerOfShip()].GetResearch()->GetPropulsionTech());
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
	for (int j = HUMAN; j <= DOMINION; j++)
	{
		// Schiffunterstützungskosten auf NULL setzen
		m_Empire[j].SetShipCosts(0);
		BOOLEAN races[7] = {FALSE};
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != j && m_MajorRace[j].GetDiplomacyStatus(i) == NON_AGGRESSION_PACT)
				races[i] = TRUE;
		starmap[j]->SynchronizeWithMap(m_Sector, races);
	}
	
	// Hier kommt die Schiffsbewegung (also keine anderen Befehle werden hier noch ausgewertet, lediglich wird überprüft,
	// dass manche Befehle noch ihre Gültigkeit haben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		BYTE race = m_ShipArray[y].GetOwnerOfShip();
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
			if (m_Sector[p.x][p.y].GetOutpost(race) == TRUE)
				m_ShipArray[y].SetCurrentOrder(AVOID);
		}
		// Prüfen, dass ein Sternbasenbaubefehl noch gültig ist
		else if (m_ShipArray[y].GetCurrentOrder() == BUILD_STARBASE)
		{
			CPoint p = m_ShipArray[y].GetKO();
			if (m_Sector[p.x][p.y].GetStarbase(race) == TRUE)
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
			nextKO = starmap[race]->CalcPath(shipKO,targetKO,range,speed,*m_ShipArray[y].GetPath());
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
			BYTE winner[7];
			memset(winner, 2, sizeof(BYTE) * 7);
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
				// Jetzt können wir einen Kampf stattfinden lassen				
				combat.SetInvolvedShips(&pShipArray, m_MajorRace);
				if (combat.GetReadyForCombat())
				{
					combat.PreCombatCalculation();
				/*	CString fg;
					fg.Format("Calculation in Sector %s\n%d",m_Sector[pShipArray[0]->GetKO().x][pShipArray[0]->GetKO().y].GetName(1),pShipArray.GetSize());
					AfxMessageBox(fg);
				*/
					combat.CalculateCombat(winner);				
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
									m_ShipHistory[m_ShipArray[i].GetOwnerOfShip()].ModifyShip(&m_ShipArray[i].GetFleet()->GetShipFromFleet(x),
										m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound, 
										CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
									destroyedShips.Add(m_ShipArray[i].GetFleet()->GetShipFromFleet(x).GetShipName()+" ("+m_ShipArray[i].GetFleet()->GetShipFromFleet(x).GetShipTypeAsString()+")");
									m_ShipArray[i].GetFleet()->RemoveShipFromFleet(x--);
								}
							m_ShipArray[i].CheckFleet();
						}
						// Wenn das Schiff selbst zerstört wurde
						if (m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() < 1)
						{
							// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
							m_ShipHistory[m_ShipArray[i].GetOwnerOfShip()].ModifyShip(&m_ShipArray[i],
								m_Sector[m_ShipArray[i].GetKO().x][m_ShipArray[i].GetKO().y].GetName(TRUE), m_iRound,
								CResourceManager::GetString("COMBAT"), CResourceManager::GetString("DESTROYED"));
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
					
					for (int i = HUMAN; i <= DOMINION; i++)
					{
						// gewonnen
						if (winner[i] == 1)
						{
							message.GenerateMessage(CResourceManager::GetString("WIN_COMBAT",0,m_Sector[p.x][p.y].GetName(TRUE)),
							i, MILITARY, "", 0,0);
							m_Empire[i].AddMessage(message);
							// win a minor battle
							CString eventText = m_MajorRace[i].GetMoralObserver()->AddEvent(3,i);
							message.GenerateMessage(eventText, i, MILITARY, "", 0, 0);
							m_Empire[i].AddMessage(message);
							if (server.IsPlayedByClient((RACE)i))
								m_iSelectedView[i] = 7;
						}
						// verloren
						else if (winner[i] == 0)
						{
							message.GenerateMessage(CResourceManager::GetString("LOSE_COMBAT",0,m_Sector[p.x][p.y].GetName(TRUE)),
							i, MILITARY, "", 0,0);
							m_Empire[i].AddMessage(message);
							// lose a minorbattle
							CString eventText = m_MajorRace[i].GetMoralObserver()->AddEvent(6,i);
							message.GenerateMessage(eventText, i, MILITARY, "", 0, 0);
							m_Empire[i].AddMessage(message);
							if (server.IsPlayedByClient((RACE)i))
								m_iSelectedView[i] = 7;
							// Die Beziehung zum Gewinner verschlechtert sich dabei. Treffen zwei computergesteuerte Rassen
							// aufeinander, so ist die Beziehungsveringerung geringer
							for (int j = HUMAN; j <= DOMINION; j++)
								if (j != i && winner[j] == 1)
								{
									if (m_Empire[i].GetPlayerOfEmpire() == COMPUTER && m_Empire[j].GetPlayerOfEmpire() == COMPUTER)
										m_MajorRace[i].SetRelationshipToMajorRace(j, -(rand()%4));
									else
										m_MajorRace[i].SetRelationshipToMajorRace(j, -(rand()%6 + 5));
								}
						}
						// unentschieden
						else if (winner[i] == 3)
						{
							message.GenerateMessage(CResourceManager::GetString("DRAW_COMBAT",0,m_Sector[p.x][p.y].GetName(TRUE)),
							i, MILITARY, "", 0,0);
							m_Empire[i].AddMessage(message);
							if (server.IsPlayedByClient((RACE)i))
								m_iSelectedView[i] = 7;
						}
					}
					for (int i = HUMAN; i <= DOMINION; i++)
					{
						// Hat die Rasse an dem Kampf teilgenommen, also gewonnen oder verloren oder unentschieden
						if (winner[i] != 2)
							for (int j = 0; j < destroyedShips.GetSize(); j++)
							{
								CString s;
								s.Format("%s", CResourceManager::GetString("DESTROYED_SHIPS_IN_COMBAT",0,destroyedShips[j]));
								message.GenerateMessage(s, i, MILITARY, "", 0, 0);
								m_Empire[i].AddMessage(message);
							}
					}
				}
				y = 0;
				combat.Reset();
			}
		}
}

/// Diese Funktion berechnet die Auswirkungen von Schiffen und Stationen auf der Karte. So werden hier z.B. Sektoren
/// gescannt, Rassen kennengelernt und die Schiffe den Sektoren bekanntgegeben.
void CBotf2Doc::CalcShipEffects()
{
	using namespace network;
	BOOLEAN getContactWithMinor[7] = {FALSE};
	// Nach einem möglichen Kampf, aber natürlich auch generell die Schiffe und Stationen den Sektoren bekanntgeben
	for (int y = 0; y < m_ShipArray.GetSize(); y++)
	{
		// Hier die Sektoren "bekannt machen", wenn ein Schiff dort ist (stand vorher noch vor dem Kampf)
		// Bis jetzt wirds bekannt bei jedem Schiff, später nach von dem Schiffstyp abhängig machen
		BYTE race = m_ShipArray[y].GetOwnerOfShip();
		CPoint NewShipKO = m_ShipArray[y].GetKO();
		m_Sector[NewShipKO.x][NewShipKO.y].SetScanned(race);
		m_Sector[NewShipKO.x][NewShipKO.y].SetFullKnown(race);
		USHORT scanPower = m_Sector[NewShipKO.x][NewShipKO.y].GetScanPower(race);
		float boni = 1.0f;
		// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet, dann
		// wird die Scanleistung um 20% erhöht
		if (race == m_Sector[NewShipKO.x][NewShipKO.y].GetOwnerOfSector() && m_ShipArray[y].HasSpecial(PATROLSHIP))
			boni = 1.2f;
		if ((USHORT)(m_ShipArray[y].GetScanPower() * boni) > scanPower)
			m_Sector[NewShipKO.x][NewShipKO.y].SetScanPower((short)(m_ShipArray[y].GetScanPower() * boni),race);
				
		// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
		for (int j = -m_ShipArray[y].GetScanRange(); j <= m_ShipArray[y].GetScanRange(); j++)
			for (int i = -m_ShipArray[y].GetScanRange(); i <= m_ShipArray[y].GetScanRange(); i++)
				if (NewShipKO.y+j < STARMAP_SECTORS_VCOUNT && NewShipKO.y+j > -1
					&& NewShipKO.x+i < STARMAP_SECTORS_HCOUNT && NewShipKO.x+i > -1)
					if (NewShipKO.x+i != NewShipKO.x || NewShipKO.y+j != NewShipKO.y)
					{
						m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanned(race);
						// Teiler für die Scanstärke berechnen
						int div = max(abs(j),abs(i));
						if (div > 0)
						{
							scanPower = m_Sector[NewShipKO.x+i][NewShipKO.y+j].GetScanPower(race);
							if ((USHORT)(m_ShipArray[y].GetScanPower() * boni) / div > scanPower)
								m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanPower((short)(m_ShipArray[y].GetScanPower() *boni / div),race);
						}
					}
		// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
		m_Empire[race].AddShipCosts(m_ShipArray[y].GetMaintenanceCosts());
		// Scanstärke der Schiffe in der Flotte auf die Sektoren abhängig von der Scanrange übertragen
		if (m_ShipArray[y].GetFleet() != 0)
			for (int x = 0; x < m_ShipArray[y].GetFleet()->GetFleetSize(); x++)
			{
				// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
				m_Empire[race].AddShipCosts(m_ShipArray[y].GetFleet()->GetShipFromFleet(x).GetMaintenanceCosts());
				// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
				CShip ship = m_ShipArray[y].GetFleet()->GetShipFromFleet(x);
				for (int j = -ship.GetScanRange(); j <= ship.GetScanRange(); j++)
					for (int i = -ship.GetScanRange(); i <= ship.GetScanRange(); i++)
						if (NewShipKO.y+j < STARMAP_SECTORS_VCOUNT && NewShipKO.y+j > -1
							&& NewShipKO.x+i < STARMAP_SECTORS_HCOUNT && NewShipKO.x+i > -1)
							{
								m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanned(race);
								// Teiler für die Scanstärke berechnen
								int div = max(abs(j),abs(i));
								if (div == 0)
									div = 1;
								scanPower = m_Sector[NewShipKO.x+i][NewShipKO.y+j].GetScanPower(race);
								// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor
								// befindet, dann wird die Scanleistung um 20% erhöht
								if (race == m_Sector[NewShipKO.x][NewShipKO.y].GetOwnerOfSector() && ship.HasSpecial(PATROLSHIP))
									boni = 1.2f;
								else
									boni = 1.0f;								
								if ((ship.GetScanPower() * boni / div) > scanPower)
									m_Sector[NewShipKO.x+i][NewShipKO.y+j].SetScanPower((short)(ship.GetScanPower() * boni / div),race);
							}
			}

		CPoint p = m_ShipArray[y].GetKO();
		// Erfahrunspunkte der Schiffe anpassen
		this->CalcShipExp(&m_ShipArray[y]);

		// Die Schiffslisten der einzelnen Imperien modifizieren
		if (m_ShipArray[y].GetOwnerOfShip() >= HUMAN && m_ShipArray[y].GetOwnerOfShip() <= DOMINION)
			m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y], m_Sector[p.x][p.y].GetName(TRUE));
		// Dem Sektor nochmal bekanntgeben, dass in ihm eine Sternbasis oder ein Außenposten steht. Weil wenn im Kampf
		// eine Station teilnahm, dann haben wir den Shipport in dem Sektor vorläufig entfernt. Es kann ja passieren,
		// dass die Station zerstört wird. Haben wir jetzt aber immernoch eine Station, dann bleibt der Shipport dort auch
		// bestehen
		if (m_ShipArray[y].GetShipType() == OUTPOST || m_ShipArray[y].GetShipType() == STARBASE)
		{
			m_Sector[p.x][p.y].SetShipPort(TRUE, m_ShipArray[y].GetOwnerOfShip());
			if (m_ShipArray[y].GetShipType() == OUTPOST)
				m_Sector[p.x][p.y].SetOutpost(TRUE, m_ShipArray[y].GetOwnerOfShip());
			else
				m_Sector[p.x][p.y].SetStarbase(TRUE, m_ShipArray[y].GetOwnerOfShip());
		}
		// Dem Sektor bekanntgeben, das in ihm ein Schiff ist
		if (m_ShipArray[y].GetShipType() != OUTPOST && m_ShipArray[y].GetShipType() != STARBASE) 
			m_Sector[p.x][p.y].SetOwnerOfShip(TRUE, m_ShipArray[y].GetOwnerOfShip());
		// Im Sektor die NeededScanPower setzen, die wir brauchen um dort Schiffe zu sehen. Wir sehen ja keine getarneten
		// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
		// sein, ansonsten gilt dort nur Stufe 3.
		USHORT stealthPower = m_ShipArray[y].GetStealthPower() * 20;
		if (m_ShipArray[y].GetStealthPower() > 3 && m_ShipArray[y].GetCloak() == FALSE)
			stealthPower = 3 * 20;
		if (stealthPower < m_Sector[p.x][p.y].GetNeededScanPower(m_ShipArray[y].GetOwnerOfShip()))
			m_Sector[p.x][p.y].SetNeededScanPower(stealthPower, m_ShipArray[y].GetOwnerOfShip());
		
		// Jedes Schiff erhöht auf seinem Sektor zusätzlich die Scanpower um 1
		m_Sector[p.x][p.y].SetScanPower(m_Sector[p.x][p.y].GetScanPower(m_ShipArray[y].GetOwnerOfShip())+1,m_ShipArray[y].GetOwnerOfShip());
		// auch die Schiffe in einer Flotte erhöhen die Scankraft
		if (m_ShipArray[y].GetFleet())
			for (int i = 0; i < m_ShipArray.GetAt(y).GetFleet()->GetFleetSize(); i++)
			{
				m_Sector[p.x][p.y].SetScanPower(m_Sector[p.x][p.y].GetScanPower(m_ShipArray[y].GetOwnerOfShip())+1,m_ShipArray[y].GetOwnerOfShip());
				// Auch die Schiffe in der Flotte beim modifizieren der Schiffslisten der einzelnen Imperien beachten
				if (m_ShipArray[y].GetOwnerOfShip() >= HUMAN && m_ShipArray[y].GetOwnerOfShip() <= DOMINION)
					m_ShipHistory[m_ShipArray[y].GetOwnerOfShip()].ModifyShip(&m_ShipArray[y].GetFleet()->GetShipFromFleet(i), m_Sector[p.x][p.y].GetName(TRUE));
			}

		// Wenn in dem Sektor eine Minorrace lebt, diese dem jeweiligen Imperium bekanntgeben
		if (m_Sector[p.x][p.y].GetMinorRace())
			for (int i = 0; i < m_MinorRaceArray.GetSize(); i++)
				if (m_MinorRaceArray.GetAt(i).GetRaceKO() == p && m_MinorRaceArray.GetAt(i).GetKnownByMajorRace(m_ShipArray[y].GetOwnerOfShip()) == FALSE)
				{
					m_MinorRaceArray.GetAt(i).SetKnownByMajorRace(m_ShipArray[y].GetOwnerOfShip(), TRUE);
					// Nachricht generieren, dass wir eine Minorrace kennengelernt haben
					CString s = CResourceManager::GetString("GET_CONTACT_TO_MINOR",FALSE,m_MinorRaceArray.GetAt(i).GetRaceName());
					message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),DIPLOMACY,"",0,FALSE);
					m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
					if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip()))
					{
						if (getContactWithMinor[m_ShipArray[y].GetOwnerOfShip()] == FALSE)
						{
							SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_ALIENCONTACT, (RACE)m_ShipArray[y].GetOwnerOfShip(), 1, 1.0f};
							m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry);
							getContactWithMinor[m_ShipArray[y].GetOwnerOfShip()] = TRUE;
						}
						m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
					}
				}
		// Wenn dieser Sektor einer anderen Majorrace gehört, wir die noch nicht kannten, dann bekanntgeben
		if (m_Sector[p.x][p.y].GetOwnerOfSector() != NOBODY &&
			m_Sector[p.x][p.y].GetOwnerOfSector() != UNKNOWN &&
			m_ShipArray[y].GetOwnerOfShip() != NOBODY &&
			m_ShipArray[y].GetOwnerOfShip() != UNKNOWN &&
			m_Sector[p.x][p.y].GetOwnerOfSector() != m_ShipArray[y].GetOwnerOfShip() &&			
			m_MajorRace[m_Sector[p.x][p.y].GetOwnerOfSector()].GetKnownMajorRace(m_ShipArray[y].GetOwnerOfShip()) == FALSE &&
			m_MajorRace[m_ShipArray[y].GetOwnerOfShip()].GetKnownMajorRace(m_Sector[p.x][p.y].GetOwnerOfSector()) == FALSE)
		{
			m_MajorRace[m_ShipArray[y].GetOwnerOfShip()].SetKnownMajorRace(m_Sector[p.x][p.y].GetOwnerOfSector());
			m_MajorRace[m_Sector[p.x][p.y].GetOwnerOfSector()].SetKnownMajorRace(m_ShipArray[y].GetOwnerOfShip());
			// Nachricht generieren, dass wir eine Majorrace kennengelernt haben
			CString s;
			CString sect; sect.Format("%c%i",(char)(p.y+97),p.x+1);
			// der Major, der der Sektor gehört
			s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR",FALSE,CMajorRace::GetRaceName(m_ShipArray[y].GetOwnerOfShip()),sect);
			message.GenerateMessage(s,m_Sector[p.x][p.y].GetOwnerOfSector(),DIPLOMACY,"",0,FALSE);
			m_Empire[m_Sector[p.x][p.y].GetOwnerOfSector()].AddMessage(message);
			// der Major, der das Schiff gehört
			s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR",FALSE, CMajorRace::GetRaceName(m_Sector[p.x][p.y].GetOwnerOfSector()),sect);
			message.GenerateMessage(s,m_ShipArray[y].GetOwnerOfShip(),DIPLOMACY,"",0,FALSE);
			m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
			if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip())
				|| server.IsPlayedByClient((RACE)m_Sector[p.x][p.y].GetOwnerOfSector()))
			{
				// Audiovorstellung der kennengelernten Majorrace
				SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_FIRSTCONTACT, (RACE)m_ShipArray[y].GetOwnerOfShip(), 2, 1.0f};
				m_SoundMessages[m_Sector[p.x][p.y].GetOwnerOfSector()].Add(entry);
				SNDMGR_MESSAGEENTRY entry2 = {SNDMGR_MSG_FIRSTCONTACT, (RACE)m_Sector[p.x][p.y].GetOwnerOfSector(), 2, 1.0f};
				m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry2);
				m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
				m_iSelectedView[m_Sector[p.x][p.y].GetOwnerOfSector()] = EMPIRE_VIEW;
			}
		}
		// wenn zwei Schiffe einer Rasse in diesem Sektor stationiert sind, so können sich die Besitzer auch kennenlernen
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != m_ShipArray[y].GetOwnerOfShip())
				if (m_Sector[p.x][p.y].GetOwnerOfShip(i) == TRUE)
					if (m_MajorRace[m_ShipArray[y].GetOwnerOfShip()].GetKnownMajorRace(i) == FALSE)
					{
						m_MajorRace[m_ShipArray[y].GetOwnerOfShip()].SetKnownMajorRace(i);
						m_MajorRace[i].SetKnownMajorRace(m_ShipArray[y].GetOwnerOfShip());
						// Nachricht generieren, dass wir eine Majorrace kennengelernt haben
						CString s;
						CString sect; sect.Format("%c%i",(char)(p.y+97),p.x+1);
						// der Major, der das erste Schiff gehört
						s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR", FALSE, CMajorRace::GetRaceName(m_ShipArray[y].GetOwnerOfShip()),sect);
						message.GenerateMessage(s, i ,DIPLOMACY, "", 0, FALSE);
						m_Empire[i].AddMessage(message);
						// der Major, der das zweite Schiff gehört
						s = CResourceManager::GetString("GET_CONTACT_TO_MAJOR", FALSE, CMajorRace::GetRaceName(i), sect);
						message.GenerateMessage(s, m_ShipArray[y].GetOwnerOfShip(), DIPLOMACY, "", 0, FALSE);
						m_Empire[m_ShipArray[y].GetOwnerOfShip()].AddMessage(message);
						if (server.IsPlayedByClient((RACE)m_ShipArray[y].GetOwnerOfShip())
							|| server.IsPlayedByClient((RACE)i))
						{
							// Audiovorstellung der kennengelernten Majorrace
							SNDMGR_MESSAGEENTRY entry = {SNDMGR_MSG_FIRSTCONTACT, (RACE)m_ShipArray[y].GetOwnerOfShip(), 2, 1.0f};
							m_SoundMessages[i].Add(entry);
							SNDMGR_MESSAGEENTRY entry2 = {SNDMGR_MSG_FIRSTCONTACT, (RACE)i, 2, 1.0f};
							m_SoundMessages[m_ShipArray[y].GetOwnerOfShip()].Add(entry2);
							m_iSelectedView[m_ShipArray[y].GetOwnerOfShip()] = EMPIRE_VIEW;
							m_iSelectedView[i] = EMPIRE_VIEW;
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

	TRACE("loading savegame \"%s\"\n", lpszPathName);

	CFile file;
	BYTE *lpBuf = NULL;

	{
	CFileException ex;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		TRACE("savegame: could not open file (%s)\n", cause);
		return FALSE;
	}

	// Header lesen
	UINT nSize = 4 + 2 * sizeof(UINT);
	lpBuf = new BYTE[nSize];
	UINT nDone = file.Read(lpBuf, nSize);
	if (nDone < nSize)
	{
		TRACE("savegame: unexpected end of file\n");
		goto error;
	}

	// Magic Number prüfen
	BYTE *p = lpBuf;
	if (memcmp(p, "BotE", 4) != 0)
	{
		TRACE("savegame: invalid magic number\n");
		goto error;
	}
	p += 4;

	// Versionsnummer prüfen
	UINT nVersion = 0;
	memcpy(&nVersion, p, sizeof(UINT));
	p += sizeof(UINT);
	if (nVersion != DOCUMENT_VERSION)
	{
		TRACE("savegame: wrong version\n");
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
		TRACE("savegame: unexpected end of file\n");
		goto error;
	}

	// Dekomprimieren
	CMemFile memFile;
	if (!BotE_LzmaDecompress(lpBuf, nSize, memFile))
	{
		TRACE("savegame: error during decompression\n");
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
	
	TRACE("storing savegame \"%s\"\n", lpszPathName);

	// Savegame schreiben
	CFileException ex;
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeBinary,
		&ex))
	{
		TCHAR cause[255];
		ex.GetErrorMessage(cause, 255);
		TRACE("savegame: could not open file (%s)\n", cause);
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
		TRACE("savegame: error during compression\n");
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
			TRACE("savegame: could not delete corrupted savegame (%s)\n", cause);
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