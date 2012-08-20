#include "stdafx.h"
#include "IntelAI.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

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

	// Struktur für eine Liste mit Rassen-ID und Geheimdienstpunkten
	struct INTELLIST {
		CString sRace;
		UINT points;

		bool operator< (const INTELLIST& elem2) const { return points < elem2.points;}
		bool operator> (const INTELLIST& elem2) const { return points > elem2.points;}
		INTELLIST() : sRace(""), points(0) {}
		INTELLIST(const CString& _sRace, UINT _points) : sRace(_sRace), points(_points) {}
	};

	// produzierte Geheimdienstpunkte und Punkte aus allen Geheimdienstlagern einer Rasse zusammenaddieren
	CArray<INTELLIST> intellist;
	std::map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CIntelligence* pIntel = it->second->GetEmpire()->GetIntelligence();
		UINT points = pIntel->GetSecurityPoints() + pIntel->GetInnerSecurityStorage();
		for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			if (itt->first != it->first)
				points += pIntel->GetSPStorage(0, itt->first) + pIntel->GetSPStorage(1, itt->first);
		intellist.Add(INTELLIST(it->first, points));
	}
	// nun Feld nach den gesammten Geheimdienstpunkten absteigend sortiren lassen.
	c_arraysort<CArray<INTELLIST>, INTELLIST> (intellist, sort_desc);
	// unere Priorität ist der Index der Rasse im Feld.
	// wenn die Punkte sich nicht mehr als 10% bzw. 100SP unterscheiden, dann wird die Priorität des vorherigen
	// Indexes benutzt.
	m_byIntelPrio[intellist.GetAt(0).sRace] = rand()%2;
	for (int i = 1; i < intellist.GetSize(); i++)	// beim zweiten Index starten! Da das erste Element eine 0er Priorität hat
	{
		if (intellist.GetAt(i-1).points - intellist.GetAt(i).points > 100
			&& (intellist.GetAt(i).points * 100 / (intellist.GetAt(i-1).points+1) < 90))
			m_byIntelPrio[intellist.GetAt(i).sRace] = i;
		// ansonsten ist die Priorität der des Vorgängers
		else
			m_byIntelPrio[intellist.GetAt(i).sRace] = m_byIntelPrio[intellist.GetAt(i-1).sRace];
	}

	// jeder Geheimdienstbericht mit uns als Ziel aus den letzten 5 Runden erhöht die Priorität nochmal um eins, sofern
	// es sich dabei um eine Sabotageaktion handelte
	std::map<CString, int> badReports;
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->second->IsHumanPlayer() == false)
		{
			CIntelligence* pIntel = it->second->GetEmpire()->GetIntelligence();
			for (int l = 0; l < pIntel->GetIntelReports()->GetNumberOfReports(); l++)
			{
				CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(l);
				if (intelObj->GetEnemy() == it->first && pDoc->GetCurrentRound() - intelObj->GetRound() < 6 && intelObj->GetIsSabotage())
					badReports[it->first] += 1;
			}
			m_byIntelPrio[it->first] += badReports[it->first];
		}

	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		MYTRACE_DOMAIN("intelai")(MT::LEVEL_INFO, "Intel-AI: Intel Prio of %s is %d\n", it->first, m_byIntelPrio[it->first]);

	// nun liegen die Prioritäten und die Listen mit den Punkten vor. Jetzt kann begonnen werde die Rassen zu
	// vergeheimdiensteln. Ab hier kommt die KI für den Geheimdienst richtig ins Spiel.
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (it->second->IsHumanPlayer() == false)
		{
			CIntelligence* pIntel = it->second->GetEmpire()->GetIntelligence();
			// wenn in den letzten 5 Runden Geheimdienstberichte mit uns als Ziel vorliegen, so wird die innere
			// Sicherheit maximiert
			if (badReports[it->first] > NULL)
			{
				// Es liegt mindst. ein Bericht mit uns als Ziel aus den letzten 5 Runden vor.
				// Dann wird die Innere Sicherheit auf 100% gesetzt
				pIntel->SetAssignment()->SetGlobalPercentage(2, 100, it->second, "", pmMajors);
				continue;
			}

			// wenn die innere Sicherheit nicht verändert werden musste, dann können wir vielleicht selbst aktiv werden
			// KI benutzt nur Sabotage

			// Wie wird ein mögliches Geheimdienstopfer ermittelt?
			// - haben die schlechteste Beziehung zum Opfer
			// - Beziehung unter 50% oder aktueller Vertrag kleiner Freundschaft und kein Verteidigungsbündnis
			USHORT worstRel = MAXBYTE;
			CMajor* pWorstRace = NULL;
			for (std::map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); ++jt)
				if (jt->first != it->first && it->second->IsRaceContacted(jt->first) == true)
				{
					// zufällig wird hier eine bekannte andere Rasse als ResponsibleRace ausgewählt
					pIntel->SetResponsibleRace(it->first);	// erstmal uns wieder auf die ResponsibleRace setzen
					if (jt->second->GetEmpire()->GetNumberOfSystems() > 0 && rand()%3 == NULL)
					{
						pIntel->SetResponsibleRace(jt->first);
						break;
					}

					// vertragliche Situation und Mindestbeziehung checken
					if ((it->second->GetAgreement(jt->first) < DIPLOMATIC_AGREEMENT::FRIENDSHIP && it->second->GetDefencePact(jt->first) == false)
						||	it->second->GetRelation(jt->first) < 50)
					{
						// schlechteste Beziehung ermitteln
						if (it->second->GetRelation(jt->first) < worstRel)
						{
							worstRel = it->second->GetRelation(jt->first);
							pWorstRace = jt->second;
						}
						// bei Gleichheit zu 50% die neue Rasse mit schlechtester Bezeihung
						else if (it->second->GetRelation(jt->first) == worstRel && rand()%2 == NULL)
						{
							worstRel = it->second->GetRelation(jt->first);
							pWorstRace = jt->second;
						}
					}
				}

			// jetzt der ausgesuchten Rasse Geheimdienstpunkte zuweisen
			if (pWorstRace != NULL)
			{
				CIntelligence* pWorstIntel = pWorstRace->GetEmpire()->GetIntelligence();
				MYTRACE_DOMAIN("intelai")(MT::LEVEL_INFO, "Intel-AI: assigned intel victim of %s is %s\n", it->first, pWorstRace->GetRaceID());
				// jede Rasse läßt immer einen bestimmten prozentualen Anteil in der inneren Sicherheit.
				int innerSecPerc = 25;

				if (it->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
					innerSecPerc += 15;
				if (it->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
					innerSecPerc += 25;
				if (it->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
					innerSecPerc += 25;
				if (it->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
					innerSecPerc += 10;
				if (it->second->IsRaceProperty(RACE_PROPERTY::SECRET))
					innerSecPerc -= 10;
				if (it->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
					innerSecPerc += 0;
				if (it->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
					innerSecPerc += 5;
				if (it->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
					innerSecPerc += 35;
				if (it->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
					innerSecPerc -= 15;
				if (it->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
					innerSecPerc += 40;
				if (it->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
					innerSecPerc += 0;

				if (innerSecPerc > 100)
					innerSecPerc = 100;
				else if (innerSecPerc < 0)
					innerSecPerc = 0;

				if (pIntel->GetAssignment()->GetGlobalSabotagePercentage(pWorstRace->GetRaceID()) != 100 - innerSecPerc)
					pIntel->SetAssignment()->SetGlobalPercentage(2, 100, it->second, "", pmMajors);
				pIntel->SetAssignment()->SetGlobalPercentage(1, 100 - innerSecPerc, it->second, pWorstRace->GetRaceID(), pmMajors);

				// Wann wird die Geheimdiensaktion gestartet
				// - wenn unsere Geheimdienstpunkte + Punkte aus Depot > gegnerische Innere Sicherheit + deren Inneres Depot

				int type = rand()%4;	// Typ der Aktion (Wirtschaft, Wissenschaft, Militär oder Diplomatie)
				UINT ourPoints = pIntel->GetSecurityPoints()
					* pIntel->GetAssignment()->GetGlobalSabotagePercentage(pWorstRace->GetRaceID()) / 100
					+ pIntel->GetSPStorage(1, pWorstRace->GetRaceID()) * pIntel->GetAssignment()->GetSabotagePercentages(pWorstRace->GetRaceID(), type) / 100;
				ourPoints += ourPoints * pIntel->GetBonus(type, 1) / 100;

				UINT enemyPoints = pWorstIntel->GetSecurityPoints()
					* pWorstIntel->GetAssignment()->GetInnerSecurityPercentage() / 100;
				// + Bonus auf innere Sicherheit
				enemyPoints += enemyPoints * pWorstIntel->GetInnerSecurityBoni() / 100;
				// + Punkte aus dem Lager (darin ist der Bonus schon vorhanden)
				enemyPoints += pWorstIntel->GetInnerSecurityStorage();
				if (ourPoints > enemyPoints + rand()%1500)
				{
					// zuerst komplette Zuteilung ins Lager übernehmen, damit man dann auch wirklich 100% einem einzelnen
					// Ressort zuweisen kann
					pIntel->SetAssignment()->SetSabotagePercentage(4, 100, pWorstRace->GetRaceID());
					pIntel->SetAssignment()->SetSabotagePercentage(type, 100, pWorstRace->GetRaceID());
				}
				else
					pIntel->SetAssignment()->SetSabotagePercentage(4, 100, pWorstRace->GetRaceID());
				MYTRACE_DOMAIN("intelai")(MT::LEVEL_INFO, "Intel-AI: our SP: %d - enemies SP: %d\n", ourPoints, enemyPoints);

			}
			// finden wir keine Rasse zum vergeheimdiensteln, so die innere Sicherheit auf 100% stellen
			else
				pIntel->SetAssignment()->SetGlobalPercentage(2, 100, it->second, "", pmMajors);
		}
	}
}

/// Resetfunktion für ein CIntelAI-Objekt.
void CIntelAI::Reset(void)
{
	m_byIntelPrio.clear();
}
