#include "stdafx.h"
#include "IntelAI.h"
#include "Botf2Doc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelAI::CIntelAI(void)
{
	Reset();
}

CIntelAI::~CIntelAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion berechnet den Umgang mit dem Geheimdienst für die KI. Sie trifft Zuteilungen für die einzelnen Rassen.
void CIntelAI::CalcIntelligence(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	
	// Struktur für eine Liste mit Rassennummer und Geheimdienstpunkten
	struct INTELLIST {
		BYTE race;
		UINT points;
		
		bool operator< (const INTELLIST& elem2) const { return points < elem2.points;}
		bool operator> (const INTELLIST& elem2) const { return points > elem2.points;}
		INTELLIST() : race(NOBODY), points(0) {}
		INTELLIST(BYTE _race, UINT _points) : race(_race), points(_points) {}
	};

	// produzierte Geheimdienstpunkte und Punkte aus allen Geheimdienstlagern einer Rasse zusammenaddieren
	CArray<INTELLIST> intellist;
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		UINT points = pDoc->GetEmpire(i)->GetIntelligence()->GetSecurityPoints()
			+ pDoc->GetEmpire(i)->GetIntelligence()->GetInnerSecurityStorage();
		for (int j = HUMAN; j <= DOMINION; j++)
			if (j != i)
				points += pDoc->GetEmpire(i)->GetIntelligence()->GetSPStorage(0,j) + pDoc->GetEmpire(i)->GetIntelligence()->GetSPStorage(1,j);
		intellist.Add(INTELLIST(i, points));
	}
	// nun Feld nach den gesammten Geheimdienstpunkten absteigend sortiren lassen.		
	c_arraysort<CArray<INTELLIST>, INTELLIST> (intellist, sort_desc);
	// unere Priorität ist der Index der Rasse im Feld.
	// wenn die Punkte sich nicht mehr als 10% bzw. 100SP unterscheiden, dann wird die Priorität des vorherigen
	// Indexes benutzt.
	m_byIntelPrio[intellist.GetAt(0).race-1] = rand()%2;
	for (int i = 1; i < intellist.GetSize(); i++)	// beim zweiten Index starten! Da das erste Element eine 0er Priorität hat
	{
		if (intellist.GetAt(i-1).points - intellist.GetAt(i).points > 100			
			&& (intellist.GetAt(i).points * 100 / (intellist.GetAt(i-1).points+1) < 90))
			m_byIntelPrio[intellist.GetAt(i).race-1] = i;
		// ansonsten ist die Priorität der des Vorgängers
		else
			m_byIntelPrio[intellist.GetAt(i).race-1] = m_byIntelPrio[intellist.GetAt(i-1).race-1];
	}

	// kennt die Rasse keine andere Majorrace, so ist die Priorität auch viel geringer. Sie ist maximal 1.
//	for (int i = HUMAN; i <= DOMINION; i++)
//		if (pDoc->GetMajorRace(i)->GetNumberOfKnownMajorRaces(i) == NULL)
//			m_byIntelPrio[i-1] = rand()%2;	

	// jeder Geheimdienstbericht mit uns als Ziel aus den letzten 5 Runden erhöht die Priorität nochmal um eins, sofern
	// es sich dabei um eine Sabotageaktion handelte
	int badReports[DOMINION] = {0};
	for (int i = HUMAN; i <= DOMINION; i++)
		if (pDoc->GetEmpire(i)->GetPlayerOfEmpire() == COMPUTER)
		{
			for (int l = 0; l < pDoc->GetEmpire(i)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); l++)
			{
				CIntelObject* intelObj = pDoc->GetEmpire(i)->GetIntelligence()->GetIntelReports()->GetReport(l);
				if (intelObj->GetEnemy() == i && pDoc->GetCurrentRound() - intelObj->GetRound() < 6 && intelObj->GetIsSabotage())
					badReports[i-1]++;
			}
			m_byIntelPrio[i-1] += badReports[i-1];
		}
	
#ifdef TRACE_INTELAI
	for (int i = 0; i < DOMINION; i++)
		TRACE("IntelPrio von %d: %d\n", i+1, m_byIntelPrio[i]);
