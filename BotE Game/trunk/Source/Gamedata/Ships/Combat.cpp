#include "stdafx.h"
#include "Combat.h"
#include "Races\RaceController.h"
#include "Galaxy\Anomaly.h"

vec3i GivePosition(BYTE pos, USHORT posNumber)
{
	vec3i p;
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
void CCombat::SetInvolvedShips(CArray<CShip*>* pShips, std::map<CString, CRace*>* pmRaces, const CAnomaly* pAnomaly)
{
	Reset();
	ASSERT(pmRaces);

	// involvierte Rassen bestimmen
	m_mRaces = pmRaces;

	for (int i = 0; i < pShips->GetSize(); i++)
		m_mInvolvedRaces.insert(pShips->GetAt(i)->GetOwnerOfShip());

	// Check machen, dass die Rassen wegen ihrer diplomatischen Beziehung auch angreifen können
	for (std::map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
		if (!m_bReady)
		{
			for (std::map<CString, CRace*>::const_iterator itt = pmRaces->begin(); itt != pmRaces->end(); ++itt)
				if (it->first != itt->first && m_mInvolvedRaces.find(it->first) != m_mInvolvedRaces.end() && m_mInvolvedRaces.find(itt->first) != m_mInvolvedRaces.end())
					if (CCombat::CheckDiplomacyStatus(it->second, itt->second))
					{
						m_bReady = TRUE;
						break;
					}
		}

	if (m_bReady)
	{
		for (int i = 0; i < pShips->GetSize(); i++)
		{
			CCombatShip* cs = new CCombatShip();;
			cs->m_pShip = pShips->GetAt(i);
			cs->m_byManeuverability = pShips->GetAt(i)->GetManeuverability();
			cs->m_KO.x = 0;
			cs->m_KO.y = 0;
			cs->m_KO.z = 0;
						
			// Anomalien beachten
			if (pAnomaly)
			{
				if (pAnomaly->GetType() == METNEBULA || pAnomaly->GetType() == TORIONGASNEBULA)
					cs->m_bCanUseShields = false;
				if (pAnomaly->GetType() == TORIONGASNEBULA)
					cs->m_bCanUseTorpedos = false;
				if (pAnomaly->GetType() == BINEBULA)
					cs->m_bFasterShieldRecharge = true;
			}

			if (pShips->GetAt(i)->GetCloak())
				cs->m_byCloak = rand()%21 + 50;	// Wert zwischen 50 und 70 zuweisen
			cs->m_Fire.phaser.SetSize(pShips->GetAt(i)->GetBeamWeapons()->GetSize());
			cs->m_Fire.torpedo.SetSize(pShips->GetAt(i)->GetTorpedoWeapons()->GetSize());
			for (int i = 0; i < cs->m_Fire.phaser.GetSize(); i++)
				cs->m_Fire.phaser[i] = 0;
			for (int i = 0; i < cs->m_Fire.torpedo.GetSize(); i++)
				cs->m_Fire.torpedo[i] = 0;
			
			m_InvolvedShips.Add(cs);
			m_CS.Add(m_InvolvedShips.GetAt(m_InvolvedShips.GetUpperBound()));
		}
	}
}

// Diese Funktion muß vor der Funktion <code>CalculateCombat()<code> aufgerufen werden. Sie stellt alle
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
	for (std::set<CString>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); ++it)
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
			if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() != *it)
				continue;

			// Schiffsposition zuweisen
			m_CS.GetAt(i)->m_KO = GivePosition(nRacePos, nShipPos++);

			/*
			// Wenn das Schiff den Rückzugsbefehl hat, aber keine Speed oder keine Manövrierfähigkeit,
			// so bekommt das Schiff den Befehl meiden
			if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() == RETREAT)
				if (m_CS.GetAt(i)->m_pShip->GetSpeed() == 0 || m_CS.GetAt(i)->m_pShip->GetManeuverability() == 0)
					m_CS.GetAt(i)->m_pShip->SetCombatTactic(AVOID);
			*/
						
			// Wenn das Schiff den Rückzugbefehl hat, so gelten dessen Boni nicht für andere Schiffe
			if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
				continue;
			
			if (m_CS.GetAt(i)->m_pShip->GetIsShipFlagShip())
				bFlagship = true;
			if (m_CS.GetAt(i)->m_pShip->HasSpecial(COMMANDSHIP))
				bCommandship = true;
			// Kolonieschiffe, Transporter, Sonden, Outposts und Starbases gelten dabei nicht mit!
			if (m_CS.GetAt(i)->m_pShip->GetShipType() >= SCOUT && m_CS.GetAt(i)->m_pShip->GetShipType() <= DREADNOUGHT)
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
			if (m_CS.GetAt(i)->m_pShip->GetOwnerOfShip() == *it)
			{
				// Schiffe mit dem Befehl "Meiden" bekommen einen 25% Bonus
				if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_AVOID)
					m_CS.ElementAt(i)->m_iModifier += 25;
				// 20% Bonus wenn Schiff mit Kommandoeigenschaft am Kampf teilnimmt
				if (bCommandship)
					m_CS.ElementAt(i)->m_iModifier += 20;
				// 10% Bonus wenn Flagschiff am Kampf teilnehmen
				if (bFlagship)
					m_CS.ElementAt(i)->m_iModifier += 10;
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
				if (CCombat::CheckDiplomacyStatus((*m_mRaces)[*it], (*m_mRaces)[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()]))
					m_mEnemies[*it].push_back(m_CS.GetAt(i));
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
		// Wenn 20 Runden lang niemand angegriffen hat, so wird hier abgebrochen. Kann passieren, wenn alle Schiffe 
		// getarnt sind und sich nicht sehen können
		if (m_bAttackedSomebody == FALSE && m_iTime > 20)
			break;
		
		// Flugroute aller Schiffe berechnen und mögliches Ziel aufschalten
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			// Das Schiff hat ein aufgeschaltetes Ziel...
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

			// Das Schiff hat noch kein Ziel aufgeschaltet...
			// Ziel aufschalten
			if (m_CS.GetAt(i)->m_pTarget == NULL && m_CS.GetAt(i)->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_ATTACK)
			{				
				if (SetTarget(i) == false)
					continue;
			}
			// Das Schiff soll sich zurückziehen
			else if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
			{
				// Kampf als Stattgefunden ansehen, damit nicht nach 20 Ticks abgebrochen wird.
				// Ein Rückzug dauert länger.				
				m_bAttackedSomebody = true;
			}
			
			// Kampf läuft weiter
			m_bReady = true;
			
			// Flug zur nächsten Position
			m_CS.ElementAt(i)->CalculateNextPosition();
			
			// Wenn wir ein Ziel haben, dann greifen wir dieses auch an
			if (m_CS.GetAt(i)->m_pTarget != NULL)
			{
				m_bAttackedSomebody = true;
				// --- BEAMWAFFEN ---
				// solange wir immer mit unseren Beamwaffen ein Ziel zerstört haben, aber noch Beams übrig haben
				// können wir ein neues Ziel mit diesen Angreifen
				CPoint value(0,0);
				do {
					value = m_CS.ElementAt(i)->AttackEnemyWithBeam(value);
					// Wenn wir einen Wert ungleich -1 zurückbekommen, dann wurde das gegnerische Schiff vernichtet.
					// Wir müssen dann also uns ein neues Ziel suchen
					if (value.x != -1)
					{						
						// aktuelles Ziel nullen und Schiff aus Enemy-Listen nehmen
						for (int j = 0; j < m_CS.GetSize(); j++)
							if (m_CS.GetAt(j) == m_CS.GetAt(i)->m_pTarget)
							{
								if (CheckShipStayInCombat(j))	// sollte immer false zurückgeben (Schiff zerstört)
									AfxMessageBox("ERROR in CalculateCombat()");
								break;
							}

						// wird kein Ziel mehr gefunden, dann können wir aus der Schleife gehen
						if (!SetTarget(i))
							break;
					}
				} while (value.x != -1);
				
				// --- TORPEDOWAFFEN ---
				// ab hier jetzt ein Angriff mit den Torpedowaffen
				// wenn wir kein Ziel mehr haben, weil es z.B. durch den letzten Beamstrahl ausgeschaltet wurde,
				// versuchen ein neues aufzuschalten
				if (m_CS.GetAt(i)->m_pTarget == NULL)
				{
					// Finden wir kein Ziel müssen wir die Schleife abbrechen
					if (!SetTarget(i))
						continue;
				}

				if (m_CS.ElementAt(i)->m_bCanUseTorpedos)
				{
					value.x = value.y = 0;
					// Schleife ähnlich wie oben bei Beamangriff durchgehen
					do {
						value =	m_CS.ElementAt(i)->AttackEnemyWithTorpedo(&m_CT, value);
						// Wenn wir einen Wert ungleich -1 zurückbekommen, dann haben wir genug Torpedos auf das 
						// gegnerische Schiff abgefeuert, so das es vermutlich zerstört werden würde. Somit suchen
						// wir uns ein neues Ziel.
						if (value.x != -1)
						{
							// aktuelles Ziel nullen und Schiff aus Enemy-Listen nehmen
							for (int j = 0; j < m_CS.GetSize(); j++)
								if (m_CS.GetAt(j) == m_CS.GetAt(i)->m_pTarget)
								{
									if (CheckShipStayInCombat(j))	// sollte immer false zurückgeben (Schiff zerstört)
										AfxMessageBox("ERROR in CalculateCombat()");
									break;
								}
							// wird kein Ziel mehr gefunden, dann können wir aus der Schleife gehen
							if (!SetTarget(i))
								break;
						}
					} while (value.x != -1);
				}
			}
		}
		
		// Das Torpedofeld durchgehen und deren Flug berechnen
		for (list<CTorpedo*>::iterator it = m_CT.begin(); it != m_CT.end(); )
		{
			CTorpedo* pTorpedo = *it;
			if (pTorpedo->Fly(&m_CS) == TRUE)
			{
				// Torpedo aus Liste entfernen
				delete pTorpedo;
				pTorpedo = NULL;
				it = m_CT.erase(it);
			}
			else
				// weiter in Liste
				++it;
		}
		
		// Wenn noch Torpedos rumschwirren, dann den Kampf noch nicht abbrechen
		if (m_CT.size())
			m_bReady = true;

		// sind nur noch Schiffe vorhanden die sich Zurückziehen soll aber sich nicht Bewegen können, so wird abgebrochen
		bool bOnlyRetreatShipsWithSpeedNull = true;

		// Auf das Ziel zufliegen
		for (int i = 0; i < m_CS.GetSize(); i++)
		{
			// Prüfen ob das Schiff noch am Kampf teilnehmen kann
			if (!CheckShipStayInCombat(i))
			{
				m_CS.RemoveAt(i--);
				continue;
			}
			
			if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() != COMBAT_TACTIC::CT_RETREAT || m_CS.GetAt(i)->m_byManeuverability > 0)
				bOnlyRetreatShipsWithSpeedNull = false;	

			m_CS.ElementAt(i)->GotoNextPosition();
			// wenn die Schilde noch nicht komplett zusammengebrochen sind
			if (m_CS.ElementAt(i)->m_pShip->GetShield()->GetCurrentShield() > 0)
			{
				m_CS.ElementAt(i)->m_pShip->GetShield()->RechargeShields();
				// bei doppelter Schildaufladung können die Schilde nochmals aufgeladen werden
				if (m_CS.ElementAt(i)->m_bFasterShieldRecharge)
					m_CS.ElementAt(i)->m_pShip->GetShield()->RechargeShields();
			}			
		}

		// Haben sich alle aus dem Kampf zurückgezogen, dann ist dieser beendet
		if (bOnlyRetreatShipsWithSpeedNull)
		{
			m_CS.RemoveAll();
			// Attacke zurücksetzen, damit bei der Auswertung ein Unentschieden herauskommt
			m_bAttackedSomebody = false;
		}
		
		// Wenn keine Schiffe mehr am Kampf teilnehmen, weil vielleicht alle vernichtet wurden, dann können wir abbrechen
		if (m_CS.IsEmpty())
			break;
	}

	// Nach einem Kampf alle Schiffe mit Rückzugsbefehl entfernen. Wenn der andere auf Meiden stand, können immernoch
	// Rückzugsschiffe vorhanden sein, die keine Manövrierfähigkeit hatten.
	for (int i = 0; i < m_CS.GetSize(); i++)
		if (m_CS.GetAt(i)->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
			m_CS.RemoveAt(i--);

	if (m_bAttackedSomebody)
	{
		// Nach einem Kampf kann geschaut werden, wer noch Schiffe besitzt. Diese Rassen haben den Kampf danach gewonnen
		// Erstmal wird für alle beteiligten Rassen der Kampf auf verloren gesetzt. Danach wird geschaut, wer noch
		// Schiffe besitzt. Für diese Rassen wird der Kampf dann auf gewonnen gesetzt. Alle anderen Rassen gelten als
		// nicht kampfbeteiligt.
		for (std::set<CString>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); ++it)
			winner[*it] = 2;
		for (int i = 0; i < m_CS.GetSize(); i++)
			winner[m_CS.GetAt(i)->m_pShip->GetOwnerOfShip()] = 1;

		// ein Unentschieden wurde erreicht, wenn es mehrere "Gewinner" gibt, diese aber keine diplomatische
		// Beziehung haben, um sich nicht anzugreifen
		for (std::set<CString>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); ++it)
			for (std::set<CString>::const_iterator itt = m_mInvolvedRaces.begin(); itt != m_mInvolvedRaces.end(); ++itt)
				if (*it != *itt && winner[*it] == 1 && winner[*itt] == 1)
					if (CCombat::CheckDiplomacyStatus((*m_mRaces)[*it], (*m_mRaces)[*itt]))
					{
						winner[*it]  = 3;
						winner[*itt] = 3;
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
		for (std::set<CString>::const_iterator it = m_mInvolvedRaces.begin(); it != m_mInvolvedRaces.end(); ++it)
			winner[*it] = 3;		
	}
}

