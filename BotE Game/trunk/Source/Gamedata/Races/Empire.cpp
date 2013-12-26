// Empire.cpp: Implementierung der Klasse CEmpire.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Empire.h"
#include "System\System.h"
#include "Galaxy\Sector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CEmpire, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CEmpire::CEmpire()
{
	Reset();
}
CEmpire::~CEmpire()
{
	Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////

static boost::shared_ptr<CEventScreen> make_screen(CEventScreen::EVENT_SCREEN_TYPE type)
{
	switch(type)
	{
		case CEventScreen::EVENT_SCREEN_TYPE_ALIEN_ENTITY: return boost::make_shared<CEventAlienEntity>();
		case CEventScreen::EVENT_SCREEN_TYPE_BLOCKADE: return boost::make_shared<CEventBlockade>();
		case CEventScreen::EVENT_SCREEN_TYPE_BOMBARDMENT: return boost::make_shared<CEventBombardment>();
		case CEventScreen::EVENT_SCREEN_TYPE_COLONIZATION: return boost::make_shared<CEventColonization>();
		case CEventScreen::EVENT_SCREEN_TYPE_FIRST_CONTACT: return boost::make_shared<CEventFirstContact>();
		case CEventScreen::EVENT_SCREEN_TYPE_GAME_OVER: return boost::make_shared<CEventGameOver>();
		case CEventScreen::EVENT_SCREEN_TYPE_RACE_KILLED: return boost::make_shared<CEventRaceKilled>();
		case CEventScreen::EVENT_SCREEN_TYPE_RANDOM: return boost::make_shared<CEventRandom>();
		case CEventScreen::EVENT_SCREEN_TYPE_RESEARCH: return boost::make_shared<CEventResearch>();
		case CEventScreen::EVENT_SCREEN_TYPE_VICTORY: return boost::make_shared<CEventVictory>();
	}
	AssertBotE(false);
	return boost::shared_ptr<CEventScreen>();
}

void CEmpire::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	m_Research.Serialize(ar);
	m_Intelligence.Serialize(ar);
	m_GlobalStorage.Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_nNumberOfSystems;
		ar << m_iCredits;
		ar << m_iCreditsChange;
		ar << m_iShipCosts;
		ar << m_iPopSupportCosts;
		ar << m_lFP;
		for (int i = 0; i <= RESOURCES::DERITIUM; i++)
			ar << m_lResourceStorages[i];
		ar << m_sEmpireID;
		ar << m_vMessages.GetSize();
		for (int i = 0; i < m_vMessages.GetSize(); i++)
			m_vMessages.GetAt(i).Serialize(ar);
		ar << m_Events.size();
		for(std::vector<boost::shared_ptr<CEventScreen>>::iterator it = m_Events.begin();
				it != m_Events.end(); ++it)
		{
			ar << static_cast<int>((*it)->GetType());
			(*it)->Serialize(ar);
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ClearMessagesAndEvents();
		int number = 0;
		ar >> m_nNumberOfSystems;
		ar >> m_iCredits;
		ar >> m_iCreditsChange;
		ar >> m_iShipCosts;
		ar >> m_iPopSupportCosts;
		ar >> m_lFP;
		for (int i = 0; i <= RESOURCES::DERITIUM; i++)
			ar >> m_lResourceStorages[i];
		ar >> m_sEmpireID;
		ar >> number;
		m_vMessages.SetSize(number);
		for (int i = 0; i < number; i++)
			m_vMessages.GetAt(i).Serialize(ar);
		unsigned size = 0;
		ar >> size;
		m_Events.resize(size);
		for(std::vector<boost::shared_ptr<CEventScreen>>::iterator it = m_Events.begin(); it != m_Events.end(); ++it)
		{
			int type = 0;
			ar >> type;
			*it = make_screen(static_cast<CEventScreen::EVENT_SCREEN_TYPE>(type));
			(*it)->Serialize(ar);
		}

	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert die Liste der Systeme für das Imperium anhand aller Systeme.
void CEmpire::GenerateSystemList(const std::vector<CSystem>& systems)
{
	m_SystemList.RemoveAll();
	for(std::vector<CSystem>::const_iterator it = systems.begin(); it != systems.end(); ++it)
		if(it->Majorized() && it->OwnerID() == m_sEmpireID)
			m_SystemList.Add(EMPIRE_SYSTEMS(it->GetName(), it->GetCo()));
}

/// Funktion addiert die übergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
void CEmpire::AddFP(int add)
{
	if (((long)m_lFP + add) < 0)
		m_lFP = 0;
	else
		m_lFP += add;
}

// Funktion setzt die Lager wieder auf NULL, damit wir die später wieder füllen können
// die wirklichen Lager in den einzelnen Systemen werden dabei nicht angerührt
void CEmpire::ClearAllPoints(void)
{
	memset(m_lResourceStorages, 0, sizeof(m_lResourceStorages));
	m_lFP = 0;
	m_Intelligence.ClearSecurityPoints();
}

/// Funktion löscht alle Nachrichten und Antworten an das Imperiums.
void CEmpire::ClearMessagesAndEvents(void)
{
	m_vMessages.RemoveAll();
	m_Events.clear();
}

void CEmpire::Reset(void)
{
	m_nNumberOfSystems = 1;
	m_sEmpireID = "";
	m_iCredits = 1000;
	m_iCreditsChange = 0;
	m_iShipCosts = 0;
	m_iPopSupportCosts = 0;

	m_Research.Reset();
	m_Intelligence.Reset();
	m_GlobalStorage.Reset();

	ClearAllPoints();
	ClearMessagesAndEvents();

	m_SystemList.RemoveAll();
}

const boost::shared_ptr<CEventScreen> CEmpire::FirstEvent(bool pop)
{
	const boost::shared_ptr<CEventScreen> screen = m_Events.at(0);
	if(pop)
		m_Events.erase(m_Events.begin());
	return screen;
}
