#include "stdafx.h"
#include "CombatShip.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CCombatShip::CCombatShip(void)
{
	m_pShip = NULL;
	m_pTarget = NULL;
	m_iModifier = 100;
	m_Fire.phaserIsShooting = FALSE;
	m_bRegShieldStatus = FALSE;
	m_byCloak = FALSE;
	m_bShootCloaked = FALSE;
}

CCombatShip::~CCombatShip(void)
{
	m_pShip = NULL;
	m_pTarget = NULL;
	for (int i = 0; i < m_Fire.phaser.GetSize(); )
		m_Fire.phaser.RemoveAt(i);
	m_Fire.phaser.RemoveAll();
	for (int i = 0; i < m_Fire.torpedo.GetSize(); )
		m_Fire.torpedo.RemoveAt(i);
	m_Fire.torpedo.RemoveAll();
	for (int i = 0; i < m_Route.GetSize(); )
		m_Route.RemoveAt(i);
	m_Route.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CCombatShip::CCombatShip(const CCombatShip &rhs)
{
	m_pShip = rhs.m_pShip;
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_iModifier = rhs.m_iModifier;
	for (int i = 0; i < rhs.m_Fire.phaser.GetSize(); i++)
		m_Fire.phaser.Add(rhs.m_Fire.phaser.GetAt(i));
	for (int i = 0; i < rhs.m_Fire.torpedo.GetSize(); i++)
		m_Fire.torpedo.Add(rhs.m_Fire.torpedo.GetAt(i));
	m_Fire.phaserIsShooting = rhs.m_Fire.phaserIsShooting;
	m_byManeuverability = rhs.m_byManeuverability;
	m_pTarget = rhs.m_pTarget;
	m_bRegShieldStatus = rhs.m_bRegShieldStatus;
	m_byCloak = rhs.m_byCloak;
	m_bShootCloaked = rhs.m_bShootCloaked;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CCombatShip & CCombatShip::operator=(const CCombatShip & rhs)
{
	if (this == &rhs)
		return *this;
	m_pShip = rhs.m_pShip;
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_iModifier = rhs.m_iModifier;
	for (int i = 0; i < rhs.m_Fire.phaser.GetSize(); i++)
		m_Fire.phaser.Add(rhs.m_Fire.phaser.GetAt(i));
	for (int i = 0; i < rhs.m_Fire.torpedo.GetSize(); i++)
		m_Fire.torpedo.Add(rhs.m_Fire.torpedo.GetAt(i));
	m_Fire.phaserIsShooting = rhs.m_Fire.phaserIsShooting;
	m_byManeuverability = rhs.m_byManeuverability;
	m_pTarget = rhs.m_pTarget;
	m_bRegShieldStatus = rhs.m_bRegShieldStatus;
	m_byCloak = rhs.m_byCloak;
	m_bShootCloaked = rhs.m_bShootCloaked;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion gibt den möglichen Bonus durch die Manövriebarkeit bei einem Angriff zurück. Übergeben müssen
/// dafür die Manövrierbarkeit des Angreifers und die Manövrierbarkeit des Verteidigers werden.
BYTE CCombatShip::GetToHitBoni(BYTE Att, BYTE Def)
{
/*
	Maneuvrierfähigkeit - Trefferbonus (= to hit bonus) 
	---0---1---2---3---4---5---6---7---8
	0 00% 00% 00% 00% 00% 00% 00% 00% 00%
	1 05% 00% 00% 00% 00% 00% 00% 00% 00%
	2 10% 05% 00% 00% 00% 00% 00% 00% 00%
	3 15% 10% 05% 00% 00% 00% 00% 00% 00%
	4 20% 15% 10% 05% 00% 00% 00% 00% 00%
	5 25% 20% 15% 10% 05% 00% 00% 00% 00%
	6 30% 25% 20% 15% 10% 05% 00% 00% 00%
	7 35% 30% 25% 20% 15% 10% 05% 00% 00%
	8 40% 35% 30% 25% 20% 15% 10% 05% 00%
*/
	BYTE boni[9][9] = {
		0,	0,	0,	0,	0,	0,	0,	0,	0,
		5,	0,	0,	0,	0,	0,	0,	0,	0,
		10, 5,	0,	0,	0,	0,	0,	0,	0,
		15, 10, 5,	0,	0,	0,	0,	0,	0,
		20, 15, 10, 5,	0,	0,	0,	0,	0,
		25, 20, 15, 10, 5,	0,	0,	0,	0,
		30, 25, 20, 15, 10, 5,	0,	0,	0,
		35, 30, 25, 20, 15, 10,	5,	0,	0,
		40, 35, 30, 25, 20, 15,	10,	5,	0,
	
	};

	return boni[Att][Def];
}

/// Diese Funktion gibt den möglichen Malus durch die Manövriebarkeit bei einem Angriff zurück. Übergeben müssen
/// dafür die Manövrierbarkeit des Angreifers und die Manövrierbarkeit des Verteidigers werden.
BYTE CCombatShip::GetToHitMali(BYTE Att, BYTE Def)
{
/*
	Maneuvrierfähigkeit / Verteidigungsbonus in % (= to hit malus für Gegner)
	---0---1---2---3---4---5---6---7---8
	0 00% 00% 00% 00% 00% 00% 00% 00% 00% 
	1 04% 00% 00% 00% 00% 00% 00% 00% 00%
	2 10% 04% 00% 00% 00% 00% 00% 00% 00%
	3 18% 10% 04% 00% 00% 00% 00% 00% 00%
	4 28% 18% 10% 04% 00% 00% 00% 00% 00%
	5 40% 28% 18% 10% 04% 00% 00% 00% 00%
	6 54% 40% 28% 18% 10% 04% 00% 00% 00%
	7 70% 54% 40% 28% 18% 10% 04% 00% 00%
	8 88% 70% 54% 40% 28% 18% 10% 04% 00%
*/
	BYTE mali[9][9] = {
		0,	0,	0,	0,	0,	0,	0,	0,	0,
		4,	0,	0,	0,	0,	0,	0,	0,	0,
		10, 4,	0,	0,	0,	0,	0,	0,	0,
		18, 10, 4,	0,	0,	0,	0,	0,	0,
		28, 18, 10, 4,	0,	0,	0,	0,	0,
		40, 28, 18, 10, 4,	0,	0,	0,	0,
		54, 40, 28, 18, 10, 4,	0,	0,	0,
		70, 54, 40, 28, 18, 10,	4,	0,	0,
		88, 70, 54, 40, 28, 18,	10,	4,	0};

	return mali[Def][Att];
}

//Diese Funktion berechnet den nächsten Punkt auf dem Weg zum Ziel, den das Schiff erreichen wird.
void CCombatShip::CalculateNextPosition()
{
	// Firecounter werden hier heruntergezählt. Später sollte wir das erst in der Schadensberechnung machen
	for (int i = 0; i < m_Fire.phaser.GetSize(); i++)
		if (m_Fire.phaser[i] > 0)
			m_Fire.phaser[i]--;
	for (int i = 0; i < m_Fire.torpedo.GetSize(); i++)
		if (m_Fire.torpedo[i] > 0)
			m_Fire.torpedo[i]--;
	// Wenn das Schiff getarnt war und geschossen hat, dann den Tarncounter runterzählen
	if (m_bShootCloaked == TRUE && m_byCloak > 0)
		m_byCloak--;

	// Wenn wir noch keine Flugroute berechnet haben
	if (m_Route.IsEmpty() && this->m_byManeuverability > 0)
	{
		// Wenn wir ein Ziel aufgeschaltet haben
		if (m_pTarget != NULL)
		{
			// unsere aktuelle Position im Raum
			Position a = m_KO;
			// die Zielposition im Raum
			Position b = m_pTarget->m_KO;
			// der Abstand zwischen diesen beiden Punkten
			float distance = a.DistanceToPosition_B(b);
			
			// Damit die nicht immer auf eine Stelle hängen
			if (distance <= 10.0f)
			{
				short multi = rand()%2;
				if (multi == 0)
					multi = -1;
				b.x = rand()%200 * multi;
				b.y = rand()%200 * multi;
				b.z = rand()%200 * multi;
				distance = a.DistanceToPosition_B(b);
			}

			// die Beschleunigung des Schiffes. Bei z.B. einer Manövriebarkeit von 5 wird bei Speed 1 gestartet und dann
			// immer um 1 erhöht, bis die Speed von 5 erreicht ist. Genauso muss dann auch wieder verlangsamt werden.
			BYTE speed_up = 0;
			BYTE speed = this->m_byManeuverability;
			while (distance > 1.0f)
			{
				// "Bremsweg" berechnen -> speed
				BYTE speed_up_way = 0;	// Bremsweg, den wir mindst. brauchen um anzuhalten
				for (int i = speed_up; i > 0; i--)
					speed_up_way += i;
				// müssen bremsen
				if (distance <= speed_up_way && speed_up > 0)
					speed_up--;
				// können beschleunigen
				else if (speed_up < speed)
					speed_up++;
				// Wenn unser Schiff weiterfliegen könnte als der Abstand zwischen den Schiffen, dann 
				// müssen wir die Geschwindigkeit herabsetzen
				
		/*		if (speed > distance)
					speed = (BYTE)distance;
				a = a + (b-a) * (speed/distance);
				m_Route.Add(a);
				distance = a.DistanceToPosition_B(b);
		*/
				if (speed_up > distance)
					speed_up = (BYTE)distance;
				a = a + (b-a) * (speed_up/distance);
				m_Route.Add(a);
				distance = a.DistanceToPosition_B(b);
			} 
		}
	}
}

// Diese Funktion setzt das Schiff auf den nächsten Punkt seiner vorher berechneten Flugroute.
void CCombatShip::GotoNextPosition()
{
	// Wenn wir schon eine Route berechnet haben, dann nur auf den nächsten Punkt in der Route gehen
	if (!m_Route.IsEmpty())
	{
		m_KO = m_Route.GetAt(0);
		// Routeneintrag entfernen
		m_Route.RemoveAt(0);
	}
}

// Diese Funktion führt einen Beamangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
// gespeichert ist
CPoint CCombatShip::AttackEnemyWithBeam(CPoint beamStart)
{
	// Wenn wir ein Ziel aufgeschaltet haben
	if (m_pTarget != NULL)
	{		
		// Wenn getarnt geschossen wird, so gibt es einen 20% Trefferbonus und Schadensbonus
		BYTE boni = 0;
		if (m_byCloak > 0)
			boni += 20;
		// Wenn wir die Dogfighter/Nahkämpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus, wenn
		// wir gegen kleine Gegner kämpfen
		if (m_pTarget->m_pShip->GetShipSize() == SMALL && m_pShip->HasSpecial(DOGFIGHTER))
			boni += 20;
		// Wenn wir gegen einen Dogfighter/Nahkämpfer kämpfen und mindst. eine mittlere Größe haben, dann bekommen
		// wir einen 20% Bonus gegen dieses Schiff
		if (m_pShip->GetShipSize() >= NORMAL && m_pTarget->m_pShip->HasSpecial(DOGFIGHTER))
			boni += 20;
		// Wenn wir die Dogkiller/Antinahkämpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus gegen kleine
		// Schiffe
		if (m_pTarget->m_pShip->GetShipSize() == SMALL && m_pShip->HasSpecial(DOGKILLER))
			boni += 20;
		// Wenn wir gegen einen Dogkiller/Antinahkämpfer kämpfen und mindst. eine mittlere Größe haben, dann bekommen
		// wir einen 20% Bonus gegen dieses Schiff
		if (m_pShip->GetShipSize() >= NORMAL && m_pTarget->m_pShip->HasSpecial(DOGKILLER))
			boni += 20;
		// Erstmal nur die Beamwaffen beachten
		m_Fire.phaserIsShooting = FALSE;
		// Nur wenn die Distanz größer als ein Feld ist wird geschossen, sonst ja gerammt
		float distance = m_KO.DistanceToPosition_B(m_pTarget->m_KO);
		if (distance >= 1.0f)
			for (int i = beamStart.x; i < m_pShip->GetBeamWeapons()->GetSize(); i++)
			{
				// Wenn noch kein Phaser geschossen hat, dann stellen wir auf FALSE
				if (m_Fire.phaserIsShooting == FALSE)
					m_Fire.phaserIsShooting = FALSE;
				// Solange der Counter größer als die Rechargetime ist wird geschossen. Dann wird diese runtergezählt.
				if (m_Fire.phaser[i] > m_pShip->GetBeamWeapons()->GetAt(i).GetRechargeTime() || m_Fire.phaser[i] == NULL)
				{
					// Reichweite = Type(MK)*5 + (Bonus durch Feuer-System) + 50 (um auf 60 bis 160 zu kommen)
					if (distance <= (m_pShip->GetBeamWeapons()->GetAt(i).GetBeamType()*5 + m_pShip->GetBeamWeapons()->GetAt(i).GetBonus() + 50))
					{
						if (m_Fire.phaser[i] == NULL)
							// Hier wird der Phasercounter auf die Rechargetime + die Beamschussdauer festgelegt.
							// Solange also dieser Counter größer als die Rechargetime ist wird geschossen. Dann wird gewartet
							// bis die Rechargetime auch verstrichen ist. Erst dann erfolgt wieder diese Zuweisung.
							m_Fire.phaser[i] = m_pShip->GetBeamWeapons()->GetAt(i).GetRechargeTime() + m_pShip->GetBeamWeapons()->GetAt(i).GetBeamLenght();
						// Hier den Schaden für jeden Beamstrahl einzeln berechnen. Also die Schleife gleich der Anzahl der
						// Beamstrahlen der Beamwaffe durchlaufen
						for (int j = beamStart.y; j < m_pShip->GetBeamWeapons()->GetAt(i).GetBeamNumber(); j++)
						{
							m_Fire.phaserIsShooting = FALSE;

						// Nur für die Darstellung im CombatSimulator
							m_bPulseFire = FALSE;
							if (m_pShip->GetBeamWeapons()->GetAt(i).GetShootNumber() > 1)
								m_bPulseFire = TRUE;
						// Ende der besonderen Darstellung

							// Wenn das Ziel keine Hülle mehr hat, dann geben wir die aktuelle Nummer des Beams zurück.
							// Dies bedeutet, das wir kein Ziel mehr haben und uns ein neues suchen müssen. Bei der aktuellen
							// Nummer des Beams geht dann die Berechnung weiter
							if (m_pTarget->m_pShip->GetHull()->GetCurrentHull() == 0)
							{
								m_pTarget = NULL;
								for (int t = 0; t < m_Fire.phaser.GetSize(); t++)
									if (m_Fire.phaser[t] > m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime())
										m_Fire.phaser[t] = m_pShip->GetBeamWeapons()->GetAt(t).GetRechargeTime();
								return CPoint(i,j);
							}
							// hier mal Schaden am Schiff machen
							// DAMAGE_TO_HULL (10%) des Schadens gehen immer auf die Hülle.
							int beamDamage = 0;
							int toHull = 0;
							
							// dabei werden dann auch die Anzahl der Beams pro Schuss beachtet, meist sind das ja einer, bei
							// Cannon- und Pulsewaffen können das aber mehrere sein, die pro Schuss/Sekunde rauskommen
							for (int t = 0; t < m_pShip->GetBeamWeapons()->GetAt(i).GetShootNumber(); t++)
							{
								// Hier kommt dann auch die Formel zur Trefferwahrscheinlichkeit ins Spiel. Jeder Schuss wird dafür
								// einzeln beachtet:
								// Acc = 60 - Distance*0.1 + CrewExp(Attacker-Target)*0.1 + Manövriebarkeit(Attacker-Target)*0.2 + Bonus_durch_Feuersystem + BeamType(MK)
								// distance = [0,16]; CrewExp(att-def) = [-12,12]; Manöver(att-def) = [-15,6]; Feuersystembonus = [5,30]; BeamtypeMK = [1,X] X max. ca. 20
								short Acc = 60 - short(-distance*0.1 + (this->GetCrewExperienceModi()-this->m_pTarget->GetCrewExperienceModi())*0.1
									+ (GetToHitBoni(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability())-GetToHitMali(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()))*0.2
									+ m_pShip->GetBeamWeapons()->GetAt(i).GetBonus() + m_pShip->GetBeamWeapons()->GetAt(i).GetBeamType());
								if ((Acc + boni) > rand()%100)
									beamDamage += ((m_pShip->GetBeamWeapons()->GetAt(i).GetBeamPower() * (m_iModifier + boni)) / m_pTarget->m_iModifier);
							/*	CString s;
								//s.Format("Our Man: %d\nTo Hit Boni: %d\nTo Hit Mali: %d",this->m_byManeuverability,GetToHitBoni(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()),GetToHitMali(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()));
								s.Format("Acc: %d\ndistance: %d\nCrewmod: %d\nManöverMod: %d\nSystemboni: %d\nMK_Boni: %d",
									Acc,(short)(-distance*0.1),(short)((this->GetCrewExperienceModi()-this->m_pTarget->GetCrewExperienceModi())*0.1),short((GetToHitBoni(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability())-GetToHitMali(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()))*0.2),m_pShip->GetBeamWeapons()->GetAt(i).GetBonus(),m_pShip->GetBeamWeapons()->GetAt(i).GetBeamType());
								AfxMessageBox(s);*/
							}
							if (beamDamage > 0)
								m_bShootCloaked = TRUE;
							// wenn wir schilddurchdringende Beams haben und das feindliche Schiff keine auf unsere Beams eingestellten
							// regenerativen Schilde hat, dann machen wir kompletten Schaden an der Hülle
							if (m_pShip->GetBeamWeapons()->GetAt(i).GetPiercing() == TRUE && m_pTarget->GetActRegShields() == FALSE)
								toHull = beamDamage;
							// wenn wir modulierende Beams haben, dann gehen immer 50% auf die Hülle und 50% auf die Schilde, egal
							// was für Schilde an dem Zielschiff sind
							else if (m_pShip->GetBeamWeapons()->GetAt(i).GetModulating() == TRUE)
								toHull = (int)(beamDamage * 0.5);
							else
								toHull = (int)(beamDamage * DAMAGE_TO_HULL);
							m_pTarget->m_pShip->GetHull()->SetCurrentHull(-toHull);						
							/*
							CString dam;
							dam.Format("Schiff: %s\naltes Schild: %d\nHülle: %d\nBeamschaden: %d\nBeamschaden auf Hülle: %d",
								m_pTarget->m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShield()->GetCurrentShield(),
								m_pTarget->m_pShip->GetHull()->GetCurrentHull(), beamDamage, toHull);
							AfxMessageBox(dam);
							*/							
							// Wenn das feindliche Schiff eine ablative Hüllenpanzerung hat, dann werden 10% des Schadens von
							// der Hülle entfernt
							int ablative = 0;
							if (m_pTarget->m_pShip->GetHull()->GetAblative())
							{
								ablative = (int)(beamDamage * 0.1);
								// Wenn wir zusätzlich eine Hüllenpolarisation haben, so ist dieser Wert um 10% niedriger, da wir sonst nach
								// dem Angriff mehr Hülle haben als zuvor (wegen den 10% immer auf Hülle)
								if (m_pTarget->m_pShip->GetHull()->GetPolarisation())
									ablative -= (int)(ablative * 0.1);
							}
							// der restliche Beamschaden ermitteln, welcher nicht direkt auf die Hülle ging
							beamDamage -= toHull;
							// Der komplette Schaden geht auf die Schilde
							if ((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield() - beamDamage >= 0)
								m_pTarget->m_pShip->GetShield()->SetCurrentShield((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield()-beamDamage);
							// Der Schaden geht teilweise auf Schilde und Hülle oder nur auf die Hülle
							else
							{
								m_pTarget->m_pShip->GetHull()->SetCurrentHull((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield() - beamDamage);
								m_pTarget->m_pShip->GetShield()->SetCurrentShield(0);
							}
							// eventuell vorhanden Bonus durch ablative Hüllenpanzerung wieder draufrechenen. Nur wenn
							// die Hülle hier noch größer als NULL ist
							if (m_pTarget->m_pShip->GetHull()->GetCurrentHull() > 0)
								m_pTarget->m_pShip->GetHull()->SetCurrentHull(ablative);
							// Wenn wir schilddurchdringende Beams haben und das feindliche Schiff regenerative Schilde, so kann es diese
							// auf unsere Waffen einstellen
							if (m_pShip->GetBeamWeapons()->GetAt(i).GetPiercing() == TRUE && m_pTarget->m_pShip->GetShield()->GetRegenerative() == TRUE)
								m_pTarget->ActRegShield();
							m_Fire.phaserIsShooting = TRUE;
							/*
							dam.Format("Schiff: %s\nneues Schild: %d\nHülle: %d\nrestlicher Beamschaden: %d",
								m_pTarget->m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShield()->GetCurrentShield(),
								m_pTarget->m_pShip->GetHull()->GetCurrentHull(), beamDamage);
							AfxMessageBox(dam);
							*/
						}
					}
				}
			}
	}
	return CPoint(-1,-1);
}

// Diese Funktion führt einen Torpedoangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget</code>
// gespeichert ist
CPoint CCombatShip::AttackEnemyWithTorpedo(CombatTorpedos* CT, CPoint torpedoStart)
{
	// Wenn wir ein Ziel aufgeschaltet haben
	if (m_pTarget != NULL)
	{
		BYTE boni = 0;
		// Wenn getarnt geschossen wurde, so gibt es einen 20% Trefferbonus und Schadensbonus
		if (m_byCloak > 0)
			boni += 20;
		// Wenn wir die Dogfighter/Nahkämpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus, wenn
		// wir gegen kleine Gegner kämpfen
		if (m_pTarget->m_pShip->GetShipSize() == SMALL && m_pShip->HasSpecial(DOGFIGHTER))
			boni += 20;
		// Wenn wir gegen einen Dogfighter/Nahkämpfer kämpfen und mindst. eine mittlere Größe haben, dann bekommen
		// wir einen 20% Bonus gegen dieses Schiff
		if (m_pShip->GetShipSize() >= NORMAL && m_pTarget->m_pShip->HasSpecial(DOGFIGHTER))
			boni += 20;
		// Wenn wir die Dogkiller/Antinahkämpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus gegen kleine
		// Schiffe
		if (m_pTarget->m_pShip->GetShipSize() == SMALL && m_pShip->HasSpecial(DOGKILLER))
			boni += 20;
		// Wenn wir gegen einen Dogkiller/Antinahkämpfer kämpfen und mindst. eine mittlere Größe haben, dann bekommen
		// wir einen 20% Bonus gegen dieses Schiff
		if (m_pShip->GetShipSize() >= NORMAL && m_pTarget->m_pShip->HasSpecial(DOGKILLER))
			boni += 20;

		// Nur wenn die Distanz größer als drei Felder ist wird geschossen, sonst ja gerammt
		float distance = m_KO.DistanceToPosition_B(m_pTarget->m_KO);
		float old_distance = distance;
		// Wenn das Schiff getarnt ist, dann wird zu 5% gefeuert wenn das Schiff in Torpedoreichweite ist. Somit fliegt man
		// noch ein wenig näher ran und veringert die Distanz
		if (m_bShootCloaked == FALSE && rand()%100 > 4)
			return CPoint(-1,-1);
		
		if (distance >= 3.0f)
		{
			// voraussichtlicher Gesamtschaden dieser Torpedoattacke
			UINT damage = 0;
			// Anzahl der verschieden Launcher durchgehen
			for (int i = torpedoStart.x; i < m_pShip->GetTorpedoWeapons()->GetSize(); i++)
				if (m_Fire.torpedo[i] == 0 || torpedoStart.y > 0)
					// Anzahl dieses bestimmten Launchers durchgehen
					for (int j = torpedoStart.y; j < m_pShip->GetTorpedoWeapons()->GetAt(i).GetNumberOfTupes(); j++)
					{
						// Wenn der voraussichtliche Torpedogesamtschaden viel größer ist als die noch aktuelle Hülle
						// plus Schilde des Gegners, dann Abbruch und neues Ziel aufschalten und angreifen. Wir geben
						// hier die Art und die aktuelle Numer des Beams zurück
						if (damage > ((m_pTarget->m_pShip->GetShield()->GetCurrentShield() 
							+ m_pTarget->m_pShip->GetHull()->GetCurrentHull()) * 1.5))
							return CPoint(i,j);
						
						distance = old_distance;
						CTorpedo torpedo;
						// Jetzt das Ziel des Torpedos berechnen. Dafür den Kurs des Zielschiffes anschauen. Berechnen
						// nach welcher Zeit der Torpedo sich mit dem Ziel treffen würde. Dort hin müssen wir schießen.
						Position targetKO = m_pTarget->m_KO;
						if (distance > TORPEDOSPEED)
						{
							// Auf den wievielten Routeneintrag des Schiffes wird gerade gezielt? 
							USHORT route = 0;
							do {
								// Hat unser Ziel eine Route gespeichert?
								if (m_pTarget->m_Route.GetSize() > route)
								{
									distance = m_KO.DistanceToPosition_B(m_pTarget->m_Route.GetAt(route));
									targetKO = m_pTarget->m_Route.GetAt(route);
									route++;
								}
								else
									break;
							} while ((distance / TORPEDOSPEED) >= route);
						}
						torpedo.m_TargetKO = targetKO;
						
						// Ich will für den Torpedo immer eine Zielkoordinate, welche eine Entfernung von mindst. 200 hat,
						// soweit fliegt ein Torpedo.
						// DAS FUNKTIONIERT NICHT SO WIE ICH WILL, DIE TORPEDOS FLIEGEN NICHT AN DIE RICHTIGE STELLE!!!!
						/*if (old_distance < MAX_TORPEDO_RANGE)
						{
							Position temp = torpedo.m_TargetKO;
							torpedo.m_TargetKO = targetKO * (MAX_TORPEDO_RANGE / distance);
							distance = m_KO.DistanceToPosition_B(torpedo.m_TargetKO);
							CString s;
							s.Format("Distanz zu klein: %lf\nold Distanz: %lf",distance,old_distance);
							//AfxMessageBox(s);
							s.Format("old: %d|%d|%d\nmodified: %d|%d|%d\nModifier: %lf",temp.x,temp.y,temp.z,torpedo.m_TargetKO.x,torpedo.m_TargetKO.y,torpedo.m_TargetKO.z,(MAX_TORPEDO_RANGE / distance));
							//AfxMessageBox(s);
						}*/

						/* Wenn die Distanz nicht größer als 200 ist werden die Torpedos abgefeuert
						 Hier überprüfe ich mit der old_distance. Weil wenn ich die nehmen würde, die wir nach einer
						 bestimmten Zeit erst haben, sind die Schiffe, die schnell fliegen im Nachteil. Deren Gegner
						 würden zuerst schießen, weil die Distanz schneller kleiner wird.
						*/
						if (old_distance <= MAX_TORPEDO_RANGE)
						{
							m_bShootCloaked = TRUE;
							m_Fire.torpedo[i] = m_pShip->GetTorpedoWeapons()->GetAt(i).GetTupeFirerate();
							// Soviele Torpedos dem Feld zufügen wie wir mit einem Mal mit dem Launcher verschießen
							torpedo.m_iNumber = m_pShip->GetTorpedoWeapons()->GetAt(i).GetNumber();					
							torpedo.m_KO = m_KO;
							torpedo.m_iPower = (m_pShip->GetTorpedoWeapons()->GetAt(i).GetTorpedoPower() * (m_iModifier + boni) / 100);
							torpedo.m_byType = m_pShip->GetTorpedoWeapons()->GetAt(i).GetTorpedoType();
							torpedo.m_sOwner = m_pShip->GetOwnerOfShip();
							torpedo.m_byManeuverability = this->m_byManeuverability;
							torpedo.m_iModi = GetCrewExperienceModi() + m_pShip->GetTorpedoWeapons()->GetAt(i).GetAccuracy() + boni;
							CT->Add(torpedo);
							damage += torpedo.m_iPower * torpedo.m_iNumber;
						}
					}
		}
	}
	return CPoint(-1,-1);
}

// Diese Funktion gibt den Modifikator, den wir durch die Crewerfahrung erhalten zurück
BYTE CCombatShip::GetCrewExperienceModi()
{
	// Jedes Erfahrungslevel gibt einen Bonus von 20% auf den Schaden. Außerdem ist die Trefferwahrscheinlichkeit höher.
	/*CString exp;
	exp.Format("Bonus auf Grund der Schiffserfahrung: %d%%\nLevel: %d", m_pShip->GetExpLevel() * 20, m_pShip->GetExpLevel());
	AfxMessageBox(exp);
	*/
	if (m_pShip != NULL)
		return m_pShip->GetExpLevel() * 20;
	else
		return 0;
}

/// Diese Funktion stellt die vorhandenen regenerativen Schilde auf die feindliche schilddurchschlagende Waffe ein.
void CCombatShip::ActRegShield()
{
	if (m_pShip->GetShield()->GetRegenerative())
		m_bRegShieldStatus = TRUE;
}