// Diese Funktion versucht dem i-ten Schiff im Feld <code>m_CS<code> ein Ziel zu geben. Wird dem Schiff ein Ziel
// zugewiesen gibt die Funktion TRUE zurück, findet sich kein Ziel mehr gibt die Funktion FALSE zurück.
bool CCombat::SetTarget(int i)
{
	CShip* pShip = m_CS.GetAt(i)->m_pShip;
	CString sOwner = pShip->GetOwnerOfShip();
	ASSERT(m_CS.GetAt(i)->m_pTarget == NULL);
		
	// Wenn das enemy-Feld leer ist, dann gibt es keine gegnerischen Schiffe mehr in diesem Kampf und wir können
	// diesen womöglich beenden
	while (m_mEnemies[sOwner].size() > 0)
	{
		// Hier wird erstmal zufällig ein gegnerisches Schiff als Ziel genommen.
		// Es gibt noch keine weiteren Einschränkungen
		int random = rand()%m_mEnemies[sOwner].size();
		CCombatShip* targetShip = m_mEnemies[sOwner].at(random);
		
		if (targetShip->m_pShip->GetHull()->GetCurrentHull() < 1)
			AfxMessageBox("ERROR in Combat SetTarget");
				
		// ist das Schiff nicht (mehr) getarnt
		if (targetShip->m_byCloak == 0)
		{
			m_CS.ElementAt(i)->m_pTarget = targetShip;
			m_CS.ElementAt(i)->m_Fire.phaserIsShooting = FALSE;
		}
		// Wenn das Ziel getarnt ist, dann müssen wir eine ausreichend hohe Scanpower haben oder können das Ziel nicht aufschalten
		else if (pShip->GetScanPower() > targetShip->m_pShip->GetStealthPower() * 20)
		{
			// Wenn wir es aufschalten könnten, dann setzen sagen wir einfach, dass das gegnerische Schiff schon gefeuert hätte
			// somit verliert es nach einer random-Zeit die Tarnung und alle unsere Schiffe können gleichzeitig angreifen. Dadurch
			// fliegt der Scout nicht mehr allein vorneweg.
			targetShip->m_bShootCloaked = TRUE;
			m_CS.ElementAt(i)->m_Fire.phaserIsShooting = FALSE;
		}
		
		// Wenn möglich wieder Tarnen bevor ein neues Ziel gewählt wird
		if (m_CS.GetAt(i)->m_byReCloak == 255)
		{
			if (pShip->GetCloak() && m_CS.GetAt(i)->m_byCloak == 0)
			{
				m_CS.GetAt(i)->m_byCloak = rand()%21 + 50;	// Wert zwischen 50 und 70 zuweisen
				m_CS.GetAt(i)->m_bShootCloaked = FALSE;
				m_CS.GetAt(i)->m_byReCloak = 0;
			}
		}
		// !!! Jedenfalls wird hier immer abgebrochen -> Ziel gilt als gefunden, 
		//	   auch wenn wegen der Tarnung noch keine echte Aufschaltung möglich war)

		return true;		
	};

	// kein Ziel gefunden
	return false;
}