#endif

	// nun liegen die Prioritäten und die Listen mit den Punkten vor. Jetzt kann begonnen werde die Rassen zu
	// vergeheimdiensteln. Ab hier kommt die KI für den Geheimdienst richtig ins Spiel.
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (pDoc->GetEmpire(i)->GetPlayerOfEmpire() == COMPUTER)
		{
			// wenn in den letzten 5 Runden Geheimdienstberichte mit uns als Ziel vorliegen, so wird die innere
			// Sicherheit maximiert
			if (badReports[i-1] > NULL)
			{
				// Es liegt mindst. ein Bericht mit uns als Ziel aus den letzten 5 Runden vor.
				// Dann wird die Innere Sicherheit auf 100% gesetzt
				pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetGlobalPercentage(2, 100, pDoc->GetMajorRace(i), NOBODY, i);
				continue;
			}
						
			// wenn die innere Sicherheit nicht verändert werden musste, dann können wir vielleicht selbst aktiv werden
			// KI benutzt nur Sabotage
			
			// Wie wird ein mögliches Geheimdienstopfer ermittelt?
			// - haben die schlechteste Beziehung zum Opfer
			// - Beziehung unter 50% oder aktueller Vertrag kleiner Freundschaft und kein Verteidigungsbündnis
			USHORT worstRel = MAXBYTE;
			BYTE worstRace = NOBODY;
			for (int j = HUMAN; j <= DOMINION; j++)
				if (j != i && pDoc->GetMajorRace(i)->GetKnownMajorRace(j) == TRUE)
				{
					// zufällig wird hier eine bekannte andere Rasse als ResponsibleRace ausgewählt
					pDoc->GetEmpire(i)->GetIntelligence()->SetResponsibleRace(i);	// erstmal uns wieder auf die ResponsibleRace setzen
					if (pDoc->GetEmpire(j)->GetNumberOfSystems() > 0 && rand()%3 == NULL)
						{
							pDoc->GetEmpire(i)->GetIntelligence()->SetResponsibleRace(j);
							break;
						}

					// vertragliche Situation und Mindestbeziehung checken
					if ((pDoc->GetMajorRace(i)->GetDiplomacyStatus(j) < FRIENDSHIP_AGREEMENT && pDoc->GetMajorRace(i)->GetDefencePact(j) == FALSE)
						||	pDoc->GetMajorRace(i)->GetRelationshipToMajorRace(j) < 50)
					{
						// schlechteste Beziehung ermitteln
						if (pDoc->GetMajorRace(i)->GetRelationshipToMajorRace(j) < worstRel)
						{
							worstRel = pDoc->GetMajorRace(i)->GetRelationshipToMajorRace(j);
							worstRace = j;
						}
						// bei Gleichheit zu 50% die neue Rasse mit schlechtester Bezeihung
						else if (pDoc->GetMajorRace(i)->GetRelationshipToMajorRace(j) == worstRel && rand()%2 == NULL)
						{
							worstRel = pDoc->GetMajorRace(i)->GetRelationshipToMajorRace(j);
							worstRace = j;
						}
					}
				}

			// jetzt der ausgesuchten Rasse Geheimdienstpunkte zuweisen
			if (worstRace != NOBODY)
			{
#ifdef TRACE_INTELAI
	TRACE("Geheimdienstopfer von %d ist %d\n", i, worstRace);
#endif
				// jede Rasse läßt immer einen bestimmten prozentualen Anteil in der inneren Sicherheit.
				BYTE innerSecPerc = 25;
				switch (pDoc->GetMajorRace(i)->GetKind())
				{
					case NOTHING_SPECIAL:	innerSecPerc = 25; break;
					case FINANCIAL:			innerSecPerc = 40; break;
					case WARLIKE:			innerSecPerc = 50; break;
					case FARMER:			innerSecPerc = 50; break;
					case INDUSTRIAL:		innerSecPerc = 35; break;
					case SECRET:			innerSecPerc = 15; break;
					case RESEARCHER:		innerSecPerc = 25; break;
					case PRODUCER:			innerSecPerc = 30; break;
					case PACIFIST:			innerSecPerc = 60; break;
					case SNEAKY:			innerSecPerc = 10; break;
					default: innerSecPerc = 25;
				}
				if (pDoc->GetEmpire(i)->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(worstRace) != 100 - innerSecPerc)
					pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetGlobalPercentage(2, 100, pDoc->GetMajorRace(i), NOBODY, i);
				pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetGlobalPercentage(1, 100 - innerSecPerc, pDoc->GetMajorRace(i), worstRace, i);
			
				// Wann wird die Geheimdiensaktion gestartet
				// - wenn unsere Geheimdienstpunkte + Punkte aus Depot > gegnerische Innere Sicherheit + deren Inneres Depot
				
				int type = rand()%4;	// Typ der Aktion (Wirtschaft, Wissenschaft, Militär oder Diplomatie)
				UINT ourPoints = pDoc->GetEmpire(i)->GetIntelligence()->GetSecurityPoints() 
					* pDoc->GetEmpire(i)->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(worstRace) / 100
					+ pDoc->GetEmpire(i)->GetIntelligence()->GetSPStorage(1,worstRace) * pDoc->GetEmpire(i)->GetIntelligence()->GetAssignment()->GetSabotagePercentages(worstRace, type) / 100;
				ourPoints += ourPoints * pDoc->GetEmpire(i)->GetIntelligence()->GetBonus(type, 1) / 100;

				UINT enemyPoints = pDoc->GetEmpire(worstRace)->GetIntelligence()->GetSecurityPoints()
					* pDoc->GetEmpire(worstRace)->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage() / 100;
				// + Bonus auf innere Sicherheit
				enemyPoints += enemyPoints * pDoc->GetEmpire(worstRace)->GetIntelligence()->GetInnerSecurityBoni() / 100;
				// + Punkte aus dem Lager (darin ist der Bonus schon vorhanden)
				enemyPoints += pDoc->GetEmpire(worstRace)->GetIntelligence()->GetInnerSecurityStorage();
				if (ourPoints > enemyPoints + rand()%1500)
				{
					// zuerst komplette Zuteilung ins Lager übernehmen, damit man dann auch wirklich 100% einem einzelnen
					// Ressort zuweisen kann
					pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetSabotagePercentage(4, 100, worstRace);
					pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetSabotagePercentage(type, 100, worstRace);
				}
				else
					pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetSabotagePercentage(4, 100, worstRace);
#ifdef TRACE_INTELAI
		TRACE("ourPoints: %d - enemyPoints: %d\n", ourPoints, enemyPoints);
#endif

			}
			// finden wir keine Rasse zum vergeheimdiensteln, so die innere Sicherheit auf 100% stellen
			else
				pDoc->GetEmpire(i)->GetIntelligence()->SetAssignment()->SetGlobalPercentage(2, 100, pDoc->GetMajorRace(i), NOBODY, i);
		}
	}
}

/// Resetfunktion für ein CIntelAI-Objekt.
void CIntelAI::Reset(void)
{
	memset(m_byIntelPrio,   0, sizeof(*m_byIntelPrio)   * DOMINION);
}