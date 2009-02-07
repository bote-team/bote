#include "stdafx.h"
#include "IntelCalc.h"
#include "Botf2Doc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelCalc::CIntelCalc(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;
}

CIntelCalc::~CIntelCalc(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion berechnet die kompletten Geheimdienstaktionen und nimmt gleichzeitig auch alle Veränderungen vor.
/// Die Funktion verschickt auch alle relevanten Nachrichten an die betroffenen Imperien.
void CIntelCalc::StartCalc(BYTE race)
{
	ASSERT(race);
	srand((unsigned)time(NULL));
	// resultierende Spionagegeheimdienstpunkte bei einer Rasse
	UINT m_iSpySP[7] = {0};
	// resultierende Sabotagegeheimdienstpunkte bei einer Rasse
	UINT m_iSabSP[7] = {0};
	
	// Ein möglicher Anschlag wird vor allen anderen Geheimdienstaktionen durchgeführt. Bei einem Anschlag werden
	// nur die angesammelten Geheimdienspunkte benutzt.
	CIntelObject* attemptObj = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAttemptObject();
	if (attemptObj)
	{
		int ourSP = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(1,attemptObj->GetEnemy());
		// + eventuellen Bonus aus dem Teilbereich dazurechnen
		ourSP += ourSP * m_pDoc->GetEmpire(race)->GetIntelligence()->GetBonus(attemptObj->GetType(), 1) / 100;
		this->ExecuteAttempt(race, ourSP);
	}

	// zuerst werden die Geheimdienstgegner ermittelt. Nur wenn bei einem Gegner mehr als NULL resultierende
	// Geheimdienstpunkte vorhanden sind ist es ein wirklicher Gegner.
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != race)
		{
			m_iSpySP[i] = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSecurityPoints() *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(i) / 100;		
			m_iSabSP[i] = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSecurityPoints() *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(i) / 100;
		}
	
	// dann können die Geheimdienstaktionen gestartet werden
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != race)
		{
			// sind effektive Geheimdienstpunkte für Spionage gegen Rasse i vorhanden oder es befinden sich Punkte im Depot?
			if (m_iSpySP[i] > NULL || m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(0,i) > NULL)
			{
				// die speziellen Bereiche durchgehen
				// Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
				int b = rand()%4;
				int count = 0;
				while (count < 4)
				{
					// Spionagepunkte * spezielle Spionagezuteilung / 100 + Spionagedepot * spezielle Spionagezuteilung / 100
					int effectiveSP = m_iSpySP[i] * m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSpyPercentages(i,b) / 100
						+ m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(0,i) * m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSpyPercentages(i,b) / 100;
					// + eventuellen Bonus aus dem Teilbereich dazurechnen
					effectiveSP += effectiveSP * m_pDoc->GetEmpire(race)->GetIntelligence()->GetBonus(b, 0) / 100;
					if (effectiveSP > NULL)
					{
						BYTE responsibleRace = race;
						USHORT actions = this->IsSuccess(i, effectiveSP, TRUE, responsibleRace, b);
						this->DeleteConsumedPoints(race, i, TRUE, b, FALSE);
						// jetzt Geheimdienstaktion starten!
						int stop = 0;	// irgendwann mal abbrechen, falls aus irgendeinem Grund actions mal nie runtergezählt werden kann
						while (actions > 0 && stop < 100)
						{
							if (this->ExecuteAction(race, i, responsibleRace, b, TRUE))
							{
								actions--;
								// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
								if (responsibleRace != NOBODY && responsibleRace != UNKNOWN)
									m_pDoc->GetMajorRace(i)->SetRelationshipToMajorRace(responsibleRace, -rand()%5);
							}
							stop++;
						}
					}
					if (b == 3)
						b = 0;
					else
						b++;
					count++;
				}
			}
			// sind effektive Geheimdienstpunkte für Sabotage gegen Rasse i vorhanden oder es befinden sich Punkte im Depot?
			if (m_iSabSP[i] > NULL || m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(1,i) > NULL)
			{
				// die speziellen Bereiche durchgehen
				// Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
				int b = rand()%4;
				int count = 0;
				while (count < 4)
				{
					// Spionagepunkte * spezielle Spionagezuteilung / 100 + Spionagedepot * spezielle Spionagezuteilung / 100
					int effectiveSP = m_iSabSP[i] * m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(i,b) / 100
						+ m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(1,i) * m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(i,b) / 100;
					// + eventuellen Bonus aus dem Teilbereich dazurechnen
					effectiveSP += effectiveSP * m_pDoc->GetEmpire(race)->GetIntelligence()->GetBonus(b, 1) / 100;
					if (effectiveSP > NULL)
					{
						BYTE responsibleRace = race;
						USHORT actions = this->IsSuccess(i, effectiveSP, FALSE, responsibleRace, b);
						this->DeleteConsumedPoints(race, i, FALSE, b, FALSE);
						// jetzt Geheimdienstaktion starten!
						int stop = 0;	// irgendwann mal abbrechen, falls aus irgendeinem Grund actions mal nie runtergezählt werden kann
						while (actions > 0 && stop < 100)
						{
							if (this->ExecuteAction(race, i, responsibleRace, b, FALSE))
							{
								actions--;
								// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
								if (responsibleRace != NOBODY && responsibleRace != UNKNOWN)
									m_pDoc->GetMajorRace(i)->SetRelationshipToMajorRace(responsibleRace, -rand()%10);
							}
							stop++;
						}
					}
					if (b == 3)
						b = 0;
					else
						b++;
					count++;
				}
			}
		}	
}

/// Funktion addiert Innere Sicherheitspunkte sowie die ganzen Depotgeheimdienstpunkte einer Rasse zu den vorhandenen.
void CIntelCalc::AddPoints(BYTE race)
{
	ASSERT(race);

	// Punkte der inneren Sicherheit hinzufügen
	int add = m_pDoc->GetEmpire(race)->GetIntelligence()->
		GetSecurityPoints() * m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage() / 100;
	add += add * m_pDoc->GetEmpire(race)->GetIntelligence()->GetInnerSecurityBoni() / 100;
	m_pDoc->GetEmpire(race)->GetIntelligence()->AddInnerSecurityPoints(add);

	// Punkte den einzelnen Depots hinzufügen (hier kein Bonus durch eventuelle Boni auf die einzelnen Ressorts)
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != race)
		{
			// Spionagedepot auffüllen
			UINT depot = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSecurityPoints() *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(i) *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSpyPercentages(i,4) / 10000;
			m_pDoc->GetEmpire(race)->GetIntelligence()->AddSPStoragePoints(0, i, depot);

			// Sabotagedepot auffüllen
			depot = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSecurityPoints() *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(i) *
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(i,4) / 10000;
			m_pDoc->GetEmpire(race)->GetIntelligence()->AddSPStoragePoints(1, i, depot);
		}
}

/// Funktion zieht einen rassenabhängigen Prozentsatz von den einzelnen Depots ab. Funkion sollte nach Ausführung
/// aller anderen Geheimdienstfunktionen aufgerufen werden.
void CIntelCalc::ReduceDepotPoints(BYTE race, int perc)
{
	ASSERT(race);

	// bei perc == -1 wird der rassenspezifische Prozentsatz verwendet
	if (perc == -1)
	{
		switch(m_pDoc->GetMajorRace(race)->GetKind())
		{
		case NOTHING_SPECIAL:	perc = 20; break;
		case FINANCIAL:			perc = 35; break;
		case WARLIKE:			perc = 25; break;
		case FARMER:			perc = 30; break;
		case INDUSTRIAL:		perc = 25; break;
		case SECRET:			perc = 10;  break;
		case RESEARCHER:		perc = 20; break;
		case PRODUCER:			perc = 20; break;
		case PACIFIST:			perc = 20; break;
		case SNEAKY:			perc = 15; break;
		default: perc = 20;
		}	
	}
	// Prozentsatz aus den einzelnen Depots abziehen
	// zuerst das Lager der angesammelten inneren Sicherheitspunkte
	int diff = m_pDoc->GetEmpire(race)->GetIntelligence()->GetInnerSecurityStorage() * perc / 100;
	m_pDoc->GetEmpire(race)->GetIntelligence()->AddInnerSecurityPoints(-diff);

	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != race)
		{
			// Wenn die Rasse kein Systeme mehr besitzt, also ausgelöscht wurde, so verschwinden alle
			// angesammelten Geheimdienstpunkte
			int oldPerc = perc;
			if (m_pDoc->GetEmpire(i)->GetNumberOfSystems() == NULL)
				perc = 100;
			// Depot der Spionagepunkte
			diff = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(0,i) * perc / 100;
			m_pDoc->GetEmpire(race)->GetIntelligence()->AddSPStoragePoints(0,i,-diff);
			// Depot der Sabotagepunkte
			diff = m_pDoc->GetEmpire(race)->GetIntelligence()->GetSPStorage(1,i) * perc / 100;
			m_pDoc->GetEmpire(race)->GetIntelligence()->AddSPStoragePoints(1,i,-diff);
			perc = oldPerc;
		}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion berechnet ob eine Geheimdienstaktion gegen eine andere Rasse erfolgreich verläuft.
