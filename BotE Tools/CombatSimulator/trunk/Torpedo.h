/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Vec3.h"

class CCombatShip;
class CTorpedo : public CObject
{
	friend class CCombatShip;
	friend class CComabt;
	friend class CCombatSimulatorView;
public:
	/// Konstruktor
	CTorpedo(void);
	
	/// Destruktor
	~CTorpedo(void);

	/// Kopierkonstruktor
	CTorpedo(const CTorpedo & rhs);
	
	/// Zuweisungsoperatur
	CTorpedo & operator=(const CTorpedo &);

	/**
	* Funktion berechnet die Flugbahn des Torpedos und macht die Kollisionsabfrage mit allen anderen Schiffen
	* im Kampf. Wenn der Torpedo trifft, dann wird auch der Schaden am Schiff verrechnet und die Funktion gibt
	* TRUE zurück. Wenn wir keinen Treffer landen, aber MAX_TORPEDO_RANGE überschritten haben gibt die Funktion
	* auch ein TRUE zurück. Ansonsten gibt die Funktion immer FALSE zurück
	*/
	BOOLEAN Fly(CArray<CCombatShip*,CCombatShip*>* CS);

private:
	/// aktuelle Koordinate im Raum
	vec3i m_KO;

	/// Zielkoordinate im Raum
	vec3i m_TargetKO;

	/// Wenn wir mehrere Torpedos auf einmal auf ein und das selbe Ziel verschießen, so setzen wir diese Variable immer
	/// mit der Anzahl der Torpedos, die sich auf dieser Koordinate befinden. Im Normalfall ist dieser Wert eins, wenn
	/// unser Werfer aber 10 Torpedos verschießt und alle das gleiche Ziel haben, dann setzen wird diesen Wert auf 10.
	BYTE m_iNumber;
	
	/// Die Distanz, die der Torpedo schon zurückgelegt hat. Wenn MAX_TORPEDO_RANGE erreicht wurde wird der Torpedo
	/// automatisch vernichtet
	USHORT m_iDistance;

	/// Schaden des Torpedos bei Einschlag in das Ziel
	USHORT m_iPower;

	/// Der Torepdotyp
	BYTE m_byType;

	/// Welche Rasse hat den Torpedo abgefeuert. Also wer ist der Besitzer des Torpedos
	BYTE m_iOwner;

	/// Die Manövriebarkeit des Schiffes, welches den Torepdo abgefeuert hat
	BYTE m_byManeuverability;

	/**
	* Trefferwahrscheinlichkeit des Torepdowerfers + Zielgenauigkeit durch Erfahrung der Mannschaft des Schiffes,
	* welches diesen Torpedo abgeschossen hat
	*/
	short m_iModi;

	/**
	* Diese private Funktion berechnet den Schaden, den ein Torpedo am gegnerischen Schiff anrichtet. Sie übernimmt
	* dafür den Zeiger auf das gegnerische Schiff <code>CS<code>.
	*/
	void MakeDamage(CCombatShip* CS);

	/**
	* Diese private Funktion setzt den Torpedo auf den nächsten Punkt seiner Flugbahn. Diese Funktion nur aufrufen,
	* wenn der Torpedo keinen Schaden an irgendeinem Schiff gemacht hat. Hat der Torpedo aus irgendeinem Grund
	* seine maximale Lebensdauer erreicht, gibt die Funktion ein TRUE zurück, anonsten FALSE.
	*/
	BOOLEAN FlyToNextPosition();

	/**
	* Diese private Funktion gibt ein TRUE zurück, wenn der Torpedo sein Ziel getroffen hat, andernfalls wird ein
	* FALSE zurückgegeben. Ob ein Torpedo trifft hängt von mehreren Parametern ab, welche diese Funktion alle
	* berücksichtigt. Wenn er nicht trifft ruft die Funktion selbstständig <code>FlyToNextPosition<code> auf. Als
	* Parameter wird ein Zeiger auf das Schiff übergeben, welches vielleicht getroffen wird und die Distanz zwischen
	* unserem Torepdo und dem Zielschiff.
	*/
	BOOLEAN PerhapsImpact(CCombatShip* CS, USHORT minDistance);
};

typedef CArray<CTorpedo*, CTorpedo*> CombatTorpedos;
