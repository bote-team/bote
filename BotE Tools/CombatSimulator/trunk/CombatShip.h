/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Ship.h"
#include "Torpedo.h"


/// Eine Struktur für die Zeit wann wieder Beam- und Torpedowaffen abgefeuert werden können.
/// Da Folgende gilt glaube nicht mehr!
/// Die Zeiten in dieser Struktur entsprechen nicht den wahren Sekunden. Ich habe zur Umrechnung
/// 1sek == 10 Zeiteinheiten angesetzt. Somit bedeutet z.B. 5sek ein Wert von 50.
struct ShootTime
{
	CArray<BYTE,BYTE> phaser;		///< Gibt an wann wir wieder die Phaserwaffe abfeuern können
	CArray<BYTE,BYTE> torpedo;		///< Gibt an wann wir wieder die Torpedowaffe abfeuern können
	BOOLEAN phaserIsShooting;		///< Feuert ein Phaser gerade?
};

class CCombatShip :	public CObject
{
	friend class CCombat;
	friend class CTorpedo;
	friend class CCombatSimulatorView;
	friend class CCombatSimulatorDoc;
public:
	/// Konstruktor
	CCombatShip(void);

	/// Destruktor
	~CCombatShip(void);

	/// Kopierkonstruktor
	CCombatShip(const CCombatShip & rhs);
	
	/// Zuweisungsoperatur
	CCombatShip & operator=(const CCombatShip &);

	/**
	 * Diese Funktion setzt die Manövriebarkeit des Schiffes. Sie muss direkt nach anlegen des CombatSchiffes aufgerufen
	 * werden.
	 */
	void SetManeuverability(BYTE man) {m_byManeuverability = man;}

	/**
	 * Diese Funktion gibt den möglichen Bonus durch die Manövriebarkeit bei einem Angriff zurück. Übergeben müssen
	 * dafür die Manövrierbarkeit des Angreifers und die Manövrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitBoni(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion gibt den möglichen Malus durch die Manövriebarkeit bei einem Angriff zurück. Übergeben müssen
	 * dafür die Manövrierbarkeit des Angreifers und die Manövrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitMali(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion stellt die vorhandenen regenerativen Schilde auf die feindliche schilddurchschlagende Waffe ein.
	 */
	void ActRegShield();

	/**
	* Diese Funktion berechnet den nächsten Punkt auf dem Weg zum Ziel, den das Schiff erreichen wird.
	*/
	void CalculateNextPosition();

	/**
	* Diese Funktion setzt das Schiff auf den nächsten Punkt seiner vorher berechneten Flugroute.
	*/
	void GotoNextPosition();

	/**
	* Diese Funktion führt einen Beamangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei die Beamnummer übergeben werden, ab welcher die Berechung durchgeführt
	* werden soll. Wenn während des Angriff das Ziel vernichtet wird, dann gibt die Funktion die aktuelle Beamnummer
	* zurück. Ansonsten gibt sie immer (-1/-1) zurück. Der Rückgabewert ist eine Struktur mit 2 Variablen. Die erste
	* Variable gibt die Nummer des Beams im Feld an, die zweite die Nummer der Anzahl des Beams.
	*/
	CPoint AttackEnemyWithBeam(CPoint beamStart);

	/**
	* Diese Funktion führt einen Torpedoangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei ein Zeiger auf das Feld aller Torpedos im Kampf <code>CT<code>
	* übergeben. Diese Funktion generiert dann automatisch die entsprechenden Torpedoobkekte und fügt diese
	* in <code>CT<code> ein. Wenn während des Angriff das Ziel vernichtet wird, dann gibt die Funktion die 
	* aktuelle Torpedonummer zurück. Ansonsten gibt sie immer (-1/-1) zurück. Der Rückgabewert ist eine Struktur
	* mit 2 Variablen. Die erste Variable gibt die Nummer der Art des Launchers im Feld an, die zweite
	* die Nummer der Anzahl dieses Launchers.
	*/
	CPoint AttackEnemyWithTorpedo(CombatTorpedos* CT, CPoint torpedoStart);