USHORT CIntelCalc::IsSuccess(BYTE enemyRace, UINT ourSP, BOOLEAN isSpy, BYTE &responsibleRace, BYTE type)
{
#ifdef TRACE_INTEL
	TRACE("in FUNCTION IsSuccess()\n");
#endif
	ASSERT(enemyRace);
	ASSERT(responsibleRace);
	USHORT actions = NULL;

	UINT enemyInnerSec = GetCompleteInnerSecPoints(enemyRace);
		
#ifdef TRACE_INTEL
	TRACE("InnerSec von %d: %d\n", enemyRace, enemyInnerSec);
#endif

	// Aggressivität der angreifenden Rasse holen
	BYTE agg = m_pDoc->GetEmpire(responsibleRace)->GetIntelligence()->GetAggressiveness(!isSpy, enemyRace);
#ifdef TRACE_INTEL
	TRACE("Agg: %d\n", agg);
#endif
	agg = 3 - agg; // -> vorsichtig == 3, normal == 2, aggressiv == 1 -> drehen die Zahlen im Prinzip um

	// Spionage hat größere Erfolgsaussichten als Sabotage
	int minDiff = 0;
	if (isSpy)
	{
		for (int i = 0; i < 3; i++)
			minDiff += rand()%(500 * agg) + 1;
		minDiff /= 3;
	}
	else
	{
		for (int i = 0; i < 3; i++)
			minDiff += rand()%(1000 * agg) + 1;
		minDiff /= 3;
	}

#ifdef TRACE_INTEL
	TRACE("OurSP von %d: %d > %d + %d\n", responsibleRace, ourSP, enemyInnerSec, minDiff);
#endif
	if (ourSP > (enemyInnerSec + minDiff))
	{	// wenn wir viel mehr Punkte als der Gegner haben, so können auch mehrere Geheimdienstaktionen gestarten werden.
		actions = ourSP / (enemyInnerSec + minDiff);
		// wenn unsere Geheimdienstpunkte doppelt so hoch sind wie die inneren Sicherheitspunkte, dann verschleiern wir
		// unsere Geheimdienstaktion
		if (isSpy == FALSE && ourSP > (2 * enemyInnerSec))
		{
			// Wenn sogar unsere Punkte dreimal so hoch sind, dann machen wir womöglich eine andere Rasse dafür verantworlich
			if (ourSP > (3 * enemyInnerSec))
			{
				responsibleRace = m_pDoc->GetEmpire(responsibleRace)->GetIntelligence()->GetResponsibleRace();
				if (responsibleRace != NOBODY)
				{
					if (responsibleRace == enemyRace)
						responsibleRace = NOBODY;
					// checken ob unser Geheimdienstopfer die "responisibleRace" auch kennt
					else if (m_pDoc->GetMajorRace(enemyRace)->GetKnownMajorRace(responsibleRace) == FALSE)
						responsibleRace = NOBODY;
				}
			}
			else
				responsibleRace = NOBODY;
		}
	}
	// Handelt es sich um eine fehlgeschlagene Aktion, so ist es möglich, dass das vermeindliche Geheimdienstopfer
	// etwas davon erfährt.
	else if ((ourSP * agg) < (enemyInnerSec + minDiff) && rand()%2 == NULL)
		this->CreateMsg(responsibleRace, enemyRace, type);
#ifdef TRACE_INTEL
	TRACE("actions: %d\n", actions);
#endif
	return actions;
}

/// Funktion entfernt die durch eine Geheimdienstaktion verbrauchten Punkte auf Seiten des Geheimdienstopfers und
/// auf Seiten des Geheimdienstagressors.
void CIntelCalc::DeleteConsumedPoints(BYTE ourRace, BYTE enemyRace, BOOLEAN isSpy, BYTE type, BOOLEAN isAttempt)
{
#ifdef TRACE_INTEL
	TRACE("in FUNCTION DeleteConsumedPoints()\n");
#endif
	// Punkte der gesammten Inneren Sicherheit des Opfers holen
	int enemyInnerSecPoints = m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetSecurityPoints() *
		m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage() / 100;
	// + Bonus auf innere Sicherheit
	enemyInnerSecPoints += enemyInnerSecPoints * m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetInnerSecurityBoni() / 100;
	// beim Depot sind die Boni schon mit eingerechnet
	int enemyInnerSecDepot = m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetInnerSecurityStorage();
#ifdef TRACE_INTEL
	TRACE("enemyInnerSecPoints: %d - enemyInnerSecDepot: %d\n", enemyInnerSecPoints, enemyInnerSecDepot);
#endif

	// nun die generierten Punkte des Agressors bestimmen
	int racePoints = 0;
	int raceDepot = 0;
	if (isSpy && !isAttempt)
	{
		racePoints = m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetSecurityPoints() * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(enemyRace)
			* m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetSpyPercentages(enemyRace, type) / 10000;
		racePoints += racePoints * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetBonus(type, 0) / 100;
			
		raceDepot = m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetSPStorage(0, enemyRace) * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetSpyPercentages(enemyRace, type) / 100;
		raceDepot += raceDepot * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetBonus(type, 0) / 100;
	}
	else if (!isSpy && !isAttempt)
	{
		racePoints = m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetSecurityPoints() * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(enemyRace)
			* m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(enemyRace, type) / 10000;
		racePoints += racePoints * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetBonus(type, 1) / 100;
			
		raceDepot = m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetSPStorage(1, enemyRace) * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(enemyRace, type) / 100;
		raceDepot += raceDepot * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetBonus(type, 1) / 100;
	}
	else if (isAttempt)
	{
		raceDepot = m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetSPStorage(1, enemyRace) * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(enemyRace, type) / 100;
		raceDepot += raceDepot * m_pDoc->GetEmpire(ourRace)->GetIntelligence()->GetBonus(type, 1) / 100;
	}

#ifdef TRACE_INTEL
	TRACE("racePoints: %d - raceDepot: %d\n", racePoints, raceDepot);
#endif
	// jetzt gegenseitig die Punkte abziehen.
	// zuerst werden immer die Punkte abgezogen, welche nicht aus den Depots kommen
	int temp = racePoints + raceDepot - enemyInnerSecPoints;
	// in temp stehen jetzt nur noch die
	if (temp > 0)
		m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->AddInnerSecurityPoints(-temp);

	temp = enemyInnerSecPoints + enemyInnerSecDepot - racePoints;
	if (temp > 0)
		m_pDoc->GetEmpire(ourRace)->GetIntelligence()->AddSPStoragePoints(!isSpy, enemyRace, -temp);
}

/// Funktion ruft die jeweilige Unterfunktion auf, welche eine Geheimdienstaktion schlussendlich ausführt.
BOOLEAN CIntelCalc::ExecuteAction(BYTE race, BYTE enemyRace, BYTE responsibleRace, BYTE type, BOOLEAN isSpy)
{
	if (isSpy)
	{
		switch (type)
		{
		case 0: return this->ExecuteEconomySpy(race, enemyRace, responsibleRace, TRUE);
		case 1: return this->ExecuteScienceSpy(race, enemyRace, responsibleRace, TRUE);
		case 2: return this->ExecuteMilitarySpy(race, enemyRace, responsibleRace, TRUE);
		case 3: return this->ExecuteDiplomacySpy(race, enemyRace, responsibleRace, TRUE);
		}
	}
	else
	{
		switch (type)
		{
		case 0: return this->ExecuteEconomySabotage(race, enemyRace, responsibleRace);
		case 1: return this->ExecuteScienceSabotage(race, enemyRace, responsibleRace);
		case 2: return this->ExecuteMilitarySabotage(race, enemyRace, responsibleRace);
		case 3: return this->ExecuteDiplomacySabotage(race, enemyRace, responsibleRace);
		}
	}
	return FALSE;
}

