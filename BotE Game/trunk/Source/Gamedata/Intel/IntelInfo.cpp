#include "stdafx.h"
#include "IntelInfo.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

#include "EcoIntelObj.h"
#include "DiplomacyIntelObj.h"
#include "ScienceIntelObj.h"
#include "MilitaryIntelObj.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelInfo::CIntelInfo(void)
{
	Reset();
}

CIntelInfo::~CIntelInfo(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion ermittelt alle Geheimdienstinformationen für den Besitzer dieses Objektes.
void CIntelInfo::CalcIntelInfo(CBotf2Doc* pDoc, CMajor* pOurRace)
{
	// Nur berechnen, wenn noch nicht zuvor berechnet wurde
	if (m_bCalculated)
		return;

	ASSERT(pDoc);
	ASSERT(pOurRace);
	
	Reset();

	// 3-dim Feld, welches die Informationen zu den Sektoren/Systemen aufnimmt
	// der Wert gibt an, ob es sich um kontrollierte Sektoren == 1, besitzte System == 2
	// oder um bewohnte System == 3 handelt
	map<CString,BYTE> sectors[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	// 3-dim Feld, welches Informationen zu bekannten und vermitgliederten Minors aufnimmt.
	// der Wert gibt an, ob es eine bekannte Minorrace == 1 oder eine vermitgliederte Minorrace == 2 ist.
	map<CString, BYTE> races[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];


	// jetzt Sektoren aus allen möglichen Geheimdienstberichten ermitteln
	for (int i = 0; i < pOurRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
	{
		CIntelObject* intelObj = pOurRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(i);
		if (intelObj->GetEnemy() != pOurRace->GetRaceID())
		{
			CPoint ko(-1,-1);
			// Sektoren und Systeme ermitteln
			switch (intelObj->GetType())
			{
			case 0:	ko = ((CEcoIntelObj*)intelObj)->GetKO(); break;
			case 1:	ko = ((CScienceIntelObj*)intelObj)->GetKO(); break;
			case 2:	if (((CMilitaryIntelObj*)intelObj)->GetIsBuilding())
						ko = ((CMilitaryIntelObj*)intelObj)->GetKO(); break;
			}
			if (ko != CPoint(-1,-1))
			{
				// Nun noch überprüfen, ob diese Informationen auch wirklich noch zutreffen. Veraltete Geheimdienstberichte
				// können falsche Informationen beinhalten.
				if (pDoc->m_Sector[ko.x][ko.y].GetOwnerOfSector() == intelObj->GetEnemy())
				{
					sectors[ko.x][ko.y][intelObj->GetEnemy()] = 1;
					if (pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem() == intelObj->GetEnemy())
					{
						sectors[ko.x][ko.y][intelObj->GetEnemy()] = 2;
						if (pDoc->m_Sector[ko.x][ko.y].GetCurrentHabitants() > 0.0f)
							sectors[ko.x][ko.y][intelObj->GetEnemy()] = 3;
					}
				}
			}
			// Minors überprüfen
			if (intelObj->GetType() == 3)		// also Diplomatieintelobjekt
			{
				ko = ((CDiplomacyIntelObj*)intelObj)->GetMinorRaceKO();
				if (ko != CPoint(-1,-1))
					if (races[ko.x][ko.y][intelObj->GetEnemy()] < 2)
					{
						// überprüfen, ob die Minorrace noch vorhanden ist
						CMinor* pMinor = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->m_Sector[ko.x][ko.y].GetName());
						if (pMinor != NULL)
						{
							if (((CDiplomacyIntelObj*)intelObj)->GetAgreement() == MEMBERSHIP)
								races[ko.x][ko.y][intelObj->GetEnemy()] = 2;
							else
								races[ko.x][ko.y][intelObj->GetEnemy()] = 1;
						}
					}
			}
		}
	}

	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// Minorracearray durchlaufen und bekannte bzw. vermitgliederte Minors einer anderen Rasse raussuchen
	map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); it++)
	{
		CMinor* pMinor = it->second;
		// Wenn wir die Minorrace kennen und mit ihr mindst. eine Freundschaft haben, so können wir bestimmen,
		// ob eine andere Rasse diese Minorrace ebenfalls kennt.
		// Wenn wir den Heimatsektor der Minorrace kennen und die von einer uns bekannten Majorrace vermitgliedert wurde,
		// so wissen wir auch etwas von der Mitgliedschaft.
		CPoint ko = pMinor->GetRaceKO();
		if (pMinor->GetAgreement(pOurRace->GetRaceID()) >= FRIENDSHIP_AGREEMENT)
		{
			for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); jt++)
				if (jt->first != pOurRace->GetRaceID())
					if (pMinor->IsRaceContacted(jt->first))
						if (races[ko.x][ko.y][jt->first] < 1)
							races[ko.x][ko.y][jt->first] = 1;
		}
		// kann nur eintreten, wenn wir zu dieser Minorrace weniger als eine Freundschaft haben. Denn nur dann könnte
		// die andere Rasse eine Mitgliedschaft zu der Minorrace haben.
		else if (pDoc->m_Sector[ko.x][ko.y].GetKnown(pOurRace->GetRaceID()))
		{
			for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); jt++)
				if (jt->first != pOurRace->GetRaceID())
					if (jt->second->IsRaceContacted(pOurRace->GetRaceID()))
						if (pMinor->GetAgreement(jt->first) == MEMBERSHIP)
							races[ko.x][ko.y][jt->first] = 2;
		}
		// eigene Beziehung zu den Minorraces ermitteln
		if (pMinor->IsRaceContacted(pOurRace->GetRaceID()))
			races[ko.x][ko.y][pOurRace->GetRaceID()] = 1;
		if (pMinor->GetAgreement(pOurRace->GetRaceID()) == MEMBERSHIP)
			races[ko.x][ko.y][pOurRace->GetRaceID()] = 2;
	}

	// kontrollierte Sektoren, eigene und bewohnte Systeme ermitteln
	// zuerst durch das, was man auf der Karte als markiertes Gebiet sieht
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			{
				if (it->second->IsRaceContacted(pOurRace->GetRaceID()))
					if (pDoc->m_Sector[x][y].GetScanned(pOurRace->GetRaceID()))
						if (pDoc->m_Sector[x][y].GetOwnerOfSector() == it->first)
						{
							if (sectors[x][y][it->first] < 1)
								sectors[x][y][it->first] = 1;
							if (pDoc->m_Sector[x][y].GetSunSystem())
							{
								if (sectors[x][y][it->first] < 2)
									sectors[x][y][it->first] = 2;
								if (pDoc->m_Sector[x][y].GetKnown(pOurRace->GetRaceID()) && pDoc->m_System[x][y].GetHabitants() > 0.0f)
									if (sectors[x][y][it->first] < 3)
										sectors[x][y][it->first] = 3;
							}
						}
				// Nun die Informationen für das Geheimdienstinfoobjekt zusammentragen
				if (sectors[x][y][it->first] > NULL)
				{
					m_nControlledSectors[it->first]++;
					if (sectors[x][y][it->first] == 3)
					{
						m_nInhabitedSystems[it->first]++;
						m_nOwnedSystems[it->first]++;
					}
					else if (sectors[x][y][it->first] == 2)
						m_nOwnedSystems[it->first]++;
				}
				// Jetzt können wir auch die Informationen für die Minorrace zusammenstellen
				if (races[x][y][it->first] > NULL)
				{
					m_nKnownMinors[it->first]++;
					if (races[x][y][it->first] == 2)
						m_nMinorMembers[it->first]++;
				}
			}

	m_bCalculated = TRUE;
}

// Resetfunktion für das CIntelInfo Objekt.
void CIntelInfo::Reset()
{
	m_nControlledSectors.clear();
	m_nOwnedSystems.clear();
	m_nInhabitedSystems.clear();
	m_nKnownMinors.clear();
	m_nMinorMembers.clear();
	m_bCalculated = FALSE;
}