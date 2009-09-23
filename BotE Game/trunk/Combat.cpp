#include "stdafx.h"
#include "Combat.h"
#include "RaceController.h"

Position GivePosition(BYTE pos, USHORT posNumber)
{
	Position p;
	p.x = rand()%30; p.y = rand()%30; p.z = rand()%30;

	// aller 6 Rassen ist die Startposition wieder ähnlich. Doch an einem Kampf werden
	// eigentlich nie mehr als 6 Rassen teilnehmen
	switch (pos%6)
	{
	case 0: p.x = 200 + posNumber*5;	break;
	case 1: p.x = -200 - posNumber*5;	break;
	case 2: p.y = 200 + posNumber*5;	break;
	case 3: p.y = -200 - posNumber*5;	break;
	case 4: p.z = 200 + posNumber*5;	break;
	case 5: p.z = -200 - posNumber*5;	break;	
	}
	return p;
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CCombat::CCombat(void)
{
	Reset();
}

CCombat::~CCombat(void)
{
	Reset();
}	

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

// Diese Funktion verlangt beim Aufruf einen Zeiger auf ein Feld, welches Zeiger auf Schiffe beinhaltet
// <code>ships<code>. Diese Schiffe werden dann am Kampf teilnehmen. Kommt es zu einem Kampf, so muß
// diese Funktion zu allererst aufgerufen werden.
void CCombat::SetInvolvedShips(CArray<CShip*,CShip*>* ships, std::map<CString, CRace*>* pmRaces)
{
	Reset();
	ASSERT(pmRaces);

	// involvierte Rassen bestimmen
	m_mRaces = pmRaces;

	for (int i = 0; i < ships->GetSize(); i++)
		m_mInvolvedRaces[ships->GetAt(i)->GetOwnerOfShip()] = true;

	// Check machen, dass die Rassen wegen ihrer diplomatischen Beziehung auch angreifen können
	for (std::map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); it++)
		if (!m_bReady)
		{
			for (std::map<CString, CRace*>::const_iterator itt = pmRaces->begin(); itt != pmRaces->end(); itt++)
				if (it->first != itt->first && m_mInvolvedRaces.find(it->first) != m_mInvolvedRaces.end() && m_mInvolvedRaces.find(itt->first) != m_mInvolvedRaces.end())
					if (this->CheckDiplomacyStatus(it->second, itt->second))
					{
						m_bReady = TRUE;
						break;
					}
		}

	if (m_bReady)
		for (int i = 0; i < ships->GetSize(); i++)
		{
			CCombatShip* cs = new CCombatShip();;
			cs->m_pShip = ships->GetAt(i);
			cs->m_byManeuverability = ships->GetAt(i)->GetManeuverability();
			cs->m_KO.x = 0;
			cs->m_KO.y = 0;
			cs->m_KO.z = 0;
			cs->m_Tactic = 0;
			if (ships->GetAt(i)->GetCloak())
				cs->m_byCloak = rand()%21 + 30;	// Wert zwischen 30 und 50 zuweisen
			cs->m_Fire.phaser.SetSize(ships->GetAt(i)->GetBeamWeapons()->GetSize());
			cs->m_Fire.torpedo.SetSize(ships->GetAt(i)->GetTorpedoWeapons()->GetSize());
			for (int i = 0; i < cs->m_Fire.phaser.GetSize(); i++)
				cs->m_Fire.phaser[i] = 0;
			for (int i = 0; i < cs->m_Fire.torpedo.GetSize(); i++)
				cs->m_Fire.torpedo[i] = 0;
			m_InvolvedShips.Add(cs);
			m_CS.Add(m_InvolvedShips.GetAt(m_InvolvedShips.GetUpperBound()));
		}
}