/// Funktion führt eine Wirtschatfsspionageaktion aus.
BOOLEAN CIntelCalc::ExecuteEconomySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt zwei verschiedene Arten der Wirtschaftsspionage. Zuerst kann das besitzte Latinum ausspioniert
	werden. Zweitens werden Gebäude aus irgendeinem System ausspioniert.
	*/
	// 1. Versuch: Latinum ausspionieren
	if (rand()%8 == NULL)
	{
		int latinum = (int)m_pDoc->GetEmpire(enemyRace)->GetLatinum();
		CEcoIntelObj* report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, latinum);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 1, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
			return FALSE;
		}
	}

	// 2. Versuch: Gebäude ausspionieren
	// Die Spionage ist komplett zufällig. Man kann nicht beeinflussen in welchem System spioniert wird.
	CArray<CPoint> sectors;
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_pDoc->m_System[x][y].GetOwnerOfSystem() == enemyRace)
				sectors.Add(CPoint(x,y));
	if (sectors.GetSize())
	{
		// in random steht ein zufällig ermittelter Sektor, in welchem spioniert werden kann.
		int random = rand()%sectors.GetSize();
		// bei der Wirtschaftsspionage kann man über alle Gebäude, außer Intelzentren und Laboratorien etwas erfahren.
		// Welches ausgewählt wird ist zufällig. Dabei werden mit etwas höherer Wahrscheinlichkeit Gebäude ausspioniert,
		// welche Arbeiter benötigen. Doch auch arbeiterfreie Gebäude, wie z.B. eine Meeresfarm kann ermittelt werden.
		if (rand()%4 == NULL)	// zu 25% wird versucht ein arbeiterfreies Gebäude zu spionieren
		{
			int number = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetSize();
			int j = 0;
			if (number > NULL)
			{
				// zufällig ein Gebäude aussuchen
				for (int i = rand()%number; i < number; i++)
				{
					// wenn das Gebäude keine Arbeiter benötigt
					if (m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i).GetWorker() == FALSE)
					{
						const CBuilding* b = &m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i);
						// sobald das Gebäude eine nicht wirtschaftsbezogene Leistung erbringen kann, kann es nicht spioniert werden
						if (!b->GetBarrack() && !b->GetFPProd() && !b->GetGroundDefend() && !b->GetGroundDefendBoni()
							&& !b->GetMilitarySabotageBoni() && !b->GetMilitarySpyBoni() && !b->GetResearchBoni() && !b->GetResearchSabotageBoni()
							&& !b->GetResearchSpyBoni() && !b->GetResistance() && !b->GetScanPower() && !b->GetScanPowerBoni()
							&& !b->GetScanRange() && !b->GetScanRangeBoni() && !b->GetSecurityBoni() && !b->GetShieldPower()
							&& !b->GetShieldPowerBoni() && !b->GetShipDefend() && !b->GetShipDefendBoni() && !b->GetShipTraining()
							&& !b->GetShipYard() && !b->GetSPProd() && !b->GetTroopTraining())
						{
							int id = b->GetRunningNumber();
							int n = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfBuilding(id);
							CEcoIntelObj* report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n);
							// Intelreport dem Akteur hinzufügen
							if (report)
							{
								if (createText)
									report->CreateText(m_pDoc, 0, responsibleRace);
								m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
								return TRUE;
							}
							else
							{
								delete report;
								report = NULL;
								break;
							}
						}
					}
					if (i == number-1)
						i = 0;
					j++;
					if (j == number)
						break;
				}
			}
		}
		// ansonsten werden Gebäude welche Arbeiter benötigen ausspioniert
		int buildingTypes[IRIDIUM_WORKER+1] = {0};
		int start = rand()%(IRIDIUM_WORKER+1);
		int j = 0;
		for (int i = start; i <= IRIDIUM_WORKER; i++)
		{
			if (i != SECURITY_WORKER && i != RESEARCH_WORKER)
			{
				buildingTypes[i] = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(i, 0);
				if (buildingTypes[i] > NULL)
				{
					start = i;
					break;
				}
			}
			if (i == IRIDIUM_WORKER)
				i = FOOD_WORKER;
			j++;
			if (j == IRIDIUM_WORKER)
				break;
		}
		// in "start" steht möglicherweise das Gebäude zur Spionage
		if (buildingTypes[start] > 0)
		{
			CEcoIntelObj* report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
				m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(start, 1), (BYTE)buildingTypes[start]);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 0, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
			}
		}		
	}
	return FALSE;
}

/// Funktion führt eine Forschungsspionageaktion aus.
BOOLEAN CIntelCalc::ExecuteScienceSpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Forschungsspionage. Zuerst können Forschungsgebäude aus irgendeinem System
	ausspioniert werden. Zweitens können die global fabrizierten Forschungspunkte ausspioniert werden. Drittens können
	einzelne Techlevel in Erfahrung gebracht werden und viertens wird etwas über die Spezialforschung spioniert.
	*/

	// 4. Versuch: etwas über die Spezialforschung herausbekommen
	if (rand()%8 == NULL)
	{
		short specialTech = -1;
		short choosen = -1;
		// zufällig mit einem Komplex starten und schauen ob dieser erforsch ist
		int t = rand()%NoUC;
		int j = 0;
		for (int i = t; i < NoUC; i++)
		{
			if (m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetComplexStatus() == RESEARCHED)
			{
				specialTech = i;
				for (int j = 1; j <= 3; j++)
					if (m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetFieldStatus(j) == RESEARCHED)
					{
						choosen = j;
						break;
					}
				break;
			}
			if (i == NoUC-1)
				i = 0;
			j++;
			if (j == NoUC)
				break;
		}
		// wenn das Imperium irgendeinen Komplex erforscht hat, dann einen Geheimdienstbericht anlegen
		if (specialTech != -1)
		{
			CScienceIntelObj* report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, -1, -1, specialTech, choosen);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 3, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}
	}

	// 3. Versuch: etwas über ein bestimmtes Techlevel in Erfahrung bringen
	if (rand()%5 == NULL)
	{
		short techLevel = -1;
		short techType = rand()%6;
		switch (techType)
		{
		case 0: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetBioTech(); break;
		case 1: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetEnergyTech(); break;
		case 2: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetCompTech(); break;
		case 3: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetConstructionTech(); break;
		case 4: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetPropulsionTech(); break;
		case 5: techLevel = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetWeaponTech(); break;
		}
		CScienceIntelObj* report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, techLevel, techType, -1, -1);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 2, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
			return FALSE;
		}
	}

	// 2. Versuch: global produzierte Forschungspunkte ausspionieren
	if (rand()%6 == NULL)
	{
		UINT fp = m_pDoc->GetEmpire(enemyRace)->GetFP();
		CScienceIntelObj* report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, fp);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 1, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
			return FALSE;
		}
	}

	// wurde bis jetzt noch kein Versuch durchgeführt, so wird zwangsläufig der erste Versuch durchgeführt
	
	// 1. Versuch: Gebäude ausspionieren
	// Die Spionage ist komplett zufällig. Man kann nicht beeinflussen in welchem System spioniert wird.
	CArray<CPoint> sectors;
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_pDoc->m_System[x][y].GetOwnerOfSystem() == enemyRace)
				sectors.Add(CPoint(x,y));
	if (sectors.GetSize())
	{
		// in random steht ein zufällig ermittelter Sektor, in welchem spioniert werden kann.
		int random = rand()%sectors.GetSize();
		// bei der Forschungsspionage kann man nur etwas über Laboratorien oder andere Gebäude erfahren, welche in irgendeinem
		// Zusammenhang mit Forschung stehen.
		// Welches ausgewählt wird ist zufällig. Dabei werden mit etwas höherer Wahrscheinlichkeit Gebäude ausspioniert,
		// welche Arbeiter benötigen. Doch auch arbeiterfreie Gebäude, wie z.B. ein Theoriesimulator kann ermittelt werden.
		if (rand()%4 == NULL)	// zu 25% wird versucht ein arbeiterfreies Gebäude zu spionieren
		{
			int number = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetSize();
			int j = 0;
			if (number > NULL)
			{
				// zufällig ein Gebäude aussuchen
				for (int i = rand()%number; i < number; i++)
				{
					// wenn das Gebäude keine Arbeiter benötigt
					if (m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i).GetWorker() == FALSE)
					{
						const CBuilding* b = &m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i);
						// nur wenn das Gebäude einen Forschungshintergrund hat, kann es spioniert werden
						if (b->GetFPProd() > 0 || b->GetResearchBoni() > 0 || b->GetBioTechBoni() > 0 || b->GetCompTechBoni() > 0 ||
							b->GetConstructionTechBoni() > 0 || b->GetEnergyTechBoni() > 0 || b->GetPropulsionTechBoni() > 0 || 
							b->GetWeaponTechBoni() > 0)
						{
							int id = b->GetRunningNumber();
							int n = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfBuilding(id);
							CScienceIntelObj* report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n);
							// Intelreport dem Akteur hinzufügen
							if (report)
							{
								if (createText)
									report->CreateText(m_pDoc, 0, responsibleRace);
								m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
								return TRUE;
							}
							else
							{
								delete report;
								report = NULL;
								break;
							}
						}
					}
					if (i == number-1)
						i = 0;
					j++;
					if (j == number)
						break;
				}
			}
		}
		
		// ansonsten werden Gebäude welche Arbeiter benötigen ausspioniert
		USHORT buildings = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(RESEARCH_WORKER, 0);
		if (buildings > 0)
		{
			CScienceIntelObj* report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
				m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(RESEARCH_WORKER, 1), (BYTE)buildings);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 0, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
			}
		}
	}
	return FALSE;
}

