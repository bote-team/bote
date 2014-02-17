#include "stdafx.h"
#include "RaceController.h"
#include "Major.h"
#include "Minor.h"
#include "Alien.h"
#include "AI\MinorAI.h"
#include "IOData.h"
#include "BoteDoc.h"

IMPLEMENT_SERIAL (CRaceController, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CRaceController::CRaceController(void)
{
}

CRaceController::~CRaceController(void)
{
	//we need to manually call the reset of the shared CRace ptr in the CDiplomacyAI object which is
	//member to the object of which it holds that shared ptr. Use count would never reach 1 to destruct
	//races, as the inner shared ptr would only be resetted by CRace's destructor which isn't called
	//due to use count not being 1 (circular causality trap)
	for(const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		it->second->ResetDiplomacyAI();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CRaceController::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		// Rassen speichern
		ar << m_mRaces.size();
		for (const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		{
			ar << it->first;
			it->second->Serialize(ar);
		}
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		unsigned size = 0;
		ar >> size;
		const_iterator it = m_mRaces.begin();
		for(unsigned i = 0; i < size; ++i)
		{
			AssertBotE(it != m_mRaces.end());
			CString id;
			ar >> id;
			while(it->first != id)
			{
				it = RemoveRaceInternal(it);
				AssertBotE(it != m_mRaces.end());
			}
			it->second->Serialize(ar);
			AssertBotE(!it->second->Deleted());
			++it;
		}
		while(it != m_mRaces.end())
			it = RemoveRaceInternal(it);
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Einlesen und Initialisieren aller am Spiel beteiligten Rassen
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::Init(int nSource/* = RACESOURCE_DATAFILE*/)
{
	AssertBotE(m_mRaces.empty());

	if ((InitMajors(nSource) & InitMinors(nSource) & InitAlienEntities(nSource)) == false)
		return false;

	// Startbeziehungen initialisieren
	InitRelations();

	return true;
}

void CRaceController::ReInit()
{
	const_iterator it = m_mRaces.begin();
	while(!m_mRaces.empty())
		it = RemoveRaceInternal(it);
	AssertBotE(m_mMajors.empty());
	AssertBotE(m_mMinors.empty());
	Init();
}

/// Funktion gibt eine bestimmte Rasse zurück.
/// @param sID ID der gewünschten Rassen
/// @return Zeiger auf gewünschte Rasse
const CRace* CRaceController::GetRace(const CString& sID) const
{
	const const_iterator it = m_mRaces.find(sID);

	if (it != m_mRaces.end())
		return it->second.get();
	else
		return NULL;
}
CRace* CRaceController::GetRace(const CString& sID)
{
	const const_iterator it = m_mRaces.find(sID);

	if (it != m_mRaces.end())
		return it->second.get();
	else
		return NULL;
}

const boost::shared_ptr<const CRace> CRaceController::GetRaceSafe(const CString& sID) const
{
	const const_iterator it = m_mRaces.find(sID);
	if(it == end())
		return boost::shared_ptr<const CRace>();
	return it->second;
}

boost::shared_ptr<CRace> CRaceController::GetRaceSafe(const CString& sID)
{
	iterator it = m_mRaces.find(sID);
	if(it == end())
		return boost::shared_ptr<CRace>();
	return it->second;
}

const boost::shared_ptr<const CMajor> CRaceController::GetMajorSafe(const CString& sID) const
{
	return boost::dynamic_pointer_cast<const CMajor>(GetRaceSafe(sID));
}

boost::shared_ptr<CMajor> CRaceController::GetMajorSafe(const CString& sID)
{
	return boost::dynamic_pointer_cast<CMajor>(GetRaceSafe(sID));
}

/// Funktion liefert die Minorrace, welche in einem bestimmten Sektor beheimatet ist.
/// @param sMinorsHome Heimatsystem
/// @return Zeiger auf Minorrace (<code>NULL</code> wenn die Rasse nicht gefunden werden konnte)
boost::shared_ptr<CMinor> CRaceController::GetMinorRace(const CString& sMinorsHome) const
{
	for (const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		if (it->second->IsMinor() && it->second->GetHomesystemName() == sMinorsHome)
			return boost::dynamic_pointer_cast<CMinor>(it->second);

	return boost::shared_ptr<CMinor>();
}

/// Funktion entfernt eine nicht mehr zu benutzende Rasse.
/// @param sRaceID Rassen-ID
void CRaceController::RemoveRace(const CString& sRaceID)
{
	std::map<CString, RacePtr>::const_iterator race = m_mRaces.find(sRaceID);
	RemoveRaceInternal(race);
}

CRaceController::const_iterator CRaceController::RemoveRaceInternal(const const_iterator& it)
{
	AssertBotE(it != m_mRaces.end() && !it->second->Deleted());
	it->second->IsMajor() ? m_mMajors.erase(it->second->GetRaceID()) : m_mMinors.erase(it->second->GetRaceID());
	it->second->Delete();
	const CPoint& co = it->second->GetCo();
	if(co != CPoint(-1, -1))
		resources::pDoc->GetSystem(co.x, co.y).SetHomeOf("");
	it->second->ResetDiplomacyAI();
	return m_mRaces.erase(it);
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion zum Einlesen und Initialisieren der beteiligten Majors
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitMajors(int nSource/* = RACESOURCE_DATAFILE*/)
{
	if (nSource == RACESOURCE_DATAFILE)
	{
		// zuerst die Majors anlegen
		int nInfoCount = 29;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu öffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorRaces.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;

		// Datei öffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile müssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"MajorRaces.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// prüfen ob vollständige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nMajorRaces.data is corrupted");
			return false;
		}
		// prüfen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file MajorRaces.data");
			return false;
		}

		// Majorrace nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			boost::shared_ptr<CRace> pNewMajor(new CMajor());
			pNewMajor->Create(saInfo, nPos);
			// überprüfen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file MajorRaces.data!\nCould not create correct CMajor object!");
				return false;
			}
			// Rasse der Map hinzufügen
			if (m_mRaces.find(pNewMajor->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file MajorRaces.data!\nThe race-ID %s exists more then one time!", pNewMajor->GetRaceID());
				AfxMessageBox(s);
			}
			else
			{
				m_mRaces[pNewMajor->GetRaceID()] = pNewMajor;
				m_mMajors[pNewMajor->GetRaceID()] = dynamic_cast<CMajor*>(pNewMajor.get());
			}
		}
	}
	return true;
}

/// Funktion zum Einlesen und Initialisieren der beteiligten Minors
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitMinors(int nSource/* = RACESOURCE_DATAFILE*/)
{
	// Minorrace nun anlegen
	if (nSource == RACESOURCE_DATAFILE)
	{
		// danach die Minors einlesen
		int nInfoCount = 14;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu öffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MinorRaces.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;
		//saInfo.SetSize(nInfoCount);

		// Datei öffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile müssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"MinorRaces.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// prüfen ob vollständige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nMinorRaces.data is corrupted");
			return false;
		}
		// prüfen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file MinorRaces.data");
			return false;
		}

		// Minorraces nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			boost::shared_ptr<CRace> pNewMinor(new CMinor());
			pNewMinor->Create(saInfo, nPos);
			// überprüfen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file MinorRaces.data!\nCould not create correct CMinor object!");
				return false;
			}
			// Rasse der Map hinzufügen
			if (m_mRaces.find(pNewMinor->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file MinorRaces.data!\nThe race-ID %s exists more then one time!", pNewMinor->GetRaceID());
				AfxMessageBox(s);
			}
			else
			{
				m_mRaces[pNewMinor->GetRaceID()] = pNewMinor;
				m_mMinors[pNewMinor->GetRaceID()] = dynamic_cast<CMinor*>(pNewMinor.get());
			}
		}
	}
	return true;
}

