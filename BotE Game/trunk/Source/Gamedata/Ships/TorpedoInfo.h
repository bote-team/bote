/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <vector>

/// Enum aller möglichen Torpedospezialeigenschaften
enum TORPEDO_SPECIALS
{
	NO_SPECIAL			= 0,
	MICROTORPEDO		= 1,	///< Microtorpedo (nur für Microwerfer)
	PENETRATING			= 2,	///< schilddurchschlagender Torpedo
	DOUBLESHIELDDMG		= 4,	///< Torpedo macht doppelten Schaden an den Schilden
	DOUBLEHULLDMG		= 8,	///< Torpedo macht doppelten Schaden an der Hülle
	IGNOREALLSHIELDS	= 16,	///< Torpedo ignoriert alle Arten von Schilden
	COLLAPSESHIELDS		= 32,	///< Torpedotreffer kann Schilde kollabieren lassen
	REDUCEMANEUVER		= 64	///< Bei einem Treffer besteht die Chance, die Manövrierbarkeit des Zieles auf NULL zu verringern.
};

// Eigenschaften eines Torpedos
struct TORPEDOINFO
{
	// Attribute
	CString		sName;			///< der Name des Torpedos
	USHORT		nDmg;			///< die Stärke eines Torpedos
	int			nSpecials;		///< Spezialeigenschaften (bitweise gespeichert)

	TORPEDOINFO(const CString& _sName, USHORT _nDmg, int _nSpecials = NO_SPECIAL)
	{
		sName		= _sName;
		nDmg		= _nDmg;
		nSpecials	= _nSpecials;
	}
};

/// Klasse enthält feste Informationen zu bestimmten Torpedotypen
class CTorpedoInfo
{
private:
	/// Standardkonstruktor
	CTorpedoInfo(void);

	/// Kopierkonstruktor
	CTorpedoInfo(const CTorpedoInfo&) {};

public:
	/// Destruktor
	~CTorpedoInfo(void);

	// Zugriffsfunktionen zum Lesen der Membervariablen

	/// Diese Funktionen gibt den Torepdonamen zurück.
	static const CString& GetName(BYTE type) { return GetInstance()->m_vInfos[type].sName; }

	/// Diese Funktionen gibt die Torepdostärke zurück.
	static USHORT GetPower(BYTE type) { return GetInstance()->m_vInfos[type].nDmg; }

	/// Diese Funktionen gibt zurück, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	static bool GetMicro(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & MICROTORPEDO) == MICROTORPEDO; }

	/// Diese Funktionen gibt zurück, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	static bool GetPenetrating(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & PENETRATING) == PENETRATING; }

	///Diese Funktionen gibt zurück, ob der Torpedo doppelten Schaden an den Schilden macht.
	static bool GetDoubleShieldDmg(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & DOUBLESHIELDDMG) == DOUBLESHIELDDMG; }

	/// Diese Funktionen gibt zurück, ob der Torpedo doppelten Schaden an der Hülle macht.
	static bool GetDoubleHullDmg(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & DOUBLEHULLDMG) == DOUBLEHULLDMG; }

	/// Diese Funktionen gibt zurück, ob der Torpedo jegliche Schildart ignoriert..
	static bool GetIgnoreAllShields(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & IGNOREALLSHIELDS) == IGNOREALLSHIELDS; }

	/// Diese Funktionen gibt zurück, ob durch einen Torpedotreffer möglicherweise die Schilde zusammenbrechen.
	static bool GetCollapseShields(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & COLLAPSESHIELDS) == COLLAPSESHIELDS; }

	/// Diese Funktionen gibt zurück, ob durch einen Torpedotreffer möglicherweise die Manövriebarkeit auf
	/// NULL verringert wird.
	static bool GetReduceManeuver(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & REDUCEMANEUVER) == REDUCEMANEUVER; }

private:
	// Funktionen
	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CTorpedoInfo* GetInstance(void);

	// Attribute
	std::vector<TORPEDOINFO> m_vInfos;	///< alle möglichen Torpedotypen
};