/// Funktion führt eine Militärspionageaktion aus.
BOOLEAN CIntelCalc::ExecuteMilitarySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Forschungsspionage. Zuerst können Militärgebäude und Intelzentren aus
	irgendeinem System ausspioniert werden. Zweitens können stationierte Truppen in einem System ausspioniert werden.
	Drittens können bestimmte Schiffe oder Außenposten ausspioniert werden.
	*/
	// 3. Versuch: Schiffe und Stationen ausspionieren
	if (rand()%3 == NULL)	// zu 33% tritt dies ein
	{
		CArray<CShip*> ships;
		CArray<CShip*> stations;
		// Felder mit allen zu spionierenden Schiffe und Stationen anlegen
		for (int i = 0; i < m_pDoc->m_ShipArray.GetSize(); i++)
			if (m_pDoc->m_ShipArray[i].GetOwnerOfShip() == enemyRace)
			{
				if (m_pDoc->m_ShipArray[i].GetShipType() == OUTPOST || m_pDoc->m_ShipArray[i].GetShipType() == STARBASE)
					stations.Add(&m_pDoc->m_ShipArray[i]);
				else
					ships.Add(&m_pDoc->m_ShipArray[i]);
			}
		CMilitaryIntelObj* report = NULL;
		// wenn Stationen vorhanden sind zu 33% dort spionieren
		if (stations.GetSize() > NULL && rand()%3 == NULL)
		{
			short t = rand()%stations.GetSize();
			report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(stations.GetAt(t)->GetKO()),
				stations.GetAt(t)->GetID(), 1, FALSE, TRUE, FALSE);
		}
		// ansonsten werden Schiffe ausspioniert
		else if (ships.GetSize() > NULL)
		{
			short t = rand()%ships.GetSize();
			USHORT number = 0;
			// Anzahl der Schiffe in dem Sektor ermitteln
			for (int i = 0; i < ships.GetSize(); i++)
				if (ships.GetAt(i)->GetKO() == ships.GetAt(t)->GetKO())
				{
					number++;
					if (ships.GetAt(i)->GetFleet())
						for (int j = 0; j < ships.GetAt(i)->GetFleet()->GetFleetSize(); j++)
							number++;
				}
			report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(ships.GetAt(t)->GetKO()),
				ships.GetAt(t)->GetID(), number, FALSE, TRUE, FALSE);
		}
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 2, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
		}	
	}

	// 2. Versuch: Stationierte Truppen ausspionieren
	if (rand()%4 == NULL)
	{
		// Systeme ermitteln, in denen überhaupt Truppen stationiert sind
		CArray<CPoint> troopSectors;
		for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (m_pDoc->m_System[x][y].GetOwnerOfSystem() == enemyRace)
					if (m_pDoc->m_System[x][y].GetTroops()->GetSize())
						troopSectors.Add(CPoint(x,y));
		if (troopSectors.GetSize())
		{
			int random = rand()%troopSectors.GetSize();
			CMilitaryIntelObj* report = NULL;
			if (m_pDoc->m_System[troopSectors.GetAt(random).x][troopSectors.GetAt(random).y].GetTroops()->GetSize() == 1)
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(troopSectors.GetAt(random)),
				m_pDoc->m_System[troopSectors.GetAt(random).x][troopSectors.GetAt(random).y].GetTroops()->GetAt(0).GetID() + 20000, 1, FALSE, FALSE, TRUE);
			else
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(troopSectors.GetAt(random)),
					20000, m_pDoc->m_System[troopSectors.GetAt(random).x][troopSectors.GetAt(random).y].GetTroops()->GetSize(), FALSE, FALSE, TRUE);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 1, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}
	}

	// 1. Versuch: Gebäude ausspionieren
	// Die Spionage ist komplett zufällig. Man kann nicht beeinflussen in welchem System spioniert wird.
	CArray<CPoint> sectors;
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_pDoc->m_System[x][y].GetOwnerOfSystem() == enemyRace)
				sectors.Add(CPoint(x,y));
	if (sectors.GetSize())
	{
		// in random steht ein zufällig ermittelter Sektor, in welchem spioniert werden kann.
		int random = rand()%sectors.GetSize();
		// bei der Militärspionage kann man nur etwas über Intelzentren oder andere Gebäude erfahren, welche in irgendeinem
		// Zusammenhang mit Miltär oder Geheimdienst stehen.
		// Welches ausgewählt wird ist zufällig. Es werden mit gleicher Wahrscheinlichkeit Gebäude ausspioniert,
		// welche Arbeiter und keine Arbeiter, wie z.B. eine Schiffswerft, benötigen. Wird kein arbeiterbenötigendes Gebäude
		// gefunden, so wird versucht ein arbeiterfreies zu finden.
		if (rand()%3 == NULL)	// zu 33% wird versucht ein arbeiterbenötigendes Gebäude zu spionieren
		{
			// ansonsten werden Gebäude welche Arbeiter benötigen ausspioniert
			USHORT buildings = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(SECURITY_WORKER, 0);
			if (buildings > 0)
			{
				CMilitaryIntelObj* report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
					m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfWorkbuildings(SECURITY_WORKER, 1), (BYTE)buildings, TRUE, FALSE, FALSE);
				// Intelreport dem Akteur hinzufügen
				if (report)
				{
					if (createText)
						report->CreateText(m_pDoc, 0, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}			
		}

		int number = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetSize();
		int j = 0;
		if (number > NULL)
		{
			// zufällig ein Gebäude aussuchen
			for (int i = rand()%number; i < number; i++)
			{
				// wenn das Gebäude keine Arbeiter benötigt
				if (m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i).GetWorker() == FALSE)
				{
					const CBuilding* b = &m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetAllBuildings()->GetAt(i);
					// nur wenn das Gebäude einen Militär- oder Geheimdiensthintergrund hat, kann es spioniert werden
					if (b->GetShipYard() || b->GetSPProd() > NULL || b->GetBarrack() || b->GetBarrackSpeed() > NULL ||
						b->GetEconomySabotageBoni() > NULL || b->GetEconomySpyBoni() > NULL || b->GetGroundDefend() > NULL ||
						b->GetGroundDefendBoni() > NULL || b->GetMilitarySabotageBoni() > NULL || b->GetMilitarySpyBoni() > NULL ||
						b->GetResistance() > NULL || b->GetScanPower() > NULL || b->GetScanPowerBoni() > NULL || b->GetScanRange() > NULL ||
						b->GetScanRangeBoni() > NULL || b->GetSecurityBoni() > NULL || b->GetShieldPower() > NULL ||
						b->GetShieldPowerBoni() > NULL || b->GetShipBuildSpeed() > NULL || b->GetShipDefend() > NULL || b->GetShipDefendBoni() > NULL ||
						b->GetShipTraining() > NULL || b->GetTroopBuildSpeed() > NULL || b->GetTroopTraining() > NULL)						
					{
						int id = b->GetRunningNumber();
						int n = m_pDoc->m_System[sectors.GetAt(random).x][sectors.GetAt(random).y].GetNumberOfBuilding(id);
						CMilitaryIntelObj* report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n, TRUE, FALSE, FALSE);
						// Intelreport dem Akteur hinzufügen
						if (report)
						{
							if (createText)
								report->CreateText(m_pDoc, 0, responsibleRace);
							m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
							return TRUE;
						}
						else
						{
							delete report;
							report = NULL;
							break;
						}
					}
				}
				if (i == number-1)
					i = 0;
				j++;
				if (j == number)
					break;
			}
		}		
	}
	return FALSE;
}

