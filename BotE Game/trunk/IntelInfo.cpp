#include "stdafx.h"
#include "IntelInfo.h"
#include "Botf2Doc.h"

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
void CIntelInfo::CalcIntelInfo(CBotf2Doc* pDoc, BYTE ourRace)
{
	// Nur berechnen, wenn noch nicht zuvor berechnet wurde
	if (m_bCalculated)
		return;

	ASSERT(pDoc);
	ASSERT(ourRace);

	Reset();

	// 3-dim Feld, welches die Informationen zu den Sektoren/Systemen aufnimmt
	// der Wert gibt an, ob es sich um kontrollierte Sektoren == 1, besitzte System == 2
	// oder um bewohnte System == 3 handelt
	BYTE sectors[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT][DOMINION] = {0};

	// 3-dim Feld, welches Informationen zu bekannten und vermitgliederten Minors aufnimmt.
	// der Wert gibt an, ob es eine bekannte Minorrace == 1 oder eine vermitgliederte Minorrace == 2 ist.
	BYTE races[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT][DOMINION] = {0};


	// jetzt Sektoren aus allen möglichen Geheimdienstberichten ermitteln
	for (int i = 0; i < pDoc->GetEmpire(ourRace)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
	{
		CIntelObject* intelObj = pDoc->GetEmpire(ourRace)->GetIntelligence()->GetIntelReports()->GetReport(i);
		if (intelObj->GetEnemy() != ourRace)
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
					sectors[ko.x][ko.y][intelObj->GetEnemy()-1] = 1;
					if (pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem() == intelObj->GetEnemy())
					{
						sectors[ko.x][ko.y][intelObj->GetEnemy()-1] = 2;
						if (pDoc->m_Sector[ko.x][ko.y].GetCurrentHabitants() > 0.0f)
							sectors[ko.x][ko.y][intelObj->GetEnemy()-1] = 3;
					}
				}
			}
			// Minors überprüfen
			if (intelObj->GetType() == 3)		// also Diplomatieintelobjekt
			{
				ko = ((CDiplomacyIntelObj*)intelObj)->GetMinorRaceKO();
				if (ko != CPoint(-1,-1))
					if (races[ko.x][ko.y][intelObj->GetEnemy()-1] < 2)
					{
						// überprüfen, ob die Minorrace noch vorhanden ist
						CMinorRace* minor = pDoc->GetMinorRace(pDoc->m_Sector[ko.x][ko.y].GetName());
						if (minor != NULL)
						{
							if (((CDiplomacyIntelObj*)intelObj)->GetAgreement() == MEMBERSHIP)
								races[ko.x][ko.y][intelObj->GetEnemy()-1] = 2;
							else
								races[ko.x][ko.y][intelObj->GetEnemy()-1] = 1;
						}
					}
			}
		}
	}

	// Minorracearray durchlaufen und bekannte bzw. vermitgliederte Minors einer anderen Rasse raussuchen
	for (int i = 0; i < pDoc->GetMinorRaceArray()->GetSize(); i++)
	{
		// Wenn wir die Minorrace kennen und mit ihr mindst. eine Freundschaft haben, so können wir bestimmen,
		// ob eine andere Rasse diese Minorrace ebenfalls kennt.
		// Wenn wir den Heimatsektor der Minorrace kennen und die von einer uns bekannten Majorrace vermitgliedert wurde,
		// so wissen wir auch etwas von der Mitgliedschaft.
		CPoint ko = pDoc->GetMinorRaceArray()->GetAt(i).GetRaceKO();
		if (pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(ourRace) >= FRIENDSHIP_AGREEMENT)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
				if (j != ourRace)
					if (pDoc->GetMinorRaceArray()->GetAt(i).GetKnownByMajorRace(j))
						if (races[ko.x][ko.y][j-1] < 1)
							races[ko.x][ko.y][j-1] = 1;
		}
		// kann nur eintreten, wenn wir zu dieser Minorrace weniger als eine Freundschaft haben. Denn nur dann könnte
		// die andere Rasse eine Mitgliedschaft zu der Minorrace haben.
		else if (pDoc->m_Sector[ko.x][ko.y].GetKnown(ourRace))
		{
			for (int j = HUMAN; j <= DOMINION; j++)
				if (j != ourRace)
					if (pDoc->GetMajorRace(j)->GetKnownMajorRace(ourRace))
						if (pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(j) == MEMBERSHIP)
							races[ko.x][ko.y][j-1] = 2;
		}
		// eigene Beziehung zu den Minorraces ermitteln
		if (pDoc->GetMinorRaceArray()->GetAt(i).GetKnownByMajorRace(ourRace))
			races[ko.x][ko.y][ourRace-1] = 1;
		if (pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(ourRace) == MEMBERSHIP)
			races[ko.x][ko.y][ourRace-1] = 2;
	}

	// kontrollierte Sektoren, eigene und bewohnte Systeme ermitteln
	// zuerst durch das, was man auf der Karte als markiertes Gebiet sieht
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			for (int i = HUMAN; i <= DOMINION; i++)
			{
				if (pDoc->GetMajorRace(i)->GetKnownMajorRace(ourRace))
					if (pDoc->m_Sector[x][y].GetScanned(ourRace))
						if (pDoc->m_Sector[x][y].GetOwnerOfSector() == i)
						{
							if (sectors[x][y][i-1] < 1)
								sectors[x][y][i-1] = 1;
							if (pDoc->m_Sector[x][y].GetSunSystem())
							{
								if (sectors[x][y][i-1] < 2)
									sectors[x][y][i-1] = 2;
								if (pDoc->m_Sector[x][y].GetKnown(ourRace) && pDoc->m_System[x][y].GetHabitants() > 0.0f)
									if (sectors[x][y][i-1] < 3)
										sectors[x][y][i-1] = 3;
							}
						}
				// Nun die Informationen für das Geheimdienstinfoobjekt zusammentragen
				if (sectors[x][y][i-1] > NULL)
				{
					m_nControlledSectors[i-1]++;
					if (sectors[x][y][i-1] == 3)
					{
						m_nInhabitedSystems[i-1]++;
						m_nOwnedSystems[i-1]++;
					}
					else if (sectors[x][y][i-1] == 2)
						m_nOwnedSystems[i-1]++;
				}
				// Jetzt können wir auch die Informationen für die Minorrace zusammenstellen
				if (races[x][y][i-1] > NULL)
				{
					m_nKnownMinors[i-1]++;
					if (races[x][y][i-1] == 2)
						m_nMinorMembers[i-1]++;
				}
			}

	m_bCalculated = TRUE;
}

// Resetfunktion für das CIntelInfo Objekt.
void CIntelInfo::Reset()
{
	memset(m_nControlledSectors, 0, sizeof(*m_nControlledSectors) * DOMINION);
	memset(m_nOwnedSystems, 0, sizeof(*m_nOwnedSystems) * DOMINION);
	memset(m_nInhabitedSystems, 0, sizeof(*m_nInhabitedSystems) * DOMINION);
	memset(m_nKnownMinors, 0, sizeof(*m_nKnownMinors) * DOMINION);
	memset(m_nMinorMembers, 0, sizeof(*m_nMinorMembers) * DOMINION);
	m_bCalculated = FALSE;
}