/// Funktion zum Einlesen und Initialisieren der Aliens (Weltraummonster)
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitAlienEntities(int nSource/* = RACESOURCE_DATAFILE*/)
{
	// Aliens nun anlegen
	if (nSource == RACESOURCE_DATAFILE)
	{
		// danach die Minors einlesen
		int nInfoCount = 9;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu öffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\AlienEntities.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;

		// Datei öffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile müssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"AlienEntities.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// prüfen ob vollständige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nAlienEntities.data is corrupted");
			return false;
		}
		// prüfen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file AlienEntities.data");
			return false;
		}

		// Minorraces nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			boost::shared_ptr<CRace> pAlien(new CAlien());
			pAlien->Create(saInfo, nPos);
			// überprüfen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file AlienEntities.data!\nCould not create correct CMinor object!");
				return false;
			}
			// Rasse der Map hinzufügen
			if (m_mRaces.find(pAlien->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file AlienEntities.data!\nThe race-ID %s exists more then one time!", pAlien->GetRaceID());
				AfxMessageBox(s);
			}
			else
			{
				m_mRaces[pAlien->GetRaceID()] = pAlien;
				m_mMinors[pAlien->GetRaceID()] = dynamic_cast<CMinor*>(pAlien.get());
			}
		}
	}

	return true;
}

/// Funktion erstellt die Beziehungen der Rassen untereinander. Dabei werden jedoch nur die
/// Majorbeziehungen in beide Richtungen vergeben. Bei den Minors wird immer nur die Beziehung
/// von Minor zu Majors erstellt.
void CRaceController::InitRelations(void)
{
	for (const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		for (const_iterator jt = m_mRaces.begin(); jt != m_mRaces.end(); ++jt)
			// handelt es sich nicht um die Rasse selbst und nicht um die Bezeihung Major -> Minor
			if (it->first != jt->first && jt->second->IsMajor())
			{
				// Grundbeziehung wird mit 50 initialisiert
				int nBase = 50;
				// Beziehungsbonus wird durch die Rassenart der anderen Rasse bestimmt
				int nBoni = 0;

				// und dann mit der Rasseneigenschaft angepasst
				if (it->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
				{
					nBase += 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
				{
					nBase -= 20;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 50;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
				{
					nBase += 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 30;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
				{
					nBase += 5;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SECRET))
				{
					nBase -= 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
				{
					nBase += 5;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
				{
					nBase += 0;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				{
					nBase += 25;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 55;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 30;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 100;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
				{
					nBase -= 20;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
				{
					nBase -= 25;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				{
					nBase -= 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 10;
				}

				int nRel = nBase + nBoni;

				if (nRel > 101)
					nRel = 101;
				else if (nRel < 1)
					nRel = 1;

				// 7 mal durchlaufen und einen Mittelwert bilden
				int nValue = 0;
				for (int i = 0; i < 7; i++)
					nValue += (rand()%nRel) * 2;
				nValue /= 7;

				it->second->SetRelation(jt->first, nValue);
			}
}