/// Funktion führt eine Diplomatiespionageaktion aus.
BOOLEAN CIntelCalc::ExecuteDiplomacySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Diplomatiespionage. Zuerst kann spioniert werden, ob eine Majorrace eine
	Minorrace kennt. Zweitens kann spioniert werden, welchen Vertrag eine Majorrace mit einer bekannten Minorrace aufrecht-
	erhält. Drittens kann spioniert werden, welchen Vertrag inkl. Rundendauer eine Majorrace mit einer anderen Majorrace
	besitzt und viertens kann in Erfahrung gebracht werden, welche Beziehung die Rassen untereinander haben.
	*/
	CArray<short> minors;
	for (short i = 0; i < m_pDoc->m_MinorRaceArray.GetSize(); i++)
		if (m_pDoc->m_MinorRaceArray.GetAt(i).GetKnownByMajorRace(enemyRace))
			minors.Add(i);
	CArray<BYTE> majors;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != race && i != enemyRace)
			if (m_pDoc->GetMajorRace(enemyRace)->GetKnownMajorRace(i) == TRUE)
				majors.Add(i);
	// 4. Versuch: Beziehungsspionage
	if (rand()%3 == NULL)
	{
		// entweder wird die Minorrace oder die Majorrace ausspioniert
		CDiplomacyIntelObj* report = NULL;
		if (rand()%2 == NULL && minors.GetSize())
		{
			short minor = minors.GetAt(rand()%minors.GetSize());
			report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE,
				CPoint(m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().x, m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().y),
				NO_AGREEMENT, m_pDoc->m_MinorRaceArray.GetAt(minor).GetRelationshipToMajorRace(enemyRace));
		}
		else if (majors.GetSize())
		{
			BYTE major = majors.GetAt(rand()%majors.GetSize());
			report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, major,
				NO_AGREEMENT, 0, m_pDoc->GetMajorRace(major)->GetRelationshipToMajorRace(enemyRace));
		}
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 3, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
			return FALSE;
		}
	}
	// 3. Versuch: Vertrag mit einer Majorrace ausspionieren
	if (rand()%4 == NULL)
	{
		if (majors.GetSize())
		{
			BYTE major = majors.GetAt(rand()%majors.GetSize());
			short agreement = NO_AGREEMENT;
			short duration = 0;
			// hier kann entweder ein normaler Vertrag, oder auch ein Verteidigungspakt spioniert werden
			if (rand()%3 == NULL && m_pDoc->GetMajorRace(enemyRace)->GetDefencePact(major))
			{
				agreement = DEFENCE_PACT;
				duration = m_pDoc->GetMajorRace(enemyRace)->GetDurationOfDefencePact(major);
			}
			else
			{
				agreement = m_pDoc->GetMajorRace(enemyRace)->GetDiplomacyStatus(major);
				duration = m_pDoc->GetMajorRace(enemyRace)->GetDurationOfAgreement(major);
			}
			
			CDiplomacyIntelObj* report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE, major, agreement,
				duration, m_pDoc->GetMajorRace(major)->GetRelationshipToMajorRace(enemyRace));
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 2, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}
	}
	// 2. Versuch: Vertrag mit einer Minorrace ausspionieren
	if (rand()%2 == NULL)
	{
		if (minors.GetSize())
		{
			// in random steht eine zufällig ermittelte Minorrace, in welchem spioniert werden kann.
			short minor = minors.GetAt(rand()%minors.GetSize());
			CDiplomacyIntelObj* report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE,
				CPoint(m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().x, m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().y),
				m_pDoc->m_MinorRaceArray.GetAt(minor).GetDiplomacyStatus(enemyRace), m_pDoc->m_MinorRaceArray.GetAt(minor).GetRelationshipToMajorRace(enemyRace));
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 1, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}
	}
	// 1. Versuch: Kennen einer Minorrace ausspionieren
	if (minors.GetSize())
	{
		// in random steht eine zufällig ermittelte Minorrace, in welchem spioniert werden kann.
		short minor = minors.GetAt(rand()%minors.GetSize());
		CDiplomacyIntelObj* report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), TRUE,
			CPoint(m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().x, m_pDoc->m_MinorRaceArray.GetAt(minor).GetRaceKO().y));
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 0, responsibleRace);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
			return TRUE;
		}
		else
		{
			delete report;
			report = NULL;
			return FALSE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AB HIER KOMMEN DIE GANZEN EINZELNEN SABOTAGEMÖGLICHKEITEN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Funktion führt eine Wirtschatfssabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteEconomySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteEconomySpy(race, enemyRace, FALSE);
		newReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CEcoIntelObj* report = (CEcoIntelObj*)m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber);
		int latinum = report->GetLatinum() / 2;
		// 1. Möglichkeit: Latinum stehlen		
		if (latinum > NULL)
		{
			latinum = rand()%latinum + 1;
			m_pDoc->GetEmpire(race)->SetLatinum(latinum);
			m_pDoc->GetEmpire(enemyRace)->SetLatinum(-latinum);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, latinum);
			if (report)
			{
				report->CreateText(m_pDoc, 1, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}

		// 2. Möglichkeit: Gebäude zerstören bzw. Bevölkerung töten
		int buildings = report->GetNumber();
		if (buildings > NULL)
		{
			// zu 16.6% werden hier kein Gebäude zerstört, sondern es werden direkt Nahrungsmittel
			// in einem System vergiftet, wodruch die Bevölkerung stirbt
			if (rand()%6 == NULL)
			{
				CPoint ko = report->GetKO();
				BYTE currentHabs = (BYTE)m_pDoc->m_Sector[ko.x][ko.y].GetCurrentHabitants();
				// maximal die Hälfte der Bevölkerung kann getötet werden
				currentHabs /= 2;
				if (currentHabs > NULL)
				{
					currentHabs = rand()%currentHabs + 1;
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ko, 0, (BYTE)currentHabs);
					m_pDoc->m_Sector[ko.x][ko.y].LetPlanetsShrink(-(float)currentHabs);
					if (report)
					{
						report->CreateText(m_pDoc, 2, responsibleRace);
						m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
						m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
						return TRUE;
					}
					else
					{
						delete report;
						report = NULL;
						return FALSE;
					}
				}
			}
			// maximal die Hälfte der vorhanden Gebäude können mit einem Mal zerstört werden
			buildings /= 2;
			// mindestens aber ein Gebäude sollte zerstört werden
			if (buildings == NULL)
				buildings++;
			buildings = rand()%buildings + 1;
			CPoint ko = report->GetKO();
			int id = report->GetID();
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			int destroyed = 0;
			BuildingArray* allBuildings = m_pDoc->m_System[ko.x][ko.y].GetAllBuildings();
			for (int i = 0; i < allBuildings->GetSize(); i++)
				if (allBuildings->GetAt(i).GetRunningNumber() == report->GetID())
				{
					allBuildings->RemoveAt(i--);
					destroyed++;
					buildings--;
					if (buildings == NULL)
						break;
				}
			if (destroyed > NULL)
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CEcoIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed);
				if (report)
				{
					report->CreateText(m_pDoc, 0, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
		}
		m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}	
	return FALSE;
}