// Diese private Funktion überprüft, ob das Schiff an der Stelle <code>i<code> im Feld <code>m_CS<code> weiterhin
// am Kampf teilnehmen kann. Ist das Schiff entweder zerstört oder hat sich erfolgreich Zurückgezogen, so kann es
// nicht weiter am Kampf teilnehmen. In diesem Fall unternimmt diese Funktion alle Arbeiten die anfallen,
// um dieses Schiff aus dem Feld zu entfernen. D.h. mögliche Ziele werden verändert, Zeiger neu zugeweisen usw.
// Wenn das Schiff nicht mehr im Kampf ist gibt die Funktion FALSE zurück, ansonsten TRUE.
bool CCombat::CheckShipStayInCombat(int i)
{
	CCombatShip* pCombatShip = m_CS[i];
	
	bool bIsAlive = true;
	if (pCombatShip->m_pShip->GetHull()->GetCurrentHull() < 1)
		bIsAlive = false;
	else if (pCombatShip->m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT && pCombatShip->m_byRetreatCounter == 0 && pCombatShip->m_lRoute.size() == 0)
		bIsAlive = false;

	// Bevor wir zur nächsten Position fliegen schauen ob das Schiff noch am Leben ist
	// Wenn nicht, dann aus dem Feld m_CS nehmen
	if (!bIsAlive)
	{
		// Wenn irgendein Schiff dieses Schiff als Ziel hatte, dann das Ziel entfernen
		for (int j = 0; j < m_CS.GetSize(); j++)
		{
			if (m_CS.GetAt(j)->m_pTarget == pCombatShip)
			{
				m_CS.ElementAt(j)->m_Fire.phaserIsShooting = FALSE;
				m_CS.ElementAt(j)->m_pTarget = NULL;
				for (int t = 0; t < m_CS.GetAt(j)->m_Fire.phaser.GetSize(); t++)
					if (m_CS.ElementAt(j)->m_Fire.phaser[t] > m_CS.GetAt(j)->m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime())
						m_CS.ElementAt(j)->m_Fire.phaser[t] = m_CS.GetAt(j)->m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime();
			}
		}

		// Dieses Schiff aus allen Enemy-Maps aller Kampfbeteiligten entfernen
		for (map<CString, vector<CCombatShip*> >::iterator it = m_mEnemies.begin(); it != m_mEnemies.end(); ++it)
		{
			// ist dieses Schiff im Vektor?
			vector<CCombatShip*>* vVec = &it->second;
			for (UINT i = 0; i < vVec->size(); i++)
				if (vVec->at(i) == pCombatShip)
				{
					vVec->erase(vVec->begin() + i);
					break;
				}
		}

		return false;
	}
	
	return true;
}

