#include "stdafx.h"
#include "Torpedo.h"
#include "CombatShip.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTorpedo::CTorpedo(void)
{
	m_iDistance = 0;
}

CTorpedo::~CTorpedo(void)
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTorpedo::CTorpedo(const CTorpedo &rhs)
{
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_TargetKO.x  = rhs.m_TargetKO.x;
	m_TargetKO.y  = rhs.m_TargetKO.y;
	m_TargetKO.z  = rhs.m_TargetKO.z;
	m_iNumber = rhs.m_iNumber;
	m_iDistance = rhs.m_iDistance;
	m_iPower = rhs.m_iPower;
	m_byType = rhs.m_byType;
	m_sOwner = rhs.m_sOwner;
	m_iModi = rhs.m_iModi;
	m_byManeuverability = rhs.m_byManeuverability;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTorpedo & CTorpedo::operator=(const CTorpedo & rhs)
{
	if (this == &rhs)
		return *this;
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_TargetKO.x  = rhs.m_TargetKO.x;
	m_TargetKO.y  = rhs.m_TargetKO.y;
	m_TargetKO.z  = rhs.m_TargetKO.z;
	m_iNumber = rhs.m_iNumber;
	m_iDistance = rhs.m_iDistance;
	m_iPower = rhs.m_iPower;
	m_byType = rhs.m_byType;
	m_sOwner = rhs.m_sOwner;
	m_iModi = rhs.m_iModi;
	m_byManeuverability = rhs.m_byManeuverability;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

// Funktion berechnet die Flugbahn des Torpedos und macht die Kollisionsabfrage mit allen anderen Schiffen
// im Kampf. Wenn der Torpedo trifft, dann wird auch der Schaden am Schiff verrechnet und die Funktion gibt
// TRUE zurück. Wenn wir keinen Treffer landen, aber MAX_TORPEDO_RANGE überschritten haben gibt die Funktion
// auch ein TRUE zurück. Ansonsten gibt die Funktion immer FALSE zurück
BOOLEAN CTorpedo::Fly(CArray<CCombatShip*,CCombatShip*>* CS)
{
	// Wir haben ja den Stützpunkt unseres Torpedos und den Vektor zum Zielpunkt.
	// Jetzt überprüfe ich, ob ein feindliches Schiff auf diesem Vektor liegt dessen
	// Distanz kleiner als der TORPEDOSPPED (20) ist. Das Schiff mit der kleinsten Distanz
	// wird dann getroffen. Wenn ich kein Schiff finde, welches auf der Bahn liegt und
	// dessen Distanz kleiner TORPEDOSPEED ist oder ich finde gar kein Schiff auf der Bahn,
	// dann können wir auf dem Punkt nach TORPEDOSPEED-Entfernung mit dem Torpedo vorrücken
	
	// Stützpunkt ist der Punkt m_KO
	// Der Vektor ist der Vektor von m_KO zu m_TargetKO
	// t.x = (D.x - A.x) / C.x mit C = (B-A) wobei A = m_KO und B = m_TargetKO und D = CS->m_KO
	
	vec3i c = m_TargetKO - m_KO;
	int	minDistance = -1;
	short shipNumber = -1;
	for (int i = 0; i < CS->GetSize(); i++)
		if (m_sOwner != CS->GetAt(i)->m_pShip->GetOwnerOfShip() && CS->GetAt(i)->m_pShip->GetHull()->GetCurrentHull() > 0)
		{
			vec3i t;
			if (c.x != 0)
				t.x = (CS->GetAt(i)->m_KO.x - m_KO.x) / c.x;
			else
				t.x = 0;
			if (c.y != 0)
				t.y = (CS->GetAt(i)->m_KO.y - m_KO.y) / c.y;
			else
				t.y = 0;
			if (c.z != 0)
				t.z = (CS->GetAt(i)->m_KO.z - m_KO.z) / c.z;
			else
				t.z = 0;
			// Wenn irgendein t ungleich null ist aber negativ ist, dann liegt das Schiff zwar auf unserer Flugbahn,
			// aber entgegengesetzt zur Torpedoflugrichtung. Also überprüfe ich das jedes t größer gleich NULL sein muß
			if (t.x >= 0 && t.y >= 0 && t.z >= 0)	
				if ((t.x == 0 && t.y == t.z) || (t.y == 0 && t.x == t.z) || (t.z == 0 && t.x == t.y) || (t.x == t.y == t.z))
					// Dann liegt das Schiff auf unserer Flugbahn, Distanz berechnen
					if (minDistance == -1 || minDistance > m_KO.Distance(CS->GetAt(i)->m_KO))
					{
						minDistance = m_KO.Distance(CS->GetAt(i)->m_KO);
						shipNumber = i;
					}
		}
	// Wenn jetzt die minimale Distanz kleiner gleich der TORPEDOSPEED ist, dann gibts nen Einschlag
	if (minDistance <= TORPEDOSPEED && shipNumber != -1)
	{
		// Hier wird vielleicht Schaden am gegnerischen Schiff gemacht
		// Die Anzahl der auf diesem Feld gleichzeitigen Torpedos mit selben Ziel durchgehen
		BYTE count = 0;
		for (int i = 0; i < m_iNumber; i++)
			// Wenn der Torpedo trifft wird count um eins inkrementiert		
			count += PerhapsImpact(CS->GetAt(shipNumber), (USHORT)minDistance);
		// Die Anzahl dieser Torpedos in dem Feld um die Anzahl der Torpedos verringern, welche ihr Ziel getroffen haben
		m_iNumber -= count;
		// Wenn m_iNumber gleich NULL ist, dann gibt es hier keine Torpedos mehr und wir können ein TRUE zurückgeben.
		// Dieses TRUE veranlaßt, dass dieses Torpedoobjekt zerstört wird
		if (m_iNumber == NULL)
			return TRUE;
		// Die restlichen Torpedos fliegen ganz normal auf ihrer Bahn weiter, so als hätten sie kein Ziel auf dieser
		// Bahn gehabt
		else
			return FlyToNextPosition();
	}
	// Wenn kein Schiff in Trefferreichweite ist können wir den Torpedo um TORPEDOSPEED weiterfliegen lassen
	else
		return FlyToNextPosition();
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

// Diese private Funktion berechnet den Schaden, den ein Torpedo am gegnerischen Schiff anrichtet.
void CTorpedo::MakeDamage(CCombatShip* CS)
{
	// DAMAGE_TO_HULL (10%) des Schadens gehen immer auf die Hülle.
	int torpedoDamage = (m_iPower * 100) / CS->m_iModifier;

//	CString dam;
//	dam.Format("Torpedoschaden an Schiff zuvor: %s Schilde: %d Hülle: %d Torpedoschaden: %d",
//		CS->m_pShip->GetShipClass(), CS->m_pShip->GetShield()->GetCurrentShield(),
//		CS->m_pShip->GetHull()->GetCurrentHull(), torpedoDamage);
//	MYTRACE(MT::LEVEL_INFO, dam);	

	int toHull = 0;
	// Wenn das feindliche Schiff keine ablative Hüllenpanzerung hat, dann gehen 10% des Schadens sofort
	// auf die Hülle
	if (CS->m_pShip->GetHull()->GetAblative() == FALSE)
		toHull = (int)(torpedoDamage * DAMAGE_TO_HULL);
	// wenn wir schilddurchschlagende Torpedos haben und das feindliche Schiff keine auf unseren Torpedo eingestellten
	// regenerativen Schilde hat, dann machen wir kompletten Schaden an der Hülle. Wenn wir Torpedos haben, die alle
	// Schilde durchdringen, dann machen wir hier immer kompletten Schaden an der Hülle.
	if ((CTorpedoInfo::GetPenetrating(m_byType) == TRUE && CS->GetActRegShields() == FALSE)
		|| CTorpedoInfo::GetIgnoreAllShields(m_byType) == TRUE)
		toHull = torpedoDamage;
	CS->m_pShip->GetHull()->SetCurrentHull(-toHull);
		
	// den restlichen Torpedoschaden ermitteln, welcher nicht direkt auf die Hülle ging
	torpedoDamage -= toHull;
	// Torpedos verlieren ihre Effizienz, wenn sie auf noch relativ starke Schilde treffen. Umso weniger von den Schilden
	// noch aktiv ist, umso stärker wirkt der Torpedo. Dies gilt jedoch nicht für Microtorpedos.
	if (CTorpedoInfo::GetMicro(m_byType) == FALSE)
	{
		UINT maxShield = CS->m_pShip->GetShield()->GetMaxShield();
		UINT curShield = CS->m_pShip->GetShield()->GetCurrentShield();
		float perc = (float)curShield / (float)maxShield;
		if (perc > 0.75f)
			perc = 0.25f;
		else if (perc > 0.50f)
			perc = 0.50f;
		else if (perc > 0.25f)
			perc = 0.75f;
		else
			perc = 1.0f;
		torpedoDamage = (int)(torpedoDamage * perc);
	}
	// Wenn die Torpedoart doppelten Schaden an den Schilden macht, dann einfach diesen hier *2 nehmen
	if (CTorpedoInfo::GetDoubleShieldDmg(m_byType))
		torpedoDamage *= 2;
	// Wenn die Torpedoart die Schilde kollabieren lassen kann, dann dies hier möglicherweise tun
	if (CTorpedoInfo::GetCollapseShields(m_byType))
		if (rand()%100 < 5)	// 5% Chance die Schilde zusammenbrechen zu lassen
			CS->m_pShip->GetShield()->SetCurrentShield(0);
	// Der komplette Schaden geht auf die Schilde
	if ((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage >= 0)
		CS->m_pShip->GetShield()->SetCurrentShield((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage);
	// Der Schaden geht teilweise auf Schilde und Hülle oder nur auf die Hülle
	else
	{
		// Wenn die Torpedoart doppelten Schaden an der Hülle macht, dann einfach diesen hier *2 nehmen
		if (CTorpedoInfo::GetDoubleHullDmg(m_byType))
			torpedoDamage *= 2;
		// Schaden der noch auf die Hülle geht
		CS->m_pShip->GetHull()->SetCurrentHull((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage);
		CS->m_pShip->GetShield()->SetCurrentShield(0);
	}
	
	// Wenn wir schilddurchschlagende Torpedos haben und das feindliche Schiff regenerative Schilde, so kann es diese
	// auf unsere Waffen einstellen
	if (CTorpedoInfo::GetPenetrating(m_byType) == TRUE && CS->m_pShip->GetShield()->GetRegenerative() == TRUE)
		CS->ActRegShield();
	// Wenn der Torpedo die Fähigkeit hat die Manövriebarkeit des Ziels auf 0 zu setzen, so wird dies hier beachtet
	if (CTorpedoInfo::GetReduceManeuver(m_byType))
		if (rand()%100 < 5)	// Nur mit einer 5% Wahrscheinlichkeit tritt dies auch ein
			CS->SetManeuverability(0);

//	dam.Format("Torpedoschaden an Schiff nachher: %s Schilde: %d Hülle: %d Torpedoschaden: %d",
//		CS->m_pShip->GetShipClass(), CS->m_pShip->GetShield()->GetCurrentShield(),
//		CS->m_pShip->GetHull()->GetCurrentHull(), torpedoDamage);
//	//AfxMessageBox(dam);
//	MYTRACE(MT::LEVEL_INFO, dam);	
}

// Diese private Funktion setzt den Torpedo auf den nächsten Punkt seiner Flugbahn. Diese Funktion nur aufrufen,
// wenn der Torpedo keinen Schaden an irgendeinem Schiff gemacht hat. Hat der Torpedo aus irgendeinem Grund
// seine maximale Lebensdauer erreicht, gibt die Funktion ein TRUE zurück, anonsten FALSE.
BOOLEAN CTorpedo::FlyToNextPosition()
{
	// TORPEDOSPEED vorrücken
	// unsere aktuelle Position im Raum
	vec3i a = m_KO;
	// die Zielposition im Raum
	vec3i b = m_TargetKO;
	// der Abstand zwischen diesen beiden Punkten
	int distance = a.Distance(b);
	if (distance == 0)
		return TRUE;
	short speed = TORPEDOSPEED;
	if (speed > distance)
		speed = (short)distance;
	
	float multi = (float)speed / (float)distance;
	vec3i temp = b - a;
	// Runden durch floor und +0.5
	temp.x = (int)(floor)(temp.x * multi + 0.5); temp.y = (int)(floor)(temp.y * multi + 0.5); temp.z = (int)(floor)(temp.z * multi + 0.5);
	m_KO = a + temp;
	//m_KO = a + (b-a) * (speed/distance);
	// Wegcounter hochrechnen
	m_iDistance += TORPEDOSPEED;
	if (m_iDistance >= MAX_TORPEDO_RANGE)
		return TRUE;
	return FALSE;
}

// Diese private Funktion gibt ein TRUE zurück, wenn der Torpedo sein Ziel getroffen hat, andernfalls wird ein
// FALSE zurückgegeben. Ob ein Torpedo trifft hängt von mehreren Parametern ab, welche diese Funktion alle
// berücksichtigt. Wenn er nicht trifft ruft die Funktion selbstständig <code>FlyToNextPosition<code> auf. Als
// Parameter wird ein Zeiger auf das Schiff übergeben, welches vielleicht getroffen wird und die Distanz zwischen
// unserem Torepdo und dem Zielschiff.
BOOLEAN CTorpedo::PerhapsImpact(CCombatShip* CS, USHORT minDistance)
{
	// Ein Torpedo trifft nicht 100prozentig. Ob ein Torpedo trifft hängt von mehreren Parametern ab.
	// Parameter von denen die Wahrscheinlichkeit eines Treffers abhängen sind
	//	+ m_iModi		... Modifikator durch Crew welche den Torpedo abfeuerte, den Acc durch den Werfer
	//						und den ToHitBonus durch die Manövriebarkeit
	m_iModi += CCombatShip::GetToHitBoni(m_byManeuverability, CS->m_byManeuverability);
	
	//	+ m_iDistance	... Schon zurückgelegte Strecke des Torpedos
	//	+ CS			... Das Zielschiff selbst

	// je größer m_iDistance + minDistance ist, desto geringer wird die Trefferwahrscheinlichkeit (Wert zwischen 0 und 200)
	// je schneller (setzt sich aus Schiffstyp und Geschwindigkeit zusammen) das Zielschiff im Kampf ist,
	//		umso geringer ist die Trefferwahrscheinlichkeit
	// je erfahrener die Crew des Zielschiffes ist,	umso geringer ist die Trefferwahrscheinlichkeit

	// zum Algorithmus:
	
	// ein Torpedo trifft normalerweise mit einer Wahrscheinlichkeit von XX%.
	// m_iMode - (m_iDistance+minDistance)*0.1 - CS->Manövrierbarkeit*2 - CS->Crewerfahrung???
	short probability = m_iModi - (short)((m_iDistance + minDistance) * 0.1) - 
		CCombatShip::GetToHitMali(m_byManeuverability, CS->m_byManeuverability) - CS->GetCrewExperienceModi();
			
	if (CS->m_pShip->GetShipSize() == 0)
		probability = (short)(probability * 0.66);
	else if (CS->m_pShip->GetShipSize() == 2)
		probability = (short)(probability * 1.33);
	else if (CS->m_pShip->GetShipSize() >= 3)
		probability = (short)(probability * 1.66);

	// Die Wahrscheinlichkeit beträgt mindestens 10% für einen Einschlag
	probability = max(probability, 10);
	
	short random = rand()%100;	// {0,99}

//	CString s;
//	s.Format("vielleicht Einschlag in Schiff %s Modi: %d Wahrscheinlichkeit: %d Randomwert: %d zurückgelegte Strecke: %d MinDistanz: %d ToHitMali: %d, Crew: %d", CS->m_pShip->GetShipClass(), m_iModi, probability, random, m_iDistance, minDistance, CCombatShip::GetToHitMali(m_byManeuverability, CS->m_byManeuverability), CS->GetCrewExperienceModi());
//	MYTRACE(MT::LEVEL_INFO, s);
//	CString s;
//	s.Format("m_iDistance: %d\nminDistance: %d\nprobability: %d\nrandom: %d",m_iDistance,minDistance,probability,random);
//	AfxMessageBox(s);

	if (probability > random)
	{
		MakeDamage(CS);
		return TRUE;
	}
	else
		return FALSE;
}