#include "stdafx.h"
#include "IntelCalc.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Ships/Ships.h"
#include "IOData.h"
#include "General/Loc.h"



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelCalc::CIntelCalc(CBotEDoc* pDoc)
{
	AssertBotE(pDoc);
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
void CIntelCalc::StartCalc(CMajor* pRace)
{
	AssertBotE(pRace);
	// resultierende Spionagegeheimdienstpunkte bei einer Rasse
	map<CString, UINT> m_iSpySP;
	// resultierende Sabotagegeheimdienstpunkte bei einer Rasse
	map<CString, UINT> m_iSabSP;

	CIntelligence* pIntel = pRace->GetEmpire()->GetIntelligence();

	// Ein möglicher Anschlag wird vor allen anderen Geheimdienstaktionen durchgeführt. Bei einem Anschlag werden
	// nur die angesammelten Geheimdienspunkte benutzt.
	CIntelObject* attemptObj = pIntel->GetIntelReports()->GetAttemptObject();
	if (attemptObj)
	{
		int ourSP = pIntel->GetSPStorage(1, attemptObj->GetEnemy());
		// + eventuellen Bonus aus dem Teilbereich dazurechnen
		ourSP += ourSP * pIntel->GetBonus(attemptObj->GetType(), 1) / 100;
		this->ExecuteAttempt(pRace, ourSP);
	}

	// zuerst werden die Geheimdienstgegner ermittelt. Nur wenn bei einem Gegner mehr als NULL resultierende
	// Geheimdienstpunkte vorhanden sind ist es ein wirklicher Gegner.
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->first != pRace->GetRaceID())
		{
			m_iSpySP[it->first] = pIntel->GetSecurityPoints() *	pIntel->GetAssignment()->GetGlobalSpyPercentage(it->first) / 100;
			m_iSabSP[it->first] = pIntel->GetSecurityPoints() *	pIntel->GetAssignment()->GetGlobalSabotagePercentage(it->first) / 100;
		}

	// dann können die Geheimdienstaktionen gestartet werden
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->first != pRace->GetRaceID())
		{
			// sind effektive Geheimdienstpunkte für Spionage gegen Rasse i vorhanden oder es befinden sich Punkte im Depot?
			if (m_iSpySP[it->first] > NULL || pIntel->GetSPStorage(0,it->first) > NULL)
			{
				// die speziellen Bereiche durchgehen
				// Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
				int b = rand()%4;
				int count = 0;
				while (count < 4)
				{
					// Spionagepunkte * spezielle Spionagezuteilung / 100 + Spionagedepot * spezielle Spionagezuteilung / 100
					int effectiveSP = m_iSpySP[it->first] * pIntel->GetAssignment()->GetSpyPercentages(it->first,b) / 100 + pIntel->GetSPStorage(0,it->first) * pIntel->GetAssignment()->GetSpyPercentages(it->first,b) / 100;
					// + eventuellen Bonus aus dem Teilbereich dazurechnen
					effectiveSP += effectiveSP * pIntel->GetBonus(b, 0) / 100;
					if (effectiveSP > NULL)
					{
						CMajor* pResponsibleRace = pRace;
						USHORT actions = this->IsSuccess(it->second, effectiveSP, TRUE, pResponsibleRace, b);
						this->DeleteConsumedPoints(pRace, it->second, TRUE, b, FALSE);
						// jetzt Geheimdienstaktion starten!
						int stop = 0;	// irgendwann mal abbrechen, falls aus irgendeinem Grund actions mal nie runtergezählt werden kann
						while (actions > 0 && stop < 100)
						{
							if (this->ExecuteAction(pRace, it->second, pResponsibleRace, b, TRUE))
							{
								actions--;
								// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
								if (pResponsibleRace != NULL)
									it->second->SetRelation(pResponsibleRace->GetRaceID(), -rand()%5);
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
			if (m_iSabSP[it->first] > NULL || pIntel->GetSPStorage(1,it->first) > NULL)
			{
				// die speziellen Bereiche durchgehen
				// Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
				int b = rand()%4;
				int count = 0;
				while (count < 4)
				{
					// Spionagepunkte * spezielle Spionagezuteilung / 100 + Spionagedepot * spezielle Spionagezuteilung / 100
					int effectiveSP = m_iSabSP[it->first] * pIntel->GetAssignment()->GetSabotagePercentages(it->first,b) / 100
						+ pIntel->GetSPStorage(1,it->first) * pIntel->GetAssignment()->GetSabotagePercentages(it->first,b) / 100;
					// + eventuellen Bonus aus dem Teilbereich dazurechnen
					effectiveSP += effectiveSP * pIntel->GetBonus(b, 1) / 100;
					if (effectiveSP > NULL)
					{
						CMajor* pResponsibleRace = pRace;
						USHORT actions = this->IsSuccess(it->second, effectiveSP, FALSE, pResponsibleRace, b);
						this->DeleteConsumedPoints(pRace, it->second, FALSE, b, FALSE);
						// jetzt Geheimdienstaktion starten!
						int stop = 0;	// irgendwann mal abbrechen, falls aus irgendeinem Grund actions mal nie runtergezählt werden kann
						while (actions > 0 && stop < 100)
						{
							if (this->ExecuteAction(pRace, it->second, pResponsibleRace, b, FALSE))
							{
								actions--;
								// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
								if (pResponsibleRace != NULL)
									it->second->SetRelation(pResponsibleRace->GetRaceID(), -rand()%10);
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
void CIntelCalc::AddPoints(CMajor* pRace)
{
	AssertBotE(pRace);
	CIntelligence* pIntel = pRace->GetEmpire()->GetIntelligence();

	// Punkte der inneren Sicherheit hinzufügen
	int add = pIntel->GetSecurityPoints() * pIntel->GetAssignment()->GetInnerSecurityPercentage() / 100;
	add += add * pIntel->GetInnerSecurityBoni() / 100;
	pIntel->AddInnerSecurityPoints(add);

	// Punkte den einzelnen Depots hinzufügen (hier kein Bonus durch eventuelle Boni auf die einzelnen Ressorts)
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->first != pRace->GetRaceID())
		{
			// Spionagedepot auffüllen
			UINT depot = pIntel->GetSecurityPoints() *	pIntel->GetAssignment()->GetGlobalSpyPercentage(it->first) * pIntel->GetAssignment()->GetSpyPercentages(it->first, 4) / 10000;
			pIntel->AddSPStoragePoints(0, it->first, depot);

			// Sabotagedepot auffüllen
			depot = pIntel->GetSecurityPoints() * pIntel->GetAssignment()->GetGlobalSabotagePercentage(it->first) *	pIntel->GetAssignment()->GetSabotagePercentages(it->first, 4) / 10000;
			pIntel->AddSPStoragePoints(1, it->first, depot);
		}
}

/// Funktion zieht einen rassenabhängigen Prozentsatz von den einzelnen Depots ab. Funkion sollte nach Ausführung
/// aller anderen Geheimdienstfunktionen aufgerufen werden.
void CIntelCalc::ReduceDepotPoints(CMajor* pRace, int perc)
{
	AssertBotE(pRace);
	CIntelligence* pIntel = pRace->GetEmpire()->GetIntelligence();

	// bei perc == -1 wird der rassenspezifische Prozentsatz verwendet
	if (perc == -1)
	{
		int nCount = 0;
		perc = 0;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		{
			perc += 15;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
		{
			perc += 5;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
		{
			perc += 10;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
		{
			perc += 5;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
		{
			perc -= 10;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
		{
			perc += 0;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::PRODUCER))
		{
			perc += 0;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
		{
			perc += 35;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::SNEAKY))
		{
			perc -= 5;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
		{
			perc -= 20;
			nCount++;
		}
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
		{
			perc += 10;
			nCount++;
		}

		// traf keine Rasseneigenschaft zu, so den Standardwert von 20% setzen
		if (nCount == 0)
			perc = 20;
		// sonst den Durschnittswert berechnen
		else
		{
			perc /= nCount;
			perc += 20;
		}

		// Bereichsüberprüfungen machen
		if (perc > 100)
			perc = 100;
		else if (perc < 0)
			perc = 0;
	}
	// Prozentsatz aus den einzelnen Depots abziehen
	// zuerst das Lager der angesammelten inneren Sicherheitspunkte
	int diff = pIntel->GetInnerSecurityStorage() * perc / 100;
	pIntel->AddInnerSecurityPoints(-diff);

	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->first != pRace->GetRaceID())
		{
			// Wenn die Rasse kein Systeme mehr besitzt, also ausgelöscht wurde, so verschwinden alle
			// angesammelten Geheimdienstpunkte
			int oldPerc = perc;
			if (it->second->GetEmpire()->CountSystems() == 0)
				perc = 100;
			// Depot der Spionagepunkte
			diff = pIntel->GetSPStorage(0,it->first) * perc / 100;
			pIntel->AddSPStoragePoints(0,it->first,-diff);
			// Depot der Sabotagepunkte
			diff = pIntel->GetSPStorage(1,it->first) * perc / 100;
			pIntel->AddSPStoragePoints(1,it->first,-diff);
			perc = oldPerc;
		}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion berechnet ob eine Geheimdienstaktion gegen eine andere Rasse erfolgreich verläuft.
USHORT CIntelCalc::IsSuccess(CMajor* pEnemyRace, int ourSP, BOOLEAN isSpy, CMajor* pResponsibleRace, BYTE type)
{
	MYTRACE("intel")(MT::LEVEL_INFO, "CIntelCalc::IsSuccess() begin...\n");
	AssertBotE(pEnemyRace);
	AssertBotE(pResponsibleRace);
	USHORT actions = NULL;

	int enemyInnerSec = GetCompleteInnerSecPoints(pEnemyRace);

	MYTRACE("intel")(MT::LEVEL_INFO, "inner security of %s is %d\n", pEnemyRace, enemyInnerSec);

	// Aggressivität der angreifenden Rasse holen

	BYTE agg = pResponsibleRace->GetEmpire()->GetIntelligence()->GetAggressiveness(!isSpy, pEnemyRace->GetRaceID());

	MYTRACE("intel")(MT::LEVEL_INFO, "intel aggressiveness is %d\n", agg);
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

	MYTRACE("intel")(MT::LEVEL_INFO, "SP of race %s %d > enemies inner security %d + random %d\n", pResponsibleRace->GetRaceID(), ourSP, enemyInnerSec, minDiff);
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
				CString sResponsibleRace = pResponsibleRace->GetEmpire()->GetIntelligence()->GetResponsibleRace();
				pResponsibleRace = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sResponsibleRace));
				if (pResponsibleRace != NULL)
				{
					if (sResponsibleRace == pEnemyRace->GetRaceID())
						pResponsibleRace = NULL;
					// checken ob unser Geheimdienstopfer die "responisibleRace" auch kennt
					else if (pEnemyRace->IsRaceContacted(sResponsibleRace) == FALSE)
						pResponsibleRace = NULL;
				}
			}
			else
				pResponsibleRace = NULL;
		}
	}
	// Handelt es sich um eine fehlgeschlagene Aktion, so ist es möglich, dass das vermeindliche Geheimdienstopfer
	// etwas davon erfährt.
	else if ((ourSP * agg) < (enemyInnerSec + minDiff) && rand()%2 == NULL)
		this->CreateMsg(pResponsibleRace, pEnemyRace, type);
	MYTRACE("intel")(MT::LEVEL_INFO, "number of starting intel actions: %d\n", actions);
	return actions;
}

/// Funktion entfernt die durch eine Geheimdienstaktion verbrauchten Punkte auf Seiten des Geheimdienstopfers und
/// auf Seiten des Geheimdienstagressors.
void CIntelCalc::DeleteConsumedPoints(CMajor* pOurRace, CMajor* pEnemyRace, BOOLEAN isSpy, BYTE type, BOOLEAN isAttempt)
{
	MYTRACE("intel")(MT::LEVEL_INFO, "CIntelCalc::DeleteConsumedPoints() begin...\n");

	CIntelligence* pOurIntel	= pOurRace->GetEmpire()->GetIntelligence();
	CIntelligence* pIntelEnemy	= pEnemyRace->GetEmpire()->GetIntelligence();


	// Punkte der gesammten Inneren Sicherheit des Opfers holen
	int enemyInnerSecPoints = pIntelEnemy->GetSecurityPoints() * pIntelEnemy->GetAssignment()->GetInnerSecurityPercentage() / 100;
	// + Bonus auf innere Sicherheit
	enemyInnerSecPoints += enemyInnerSecPoints * pIntelEnemy->GetInnerSecurityBoni() / 100;
	// beim Depot sind die Boni schon mit eingerechnet
	int enemyInnerSecDepot = pIntelEnemy->GetInnerSecurityStorage();
	MYTRACE("intel")(MT::LEVEL_INFO, "enemies inner security points: %d - enemies inner security depot: %d\n", enemyInnerSecPoints, enemyInnerSecDepot);

	// nun die generierten Punkte des Agressors bestimmen
	int racePoints = 0;
	int raceDepot = 0;
	if (isSpy && !isAttempt)
	{
		racePoints = pOurIntel->GetSecurityPoints() * pOurIntel->GetAssignment()->GetGlobalSpyPercentage(pEnemyRace->GetRaceID())
			* pOurIntel->GetAssignment()->GetSpyPercentages(pEnemyRace->GetRaceID(), type) / 10000;
		racePoints += racePoints * pOurIntel->GetBonus(type, 0) / 100;

		raceDepot = pOurIntel->GetSPStorage(0, pEnemyRace->GetRaceID()) * pOurIntel->GetAssignment()->GetSpyPercentages(pEnemyRace->GetRaceID(), type) / 100;
		raceDepot += raceDepot * pOurIntel->GetBonus(type, 0) / 100;
	}
	else if (!isSpy && !isAttempt)
	{
		racePoints = pOurIntel->GetSecurityPoints() * pOurIntel->GetAssignment()->GetGlobalSabotagePercentage(pEnemyRace->GetRaceID())
			* pOurIntel->GetAssignment()->GetSabotagePercentages(pEnemyRace->GetRaceID(), type) / 10000;
		racePoints += racePoints * pOurIntel->GetBonus(type, 1) / 100;

		raceDepot = pOurIntel->GetSPStorage(1, pEnemyRace->GetRaceID()) * pOurIntel->GetAssignment()->GetSabotagePercentages(pEnemyRace->GetRaceID(), type) / 100;
		raceDepot += raceDepot * pOurIntel->GetBonus(type, 1) / 100;
	}
	else if (isAttempt)
	{
		raceDepot = pOurIntel->GetSPStorage(1, pEnemyRace->GetRaceID()) * pOurIntel->GetAssignment()->GetSabotagePercentages(pEnemyRace->GetRaceID(), type) / 100;
		raceDepot += raceDepot * pOurIntel->GetBonus(type, 1) / 100;
	}

	MYTRACE("intel")(MT::LEVEL_INFO, "racePoints: %d - raceDepot: %d\n", racePoints, raceDepot);
	// jetzt gegenseitig die Punkte abziehen.
	// zuerst werden immer die Punkte abgezogen, welche nicht aus den Depots kommen
	int temp = racePoints + raceDepot - enemyInnerSecPoints;
	// in temp stehen jetzt nur noch die
	if (temp > 0)
		pIntelEnemy->AddInnerSecurityPoints(-temp);

	temp = enemyInnerSecPoints + enemyInnerSecDepot - racePoints;
	if (temp > 0)
		pOurIntel->AddSPStoragePoints(!isSpy, pEnemyRace->GetRaceID(), -temp);
}

/// Funktion ruft die jeweilige Unterfunktion auf, welche eine Geheimdienstaktion schlussendlich ausführt.
BOOLEAN CIntelCalc::ExecuteAction(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BYTE type, BOOLEAN isSpy)
{
	if (isSpy)
	{
		switch (type)
		{
		case 0: return this->ExecuteEconomySpy(pRace, pEnemyRace, pResponsibleRace, TRUE);
		case 1: return this->ExecuteScienceSpy(pRace, pEnemyRace, pResponsibleRace, TRUE);
		case 2: return this->ExecuteMilitarySpy(pRace, pEnemyRace, pResponsibleRace, TRUE);
		case 3: return this->ExecuteDiplomacySpy(pRace, pEnemyRace, pResponsibleRace, TRUE);
		}
	}
	else
	{
		switch (type)
		{
		case 0: return this->ExecuteEconomySabotage(pRace, pEnemyRace, pResponsibleRace);
		case 1: return this->ExecuteScienceSabotage(pRace, pEnemyRace, pResponsibleRace);
		case 2: return this->ExecuteMilitarySabotage(pRace, pEnemyRace, pResponsibleRace);
		case 3: return this->ExecuteDiplomacySabotage(pRace, pEnemyRace, pResponsibleRace);
		}
	}
	return FALSE;
}

/// Funktion führt eine Wirtschatfsspionageaktion aus.
BOOLEAN CIntelCalc::ExecuteEconomySpy(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt zwei verschiedene Arten der Wirtschaftsspionage. Zuerst kann das besitzte Credits ausspioniert
	werden. Zweitens werden Gebäude aus irgendeinem System ausspioniert.
	*/
	// 1. Versuch: Credits ausspionieren
	if (rand()%8 == NULL)
	{
		int credits = (int)pEnemyRace->GetEmpire()->GetCredits();
		CEcoIntelObj* report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, credits);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
			if (m_pDoc->GetSystem(x, y).OwnerID() == pEnemyRace->GetRaceID())
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
			int number = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetSize();
			if (number > NULL)
			{
				int j = 0;
				// zufällig ein Gebäude aussuchen
				for (int i = rand()%number; i < number; i++)
				{
					CBuildingInfo buildingInfo = m_pDoc->GetBuildingInfo(m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetAt(i).GetRunningNumber());
					// wenn das Gebäude keine Arbeiter benötigt
					if (buildingInfo.GetWorker() == FALSE)
					{
						// sobald das Gebäude eine nicht wirtschaftsbezogene Leistung erbringen kann, kann es nicht spioniert werden
						if (!buildingInfo.GetBarrack() && !buildingInfo.GetFPProd() && !buildingInfo.GetGroundDefend() && !buildingInfo.GetGroundDefendBoni()
							&& !buildingInfo.GetMilitarySabotageBoni() && !buildingInfo.GetMilitarySpyBoni() && !buildingInfo.GetResearchBoni() && !buildingInfo.GetResearchSabotageBoni()
							&& !buildingInfo.GetResearchSpyBoni() && !buildingInfo.GetResistance() && !buildingInfo.GetScanPower() && !buildingInfo.GetScanPowerBoni()
							&& !buildingInfo.GetScanRange() && !buildingInfo.GetScanRangeBoni() && !buildingInfo.GetSecurityBoni() && !buildingInfo.GetShieldPower()
							&& !buildingInfo.GetShieldPowerBoni() && !buildingInfo.GetShipDefend() && !buildingInfo.GetShipDefendBoni() && !buildingInfo.GetShipTraining()
							&& !buildingInfo.GetShipYard() && !buildingInfo.GetSPProd() && !buildingInfo.GetTroopTraining())
						{
							int id = buildingInfo.GetRunningNumber();
							int n = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfBuilding(id);
							CEcoIntelObj* report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n);
							// Intelreport dem Akteur hinzufügen
							if (report)
							{
								if (createText)
									report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
								pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
		int buildingTypes[WORKER::IRIDIUM_WORKER+1] = {0};
		int start = rand()%(WORKER::IRIDIUM_WORKER+1);
		int j = 0;
		for (int i = start; i <= WORKER::IRIDIUM_WORKER; i++)
		{
			if (i != WORKER::SECURITY_WORKER && i != WORKER::RESEARCH_WORKER)
			{
				WORKER::Typ nWorker = (WORKER::Typ)i;
				buildingTypes[i] = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(nWorker, 0);
				if (buildingTypes[i] > NULL)
				{
					start = i;
					break;
				}
			}
			if (i == WORKER::IRIDIUM_WORKER)
				i = WORKER::FOOD_WORKER;
			j++;
			if (j == WORKER::IRIDIUM_WORKER)
				break;
		}
		// in "start" steht möglicherweise das Gebäude zur Spionage
		if (buildingTypes[start] > 0)
		{
			WORKER::Typ nWorker = (WORKER::Typ)start;
			CEcoIntelObj* report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
				m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(nWorker, 1), (BYTE)buildingTypes[start]);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
BOOLEAN CIntelCalc::ExecuteScienceSpy(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Forschungsspionage. Zuerst können Forschungsgebäude aus irgendeinem System
	ausspioniert werden. Zweitens können die global fabrizierten Forschungspunkte ausspioniert werden. Drittens können
	einzelne Techlevel in Erfahrung gebracht werden und viertens wird etwas über die Spezialforschung spioniert.
	*/

	// 4. Versuch: etwas über die Spezialforschung herausbekommen
	if (rand()%8 == NULL)
	{
		RESEARCH_COMPLEX::Typ specialTech = RESEARCH_COMPLEX::NONE;
		short choosen = -1;
		// zufällig mit einem Komplex starten und schauen ob dieser erforsch ist
		int t = rand()%NoUC;
		int j = 0;
		for (int i = t; i < NoUC; i++)
		{
			RESEARCH_COMPLEX::Typ nComplex = (RESEARCH_COMPLEX::Typ)i;
			if (pEnemyRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
			{
				specialTech = nComplex;
				for (int j = 1; j <= 3; j++)
					if (pEnemyRace->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldStatus(j) == RESEARCH_STATUS::RESEARCHED)
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
		if (specialTech != RESEARCH_COMPLEX::NONE)
		{
			CScienceIntelObj* report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, -1, -1, specialTech, choosen);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 3, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
		case 0: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetBioTech(); break;
		case 1: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetEnergyTech(); break;
		case 2: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetCompTech(); break;
		case 3: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetConstructionTech(); break;
		case 4: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetPropulsionTech(); break;
		case 5: techLevel = pEnemyRace->GetEmpire()->GetResearch()->GetWeaponTech(); break;
		}
		CScienceIntelObj* report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, techLevel, techType, RESEARCH_COMPLEX::NONE, -1);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
		UINT fp = pEnemyRace->GetEmpire()->GetFP();
		CScienceIntelObj* report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, fp);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
			if (m_pDoc->GetSystem(x, y).OwnerID() == pEnemyRace->GetRaceID())
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
			int number = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetSize();
			if (number > NULL)
			{
				int j = 0;
				// zufällig ein Gebäude aussuchen
				for (int i = rand()%number; i < number; i++)
				{
					CBuildingInfo buildingInfo = m_pDoc->GetBuildingInfo(m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetAt(i).GetRunningNumber());
					// wenn das Gebäude keine Arbeiter benötigt
					if (buildingInfo.GetWorker() == FALSE)
					{
						// nur wenn das Gebäude einen Forschungshintergrund hat, kann es spioniert werden
						if (buildingInfo.GetFPProd() > 0 || buildingInfo.GetResearchBoni() > 0 || buildingInfo.GetBioTechBoni() > 0 || buildingInfo.GetCompTechBoni() > 0 ||
							buildingInfo.GetConstructionTechBoni() > 0 || buildingInfo.GetEnergyTechBoni() > 0 || buildingInfo.GetPropulsionTechBoni() > 0 ||
							buildingInfo.GetWeaponTechBoni() > 0)
						{
							int id = buildingInfo.GetRunningNumber();
							int n = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfBuilding(id);
							CScienceIntelObj* report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n);
							// Intelreport dem Akteur hinzufügen
							if (report)
							{
								if (createText)
									report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
								pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
		USHORT buildings = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(WORKER::RESEARCH_WORKER, 0);
		if (buildings > 0)
		{
			CScienceIntelObj* report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
				m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(WORKER::RESEARCH_WORKER, 1), (BYTE)buildings);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
BOOLEAN CIntelCalc::ExecuteMilitarySpy(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Forschungsspionage. Zuerst können Militärgebäude und Intelzentren aus
	irgendeinem System ausspioniert werden. Zweitens können stationierte Truppen in einem System ausspioniert werden.
	Drittens können bestimmte Schiffe oder Außenposten ausspioniert werden.
	*/
	// 3. Versuch: Schiffe und Stationen ausspionieren
	if (rand()%3 == NULL)	// zu 33% tritt dies ein
	{
		CArray<boost::shared_ptr<CShips>> ships;
		CArray<boost::shared_ptr<CShips>> stations;
		// Felder mit allen zu spionierenden Schiffe und Stationen anlegen
		for(CShipMap::iterator i = m_pDoc->m_ShipMap.begin(); i != m_pDoc->m_ShipMap.end(); ++i)
			if (i->second->GetOwnerOfShip() == pEnemyRace->GetRaceID())
			{
				if (i->second->IsStation())
					stations.Add(i->second);
				else
					ships.Add(i->second);
			}
		CMilitaryIntelObj* report = NULL;
		// wenn Stationen vorhanden sind zu 33% dort spionieren
		if (stations.GetSize() > NULL && rand()%3 == NULL)
		{
			short t = rand()%stations.GetSize();
			report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(stations.GetAt(t)->GetCo()),
				stations.GetAt(t)->GetID(), 1, FALSE, TRUE, FALSE);
		}
		// ansonsten werden Schiffe ausspioniert
		else if (ships.GetSize() > NULL)
		{
			short t = rand()%ships.GetSize();
			USHORT number = 0;
			// Anzahl der Schiffe in dem Sektor ermitteln
			for (int i = 0; i < ships.GetSize(); i++)
				if (ships.GetAt(i)->GetCo() == ships.GetAt(t)->GetCo())
				{
					number++;
					number += ships.GetAt(i)->GetFleetSize();
				}
			report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(ships.GetAt(t)->GetCo()),
				ships.GetAt(t)->GetID(), number, FALSE, TRUE, FALSE);
		}
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
				if (m_pDoc->GetSystem(x, y).OwnerID() == pEnemyRace->GetRaceID())
					if (m_pDoc->GetSystem(x, y).GetTroops()->GetSize())
						troopSectors.Add(CPoint(x,y));
		if (troopSectors.GetSize())
		{
			int random = rand()%troopSectors.GetSize();
			CMilitaryIntelObj* report = NULL;
			if (m_pDoc->GetSystem(troopSectors.GetAt(random).x, troopSectors.GetAt(random).y).GetTroops()->GetSize() == 1)
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(troopSectors.GetAt(random)),
				m_pDoc->GetSystem(troopSectors.GetAt(random).x, troopSectors.GetAt(random).y).GetTroops()->GetAt(0).GetID() + 20000, 1, FALSE, FALSE, TRUE);
			else
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(troopSectors.GetAt(random)),
					20000, m_pDoc->GetSystem(troopSectors.GetAt(random).x, troopSectors.GetAt(random).y).GetTroops()->GetSize(), FALSE, FALSE, TRUE);
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
			if (m_pDoc->GetSystem(x, y).OwnerID() == pEnemyRace->GetRaceID())
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
			USHORT buildings = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(WORKER::SECURITY_WORKER, 0);
			if (buildings > 0)
			{
				CMilitaryIntelObj* report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)),
					m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfWorkbuildings(WORKER::SECURITY_WORKER, 1), (BYTE)buildings, TRUE, FALSE, FALSE);
				// Intelreport dem Akteur hinzufügen
				if (report)
				{
					if (createText)
						report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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

		int number = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetSize();
		if (number > NULL)
		{
			int j = 0;
			// zufällig ein Gebäude aussuchen
			for (int i = rand()%number; i < number; i++)
			{
				CBuildingInfo buildingInfo = m_pDoc->GetBuildingInfo(m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetAllBuildings()->GetAt(i).GetRunningNumber());
				// wenn das Gebäude keine Arbeiter benötigt
				if (buildingInfo.GetWorker() == FALSE)
				{
					// nur wenn das Gebäude einen Militär- oder Geheimdiensthintergrund hat, kann es spioniert werden
					if (buildingInfo.GetShipYard() || buildingInfo.GetSPProd() > NULL || buildingInfo.GetBarrack() || buildingInfo.GetBarrackSpeed() > NULL ||
						buildingInfo.GetEconomySabotageBoni() > NULL || buildingInfo.GetEconomySpyBoni() > NULL || buildingInfo.GetGroundDefend() > NULL ||
						buildingInfo.GetGroundDefendBoni() > NULL || buildingInfo.GetMilitarySabotageBoni() > NULL || buildingInfo.GetMilitarySpyBoni() > NULL ||
						buildingInfo.GetResistance() > NULL || buildingInfo.GetScanPower() > NULL || buildingInfo.GetScanPowerBoni() > NULL || buildingInfo.GetScanRange() > NULL ||
						buildingInfo.GetScanRangeBoni() > NULL || buildingInfo.GetSecurityBoni() > NULL || buildingInfo.GetShieldPower() > NULL ||
						buildingInfo.GetShieldPowerBoni() > NULL || buildingInfo.GetShipBuildSpeed() > NULL || buildingInfo.GetShipDefend() > NULL || buildingInfo.GetShipDefendBoni() > NULL ||
						buildingInfo.GetShipTraining() > NULL || buildingInfo.GetTroopBuildSpeed() > NULL || buildingInfo.GetTroopTraining() > NULL)
					{
						int id = buildingInfo.GetRunningNumber();
						int n = m_pDoc->GetSystem(sectors.GetAt(random).x, sectors.GetAt(random).y).GetNumberOfBuilding(id);
						CMilitaryIntelObj* report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(sectors.GetAt(random)), id, n, TRUE, FALSE, FALSE);
						// Intelreport dem Akteur hinzufügen
						if (report)
						{
							if (createText)
								report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
							pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
BOOLEAN CIntelCalc::ExecuteDiplomacySpy(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText)
{
	/*
	Es gibt bis jetzt vier verschiedene Arten der Diplomatiespionage. Zuerst kann spioniert werden, ob eine Majorrace eine
	Minorrace kennt. Zweitens kann spioniert werden, welchen Vertrag eine Majorrace mit einer bekannten Minorrace aufrecht-
	erhält. Drittens kann spioniert werden, welchen Vertrag inkl. Rundendauer eine Majorrace mit einer anderen Majorrace
	besitzt und viertens kann in Erfahrung gebracht werden, welche Beziehung die Rassen untereinander haben.
	*/

	CArray<CString> minors;

	map<CString, CMinor*>* pmMinors = m_pDoc->GetRaceCtrl()->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		if (pEnemyRace->IsRaceContacted(it->first))
			minors.Add(it->first);
	CArray<CString> majors;
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->first != pRace->GetRaceID() && it->first != pEnemyRace->GetRaceID())
			if (pEnemyRace->IsRaceContacted(it->first))
				majors.Add(it->first);
	// 4. Versuch: Beziehungsspionage
	if (rand()%3 == NULL)
	{
		// entweder wird die Minorrace oder die Majorrace ausspioniert
		CDiplomacyIntelObj* report = NULL;
		if (rand()%2 == NULL && minors.GetSize())
		{
			CString minor = minors.GetAt(rand()%minors.GetSize());
			CPoint ko = (*pmMinors)[minor]->GetRaceKO();
			if (ko != CPoint(-1,-1))
				report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, ko, DIPLOMATIC_AGREEMENT::NONE, (*pmMinors)[minor]->GetRelation(pEnemyRace->GetRaceID()));
		}
		else if (majors.GetSize())
		{
			CString major = majors.GetAt(rand()%majors.GetSize());
			report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, major,
				DIPLOMATIC_AGREEMENT::NONE, 0, (*pmMajors)[major]->GetRelation(pEnemyRace->GetRaceID()));
		}
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 3, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
			CString major = majors.GetAt(rand()%majors.GetSize());
			DIPLOMATIC_AGREEMENT::Typ agreement = DIPLOMATIC_AGREEMENT::NONE;
			short duration = 0;
			// hier kann entweder ein normaler Vertrag, oder auch ein Verteidigungspakt spioniert werden
			if (rand()%3 == NULL && pEnemyRace->GetDefencePact(major))
			{
				agreement = DIPLOMATIC_AGREEMENT::DEFENCEPACT;
				duration = pEnemyRace->GetDefencePactDuration(major);
			}
			else
			{
				agreement = pEnemyRace->GetAgreement(major);
				duration = pEnemyRace->GetAgreementDuration(major);
			}

			CDiplomacyIntelObj* report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, major, agreement,
				duration, (*pmMajors)[major]->GetRelation(pEnemyRace->GetRaceID()));
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
			CDiplomacyIntelObj* report = NULL;
			// in random steht eine zufällig ermittelte Minorrace, in welchem spioniert werden kann.
			CString minor = minors.GetAt(rand()%minors.GetSize());
			CPoint ko = (*pmMinors)[minor]->GetRaceKO();
			if (ko != CPoint(-1,-1))
				 report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, ko, (*pmMinors)[minor]->GetAgreement(pEnemyRace->GetRaceID()), (*pmMinors)[minor]->GetRelation(pEnemyRace->GetRaceID()));
			// Intelreport dem Akteur hinzufügen
			if (report)
			{
				if (createText)
					report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
		CString minor = minors.GetAt(rand()%minors.GetSize());
		CDiplomacyIntelObj* report = NULL;
		CPoint ko = (*pmMinors)[minor]->GetRaceKO();
		if (ko != CPoint(-1,-1))
			report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, ko);
		// Intelreport dem Akteur hinzufügen
		if (report)
		{
			if (createText)
				report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
BOOLEAN CIntelCalc::ExecuteEconomySabotage(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteEconomySpy(pRace, pEnemyRace, FALSE);
		newReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CEcoIntelObj* report = dynamic_cast<CEcoIntelObj*>(pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber));
		int credits = report->GetCredits() / 2;
		// 1. Möglichkeit: Credits stehlen
		if (credits > NULL)
		{
			credits = rand()%credits + 1;
			pRace->GetEmpire()->SetCredits(credits);
			pEnemyRace->GetEmpire()->SetCredits(-credits);
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, credits);
			if (report)
			{
				report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
				pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
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
				CPoint ko = report->GetCo();
				BYTE currentHabs = (BYTE)m_pDoc->GetSystem(ko.x, ko.y).GetCurrentHabitants();
				// maximal die Hälfte der Bevölkerung kann getötet werden
				currentHabs /= 2;
				if (currentHabs > NULL)
				{
					currentHabs = rand()%currentHabs + 1;
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko, 0, (BYTE)currentHabs);
					m_pDoc->GetSystem(ko.x, ko.y).LetPlanetsShrink(-(float)currentHabs);
					if (report)
					{
						report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
						pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
						pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
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
			CPoint ko = report->GetCo();
			int id = report->GetID();
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			int destroyed = 0;
			BuildingArray* allBuildings = m_pDoc->GetSystem(ko.x, ko.y).GetAllBuildings();
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
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CEcoIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed);
				if (report)
				{
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CEcoIntelObj(*report));
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
		pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion führt eine Forschungssabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteScienceSabotage(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteScienceSpy(pRace, pEnemyRace, FALSE);
		newReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CScienceIntelObj* report = dynamic_cast<CScienceIntelObj*>(pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber));

		// 4. Versuch: Spezialforschungen
		// ->	kann nicht sabotiert werden, d.h. wenn der Report etwas mit Spezialforschungen zu tun hatte, dann wird er
		//		sofort gelöscht und es wird versucht einen neuen zu erstellen. Solange bis nix mit Spezialforschungen drin
		//		vorkommt.
		int specialTech = report->GetSpecialTechComplex();
		while (specialTech != -1)
		{
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			oldReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
			BOOLEAN isTrue = FALSE;
			do {
				isTrue = this->ExecuteScienceSpy(pRace, pEnemyRace, FALSE);
			} while (isTrue == FALSE);
			report = dynamic_cast<CScienceIntelObj*>(pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber));
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
			case 0: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetBioFP(); break;
			case 1: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetEnergyFP(); break;
			case 2: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetComputerFP(); break;
			case 3: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetConstructionFP(); break;
			case 4: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetPropulsionFP(); break;
			case 5: currentFP = pEnemyRace->GetEmpire()->GetResearch()->GetWeaponFP(); break;
			}
			// die aktuell schon erforschten Forschungspunkte zufällig neu bestimmen [0, aktuell erforschte FP]
			if (currentFP > NULL)
			{
				currentFP = rand()%currentFP + 1;
				short techLevel = report->GetTechLevel();
				pEnemyRace->GetEmpire()->GetResearch()->SetFP(techType, currentFP);
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, techLevel, techType, RESEARCH_COMPLEX::NONE, -1);
				if (report)
				{
					report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
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
		if (fp > (int)pEnemyRace->GetEmpire()->GetFP())
			fp = pEnemyRace->GetEmpire()->GetFP();
		if (fp > NULL)
		{
			pRace->GetEmpire()->AddFP(fp);
			pEnemyRace->GetEmpire()->AddFP(-fp);
			pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
			report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, fp);
			if (report)
			{
				report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
				pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
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
			CPoint ko = report->GetCo();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			BuildingArray* allBuildings = m_pDoc->GetSystem(ko.x, ko.y).GetAllBuildings();
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
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CScienceIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed);
				if (report)
				{
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CScienceIntelObj(*report));
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
		pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion führt eine Militärsabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteMilitarySabotage(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteMilitarySpy(pRace, pEnemyRace, FALSE);
		newReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CMilitaryIntelObj* report = dynamic_cast<CMilitaryIntelObj*>(pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber));

		// 3. Versuch: Schiffe beschädigen/zerstören/stehlen oder Stationen beschädigen/zerstören
		if (report->GetIsShip())
		{
			boost::shared_ptr<CShips> ship;
			// überprüfen, ob die jeweilige Station oder das jeweilige Schiff auch noch im System vorhanden ist
			std::vector<CPoint> allShips;//x: ship key in global shipmap; y: ship key within another ship's fleet
			for(CShipMap::const_iterator i = m_pDoc->m_ShipMap.begin(); i != m_pDoc->m_ShipMap.end(); ++i)
			{
				if (i->second->GetCo() == report->GetCo() && i->second->GetOwnerOfShip() != report->GetOwner())
				{
					// besitzt dieses Schiff eine Flotte, so könnte sich unser Schiff auch in der Flotte befinden
					for(CShips::const_iterator j = i->second->begin(); j != i->second->end(); ++j)
					{
						if (j->second->GetID() == report->GetID())
							allShips.push_back(CPoint(i->second->Key(), j->second->Key()));
					}
					if (i->second->GetID() == report->GetID())
						allShips.push_back(CPoint(i->second->Key(),-1));	// -1 als y Wert bedeutet, dass dieses Schiff in keiner Flotte vorkommt
				}
			}
			// aus allen möglichen Schiffen, welche die ID unseres Reports und im richtigen Sektor sind eins zufällig aussuchen
			CPoint n(-1,0);
			if (!allShips.empty())
			{
				int random = rand()%allShips.size();
				n = allShips.at(random);
				// ist das Schiff in keiner Flotte sondern direkt im Feld zu finden
				boost::shared_ptr<CShips>& s = m_pDoc->m_ShipMap.at(n.x);
				if (n.y == -1)
					ship = s;
				else
					ship = s->at(n.y);
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
			if (rand()%2 == 0 && !ship->IsStation())
			{
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ship->GetCo(), ship->GetID(), 1, FALSE, TRUE, FALSE);
				// die Station aus der ShipHistory der aktuellen Schiffe entfernen und den zerstörten Schiffen hinzufügen
				pEnemyRace->GetShipHistory()->ModifyShip(ship, m_pDoc->GetSystem(ship->GetCo().x, ship->GetCo().y).GetLongName(),
					m_pDoc->GetCurrentRound(), CLoc::GetString("SABOTAGE"), CLoc::GetString("MISSED"));

				// neuen Besitzer hinzufügen
				ship->SetOwnerOfShip(pRace->GetRaceID());
				ship->SetIsShipFlagShip(FALSE);
				ship->UnsetCurrentOrder();
				ship->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				// gestohlenes Schiff zum nächsten eigenen System verschieben
				CPoint oldKO = ship->GetCo();
				CPoint newKO = ship->GetCo();
				short minDist = MAXSHORT;
				for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						if (m_pDoc->GetSystem(x, y).GetShipPort(pRace->GetRaceID()))
							if (minDist > min(abs(oldKO.x - x), abs(oldKO.y - y)))
							{
								minDist = (short)min(abs(oldKO.x - x), abs(oldKO.y - y));
								newKO = CPoint(x,y);
							}
				ship->SetKO(newKO.x, newKO.y);
				ship->SetTargetKO(CPoint(-1, -1));
				// wurde dieses Schiff jedoch schonmal gestohlen, dann ist es in der Missed Shiphistory. Ist dies der Fall kann das Schiff
				// wieder als aktives Schiff betrachtet werden.
				if (pRace->GetShipHistory()->ModifyShip(ship, m_pDoc->GetSystem(ship->GetCo().x, ship->GetCo().y).GetLongName(), 0) == false)
					// dem neuen Besitzer das Schiff als aktives Schiff hinzufügen
					pRace->GetShipHistory()->AddShip(ship, m_pDoc->GetSystem(ship->GetCo().x, ship->GetCo().y).GetLongName(), m_pDoc->GetCurrentRound());

				// jetzt Dinge wegen einer möglichen Flotte beachten
				if (n.y == -1)		// Schiff nicht in Flotte
				{
					// das Schiff selbst kann aber eine Flotte anführen
					// Wenn das Schiff eine Flotte besaß, so geht die Flotte auf das erste Schiff in der Flotte über
					if (ship->HasFleet())
					{
						const boost::shared_ptr<CShips>& new_fleetship = ship->GiveFleetToFleetsFirstShip();
						// neues Flottenschiff dem Array hinzufügen
						m_pDoc->m_ShipMap.Add(new_fleetship);
						// Schiff nochmal neu holen, da der Vektor verändert wurde und so sich auch der Zeiger ändern kann
						ship = m_pDoc->m_ShipMap.at(n.x);
					}
				}
				else	// Schiff ist in Flotte
				{
					m_pDoc->m_ShipMap.Add(ship);
					const boost::shared_ptr<CShips>& old_leader = m_pDoc->m_ShipMap.at(n.x);
					CShips::iterator stolen_ship = old_leader->find(n.y);
					AssertBotE(stolen_ship != old_leader->end());
					old_leader->RemoveShipFromFleet(stolen_ship);
				}
				if (report)
				{
					report->CreateText(m_pDoc, 3, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
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
			else if ((rand()%4 == NULL && ship->IsStation())
				|| (rand()%2 == NULL && !ship->IsStation()))
			{
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ship->GetCo(), ship->GetID(), 1, FALSE, TRUE, FALSE);
				// die Station aus der ShipHistory der aktuellen Schiffe entfernen und den zerstörten Schiffen hinzufügen
				pEnemyRace->GetShipHistory()->ModifyShip(ship, m_pDoc->GetSystem(ship->GetCo().x, ship->GetCo().y).GetLongName(),
					m_pDoc->GetCurrentRound(), CLoc::GetString("SABOTAGE"), CLoc::GetString("DESTROYED"));
				if (n.y == -1)		// nicht in Flotte
				{
					CShips::iterator destroyed_ship = m_pDoc->m_ShipMap.find(n.x);
					AssertBotE(destroyed_ship != m_pDoc->m_ShipMap.end());
					m_pDoc->RemoveShip(destroyed_ship);
				}
				else	// Schiff ist in Flotte
				{
					const boost::shared_ptr<CShips>& s = m_pDoc->m_ShipMap.at(n.x);
					CShips::iterator destroyed_ship = s->find(n.y);
					AssertBotE(destroyed_ship != s->end());
					s->RemoveShipFromFleet(destroyed_ship);
				}
				if (report)
				{
					report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
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
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ship->GetCo(), ship->GetID(), 1, FALSE, TRUE, FALSE);
					if (report)
					{
						report->CreateText(m_pDoc, 4, pResponsibleRace->GetRaceID());
						pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
						pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
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
			CPoint ko = report->GetCo();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Truppe/Truppen auf dem jeweiligen System zerstören
			CArray<CTroop>* allTroops = m_pDoc->GetSystem(ko.x, ko.y).GetTroops();
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
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed, FALSE, FALSE, TRUE);
				if (report)
				{
					report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
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
			buildings = rand()%buildings + 1;
			CPoint ko = report->GetCo();
			int id = report->GetID();
			int destroyed = 0;
			// jetzt die Gebäude auf dem jeweiligen System zerstören
			BuildingArray* allBuildings = m_pDoc->GetSystem(ko.x, ko.y).GetAllBuildings();
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
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CMilitaryIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko, id, destroyed, TRUE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CMilitaryIntelObj(*report));
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
		pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion führt eine Diplomatiesabotageaktion aus.
BOOLEAN CIntelCalc::ExecuteDiplomacySabotage(CMajor* pRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber)
{
	// Es wird die zugehörige Spionageaktion durchgeführt. Diese generiert uns einen Geheimdienstreport.
	// Damit es schneller geht wird auf die Textgenerierung verzichtet. Sobald der neue Geheimdienstbericht
	// vorliegt wird aus dessen Informationen die Sabotageaktion generiert. Danach wird der alte Spionagebericht
	// entfernt und durch den neuen Sabotagebericht ersetzt.
	int oldReportNumber = reportNumber;
	int newReportNumber = oldReportNumber + 1;
	if (reportNumber == -1)
	{
		oldReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
		this->ExecuteDiplomacySpy(pRace, pEnemyRace, FALSE);
		newReportNumber = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports();
	}
	// gab es eine erfolgreiche Spionageaktion, so kann diese jetzt in eine Sabotageaktion umgewandelt werden
	if (newReportNumber > oldReportNumber)
	{
		CDiplomacyIntelObj* report = dynamic_cast<CDiplomacyIntelObj*>(pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(oldReportNumber));
		// Bei der Diplomatiesabotage kann entweder die Beziehung zwischen zwei Majorraces oder zwischen
		// einer Majorrace und einer Minorrace verschlechtert werden. Eine andere Möglichkeit wäre die Beziehung
		// zwischen uns und einer Minorrace oder einer Majorrace zu verbessern.

		// zwischen Major <-> Major
		if (report->GetMajorRaceID() != "")
		{
			CString major = report->GetMajorRaceID();
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(major));
			AssertBotE(pMajor);
			// hier zwei Möglichkeiten: verschlechtern der Beziehung zwischen Geheimdienstopfer und Major oder Verbesserung
			// der Beziehung zwischen uns und dem Geheimdienstopfer

			// zu uns selbst verbessern
			if (rand()%5 == NULL)
			{
				int relationAdd = rand()%20 + 1;
				pEnemyRace->SetRelation(pRace->GetRaceID(), relationAdd);
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, pEnemyRace->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 0, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
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
				if (pRace->AHumanPlays() == false && pEnemyRace->AHumanPlays() == false	&& pMajor->AHumanPlays() == false)
					relationSub /= 4;
				pEnemyRace->SetRelation(pMajor->GetRaceID(), relationSub);
				pMajor->SetRelation(pEnemyRace->GetRaceID(), relationSub);
				pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
				report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, major, DIPLOMATIC_AGREEMENT::NONE, FALSE, FALSE);
				if (report)
				{
					report->CreateText(m_pDoc, 1, pResponsibleRace->GetRaceID());
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
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
			CMinor* minor = m_pDoc->GetRaceCtrl()->GetMinorRace(m_pDoc->GetSystem(report->GetMinorRaceKO().x, report->GetMinorRaceKO().y).GetName());
			if (minor)
			{
				// hier zwei Möglichkeiten: verschlechtern der Beziehung zwischen Geheimdienstopfer und Minor oder Verbesserung
				// der Beziehung zwischen uns und der Minor
				// zu uns selbst verbessern -> kein Bericht bei anderer Majorrace
				if (rand()%2 == NULL && minor->IsRaceContacted(pRace->GetRaceID()) == true && minor->GetRelation(pRace->GetRaceID()) < 90)
				{
					int relationAdd = rand()%20 + 1;
					minor->SetRelation(pRace->GetRaceID(), relationAdd);
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					CPoint ko = minor->GetRaceKO();
					if (ko != CPoint(-1,-1))
						report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko);

					if (report)
					{
						report->CreateText(m_pDoc, 2, pResponsibleRace->GetRaceID());
						pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
					int relationSub = -rand()%20 + 1;
					minor->SetRelation(pEnemyRace->GetRaceID(), relationSub);
					pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
					CPoint ko = minor->GetRaceKO();
					if (ko != CPoint(-1,-1))
						report = new CDiplomacyIntelObj(pRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), FALSE, ko);

					if (report)
					{
						report->CreateText(m_pDoc, 3, pResponsibleRace->GetRaceID());
						pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
						pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(new CDiplomacyIntelObj(*report));
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
		pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveReport(oldReportNumber);
	}
	return FALSE;
}

/// Funktion erstellt den Report, welcher aussagt, dass versucht wurde eine Rasse auszuspionieren/auszusabotieren.
void CIntelCalc::CreateMsg(CMajor* pResponsibleRace, CMajor* pEnemyRace, BYTE type)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntel.data";				// Name des zu Öffnenden Files
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			// Rasse bestimmen
			if (s == pEnemyRace->GetRaceID())
			{
				csInput.Delete(0, pos);
				// in csInput steht nun die Beschreibung
				// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
				s = pResponsibleRace->GetEmpireNameWithAssignedArticle();
				csInput.Replace("$race$", s);
				CIntelObject* report = NULL;
				switch (type)
				{
				case 0: report = new CEcoIntelObj(pResponsibleRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, 0); break;
				case 1: report = new CScienceIntelObj(pResponsibleRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, 0); break;
				case 2: report = new CMilitaryIntelObj(pResponsibleRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(-1,-1), 0, 0, FALSE, FALSE, FALSE); break;
				case 3: report = new CDiplomacyIntelObj(pResponsibleRace->GetRaceID(), pEnemyRace->GetRaceID(), m_pDoc->GetCurrentRound(), TRUE, CPoint(-1,-1)); break;
				}
				if (report)
				{
					report->SetEnemyDesc(csInput);
					pEnemyRace->GetEmpire()->GetIntelligence()->GetIntelReports()->AddReport(report);
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
BOOLEAN CIntelCalc::ExecuteAttempt(CMajor* pRace, UINT ourSP)
{
	AssertBotE(pRace);
	CMajor* pResponsibleRace = pRace;
	// Ist ein Anschlagsobjekt vorhanden?
	if (pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAttemptObject())
	{
		CIntelObject* attemptObj = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAttemptObject();
		CMajor* pEnemy = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(attemptObj->GetEnemy()));
		if (!pEnemy)
			return false;
		this->DeleteConsumedPoints(pRace, pEnemy, FALSE, attemptObj->GetType(), TRUE);
		// bei Anschlägen werden die benötigten Punkte etwas reduziert, indem hier einfach gesagt wird, dass es sich
		// um Spionage handeln würde.
		if (IsSuccess(pEnemy, ourSP, TRUE, pResponsibleRace, attemptObj->GetType()))
		{
			// Die Moral verschlechtert sich beim betroffenen Imperium gegebüber der responsible Race
			if (pResponsibleRace)
				pEnemy->SetRelation(pResponsibleRace->GetRaceID(), -rand()%10);
			// wenn der Anschlag erfolgreich durchgeführt werden konnte, muss der dazugehörige Spionagereport aus dem
			// Feld gelöscht werden
			for (int i = 0; i < pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
			{
				CIntelObject* intelObj = pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(i);
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
									case 0: returnValue = this->ExecuteEconomySabotage(pRace, pEnemy, pResponsibleRace, i); break;
									case 1: returnValue = this->ExecuteScienceSabotage(pRace, pEnemy, pResponsibleRace, i); break;
									case 2: returnValue = this->ExecuteMilitarySabotage(pRace, pEnemy, pResponsibleRace, i); break;
									case 3: returnValue = this->ExecuteDiplomacySabotage(pRace, pEnemy, pResponsibleRace, i); break;
									}
									intelObj = NULL;
									pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
									return returnValue;
								}
			}
		}
	}
	pRace->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
	return FALSE;
}

/// Funktion gibt die aktuell komplett generierten inneren Sicherheitspunkte eines Imperiums zurück.
UINT CIntelCalc::GetCompleteInnerSecPoints(CMajor* pEnemyRace)
{
	AssertBotE(pEnemyRace);

	CIntelligence* pIntel = pEnemyRace->GetEmpire()->GetIntelligence();

	// Depot der inneren Sicherheitspunkte + aktuell produzierte Sicherheitspunkte * prozentuale Zuweisung + Bonus auf innere Sicherheit + prozentuale Zuteilung
	int enemyInnerSec = pIntel->GetSecurityPoints() * pIntel->GetAssignment()->GetInnerSecurityPercentage() / 100;
	// + Bonus auf innere Sicherheit (Bonus könnte negativ sein, daher nicht unter NULL)
	enemyInnerSec += enemyInnerSec * pIntel->GetInnerSecurityBoni() / 100;
	// + prozentuale Zuteilung
	enemyInnerSec += pIntel->GetAssignment()->GetInnerSecurityPercentage();
	// + Punkte aus dem Lager (darin ist der Bonus schon vorhanden)
	enemyInnerSec += pIntel->GetInnerSecurityStorage();

	return max(0, enemyInnerSec);
}
