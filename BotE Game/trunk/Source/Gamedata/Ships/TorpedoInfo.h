/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"

class CTorpedoInfo : public CObject
{
private:
	/// Konstruktor
	CTorpedoInfo(void);

	/// Destruktor
	~CTorpedoInfo(void);

public:
	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktionen gibt den Torepdonamen zurück.
	 */
	static const CString& GetName(BYTE type) {SetValues(type); return m_strTorpedoName;}

	/**
	 * Diese Funktionen gibt die Torepdostärke zurück.
	 */
	static USHORT GetPower(BYTE type) {SetValues(type); return m_iTorpedoPower;}

	/**
	 * Diese Funktionen gibt zurück, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	 */
	static BOOLEAN GetPenetrating(BYTE type) {SetValues(type); return m_bPenetrating;}

	/**
	 * Diese Funktionen gibt zurück, ob der Torpedo doppelten Schaden an den Schilden macht.
	 */
	static BOOLEAN GetDoubleShieldDmg(BYTE type) {SetValues(type); return m_bDoubleShieldDmg;}

	/**
	 * Diese Funktionen gibt zurück, ob der Torpedo doppelten Schaden an der Hülle macht.
	 */
	static BOOLEAN GetDoubleHullDmg(BYTE type) {SetValues(type); return m_bDoubleHullDmg;}

	/**
	 * Diese Funktionen gibt zurück, ob der Torpedo jegliche Schildart ignoriert..
	 */
	static BOOLEAN GetIgnoreAllShields(BYTE type) {SetValues(type); return m_bIgnoreAllShields;}

	/**
	 * Diese Funktionen gibt zurück, ob durch einen Torpedotreffer möglicherweise die Schilde zusammenbrechen.
	 */
	static BOOLEAN GetCollapseShields(BYTE type) {SetValues(type); return m_bCollapseShields;}

	/**
	 * Diese Funktionen gibt zurück, ob durch einen Torpedotreffer möglicherweise die Manövriebarkeit auf
	 * NULL verringert wird.
	 */
	static BOOLEAN GetReduceManeuver(BYTE type) {SetValues(type); return m_bReduceManeuver;}

private:
	/**
	 * Private Funktion, die die Variablen anhand des übergebenen Torpedotypes setzt.
	 */
	static void SetValues(BYTE type);

	// Eigenschaften der Torpedos
	static BYTE	   m_byType;			///> letzter gesuchter Typ des Torpedos
	static CString m_strTorpedoName;	///> der Name des Torpedos
	static USHORT  m_iTorpedoPower;		///> die Stärke eines Torpedos
	static BOOLEAN m_bPenetrating;		///> sind es schilddurchschlagende Torpedos?
	static BOOLEAN m_bDoubleShieldDmg;	///> macht dieser Torpedo doppelten Schaden an den Schilden?
	static BOOLEAN m_bDoubleHullDmg;	///> macht dieser Torpedo doppelten Schaden an der Hülle?
	static BOOLEAN m_bIgnoreAllShields;	///> ignoriert der Torpedo alle Arten von Schilden?
	static BOOLEAN m_bCollapseShields;	///> Torpedotreffer kann Schilde kollabieren lassen
	static BOOLEAN m_bReduceManeuver;	///> bei einem Treffer besteht die Chance, die Manövriebarkeit auf NULL zu verringern
};