// Funktion zum Berechnen der groben prozentualen Siegchance einer Rasse. Die Siegchance liegt zwischen 0 und 1.
double CCombat::GetWinningChance(const CRace* pOurRace, const CArray<CShip*>& vInvolvedShips, const std::map<CString, CRace*>* pmRaces, std::set<const CRace*>& sFriends, std::set<const CRace*>& sEnemies, const CAnomaly* pAnomaly)
{
	ASSERT(pOurRace);
	ASSERT(pmRaces);

	double dWinningChance	= 0.5;
	double dOurStrenght		= 0.0;
	double dEnemyStrenght	= 0.0;
	double dOurOffensive	= 0.0;
	double dEnemyOffensive	= 0.0;

	// beteiligte Rassen berechnen
	sFriends.clear();
	sEnemies.clear();

	// Anomalien beachten
	bool bCanUseShields = true;
	bool bCanUseTorpedos = true;
	if (pAnomaly)
	{
		if (pAnomaly->GetType() == METNEBULA || pAnomaly->GetType() == TORIONGASNEBULA)
			bCanUseShields = false;
		if (pAnomaly->GetType() == TORIONGASNEBULA)
			bCanUseTorpedos = false;		
	}

	for (int i = 0; i < vInvolvedShips.GetSize(); i++)
	{
		const CShip* pShip = vInvolvedShips[i];

		double dOffensive = pShip->GetCompleteOffensivePower(true, bCanUseTorpedos);
		double dDefensive = pShip->GetCompleteDefensivePower(bCanUseShields) / 2.0;
		
		if (pShip->GetOwnerOfShip() == pOurRace->GetRaceID())
		{
			sFriends.insert(pOurRace);
			dOurOffensive += dOffensive;
			dOurStrenght += dOffensive + dDefensive;
		}
		else
		{
			if (pmRaces->find(pShip->GetOwnerOfShip()) == pmRaces->end())
				continue;
			
			const CRace* pOtherRace  = pmRaces->find(pShip->GetOwnerOfShip())->second;
			ASSERT(pOtherRace);

			if (CheckDiplomacyStatus(pOurRace, pOtherRace))
			{
				sEnemies.insert(pOtherRace);
				dEnemyOffensive += dOffensive;
				dEnemyStrenght += dOffensive + dDefensive;
			}
			else
			{
				sFriends.insert(pOtherRace);
				dOurOffensive += dOffensive;
				dOurStrenght += dOffensive + dDefensive;
			}
		}
	}

	if (dOurOffensive == 0.0)
		dWinningChance = 0.0;
	else if (dEnemyOffensive == 0.0)
		dWinningChance = 1.0;
	else if (dEnemyStrenght > 0.0)
		dWinningChance = dOurStrenght / dEnemyStrenght / 2.0;

	return dWinningChance;
}

// Funktion überprüft, ob die Rassen in einem Kampf sich gegeneinander aus diplomatischen Gründen
// überhaupt attackieren. Die Funktion gibt <code>TRUE</code> zurück, wenn sie sich angreifen können,
// ansonsten gibt sie <code>FALSE</code> zurück.
bool CCombat::CheckDiplomacyStatus(const CRace* raceA, const CRace* raceB)
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

	for (int i = 0; i < m_InvolvedShips.GetSize(); i++)
		delete m_InvolvedShips.GetAt(i);
	m_InvolvedShips.RemoveAll();

	m_CS.RemoveAll();
	for (list<CTorpedo*>::iterator it = m_CT.begin(); it != m_CT.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	m_CT.clear();
	
	m_bReady = FALSE;
	m_iTime = 0;
	m_bAttackedSomebody = FALSE;
}