/// Funktion führt eine Forschungssabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteScienceSabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteScienceSpy(race, enemyRace, FALSE);
		newReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CScienceIntelObj* report = (CScienceIntelObj*)m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber);

		// 4. Versuch: Spezialforschungen
		// ->	kann nicht sabotiert werden, d.h. wenn der Report etwas mit Spezialforschungen zu tun hatte, dann wird er
		//		sofort gelöscht und es wird versucht einen neuen zu erstellen. Solange bis nix mit Spezialforschungen drin
		//		vorkommt.
		int specialTech = report->GetSpecialTechComplex();
		while (specialTech != -1)
		{
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			oldReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
			BOOLEAN isTrue = FALSE;
			do {
				isTrue = this->ExecuteScienceSpy(race, enemyRace, FALSE);
			} while (isTrue == FALSE);
			report = (CScienceIntelObj*)m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber);
			specialTech = report->GetSpecialTechComplex();
			
		} 

		// 3. Versuch: einen Techbereich sabotieren
		int techType = report->GetTechType();
		if (techType != -1)
		{
			// in diesem Gebiet die prozentuale Erforschung einfach wieder niedriger ansetzen
			UINT currentFP = 0;
			switch (techType)
			{
			case 0: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetBioFP(); break;
			case 1: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetEnergyFP(); break;
			case 2: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetComputerFP(); break;
			case 3: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetConstructionFP(); break;
			case 4: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetPropulsionFP(); break;
			case 5: currentFP = m_pDoc->GetEmpire(enemyRace)->GetResearch()->GetWeaponFP(); break;
			}
			// die aktuell schon erforschten Forschungspunkte zufällig neu bestimmen [0, aktuell erforschte FP]
			if (currentFP > NULL)
			{
				currentFP = rand()%currentFP + 1;
				short techLevel = report->GetTechLevel();
				m_pDoc->GetEmpire(enemyRace)->GetResearch()->SetFP(techType, currentFP);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, techLevel, techType, -1, -1);
				if (report)
				{
					report->CreateText(m_pDoc, 2, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
		}
		// 2. Versuch: globale Forschungspunkte stehlen
		int fp = report->GetFP();
		if (fp > (int)m_pDoc->GetEmpire(enemyRace)->GetFP())
			fp = m_pDoc->GetEmpire(enemyRace)->GetFP();
		if (fp > NULL)
		{
			m_pDoc->GetEmpire(race)->AddFP(fp);
			m_pDoc->GetEmpire(enemyRace)->AddFP(-fp);
			m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, fp);
			if (report)
			{
				report->CreateText(m_pDoc, 1, responsibleRace);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
				m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
				return TRUE;
			}
			else
			{
				delete report;
				report = NULL;
				return FALSE;
			}
		}	
		// 1. Versuch: Forschungsgebäude zerstören
		int buildings = report->GetNumber();
		if (buildings > NULL)
		{
			// maximal die Hälfte der vorhanden Gebäude können mit einem Mal zerstört werden
			buildings /= 2;
			// mindestens aber ein Gebäude sollte zerstört werden
			if (buildings == NULL)
				buildings++;
			buildings = rand()%buildings + 1;
			CPoint ko = report->GetKO();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			BuildingArray* allBuildings = m_pDoc->m_System[ko.x][ko.y].GetAllBuildings();
			for (int i = 0; i < allBuildings->GetSize(); i++)
				if (allBuildings->GetAt(i).GetRunningNumber() == report->GetID())
				{
					allBuildings->RemoveAt(i--);
					buildings--;
					destroyed++;
					if (buildings == NULL)
						break;
				}
			if (destroyed > NULL)
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CScienceIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed);
				if (report)
				{
					report->CreateText(m_pDoc, 0, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
		}
		m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion führt eine Militärsabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteMilitarySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteMilitarySpy(race, enemyRace, FALSE);
		newReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CMilitaryIntelObj* report = (CMilitaryIntelObj*)m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber);

		// 3. Versuch: Schiffe beschädigen/zerstören/stehlen oder Stationen beschädigen/zerstören
		if (report->GetIsShip())
		{
			CShip* ship = NULL;
			// überprüfen, ob die jeweilige Station oder das jeweilige Schiff auch noch im System vorhanden ist
			CArray<CPoint> allShips;	// x für Schiffsposition im Feld, y für Schiffsposition in der Flotte
			for (int i = 0; i < m_pDoc->m_ShipArray.GetSize(); i++)
				if (m_pDoc->m_ShipArray.GetAt(i).GetKO() == report->GetKO())
				{
					// besitzt dieses Schiff eine Flotte, so könnte sich unser Schiff auch in der Flotte befinden
					if (m_pDoc->m_ShipArray.GetAt(i).GetFleet())
					{
						for (int j = 0; j < m_pDoc->m_ShipArray.GetAt(i).GetFleet()->GetFleetSize(); j++)
							if (m_pDoc->m_ShipArray.GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j)->GetID() == report->GetID())
								allShips.Add(CPoint(i,j));
					}
					if (m_pDoc->m_ShipArray.GetAt(i).GetID() == report->GetID())
						allShips.Add(CPoint(i,-1));	// -1 als y Wert bedeutet, dass dieses Schiff in keiner Flotte vorkommt
				}
			// aus allen möglichen Schiffen, welche die ID unseres Reports und im richtigen Sektor sind eins zufällig aussuchen
			CPoint n = CPoint(-1,0);
			if (allShips.GetSize() > NULL)
			{
				int random = rand()%allShips.GetSize();
				n = allShips.GetAt(random);
				// ist das Schiff in keiner Flotte sondern direkt im Feld zu finden
				if (n.y == -1)
					ship = &m_pDoc->m_ShipArray.GetAt(n.x);
				else
					ship = m_pDoc->m_ShipArray.GetAt(n.x).GetFleet()->GetPointerOfShipFromFleet(n.y);
			}
			// wurde kein Schiff mehr in diesem Sektor gefunden, sei es da es zerstört wurde oder jetzt in einem anderen
			// Sektor ist, kann es auch nicht mehr zerstört werden.
			if (ship == NULL)
				return FALSE;

			// Haben wir das alles überstanden, so können wir jetzt ein Schiff/Station sabotieren und wissen gleichzeitig,
			// an welcher Position in welchem Feld (ShipArray oder Flotte eines Schiffes) es sich befindet.
			
			// eine Station wird entweder beschädigt oder mit geringerer Wahrscheinlichkeit komplett zerstört
			// eine Schiff wird entweder beschädigt, mit geringerer Wahrscheinlichkeit komplett zerstört oder sogar
			// gestohlen
			
			// Schiff stehlen
			if (rand()%2 == NULL && ship->GetShipType() != OUTPOST && ship->GetShipType() != STARBASE)
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ship->GetKO(), ship->GetID(), 1, FALSE, TRUE, FALSE);
				// die Station aus der ShipHistory der aktuellen Schiffe entfernen und den zerstörten Schiffen hinzufügen
				m_pDoc->m_ShipHistory[enemyRace].ModifyShip(ship, m_pDoc->m_Sector[ship->GetKO().x][ship->GetKO().y].GetName(TRUE),
					m_pDoc->GetCurrentRound(), CResourceManager::GetString("SABOTAGE"), CResourceManager::GetString("MISSED"));
				
				// neuen Besitzer hinzufügen
				ship->SetOwnerOfShip(race);
				ship->SetIsShipFlagShip(FALSE);
				ship->SetCurrentOrder(AVOID);
				// gestohlenes Schiff zum nächsten eigenen System verschieben
				CPoint oldKO = ship->GetKO();
				CPoint newKO = ship->GetKO();
				short minDist = MAXSHORT;
				for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						if (m_pDoc->m_Sector[x][y].GetShipPort(race))
							if (minDist > min(abs(oldKO.x - x), abs(oldKO.y - y)))
							{
								minDist = (short)min(abs(oldKO.x - x), abs(oldKO.y - y));
								newKO = CPoint(x,y);
							}
				ship->SetKO(newKO);
				ship->SetTargetKO(newKO, 0);
				// dem neuen Besitzer das Schiff als aktives Schiff hinzufügen
				m_pDoc->m_ShipHistory[race].AddShip(ship, m_pDoc->m_Sector[ship->GetKO().x][ship->GetKO().y].GetName(TRUE), m_pDoc->GetCurrentRound());

				// jetzt Dinge wegen einer möglichen Flotte beachten
				if (n.y == -1)		// Schiff nicht in Flotte
				{
					// das Schiff selbst kann aber eine Flotte anführen
					// Wenn das Schiff eine Flotte besaß, so geht die Flotte auf das erste Schiff in der Flotte über
					if (ship->GetFleet())
					{
						CFleet f = *(m_pDoc->m_ShipArray[n.x].GetFleet());
						// Flotte löschen
						m_pDoc->m_ShipArray[n.x].GetFleet()->DeleteFleet();
						m_pDoc->m_ShipArray[n.x].DeleteFleet();
						// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
						m_pDoc->m_ShipArray.Add(f.RemoveShipFromFleet(0));
						// für dieses eine Flotte erstellen
						m_pDoc->m_ShipArray[m_pDoc->m_ShipArray.GetUpperBound()].CreateFleet();
						for (USHORT i = 0; i < f.GetFleetSize(); i++)
							m_pDoc->m_ShipArray[m_pDoc->m_ShipArray.GetUpperBound()].GetFleet()->AddShipToFleet(f.GetShipFromFleet(i));
						m_pDoc->m_ShipArray[m_pDoc->m_ShipArray.GetUpperBound()].CheckFleet();
					}					
				}
				else	// Schiff ist in Flotte
				{
					m_pDoc->m_ShipArray.Add(m_pDoc->m_ShipArray.GetAt(n.x).GetFleet()->RemoveShipFromFleet(n.y));
					m_pDoc->m_ShipArray.GetAt(n.x).CheckFleet();
				}
				if (report)
				{
					report->CreateText(m_pDoc, 3, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
			// Schiff/Station zerstören
			else if ((rand()%4 == NULL && (ship->GetShipType() == OUTPOST || ship->GetShipType() == STARBASE))
				|| (rand()%2 == NULL && ship->GetShipType() != OUTPOST && ship->GetShipType() != STARBASE))
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ship->GetKO(), ship->GetID(), 1, FALSE, TRUE, FALSE);
				// die Station aus der ShipHistory der aktuellen Schiffe entfernen und den zerstörten Schiffen hinzufügen
				m_pDoc->m_ShipHistory[enemyRace].ModifyShip(ship, m_pDoc->m_Sector[ship->GetKO().x][ship->GetKO().y].GetName(TRUE),
					m_pDoc->GetCurrentRound(), CResourceManager::GetString("SABOTAGE"), CResourceManager::GetString("DESTROYED"));
				if (n.y == -1)		// nicht in Flotte
				{
					// das Schiff selbst kann aber eine Flotte anführen
					// Wenn das Schiff eine Flotte besaß, so geht die Flotte auf das erste Schiff in der Flotte über
					if (ship->GetFleet())
					{
						BYTE oldOrder = m_pDoc->m_ShipArray.GetAt(n.x).GetCurrentOrder();
						CFleet f = *(m_pDoc->m_ShipArray.GetAt(n.x).GetFleet());
						// Flotte löschen
						m_pDoc->m_ShipArray.GetAt(n.x).GetFleet()->DeleteFleet();
						m_pDoc->m_ShipArray.GetAt(n.x).DeleteFleet();
						// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
						m_pDoc->m_ShipArray.SetAt(n.x, f.RemoveShipFromFleet(0));
						// für dieses eine Flotte erstellen
						m_pDoc->m_ShipArray.GetAt(n.x).CreateFleet();
						for (USHORT x = 0; x < f.GetFleetSize(); x++)
							m_pDoc->m_ShipArray.GetAt(n.x).GetFleet()->AddShipToFleet(f.GetShipFromFleet(x));
						m_pDoc->m_ShipArray.GetAt(n.x).CheckFleet();
						// Brauchen das alte Schiff hier auch nicht löschen, da es mit einem neuen überschrieben wurde
						m_pDoc->m_ShipArray.GetAt(n.x).SetCurrentOrder(oldOrder);
					}
					// ansonsten kann es einfach gelöscht werden
					else
						m_pDoc->m_ShipArray.RemoveAt(n.x);
				}
				else	// Schiff ist in Flotte
				{
					m_pDoc->m_ShipArray.GetAt(n.x).GetFleet()->RemoveShipFromFleet(n.y);
					m_pDoc->m_ShipArray.GetAt(n.x).CheckFleet();
				}
				if (report)
				{
					report->CreateText(m_pDoc, 2, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
			// Schiff/Station beschädigen
			else
			{
				UINT hull = ship->GetHull()->GetCurrentHull();
				if (hull > NULL)
				{
					UINT newhull = rand()%hull + 1;
					ship->GetHull()->SetCurrentHull(newhull - hull);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ship->GetKO(), ship->GetID(), 1, FALSE, TRUE, FALSE);
					if (report)
					{
						report->CreateText(m_pDoc, 4, responsibleRace);
						m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
						m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
						return TRUE;
					}
					else
					{
						delete report;
						report = NULL;
						return FALSE;
					}
				}
			}
		}		
		
		// 2. Versuch: Stationierte Truppen umbringen
		int troopNumber = report->GetNumber();
		if (troopNumber > NULL && report->GetIsTroop())
		{
			// maximal die Hälfte der vorhanden Truppen können mit einem Mal zerstört werden
			troopNumber /= 2;
			// mindestens aber eine vorhandene Truppe sollte zerstört werden
			if (troopNumber == NULL)
				troopNumber++;
			troopNumber = rand()%troopNumber + 1;
			CPoint ko = report->GetKO();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Truppe/Truppen auf dem jeweiligen System zerstören
			CArray<CTroop>* allTroops = m_pDoc->m_System[ko.x][ko.y].GetTroops();
			for (int i = 0; i < allTroops->GetSize(); i++)
			{
				allTroops->RemoveAt(i--);
				troopNumber--;
				destroyed++;
				if (troopNumber == NULL)
					break;
			}
			if (destroyed > NULL)
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed, FALSE, FALSE, TRUE);
				if (report)
				{
					report->CreateText(m_pDoc, 1, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
		}
		// 1. Versuch: Militärgebäude zerstören
		int buildings = report->GetNumber();
		if (buildings > NULL && report->GetIsBuilding())
		{
			// maximal die Hälfte der vorhanden Gebäude können mit einem Mal zerstört werden
			buildings /= 2;
			// mindestens aber ein Gebäude sollte zerstört werden
			if (buildings == NULL)
				buildings++;
			int n = buildings = rand()%buildings + 1;
			CPoint ko = report->GetKO();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			BuildingArray* allBuildings = m_pDoc->m_System[ko.x][ko.y].GetAllBuildings();
			for (int i = 0; i < allBuildings->GetSize(); i++)
				if (allBuildings->GetAt(i).GetRunningNumber() == report->GetID())
				{
					allBuildings->RemoveAt(i--);
					buildings--;
					destroyed++;
					if (buildings == NULL)
						break;
				}
			if (destroyed > NULL)
			{
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed, TRUE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 0, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
		}
		m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}	
	return FALSE;
}

/// Funktion führt eine Diplomatiesabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteDiplomacySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteDiplomacySpy(race, enemyRace, FALSE);
		newReportNumber = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CDiplomacyIntelObj* report = (CDiplomacyIntelObj*)m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber);
		// Bei der Diplomatiesabotage kann entweder die Beziehung zwischen zwei Majorraces oder zwischen
		// einer Majorrace und einer Minorrace verschlechtert werden. Eine andere Möglichkeit wäre die Beziehung
		// zwischen uns und einer Minorrace oder einer Majorrace zu verbessern.
		
		// zwischen Major <-> Major
		if (report->GetMajorRace() != NULL)
		{
			BYTE major = report->GetMajorRace();
			// hier zwei Möglichkeiten: verschlechtern der Beziehung zwischen Geheimdienstopfer und Major oder Verbesserung
			// der Beziehung zwischen uns und dem Geheimdienstopfer
			
			// zu uns selbst verbessern
			if (rand()%5 == NULL)
			{
				int relationAdd = rand()%20 + 1;
				m_pDoc->GetMajorRace(enemyRace)->SetRelationshipToMajorRace(race, relationAdd);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, enemyRace, FALSE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 0, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}
			// zwischen zwei Majors verschlechtern
			else
			{
				int relationSub = -rand()%20 + 1;
				// falls nur computergesteuerte Rassen an der Aktion beteiligt sind, so wird die Beziehung nur um ein
				// Viertel verschlechtert. Liegt ganz einfach daran, dass sich die Computergegner nicht alle immer
				// untereinadner plattmachen
				if (m_pDoc->GetEmpire(race)->GetPlayerOfEmpire() == COMPUTER && m_pDoc->GetEmpire(enemyRace)->GetPlayerOfEmpire() == COMPUTER
					&& m_pDoc->GetEmpire(major)->GetPlayerOfEmpire() == COMPUTER)
					relationSub /= 4;
				m_pDoc->GetMajorRace(enemyRace)->SetRelationshipToMajorRace(major, relationSub);
				m_pDoc->GetMajorRace(major)->SetRelationshipToMajorRace(enemyRace, relationSub);
				m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, major, FALSE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 1, responsibleRace);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
					return TRUE;
				}
				else
				{
					delete report;
					report = NULL;
					return FALSE;
				}
			}			
		}
		// zwischen Major und Minor
		else
		{
			CMinorRace* minor =  m_pDoc->GetMinorRace(m_pDoc->m_Sector[report->GetMinorRaceKO().x][report->GetMinorRaceKO().y].GetName());
			if (minor)
			{
				// hier zwei Möglichkeiten: verschlechtern der Beziehung zwischen Geheimdienstopfer und Minor oder Verbesserung
				// der Beziehung zwischen uns und der Minor
				// zu uns selbst verbessern -> kein Bericht bei anderer Majorrace
				if (rand()%2 == NULL && minor->GetKnownByMajorRace(race) == TRUE && minor->GetRelationshipToMajorRace(race) < 90)
				{
					int relationAdd = rand()%20 + 1;
					minor->SetRelationshipToMajorRace(race, relationAdd);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, minor->GetRaceKO());
					if (report)
					{
						report->CreateText(m_pDoc, 2, responsibleRace);
						m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
						return TRUE;
					}
					else
					{
						delete report;
						report = NULL;
						return FALSE;
					}					
				}
				// bei unserem gegenüber verschlechtern
				else
				{
					int relationSub = rand()%20 + 1;
					minor->SetRelationshipToMajorRace(enemyRace, relationSub);
					m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CDiplomacyIntelObj(race, enemyRace, m_pDoc->GetCurrentRound(), FALSE, minor->GetRaceKO());
					if (report)
					{
						report->CreateText(m_pDoc, 3, responsibleRace);
						m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->AddReport(report);
						m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
						return TRUE;
					}
					else
					{
						delete report;
						report = NULL;
						return FALSE;
					}
				}
			}
		}
		m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion erstellt den Report, welcher aussagt, dass versucht wurde eine Rasse auszuspionieren/auszusabotieren.