//Diese Funktion muß vor der Funktion <code>CalculateCombat()<code> aufgerufen werden. Sie stellt alle
// Berechnungen an, welche für den späteren Kampfverlauf nötig sind. Wird diese Funktion nicht ordnungsgemäß
// durchgeführt, kann die Funktion <code>CalculateCombat()<code> nicht durchgeführt werden.
void CCombat::PreCombatCalculation()
{
	if (!m_bReady)
		return;
	// 1. Position der Schiffe zu Beginn des Kampfes festlegen, also wo befinden sich die einzelnen "Flotten"
	//	  zu Kampfbeginn im Raum? Die Koordinate in der Mitte hat den Wert (0,0,0). Die einzelnen "Flotten" befinden
	//    sich zu Beginn jeweils ca. 200 Einheiten davon entfernt.
	//	  Es gibt 7 verschiedene Positionen für einen Kampfbeginn um den Mittelpunkt. 6 für die Hauptrassen und noch eine

	int nRacePos = 0;
	for (std::map<CString, bool>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); it++)
	{
		// Das wievielte Schiff dieser Rasse ist auf dieser Position? Wird benötigt, wenn wir an der Position Formationen
		// machen wollen
		int nShipPos = 0;
		// Hat eine Rasse ihr Flagschiff im Kampf?
		bool bFlagship = false;
		// Hat eine Rasse ein Schiff mit der Kommandoeigenschaft im Kampf?
		bool bCommandship = false;
		// Umso mehr verschiedene Schiffstypen am Kampf teilnehmen, desto größer ist auch der Bonus. Für jeden Schifftyp
		// gibt es einen 5% Bonus.
		std::map<BYTE, int> mShipTypes;

		// Hier das Feld aller beteiligten Schiffe durchgehen
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() != it->first)
				continue;

			// Schiffsposition zuweisen
			m_CS.GetAt(i)->m_KO = GivePosition(nRacePos, nShipPos++);			
			
			if (m_CS.GetAt(i)->m_pShip->GetIsShipFlagShip())
				bFlagship = true;
			if (m_CS.GetAt(i)->m_pShip->HasSpecial(COMMANDSHIP))
				bCommandship = true;
			// Kolonieschiffe, Transporter, Sonden, Outposts und Starbases gelten dabei nicht mit!
			if (m_CS.GetAt(i)->m_pShip->GetShipType() >= SCOUT && m_CS.GetAt(i)->m_pShip->GetShipType() <= FLAGSHIP)
				mShipTypes[m_CS.GetAt(i)->m_pShip->GetShipType()] += 1;
		}

		// Wir brauchen um den Bonus für viele verschiedene Schiffstypen zu bekommen eine bestimmte Anzahl von jedem
		// Schiffstyp. Dieser Wert beträgt "Anzahl Schiffstypen" - 1 von jedem Typ. Schaffen wir diesen Wert nicht, dann
		// bekommen wir den Bonus nur für "minimalen Wert + 1".
		int mod = 0;		
		int nSmallest = INT_MAX;
		int nDifferentTypes = mShipTypes.size();
		// Schiffstypen durchgehen
		for (std::map<BYTE, int>::const_iterator types = mShipTypes.begin(); types != mShipTypes.end(); types++)
			nSmallest = min(nSmallest, types->second);
			
		// Schaffen wir den Wert nicht
		if ((nDifferentTypes - 1) > nSmallest)
			// dann ist der maximale Bonus smallest + 1
			mod = nSmallest + 1;
		else if (nDifferentTypes > 1 && nSmallest > 1)
			mod = nDifferentTypes;
		else if (nDifferentTypes > 1)
			mod = 1;
				
		// Jetzt noch die ganzen Boni/Mali den Schiffen zuweisen und die Felder mit den Gegnern für die einzelnen Rassen füllen
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() == it->first)
			{
				// 10% Bonus wenn Flagschiff am Kampf teilnehmen
				if (bFlagship)
					m_CS.ElementAt(i)->m_iModifier += 10;
				// 20% Bonus wenn Schiff mit Kommandoeigenschaft am Kampf teilnimmt
				if (bCommandship)
					m_CS.ElementAt(i)->m_iModifier += 20;
				// möglichen Bonus durch verschiedene Schiffstypen draufrechnen
				m_CS.ElementAt(i)->m_iModifier += mod * 5;	// 5% pro Schiffstyp
				// möglichen Bonus durch Erfahrung der Crew draufrechnen
				m_CS.ElementAt(i)->m_iModifier += m_CS.GetAt(i)->GetCrewExperienceModi();
				// Darf nicht 0% sein (normal sind 100 eingestellt)
				if (m_CS.GetAt(i)->m_iModifier <= 0)
					m_CS.ElementAt(i)->m_iModifier = 1;
				
				//CString mod;
				//mod.Format("Modifikator: %d%%", m_CS.ElementAt(i)->m_iModifier);
				//AfxMessageBox(mod);
			}
			else
			{
				// Feld mit allen möglichen gegnerischen Schiffen füllen				
				if (CheckDiplomacyStatus((*m_mRaces)[it->first], (*m_mRaces)[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()]))
					m_mEnemies[it->first].push_back(m_CS.GetAt(i));
			}			
		}
		nRacePos++;
	}	
}