	/**
	* Diese Funktion gibt den Modifikator, den wir durch die Crewerfahrung erhalten zurück
	*/
	BYTE GetCrewExperienceModi();

	/**
	 * Diese Funktion gibt einen Wahrheitswert zurück, ob sich die Schilde schon auf eine schilddurchschlagende
	 * Waffe eingestellt haben.
	 */
	BOOLEAN GetActRegShields() const {return m_bRegShieldStatus;}

	/// Funktion berechnet, ob ein Feuersystem aufgrund der Position des Schiffes, der Position des Systems auf dem Schiff und
	/// dessen Feuerwinkel auch feuern kann.
	/// @param arc Zeiger auf Schussfeld
	/// @return Wahrheitswert
	bool AllowFire(const CFireArc* arc);
	
private:
	// private Funktionen
	
	/// Diese Funktion berechnet den Angriffsbonus, den Schiffe auf Grund ihrer Spezialeigenschaften womöglich erhalten.
	/// @return Angriffsbonus durch Schiffseienschaften
	BYTE GetAccBoniFromSpecials(void);

	/// Diese Funktion berechnet die Trefferwahrscheinlichkeit des Beams und fügt dem Feindschiff wenn möglich Schaden zu.
	/// @param beamWeapon Nummer der Beamwaffe
	/// @param distance Distanz zwischen unserem Schiff und dem Gegner
	/// @param boni Bonus durch Schiffseigenschaften
	void FireBeam(int beamWeapon, int distance, BYTE boni);

	/// Diese Funktion berechnet die Trefferwahrscheinlichkeit des Beams und fügt dem Feindschiff wenn möglich Schaden zu.
	/// @param CT Feld der Torpedos im Kampf
	/// @param beamWeapon Nummer der Beamwaffe
	/// @param targetKO Zielkoordinate des Torpedos
	/// @param boni Bonus durch Schiffseigenschaften
	/// @return maximal zu erwartender Schaden
	UINT FireTorpedo(CombatTorpedos* CT, int torpedoWeapon, vec3i targetKO, BYTE boni);

	// Attribute
	
	/// Zeiger auf das Schiff, welches hier im Kampf ist
	CShip*	m_pShip;
	
	/// Aktuelle Position (Koordinate im Raum) des Schiffes
	vec3i m_KO;					
	
	/// Flugroute des Schiffes, welche die folgenden Koordinaten beinhaltet
	CArray<vec3i, vec3i> m_Route;
	
	/// Zeit bis das Schiff wieder seine Waffen abfeuern kann. Wenn dieser Wert NULL erreicht hat, dann kann es
	/// die Waffen wieder feuern.
	ShootTime m_Fire;
	
	/// werden Pulsebeams geschossen? Nur für den CombatSimulator
	BOOLEAN m_bPulseFire;
	
	/// Die Manövrierbarkeit des Schiffes im Kampf.
	BYTE m_byManeuverability;

	/// Die aktuelle Taktik des Schiffes
	BYTE m_Tactic;

	/// Der Schadens- und Verteidigungsbonus/malus der Schiffe der Rasse
	USHORT m_iModifier;

	/// Zeiger auf das Schiff, welches es als Ziel erfasst hat
	CCombatShip* m_pTarget;

	/// Status der regenerativen Schilde, angepaßt oder nicht 
	BOOLEAN m_bRegShieldStatus;

	/// Ist das Schiff noch getarnt oder nicht. Nach dem Feuern hat das Schiff noch 50 bis 70 Ticks Zeit,
	/// bis es wirklich angegriffen werden kann. Solange m_byCloak größer als NULL ist, ist das Schiff getarnt.
	BYTE m_byCloak;

	/// Wenn dieser Counter auf 255 hochgezählt wurde, dann kann sich das Schiff wieder tarnen
	BYTE m_byReCloak;

	/// Hat das Schiff schonmal getarnt geschossen?
	BOOLEAN m_bShootCloaked;

public:
	float SCAL, SCAL2;
};

typedef CArray<CCombatShip,CCombatShip> CombatShips;