void CIntelCalc::CreateMsg(BYTE responsibleRace, BYTE enemyRace, BYTE type)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntel.data";				// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			// Rasse bestimmen
			if (atoi(s) == enemyRace)
			{
				csInput.Delete(0, pos);
				// in csInput steht nun die Beschreibung
				// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
				switch (responsibleRace)
				{
				case HUMAN:		{s = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
				case FERENGI:	{s = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
				case KLINGON:	{s = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
				case ROMULAN:	{s = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
				case CARDASSIAN:{s = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
				case DOMINION:	{s = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
				}
				csInput.Replace("$race$", s);
				CIntelObject* report = NULL;
				switch (type)
				{
				case 0: report = new CEcoIntelObj(responsibleRace, enemyRace, m_pDoc->GetCurrentRound(), TRUE, 0); break;
				case 1: report = new CScienceIntelObj(responsibleRace, enemyRace, m_pDoc->GetCurrentRound(), TRUE, 0); break;
				case 2: report = new CMilitaryIntelObj(responsibleRace, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(-1,-1), 0, 0, FALSE, FALSE, FALSE); break;
				case 3: report = new CDiplomacyIntelObj(responsibleRace, enemyRace, m_pDoc->GetCurrentRound(), TRUE, CPoint(-1,-1)); break;
				}
				if (report)
				{
					report->SetEnemyDesc(csInput);
					m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetIntelReports()->AddReport(report);						
				}
				break;
			}			
		}
	}
	else
	{	
		AfxMessageBox("Error! Could not open file \"MajorIntel.data\"...");
		exit(1);
	}
	file.Close();
}

/// Funktion führt einen Anschlag durch.
BOOLEAN CIntelCalc::ExecuteAttempt(BYTE race, UINT ourSP)
{
	ASSERT(race);
	BYTE responsibleRace = race;
	// Ist ein Anschlagsobjekt vorhanden?
	if (m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAttemptObject())
	{
		CIntelObject* attemptObj = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAttemptObject();
		this->DeleteConsumedPoints(race, attemptObj->GetEnemy(), FALSE, attemptObj->GetType(), TRUE);
		// bei Anschlägen werden die benötigten Punkte etwas reduziert, indem hier einfach gesagt wird, dass es sich
		// um Spionage handeln würde.
		if (IsSuccess(attemptObj->GetEnemy(), ourSP, TRUE, responsibleRace, attemptObj->GetType()))
		{
			// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
			if (responsibleRace != NOBODY && responsibleRace != UNKNOWN)
				m_pDoc->GetMajorRace(attemptObj->GetEnemy())->SetRelationshipToMajorRace(responsibleRace, -rand()%10);
			// wenn der Anschlag erfolgreich durchgeführt werden konnte, muss der dazugehörige Spionagereport aus dem
			// Feld gelöscht werden
			for (int i = 0; i < m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
			{
				CIntelObject* intelObj = m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
				// Sind immer Spionageobjekte
				if (intelObj->GetIsSpy())
					if (intelObj->GetRound() == attemptObj->GetRound())
						if (intelObj->GetEnemy() == attemptObj->GetEnemy())
							if (intelObj->GetType() == attemptObj->GetType())
								if (*intelObj->GetOwnerDesc() == *attemptObj->GetOwnerDesc())
								{
									BOOLEAN returnValue = FALSE;
									switch (attemptObj->GetType())
									{
									case 0: returnValue = this->ExecuteEconomySabotage(race, attemptObj->GetEnemy(), responsibleRace, i); break;
									case 1: returnValue = this->ExecuteScienceSabotage(race, attemptObj->GetEnemy(), responsibleRace, i); break;
									case 2: returnValue = this->ExecuteMilitarySabotage(race, attemptObj->GetEnemy(), responsibleRace, i); break;
									case 3: returnValue = this->ExecuteDiplomacySabotage(race, attemptObj->GetEnemy(), responsibleRace, i); break;
									}
									intelObj = NULL;
									m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
									return returnValue;									
								}						
			}			
		}
	}
	m_pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
	return FALSE;
}

/// Funktion gibt die aktuell komplett generierten inneren Sicherheitspunkte eines Imperiums zurück.
UINT CIntelCalc::GetCompleteInnerSecPoints(BYTE enemyRace)
{
	ASSERT(enemyRace);

	// Depot der inneren Sicherheitspunkte + aktuell produzierte Sicherheitspunkte * prozentuale Zuweisung + Bonus auf innere Sicherheit + prozentuale Zuteilung
	UINT enemyInnerSec = m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetSecurityPoints() *
		m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage() / 100;
	// + Bonus auf innere Sicherheit
	enemyInnerSec += enemyInnerSec * m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetInnerSecurityBoni() / 100;
	// + prozentuale Zuteilung
	enemyInnerSec += m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage();
	// + Punkte aus dem Lager (darin ist der Bonus schon vorhanden)
	enemyInnerSec += m_pDoc->GetEmpire(enemyRace)->GetIntelligence()->GetInnerSecurityStorage();

	return enemyInnerSec;
}