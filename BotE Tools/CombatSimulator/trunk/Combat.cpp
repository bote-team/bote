#include "stdafx.h"
#include "Combat.h"

vec3i GivePosition(BYTE pos, USHORT posNumber)
{
	vec3i p;
	//p.x = 0; p.y = 0; p.z = 0;
	p.x = rand()%30; p.y = rand()%30; p.z = rand()%30;

	switch(pos)
	{
	case 0: p.x = 200 + posNumber*5;	break;
	case 1: p.x = -200 - posNumber*5;	break;
	case 2: p.y = 200 + posNumber*5;	break;
	case 3: p.y = -200 - posNumber*5;	break;
	case 4: p.z = 200 + posNumber*5;	break;
	case 5: p.z = 0-200 - posNumber*5;	break;
	case 6: p.x += posNumber*5;			break;
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
void CCombat::SetInvolvedShips(CArray<CShip*,CShip*>* ships/*, CMajorRace* majors*/)
{
	Reset();	
	// involvierte Rassen bestimmen
	//m_MajorRaces = majors;
	memset(m_bInvolvedRaces, FALSE, sizeof(BOOLEAN) * 7);
	for (int i = 0; i < ships->GetSize(); i++)
		if (ships->GetAt(i)->GetOwnerOfShip() >= HUMAN && ships->GetAt(i)->GetOwnerOfShip() <= DOMINION)
			m_bInvolvedRaces[ships->GetAt(i)->GetOwnerOfShip()] = TRUE;

	// Check machen, dass die Rassen wegen ihrer diplomatischen Beziehung auch angreifen können
	/*for (int i = HUMAN; i <= DOMINION; i++)
		if (!m_bReady)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
				if (i != j && m_bInvolvedRaces[i] && m_bInvolvedRaces[j])
					if (this->CheckDiplomacyStatus(&m_MajorRaces[i], &m_MajorRaces[j]))
					{*/
						m_bReady = TRUE;
					/*	break;
					}
		}
	*/
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
				cs->m_byCloak = rand()%21 + 50;	// Wert zwischen 50 und 70 zuweisen
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
	//    sich zu Beginn jeweils 100 Einheiten davon entfernt.
	BYTE differentRaces = NULL;
	
	// Es gibt 7 verschiedene Positionen für einen Kampfbeginn um den Mittelpunkt. 6 für die Hauptrassen und noch eine
	// 7. für die Minorraces
	SBYTE position[7] = {-1,-1,-1,-1,-1,-1,-1};

	// Das wievielte Schiff dieser Rasse ist auf dieser Position? Wird benötigt, wenn wir an der Position Formationen
	// machen wollen
	USHORT posNumber[7] = {0,0,0,0,0,0,0};
	
	// Hat eine Rasse ihr Flagschiff im Kampf?
	BOOLEAN flagship[7] = {0,0,0,0,0,0,0};

	// Hat eine Rasse ein Schiff mit der Kommandoeigenschaft im Kampf?
	BOOLEAN commandship[7] = {0,0,0,0,0,0,0};

	// Umso mehr verschiedene Schiffstypen am Kampf teilnehmen, desto größer ist auch der Bonus. Für jeden Schifftyp
	// gibt es einen 5% Bonus.
	USHORT shipTypes[7][15] = {0};

	// Hier das Feld aller beteiligten Schiffe durchgehen
	for (int i = 0; i < m_CS.GetSize(); i++)
	{
		BOOL check = TRUE;
		for (int j = 0; j < differentRaces; j++)
			// Überprüfen ob die Rasse schon eine Position zugewiesen bekommen hat
			if (position[j] == (SBYTE)m_CS.GetAt(i)->m_pShip->GetOwnerOfShip())
			{
				m_CS.GetAt(i)->m_KO = GivePosition(j, posNumber[j]);
				posNumber[j]++;
				check = FALSE;
				break;
			}
		
		// !!! GivePosition ist erstmal eine Testfunktion, welche den Schiffen einfach so grobe Positionen zuweist. Diese
		// Funktion wird später sehr komplex werden, wenn wir Formationen beachten müssen.	

		// Wenn wir für diese Rasse noch keine Startposition haben, dann wird ihr eine zugewiesen
		if (check == TRUE && position[differentRaces] == -1)
		{
			position[differentRaces] = (SBYTE)m_CS.GetAt(i)->m_pShip->GetOwnerOfShip();
			m_CS.GetAt(i)->m_KO = GivePosition(differentRaces,posNumber[differentRaces]);
			posNumber[differentRaces]++;
			differentRaces++;
		}
		if (m_CS.GetAt(i)->m_pShip->GetIsShipFlagShip())
			flagship[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = TRUE;
		if (m_CS.GetAt(i)->m_pShip->HasSpecial(COMMANDSHIP))
			commandship[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = TRUE;
		// Kolonieschiffe, Transporter, Sonden, Outposts und Starbases gelten dabei nicht mit!
		if (m_CS.GetAt(i)->m_pShip->GetShipType() >= SCOUT && m_CS.GetAt(i)->m_pShip->GetShipType() <= FLAGSHIP)
			shipTypes[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()][m_CS.GetAt(i)->m_pShip->GetShipType()]++;
	}

	// Wir brauchen um den Bonus für viele verschiedene Schiffstypen zu bekommen eine bestimmte Anzahl von jedem
	// Schiffstyp. Dieser Wert beträgt "Anzahl Schiffstypen" - 1 von jedem Typ. Schaffen wir diesen Wert nicht, dann
	// bekommen wir den Bonus nur für "minimalen Wert + 1".
	USHORT mod[7] = {0};
	for (int i = 0; i <= DOMINION; i++)	// Rassen durchgehen
	{
		USHORT smallest = 60000;
		BYTE differentTypes = 0;
		for (int j = 0; j < 15; j++)	// Schiffstypen durchgehen
			if (shipTypes[i][j] > 0)
			{
				differentTypes++;
				if (shipTypes[i][j] < smallest)
					smallest = shipTypes[i][j];
			}
		// Schaffen wir den Wert nicht
		if ((differentTypes - 1) > smallest)
			// dann ist der maximale Bonus smallest + 1
			mod[i] = smallest+1;
		else if (differentTypes > 1 && smallest > 1)
			mod[i] = differentTypes;
		else if (differentTypes > 1)
			mod[i] = 1;
	}
	// Jetzt noch die ganzen Boni/Mali den Schiffen zuweisen und die Felder mit den Gegnern für die einzelnen Rassen füllen
	for (int i = 0; i < m_CS.GetSize(); i++)
	{
		// 10% Bonus wenn Flagschiff am Kampf teilnehmen
		if (flagship[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] == TRUE)
			m_CS.ElementAt(i)->m_iModifier += 10;
		// 20% Bonus wenn Schiff mit Kommandoeigenschaft am Kampf teilnimmt
		if (commandship[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] == TRUE)
			m_CS.ElementAt(i)->m_iModifier += 20;
		// möglichen Bonus durch verschiedene Schiffstypen draufrechnen
		m_CS.ElementAt(i)->m_iModifier += (mod[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] * 5);	// 5% pro Schiffstyp
		// möglichen Bonus durch Erfahrung der Crew draufrechnen
		m_CS.ElementAt(i)->m_iModifier += m_CS.GetAt(i)->GetCrewExperienceModi();
		// Darf nicht 0% sein (normal sind 100 eingestellt)
		if (m_CS.GetAt(i)->m_iModifier <= 0)
			m_CS.ElementAt(i)->m_iModifier = 1;

		// Feld mit allen möglichen gegnerischen Schiffen füllen
		for (int t = HUMAN; t <= DOMINION; t++)
			if (m_bInvolvedRaces[t])
				if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() != t)				
				{	
					/*
					if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() >= HUMAN && m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() <= DOMINION)
					{						
						if (this->CheckDiplomacyStatus(&m_MajorRaces[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()], &m_MajorRaces[t]))
							m_Enemies[t].Add(m_CS.GetAt(i));
						continue;					
					}*/
					m_Enemies[t].Add(m_CS.GetAt(i));
				}
		/*CString mod;
		mod.Format("Modifikator: %d%%", m_CS.ElementAt(i)->m_iModifier);
		AfxMessageBox(mod);
		*/
	}
}

// Diese Funktion ist das Herzstück der CCombat-Klasse. Sie führt die ganzen Kampfberechnungen durch.
void CCombat::CalculateCombat(BYTE winner[7])
{	
	if (m_bReady)
	{
		m_bReady = false;
		m_iTime++;
		if (m_iTime > MAXSHORT)
			m_bReady = false;			
		// Wenn 50 Runden lang niemand angegriffen hat, so wird hier abgebrochen. Kann passieren, wenn alle Schiffe 
		// getarnt sind und sich nicht sehen können
		if (m_bAttackedSomebody == FALSE && m_iTime > 20)
			m_bReady = false;
		// Flugroute aller Schiffe berechnen und einfach mal attackieren (Phaser abfeuern)
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			m_CS.GetAt(i)->SCAL = -1.f;	
			m_CS.GetAt(i)->LIFE = 100;
			m_CS.GetAt(i)->SHIELDS = 100;
			float life = (float)(m_CS.GetAt(i)->m_pShip->GetHull()->GetCurrentHull())
				/ (float)(m_CS.GetAt(i)->m_pShip->GetHull()->GetMaxHull());
			m_CS.GetAt(i)->LIFE = life * 100;

			float shields = (float)(m_CS.GetAt(i)->m_pShip->GetShield()->GetCurrentShield())
				/ (float)(m_CS.GetAt(i)->m_pShip->GetShield()->GetMaxShield());
			m_CS.GetAt(i)->SHIELDS = shields * 100;
			// Wenn ein wieder getarntes Schiff als Ziel aufgeschaltet ist, dieses aber nicht mehr durch
			// Scanner entdeckt werden kann, dann dieses Ziel nicht mehr als Ziel behandeln. Anderenfalls, wenn
			// unser Ziel getarnt ist, wir es aber entdecken, dann für alle sichtbar machen
			if (m_CS.GetAt(i)->m_pTarget != NULL && m_CS.GetAt(i)->m_pTarget->m_byCloak > 0)
			{
				// Ziel für alle sichtbar machen
				if (m_CS.GetAt(i)->m_pShip->GetScanPower() > m_CS.GetAt(i)->m_pTarget->m_pShip->GetStealthPower() * 20)
					m_CS.GetAt(i)->m_pTarget->m_bShootCloaked = TRUE;
				// Ziel wegnehmen
				else
				{
					m_CS.GetAt(i)->m_pTarget = NULL;
					m_CS.GetAt(i)->m_Fire.phaserIsShooting = FALSE;
				}
			}
			// Ziel aufschalten
			if (m_CS.GetAt(i)->m_pTarget == NULL || m_CS.GetAt(i)->m_pTarget->m_pShip->GetHull()->GetCurrentHull() < 1)
			{
				if (SetTarget(i))
					m_bReady = true;
				else
					continue;
			}
			else
				m_bReady = true;
			
			// Flug zur nächsten Position
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
		{
			if (m_CT.ElementAt(i)->Fly(&m_CS) == TRUE)
			{
				delete m_CT[i];
				m_CT.RemoveAt(i--);
			}
		}
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
				// wenn die Schilde noch nicht komplett zusammengebrochen sind
				if (m_CS.ElementAt(i)->m_pShip->GetShield()->GetCurrentShield() > 0)
					m_CS.ElementAt(i)->m_pShip->GetShield()->RechargeShields();
			}
		}
		// Wenn keine Schiffe mehr am Kampf teilnehmen, weil vielleicht alle vernichtet wurden, dann können wir abbrechen
		if (m_CS.IsEmpty())
			m_bReady = false;			
	}

	if (m_bAttackedSomebody && m_bReady == false)
	{
		// Nach einem Kampf kann geschaut werden, wer noch Schiffe besitzt. Diese Rassen haben den Kampf danach gewonnen
		// Erstmal wird für alle beteiligten Rassen der Kampf auf verloren gesetzt. Danach wird geschaut, wer noch
		// Schiffe besitzt. Für diese Rassen wird der Kampf dann auf gewonnen gesetzt. Alle anderen Rassen gelten als
		// nicht kampfbeteiligt.
		for (int i = HUMAN; i <= DOMINION; i++)
			if (m_bInvolvedRaces[i])
				winner[i] = FALSE;
		for (int i = 0; i < m_CS.GetSize(); i++)
			if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() >= HUMAN && m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() <= DOMINION)
				winner[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = TRUE;

		// ein unentschieden wurde erreicht, wenn es mehrere "Gewinner" gibt, diese aber keine diplomatische
		// Beziehung haben, um sich nicht anzugreifen
		for (int i = HUMAN; i <= DOMINION; i++)
			for (int j = HUMAN; j <= DOMINION; j++)
				if (i != j && winner[i] == 1 && winner[j] == 1)
//					if (CheckDiplomacyStatus(&m_MajorRaces[i], &m_MajorRaces[j]))
					{
						winner[i] = 3;
						winner[j] = 3;
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
	else  if (m_bReady == false)
	{		
		for (int i = HUMAN; i <= DOMINION; i++)
			if (m_bInvolvedRaces[i])
				winner[i] = 3;		
	}
}

// Diese Funktion versucht dem i-ten Schiff im Feld <code>m_CS<code> ein Ziel zu geben. Wird dem Schiff ein Ziel
// zugewiesen gibt die Funktion TRUE zurück, findet sich kein Ziel mehr gibt die Funktion FALSE zurück.
BOOLEAN CCombat::SetTarget(int i)
{
	BYTE owner = m_CS.GetAt(i)->m_pShip->GetOwnerOfShip();
	
	// Wenn das enemy-Feld leer ist, dann gibt es keine gegnerischen Schiffe mehr in diesem Kampf und wir können
	// diesen womöglich beenden
	while (m_Enemies[owner].GetSize() > 0)
	{
		// Hier wird erstmal zufällig ein gegnerisches Schiff als Ziel genommen. Es gibt noch keine weiteren Einschränkungen
		int random = rand()%(int)m_Enemies[owner].GetSize();
		CCombatShip* targetShip = m_Enemies[owner].GetAt(random);
		// Das Ziel hat schon keine Hülle mehr (= vernichtet), dann versuchen ein neuen Ziel zu finden
		if (targetShip->m_pShip->GetHull()->GetCurrentHull() < 1)
			m_Enemies[owner].RemoveAt(random);
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
				// Wenn wir es aufschalten könnten, dann setzen sagen wir einfach, dass das gegnerische Schiff schon gefeuert hätte
				// somit verliert es nach einer random-Zeit die Tarnung und alle unsere Schiffe können gleichzeitig angreifen. Dadurch
				// fliegt der Scout nicht mehr allein vorneweg.
				targetShip->m_bShootCloaked = TRUE;
				m_CS.ElementAt(i)->m_Fire.phaserIsShooting = FALSE;
			}	
			// jedenfalls wird hier abgebrochen -> Ziel gefunden (auch wenn wegen Tarnung noch keine Aufschlaltung mgl. war)
			
			// Wenn möglich wieder Tarnen bevor ein neues Ziel gewählt wird
			if (m_CS.GetAt(i)->m_byReCloak == 255)
			{
				if (m_CS.GetAt(i)->m_pShip->GetCloak() && m_CS.GetAt(i)->m_byCloak == 0)
				{
					m_CS.GetAt(i)->m_byCloak = rand()%21 + 50;	// Wert zwischen 50 und 70 zuweisen
					m_CS.GetAt(i)->m_bShootCloaked = FALSE;
					m_CS.GetAt(i)->m_byReCloak = 0;
				}
			}
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

/*
// Funktion überprüft, ob die Rassen in einem Kampf sich gegeneinander aus diplomatischen Gründen
// überhaupt attackieren. Die Funktion gibt <code>TRUE</code> zurück, wenn sie sich angreifen können,
// ansonsten gibt sie <code>FALSE</code> zurück.
BOOLEAN CCombat::CheckDiplomacyStatus(const CMajorRace* raceA, const CMajorRace* raceB)
{
	ASSERT(raceA != raceB);
	// Wenn wir mit der Rasse, welcher das andere Schiff gehört nicht mindst. einen Freundschaftsvertrag
	// oder einen Verteidigungspakt oder Kriegspakt oder Nichtangriffspakt haben
	if ((raceA->GetDiplomacyStatus(raceB->GetRaceNumber()) == NO_AGREEMENT
		|| raceA->GetDiplomacyStatus(raceB->GetRaceNumber()) == TRADE_AGREEMENT
		|| raceA->GetDiplomacyStatus(raceB->GetRaceNumber()) == WAR)
		&& raceA->GetDefencePact(raceB->GetRaceNumber()) == FALSE)
		return TRUE;
	else
		return FALSE;
}
*/

void CCombat::Reset()
{
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
	{
		delete m_CT[i];
		m_CT.RemoveAt(i);
	}
	m_CT.RemoveAll();
	for (int i = 0; i <= DOMINION; i++)
	{
		for (int j = 0; j < m_Enemies[i].GetSize(); )
			m_Enemies[i].RemoveAt(j);
		m_Enemies[i].RemoveAll();
	}

	for (int i = 0; i < 7; i++)
	{
		m_iFormation[i] = 0;
		m_iTactic[i] = 0;
		m_bInvolvedRaces[0] = FALSE;
	}
	m_bReady = FALSE;
	m_iTime = 0;
	m_bAttackedSomebody = FALSE;
}