// Diese Funktion ist das Herzstück der CCombat-Klasse. Sie führt die ganzen Kampfberechnungen durch.
void CCombat::CalculateCombat(std::map<CString, BYTE>& winner)
{	
	while (m_bReady)
	{
		m_bReady = false;
		m_iTime++;
		if (m_iTime > MAXSHORT)
			break;
		// Wenn 50 Runden lang niemand angegriffen hat, so wird hier abgebrochen. Kann passieren, wenn alle Schiffe 
		// getarnt sind und sich nicht sehen können
		if (m_bAttackedSomebody == FALSE && m_iTime > 20)
			break;
		// Flugroute aller Schiffe berechnen und einfach mal attackieren (Phaser abfeuern)
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			if (m_CS.GetAt(i)->m_pTarget == NULL || m_CS.GetAt(i)->m_pTarget->m_pShip->GetHull()->GetCurrentHull() < 1)
			{
				if (SetTarget(i))
					m_bReady = true;
				else
					continue;
			}
			else
				m_bReady = true;
			m_CS.ElementAt(i)->CalculateNextPosition();
			// Wenn wir ein Ziel haben, dann greifen wir dieses auch an
			if (m_CS.GetAt(i)->m_pTarget != NULL)
			{
				m_bAttackedSomebody = TRUE;
				// solange wir immer mit unseren Beamwaffen ein Ziel zerstört haben, aber noch Beams übrig haben
				// können wir ein neues Ziel mit diesen Angreifen
				CPoint value(0,0);
				do {
					value = m_CS.ElementAt(i)->AttackEnemyWithBeam(value);
					// Wenn wir einen Wert ungleich -1 zurückbekommen, dann wurde das gegnerische Schiff vernichtet.
					// Wir müssen dann also uns ein neues Ziel suchen
					if (value.x != -1)
					{
						// wird kein Ziel mehr gefunden, dann können wir aus der Schleife gehen
						if (!SetTarget(i))
							break;
					}
				} while (value.x != -1);

				// ab hier jetzt ein Angriff mit den Torpedowaffen
				// wenn wir kein Ziel mehr haben, weil es z.B. durch den letzten Beamstrahl ausgeschaltet wurde,
				// versuchen ein neues aufzuschalten
				if (m_CS.GetAt(i)->m_pTarget == NULL)
				{
					// Finden wir kein Ziel müssen wir die Schleife abbrechen
					if (!SetTarget(i))
						continue;
				}
				value.x = value.y = 0;
				// Schleife ähnlich wie oben bei Beamangriff durchgehen
				do {
					value =	m_CS.ElementAt(i)->AttackEnemyWithTorpedo(&m_CT, value);
					// Wenn wir einen Wert ungleich -1 zurückbekommen, dann haben wir genug Torpedos auf das 
					// gegnerische Schiff abgefeuert, so das es vermutlich zerstört werden würde. Somit suchen
					// wir uns ein neues Ziel.
					if (value.x != -1)
					{
						// wird kein Ziel mehr gefunden, dann können wir aus der Schleife gehen
						if (!SetTarget(i))
							break;
					}
				} while (value.x != -1);
			}
		}
		// Das Torpedofeld durchgehen und deren Flug berechnen
		for (int i = 0; i < m_CT.GetSize(); i++)
			if (m_CT.ElementAt(i).Fly(&m_CS) == TRUE)
				m_CT.RemoveAt(i--);
		// Wenn noch Torpedos rumschwirren, dann den Kampf noch nicht abbrechen
		if (!m_CT.IsEmpty())
			m_bReady = TRUE;

		// Auf das Ziel zufliegen
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			if (CheckShipLife(i) == FALSE) // Fall das Schiff nicht mehr am Leben ist in der Schleife weiter machen
			{
				m_CS.RemoveAt(i--);
			}
			else
			{
				m_CS.ElementAt(i)->GotoNextPosition();
				m_CS.ElementAt(i)->m_pShip->GetShield()->RechargeShields();
			}
		}
		// Wenn keine Schiffe mehr am Kampf teilnehmen, weil vielleicht alle vernichtet wurden, dann können wir abbrechen
		if (m_CS.IsEmpty())
			break;
	}

	if (m_bAttackedSomebody)
	{
		// Nach einem Kampf kann geschaut werden, wer noch Schiffe besitzt. Diese Rassen haben den Kampf danach gewonnen
		// Erstmal wird für alle beteiligten Rassen der Kampf auf verloren gesetzt. Danach wird geschaut, wer noch
		// Schiffe besitzt. Für diese Rassen wird der Kampf dann auf gewonnen gesetzt. Alle anderen Rassen gelten als
		// nicht kampfbeteiligt.
		for (std::map<CString, bool>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); it++)
			winner[it->first] = 2;
		for (int i = 0; i < m_CS.GetSize(); i++)
			winner[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = 1;

		// ein Unentschieden wurde erreicht, wenn es mehrere "Gewinner" gibt, diese aber keine diplomatische
		// Beziehung haben, um sich nicht anzugreifen
		for (std::map<CString, bool>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); it++)
			for (std::map<CString, bool>::const_iterator itt = m_mInvolvedRaces.begin(); itt != m_mInvolvedRaces.end(); itt++)
				if (it->first != itt->first && winner[it->first] == 1 && winner[itt->first] == 1)
					if (CheckDiplomacyStatus((*m_mRaces)[it->first], (*m_mRaces)[itt->first]))
					{
						winner[it->first] = 3;
						winner[itt->first] = 3;
					}
