#include "stdafx.h"
#include "SoundManager.h"
#include "ResourceManager.h"
#include "IOData.h"
#include <algorithm>

CSoundManager::CSoundManager(UINT nMaxLoadedSounds)
	: m_nLastResult(FMOD_OK), m_pSystem(NULL), m_pMusicChannel(NULL), m_pMusic(NULL),
	m_nMaxLoadedSounds(nMaxLoadedSounds), m_pSoundChannel(NULL), m_fSoundMasterVolume(1.f),
	m_pMessageChannel(NULL), m_pLastMessage(NULL), m_bMessagesPlaying(false), m_bInterruptMessages(false),
	m_fMessageMasterVolume(1.f), m_bUseSoftwareSound(FALSE)
{
	m_hThreadStopped = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSoundManager::~CSoundManager()
{
	StopMessages(TRUE);
	Release();
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CSoundManager* CSoundManager::GetInstance(void)
{
	static CSoundManager instance; 
    return &instance;
}

std::string CSoundManager::GetPathName(SNDMGR_VALUE nSound, network::RACE nRace)
{
	using namespace network;
	std::string msg;
	
	switch (nSound)
	{
	// Hintergrundmusik
	case SNDMGR_MUSIC_FED:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE1_PREFIX") + "music.ogg"; return msg;
	case SNDMGR_MUSIC_FER:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE2_PREFIX") + "music.ogg"; return msg;
	case SNDMGR_MUSIC_KLI:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE3_PREFIX") + "music.ogg"; return msg;
	case SNDMGR_MUSIC_ROM:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE4_PREFIX") + "music.ogg"; return msg;
	case SNDMGR_MUSIC_CAR:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE5_PREFIX") + "music.ogg"; return msg;
	case SNDMGR_MUSIC_DOM:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE6_PREFIX") + "music.ogg"; return msg;

	// Rassenauswahl
	case SNDMGR_VOICE_FED_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE1_PREFIX") + "raceSelect.ogg"; return msg;
	case SNDMGR_VOICE_FER_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE2_PREFIX") + "raceSelect.ogg"; return msg;
	case SNDMGR_VOICE_KLI_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE3_PREFIX") + "raceSelect.ogg"; return msg;
	case SNDMGR_VOICE_ROM_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE4_PREFIX") + "raceSelect.ogg"; return msg;
	case SNDMGR_VOICE_CAR_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE5_PREFIX") + "raceSelect.ogg"; return msg;
	case SNDMGR_VOICE_DOM_RACESELECT:	msg = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + CResourceManager::GetString("RACE6_PREFIX") + "raceSelect.ogg"; return msg;

	// Oberflächengeräusche
	case SNDMGR_SOUND_MAINMENU:			return (std::string)CIOData::GetInstance()->GetAppPath() + "Sounds\\MouseOver.ogg";
	case SNDMGR_SOUND_ENDOFROUND:		return (std::string)CIOData::GetInstance()->GetAppPath() + "Sounds\\RoundEnd.ogg";
	case SNDMGR_SOUND_SHIPTARGET:		return (std::string)CIOData::GetInstance()->GetAppPath() + "Sounds\\affirmative.ogg";
	case SNDMGR_SOUND_ERROR:			return (std::string)CIOData::GetInstance()->GetAppPath() + "Sounds\\error.ogg";

	// Nachrichten
	case SNDMGR_MSG_BATTLELOST:			msg = "battlelost"; break;

	case SNDMGR_MSG_ALIENCONTACT:		msg = "alienContact.ogg"; break;
	case SNDMGR_MSG_CLAIMSYSTEM:		msg = "claimSystem.ogg"; break;
	case SNDMGR_MSG_COLONIZING:			msg = "colonizing.ogg"; break;
	case SNDMGR_MSG_DIPLOMATICNEWS:		msg = "diplomatNews.ogg"; break;
	case SNDMGR_MSG_FIRSTCONTACT:		msg = "firstContact.ogg"; break;
	case SNDMGR_MSG_INTELNEWS:			msg = "intelNews.ogg"; break;
	case SNDMGR_MSG_NEWTECHNOLOGY:		msg = "newTechnology.ogg"; break;
	case SNDMGR_MSG_OUTPOST_CONSTRUCT:	msg = "outpostConstruct.ogg"; break;
	case SNDMGR_MSG_OUTPOST_READY:		msg = "outpostReady.ogg"; break;
	case SNDMGR_MSG_SCIENTISTNEWS:		msg = "scientistNews.ogg"; break;
	case SNDMGR_MSG_STARBASE_CONSTRUCT:	msg = "starbaseConstruct.ogg"; break;
	case SNDMGR_MSG_STARBASE_READY:		msg = "starbaseReady.ogg"; break;
	case SNDMGR_MSG_TERRAFORM_COMPLETE:	msg = "terraformComplete.ogg"; break;
	case SNDMGR_MSG_TERRAFORM_SELECT:	msg = "terraformSelect.ogg"; break;
	
	// TODO ergänzen

	default:							return "";
	}

	// Nachrichten
	std::string prefix;
	if (nRace >= RACE_FIRST && nRace < RACE_LAST)
	{
		std::string prefixes[] = {
			CResourceManager::GetString("RACE1_PREFIX"),
			CResourceManager::GetString("RACE2_PREFIX"),
			CResourceManager::GetString("RACE3_PREFIX"),
			CResourceManager::GetString("RACE4_PREFIX"),
			CResourceManager::GetString("RACE5_PREFIX"),
			CResourceManager::GetString("RACE6_PREFIX")
		};
		prefix = prefixes[nRace-1];
	}

	return (std::string)CIOData::GetInstance()->GetAppPath() + "Sounds\\" + prefix + msg;
}

SNDMGR_VALUE CSoundManager::GetRaceMusic(network::RACE race)
{
	using namespace network;
	ASSERT(race >= RACE_FIRST && race < RACE_LAST);

	// zum Volk gehörige Konstante zurückgeben
	switch (race)
	{
	case RACE_1:	return SNDMGR_MUSIC_FED;
	case RACE_2:	return SNDMGR_MUSIC_FER;
	case RACE_3:	return SNDMGR_MUSIC_KLI;
	case RACE_4:	return SNDMGR_MUSIC_ROM;
	case RACE_5:	return SNDMGR_MUSIC_CAR;
	case RACE_6:	return SNDMGR_MUSIC_DOM;
	}

	// keine Zuordnung vorhanden
	return SNDMGR_INVALID;
}

BOOL CSoundManager::Init(BOOL bUseSoftwareSound)
{
	if (m_pSystem) return TRUE;

	// merken, ob Software statt Hardware verwendet werden soll
	m_bUseSoftwareSound = bUseSoftwareSound;

	// Sound-System erzeugen
	m_nLastResult = FMOD::System_Create(&m_pSystem);
	if (m_nLastResult != FMOD_OK || !m_pSystem) return FALSE;

	// Sound-System initialisieren; verwenden 3 Channels: Hintergrundmusik, Oberflächen-Sounds (es werden
	// keine 2 Oberflächen-Sounds gleichzeitig abgespielt) und Nachrichten (werden nacheinander abgespielt)
	m_nLastResult = m_pSystem->init(3, FMOD_INIT_NORMAL, NULL);
	if (m_nLastResult != FMOD_OK) return FALSE;

	return TRUE;
}

void CSoundManager::Release()
{
	// Liste der Sounds leeren (Sounds werden in FMOD::System::release() freigegeben)
	m_lSounds.clear();

	// Liste der Nachrichten leeren
	m_lMessages.clear();

	// alles freigeben
	if (m_pSystem)
	{
		m_pSystem->release();
		m_pSystem = NULL;

		m_pLastMessage = NULL;
		m_pMusicChannel = m_pSoundChannel = m_pMessageChannel = NULL;
	}
}

BOOL CSoundManager::StartMusic(network::RACE race, float fVolume)
{
	// zugehörigen Dateinamen ermitteln, abbrechen, falls keine Zuordnung vorhanden ist
	std::string pathName = GetPathName(GetRaceMusic(race));
	
	// Musik starten
	return StartMusic(pathName.c_str(), fVolume);
}

BOOL CSoundManager::StartMusic(const CString& sFile, float fVolume)
{
	if (sFile == "")
		return TRUE;

	if (!m_pSystem)
	{
		m_nLastResult = FMOD_ERR_UNINITIALIZED;
		return FALSE;
	}

	// falls zuvor Musik gestartet wurde, diese anhalten und freigeben
	StopMusic();

	// Parameter für Streamerzeugung vorbereiten
	m_pMusic = NULL;

	FMOD_MODE mode = FMOD_DEFAULT | FMOD_LOOP_NORMAL | FMOD_LOWMEM;
	if (m_bUseSoftwareSound) mode |= FMOD_SOFTWARE;

	// Stream erzeugen
	m_nLastResult = m_pSystem->createStream(sFile, mode, NULL, &m_pMusic);
	if (m_nLastResult != FMOD_OK || !m_pMusic) return FALSE;

	// Abspielen vorbereiten
	m_nLastResult = m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_pMusic, true, &m_pMusicChannel);
	if (m_nLastResult != FMOD_OK || !m_pMusicChannel) goto error;

	// Priorität und Lautstärke setzen
	m_pMusicChannel->setVolume(fVolume);

	// Abspielen starten
	m_nLastResult = m_pMusicChannel->setPaused(false);
	if (m_nLastResult != FMOD_OK) goto error;

	return TRUE;

error:
	m_pMusic->release();
	m_pMusic = NULL;
	m_pMusicChannel = NULL;
	return FALSE;
}

void CSoundManager::StopMusic()
{
	if (!m_pSystem) return;
	
	if (m_pMusicChannel) m_pMusicChannel->stop();
	if (m_pMusic) m_pMusic->release();

	m_pMusic = NULL;
	m_pMusicChannel = NULL;
}

void CSoundManager::FadeOutMusic(UINT nSteps, DWORD dwMilliseconds)
{
	if (!m_pSystem || !m_pMusicChannel) return;

	// bei 0 Schritten, Lautstärke sofort auf 0 setzen
	if (nSteps == 0)
	{
		m_pMusicChannel->setVolume(0);
		return;
	}

	// sonst die aktuelle Lautstärke ermitteln
	float fVolume;
	if (m_pMusicChannel->getVolume(&fVolume) != FMOD_OK) return;

	// Lautstärke in nSteps Schritten bis 0 senken
	for (UINT i = 1; i <= nSteps; i++)
	{
		Sleep(dwMilliseconds);
		if (m_pMusicChannel->setVolume(fVolume - (i * fVolume) / nSteps) != FMOD_OK)
			return;
	}
}

void CSoundManager::SetMusicVolume(float fVolume)
{
	if (!m_pSystem || !m_pMusicChannel) return;
	m_pMusicChannel->setVolume(fVolume);
}

void CSoundManager::PauseMusic(BOOL bPaused)
{
	if (!m_pSystem || !m_pMusicChannel) return;
	m_pMusicChannel->setPaused(bPaused != FALSE);
}

void CSoundManager::SetSoundMasterVolume(float fSoundMasterVolume)
{
	if (fSoundMasterVolume < 0.f || fSoundMasterVolume > 1.f) return;
	m_fSoundMasterVolume = fSoundMasterVolume;
}

BOOL CSoundManager::PlaySound(SNDMGR_VALUE nSound, SNDMGR_PRIO nPriority, float fVolume, network::RACE nRace)
{
	if (!m_pSystem)
	{
		m_nLastResult = FMOD_ERR_UNINITIALIZED;
		return FALSE;
	}

	// nichts tun, wenn Sound-Ausgabe abgeschaltet ist
	if (m_fSoundMasterVolume == 0.f) return TRUE;

	// wenn ein Sound von zuvor noch läuft, Priorität prüfen
	int nPrio = (nPriority == SNDMGR_PRIO_HIGH) ? PRIORITY_SOUND_HIGH : PRIORITY_SOUND_NORMAL;
	bool bIsPlaying = false;
	if (m_pSoundChannel) m_pSoundChannel->isPlaying(&bIsPlaying);
	if (bIsPlaying)
	{
		int nChannelPrio = 128;
		m_nLastResult = m_pSoundChannel->getPriority(&nChannelPrio);
		if (m_nLastResult == FMOD_OK)
		{
			// abbrechen, wenn geringere Priorität (entspricht größerem Wert);
			// bei gleicher oder höherer Priorität nichts tun und damit fortsetzen
			if (nPrio > nChannelPrio) return TRUE;
		}
		else if (m_nLastResult != FMOD_ERR_INVALID_HANDLE)
			return FALSE;
		
		// FMOD_ERR_INVALID_HANDLE: Sound zuvor wurde schon fertig abgespielt, in diesem Fall
		// nicht abbrechen
	}

	// Sound laden bzw. Handle ermitteln, falls schon geladen
	FMOD::Sound *pSound = LoadSound(nSound, nRace);
	if (!pSound) return FALSE;

	// Channel zuvor läuft nicht oder wir haben gleiche oder höhere Priorität;
	// Abspielen vorbereiten
	m_nLastResult = m_pSystem->playSound(FMOD_CHANNEL_REUSE, pSound, true, &m_pSoundChannel);
	if (m_nLastResult != FMOD_OK || !m_pSoundChannel) return FALSE;

	// Priorität und Lautstärke setzen
	m_pSoundChannel->setPriority(nPrio);
	m_pSoundChannel->setVolume(fVolume * m_fSoundMasterVolume);

	// Abspielen starten
	m_nLastResult = m_pSoundChannel->setPaused(false);
	if (m_nLastResult != FMOD_OK) return FALSE;

	return TRUE;
}

FMOD::Sound *CSoundManager::LoadSound(SNDMGR_VALUE nSound, network::RACE nRace)
{
	// wenn schon geladen, ans Ende der Liste stellen und zurückgeben
	UINT nIdx = 0;
	for (SNDMGR_SOUNDLIST::iterator it = m_lSounds.begin(); it != m_lSounds.end(); it++, nIdx++)
	{
		if ((*it).nType == nSound)
		{
			FMOD::Sound *pSound = (*it).pSound;
			if (nIdx != m_lSounds.size() - 1) // nicht verschieben, wenn schon letztes Element der Liste
			{
				m_lSounds.erase(it);
				SNDMGR_SOUNDENTRY entry = {nSound, pSound};
				m_lSounds.push_back(entry);
			}
			return pSound;
		}
	}

	// wenn schon maximale Anzahl geladen, ersten Eintrag entfernen
	while (m_lSounds.size() >= m_nMaxLoadedSounds && m_lSounds.size() > 0)
	{
		SNDMGR_SOUNDLIST::iterator it = m_lSounds.begin();
		FMOD::Sound *pSound = (*it).pSound;
		m_lSounds.erase(it);
		pSound->release();
	}

	// laden
	FMOD::Sound *pSound = NULL;
	FMOD_MODE mode = FMOD_DEFAULT | FMOD_LOWMEM;
	if (m_bUseSoftwareSound) mode |= FMOD_SOFTWARE;

	std::string pathName = GetPathName(nSound, nRace);
	if (pathName.empty()) return NULL;

	m_nLastResult = m_pSystem->createSound(pathName.c_str(), mode, NULL, &pSound);
	if (m_nLastResult != FMOD_OK || !pSound) return NULL;

	// an Liste anhängen
	SNDMGR_SOUNDENTRY entry = {nSound, pSound};
	m_lSounds.push_back(entry);

	return pSound;
}

void CSoundManager::UnloadAllSounds()
{
	if (!m_pSystem) return;

	// alle Sounds freigeben
	for (SNDMGR_SOUNDLIST::iterator it = m_lSounds.begin(); it != m_lSounds.end(); it++)
		(*it).pSound->release();
	// sämtliche Einträge der Liste entfernen
	m_lSounds.clear();
}

void CSoundManager::AddMessage(SNDMGR_VALUE nMsg, network::RACE nRace, int nPriority, float fVolume)
{
	if (m_bMessagesPlaying) return;

	// Befindet sich diese Nachricht schon in der Liste wird sie nicht nochmal hinzugefügt.
	for (SNDMGR_MESSAGELIST::const_iterator it = m_lMessages.begin(); it != m_lMessages.end(); ++it)
		if (it->nMessage == nMsg && it->nRace == nRace)
			return;

	// Eintrag anhängen
	SNDMGR_MESSAGEENTRY entry = {nMsg, nRace, nPriority, fVolume};
	m_lMessages.push_back(entry);
}

void CSoundManager::ClearMessages()
{
	if (m_bMessagesPlaying) return;

	// alle Einträge entfernen
	m_lMessages.clear();
}

bool CSoundManager::CompareMessageEntries(const SNDMGR_MESSAGEENTRY &entry1,
	const SNDMGR_MESSAGEENTRY &entry2)
{
	return entry1.nPriority > entry2.nPriority;
}

void CSoundManager::SetMessageMasterVolume(float fMessageMasterVolume)
{
	m_fMessageMasterVolume = fMessageMasterVolume;
}

BOOL CSoundManager::PlayMessages(DWORD dwFirstMillis, DWORD dwMillis)
{
	if (m_bMessagesPlaying || m_lMessages.empty() || m_fMessageMasterVolume == 0.f) return TRUE;

	if (!m_pSystem)
	{
		m_nLastResult = FMOD_ERR_UNINITIALIZED;
		return FALSE;
	}

	// nach Prio sortieren
	m_lMessages.sort(CompareMessageEntries);

	// Parameter setzen, Event non-signaled setzen
	m_bMessagesPlaying = true;
	m_bInterruptMessages = false;
	m_dwMillis = dwMillis;
	m_dwFirstMillis = dwFirstMillis;
	VERIFY(ResetEvent(m_hThreadStopped));

	// Thread starten
	AfxBeginThread(ThreadProc, this);

	return TRUE;
}

BOOL CSoundManager::StopMessages(BOOL bHardInterrupt, DWORD dwTimeout)
{
	// nichts tun, wenn keine Nachrichten abgespielt werden
	if (!m_bMessagesPlaying) return TRUE;

	if (bHardInterrupt)
	{
		// sofort unterbrechen, warten, bis Thread beendet
		m_bMessagesPlaying = false;
		return (WaitForSingleObject(m_hThreadStopped, dwTimeout) == WAIT_OBJECT_0);
	}
	else
	{
		// erst in der nächsten Pause unterbrechen
		m_bInterruptMessages = true;
		return TRUE;
	}
}

UINT CSoundManager::ThreadProc(LPVOID pParam)
{
	ASSERT(pParam);
	CSoundManager *pObj = (CSoundManager *)pParam;

	while (pObj->m_bMessagesPlaying)
	{
		if (!pObj->m_pMessageChannel)
		{
			// bisher noch keine Nachricht abgespielt, erste starten
			Sleep(pObj->m_dwFirstMillis);
			pObj->PlayNextMessage();
		}
		else
		{
			// prüfen, ob letzte Nachricht beendet ist
			bool bIsPlaying = false;
			pObj->m_pMessageChannel->isPlaying(&bIsPlaying);

			if (!bIsPlaying)
			{
				// abbrechen, wenn Abspielen abgebrochen werden soll oder
				// keine Nachricht mehr vorhanden ist
				if (pObj->m_bInterruptMessages || pObj->m_lMessages.empty()) break;

				// Pause, nächste Nachricht abspielen
				Sleep(pObj->m_dwMillis);
				pObj->PlayNextMessage();
			}
		}

		Sleep(50);
	}

	pObj->m_bInterruptMessages = false;
	pObj->m_bMessagesPlaying = false;

	// Event signaled setzen
	VERIFY(SetEvent(pObj->m_hThreadStopped));

	return 0;
}

BOOL CSoundManager::PlayNextMessage()
{
	// nichts mehr tun, wenn alle Nachrichten abgespielt wurden
	if (m_lMessages.empty()) return TRUE;

	// erste Nachricht aus der Liste entfernen
	SNDMGR_MESSAGELIST::iterator it = m_lMessages.begin();
	SNDMGR_MESSAGEENTRY entry = *it;
	m_lMessages.erase(it);

	// laden
	// TODO Fehler melden
	FMOD_MODE mode = FMOD_DEFAULT | FMOD_LOWMEM;
	if (m_bUseSoftwareSound) mode |= FMOD_SOFTWARE;

	std::string pathName = GetPathName(entry.nMessage, entry.nRace);
	if (pathName.empty()) return TRUE;

	if (m_pLastMessage) m_pLastMessage->release();
	m_nLastResult = m_pSystem->createSound(pathName.c_str(), mode, NULL, &m_pLastMessage);
	if (m_nLastResult != FMOD_OK || !m_pLastMessage) return FALSE;

	// Abspielen vorbereiten
	m_nLastResult = m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_pLastMessage, true, &m_pMessageChannel);
	if (m_nLastResult != FMOD_OK || !m_pMessageChannel) return FALSE;

	// Lautstärke setzen
	m_pMessageChannel->setVolume(entry.fVolume * m_fMessageMasterVolume);

	// Abspielen starten
	m_nLastResult = m_pMessageChannel->setPaused(false);
	if (m_nLastResult != FMOD_OK) return FALSE;

	return TRUE;
}