/*
		CString s;
		s.Format("involved ships: %d", m_InvolvedShips.GetSize());
		AfxMessageBox(s);
		s.Format("m_CS: %d", m_CS.GetSize());
		AfxMessageBox(s);
		for (int i = 0; i <= DOMINION; i++)
		{
			s.Format("m_Enemies[%d]: %d", i, m_Enemies[i].GetSize());
			AfxMessageBox(s);
		}
*/
	}
	// Dann ist der Kampf unentschieden ausgegangen
	else 
	{		
		for (std::map<CString, bool>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); it++)
			winner[it->first] = 3;		
	}
}

// Diese Funktion versucht dem i-ten Schiff im Feld <code>m_CS<code> ein Ziel zu geben. Wird dem Schiff ein Ziel
// zugewiesen gibt die Funktion TRUE zurück, findet sich kein Ziel mehr gibt die Funktion FALSE zurück.
BOOLEAN CCombat::SetTarget(int i)
{
	CString sOwner = m_CS.GetAt(i)->m_pShip->GetOwnerOfShip();
		
	// Wenn das enemy-Feld leer ist, dann gibt es keine gegnerischen Schiffe mehr in diesem Kampf und wir können
	// diesen womöglich beenden
	while (m_mEnemies[sOwner].size() > 0)
	{
		// Hier wird erstmal zufällig ein gegnerisches Schiff als Ziel genommen. Es gibt noch keine weiteren Einschränkungen
		int random = rand()%m_mEnemies[sOwner].size();
		CCombatShip* targetShip = m_mEnemies[sOwner].at(random);
		// Das Ziel hat schon keine Hülle mehr (= vernichtet), dann versuchen ein neuen Ziel zu finden
		if (targetShip->m_pShip->GetHull()->GetCurrentHull() < 1)
			m_mEnemies[sOwner].erase(m_mEnemies[sOwner].begin() + random);
		// ein legitimes Ziel wurde gefunden
		else
		{
			// ist das Schiff nicht (mehr) getarnt
			if (targetShip->m_byCloak == 0)
			{
				m_CS.ElementAt(i)->m_pTarget = targetShip;
				m_CS.ElementAt(i)->m_Fire.phaserIsShooting = FALSE;
			}
			// Wenn das Ziel getarnt ist, dann müssen wir eine ausreichend hohe Scanpower haben oder können das Ziel nicht aufschalten
			else if (m_CS.GetAt(i)->m_pShip->GetScanPower() > targetShip->m_pShip->GetStealthPower() * 20)
			{
				// Wenn wir es aufschalten könnten, dann setzen sagen wir einfach, dass das gegnereische Schiff schon gefeuert hätte
				// somit verliert es nach einer random-Zeit die Tarnung und alle unsere Schiffe können gleichzeitig angreifen. Dadurch
				// fliegt der Scout nicht mehr allein vorneweg.
				targetShip->m_bShootCloaked = TRUE;
				m_CS.ElementAt(i)->m_Fire.phaserIsShooting = FALSE;
			}	
			// jedenfalls wird hier abgebrochen -> Ziel gefunden (auch wenn wegen Tarnung noch keine Aufschlaltung mgl. war)
			return TRUE;
		}
	};
	return FALSE;
}

// Diese private Funktion überprüft, ob das Schiff an der Stelle <code>i<code> im Feld <code>m_CS<code> noch am
// Leben ist, also ob es noch eine positive Hülle besitzt. Falls dies nicht der Fall sein sollte, dann
// unternimmt diese Funktion alle Arbeiten die anfallen, um dieses Schiff aus dem Feld zu entfernen.
// D.h. mögliche Ziele werden verändert, Zeiger neu zugeweisen usw. Wenn das Schiff zerstört ist gibt diese
// Funktion FALSE zurück, ansonsten TRUE.
BOOLEAN CCombat::CheckShipLife(int i)
{
	// Bevor wir zur nächsten Position fliegen schauen ob das Schiff noch am Leben ist
	// Wenn nicht, dann aus dem Feld m_CS nehmen
	if (m_CS.GetAt(i)->m_pShip->GetHull()->GetCurrentHull() < 1)
	{
		// Wenn irgendein Schiff dieses Schiff als Ziel hatte, dann das Ziel entfernen
		for (int j = 0; j < m_CS.GetSize(); j++)
		{
			if (m_CS.GetAt(j)->m_pTarget == m_CS.GetAt(i))
			{
				m_CS.ElementAt(j)->m_Fire.phaserIsShooting = FALSE;
				m_CS.ElementAt(j)->m_pTarget = NULL;
				for (int t = 0; t < m_CS.GetAt(j)->m_Fire.phaser.GetSize(); t++)
					if (m_CS.ElementAt(j)->m_Fire.phaser[t] > m_CS.GetAt(j)->m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime())
						m_CS.ElementAt(j)->m_Fire.phaser[t] = m_CS.GetAt(j)->m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime();
			}
		}		
		return FALSE;
	}
	return TRUE;
}

// Funktion überprüft, ob die Rassen in einem Kampf sich gegeneinander aus diplomatischen Gründen
// überhaupt attackieren. Die Funktion gibt <code>TRUE</code> zurück, wenn sie sich angreifen können,
// ansonsten gibt sie <code>FALSE</code> zurück.
BOOLEAN CCombat::CheckDiplomacyStatus(CRace* raceA, CRace* raceB)
{
	ASSERT(raceA != raceB);
	// Wenn wir mit der Rasse, welcher das andere Schiff gehört nicht mindst. einen Freundschaftsvertrag
	// oder einen Verteidigungspakt oder Kriegspakt oder Nichtangriffspakt haben
	if (raceA->GetAgreement(raceB->GetRaceID()) == NO_AGREEMENT
		|| raceA->GetAgreement(raceB->GetRaceID()) == TRADE_AGREEMENT
		|| raceA->GetAgreement(raceB->GetRaceID()) == WAR)
	{
		if (raceA->GetType() == MAJOR && raceB->GetType() == MAJOR)
		{
			if (((CMajor*)raceA)->GetDefencePact(raceB->GetRaceID()) == FALSE)
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}	
	else
		return FALSE;
}

void CCombat::Reset()
{
	m_mInvolvedRaces.clear();
	m_mEnemies.clear();

	for (int i = 0; i < m_InvolvedShips.GetSize(); )
	{
		delete m_InvolvedShips.GetAt(i);
		m_InvolvedShips.GetAt(i) = NULL;
		m_InvolvedShips.RemoveAt(i);
	}
	m_InvolvedShips.RemoveAll();

	for (int i = 0; i < m_CS.GetSize(); )
		m_CS.RemoveAt(i);
	m_CS.RemoveAll();
	for (int i = 0; i < m_CT.GetSize(); )
		m_CT.RemoveAt(i);
	m_CT.RemoveAll();
	
	m_bReady = FALSE;
	m_iTime = 0;
	m_bAttackedSomebody = FALSE;
}