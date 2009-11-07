// Ship.cpp: Implementierung der Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ship.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShip, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShip::CShip()
{
	m_bySpecial[0] = m_bySpecial[1] = NONE;
}

CShip::~CShip()
{
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); )
		m_TorpedoWeapons.RemoveAt(i);
	for (int i = 0; i < m_BeamWeapons.GetSize(); )
		m_BeamWeapons.RemoveAt(i);
	m_TorpedoWeapons.RemoveAll();
	m_BeamWeapons.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktion
//////////////////////////////////////////////////////////////////////
// Funktion gibt den Schiffstyp als char* zurück
CString CShip::GetShipTypeAsString(BOOL plural)
{
	/*	#define TRANSPORTER			0
		#define COLONYSHIP            1
		#define PROBE				2
		#define SCOUT				3
		#define FIGHTER				4	// Jäger
		#define FRIGATE				5
		#define DESTROYER			6
		#define CRUISER				7
		#define HEAVY_DESTROYER     8
		#define HEAVY_CRUISER       9
		#define BATTLESHIP			10
		#define FLAGSHIP			11
		#define OUTPOST				12
		#define STARBASE            13
		#define ALIEN				14 */
	// Oben im Beschreibungsrechteck den Namen des Projektes hinschreiben
	char* shipType;
	switch (m_iShipType)
	{
	case TRANSPORTER: shipType = "Transportship"; break;
	case KOLOSHIP: shipType = "Colonyship"; break;
	case PROBE: shipType = "Probe"; break;
	case SCOUT: shipType = "Scout"; break;
	case FIGHTER: shipType = "Fighter"; break;
	case FRIGATE: shipType = "Frigate"; break;
	case DESTROYER: shipType = "Destroyer"; break;
	case CRUISER: shipType = "Cruiser"; break;
	case HEAVY_DESTROYER: shipType = "Heavy Destroyer"; break;
	case HEAVY_CRUISER: shipType = "Heavy Cruiser"; break;
	case BATTLESHIP: shipType = "Battleship"; break;
	case FLAGSHIP: shipType = "Flagship"; break;
	case OUTPOST: shipType = "Outpost"; break;
	case STARBASE: shipType = "Starbase"; break;
	case ALIEN: shipType = "Alien"; break;
	}
	return shipType;
}

/// Funktion gibt die gesamte Offensivpower des Schiffes zurück, welches es in 100s anrichten würde. Dieser
/// Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum Vergleich heranzuziehen.
UINT CShip::GetCompleteOffensivePower()
{
	UINT beamDmg	 = 0;
	UINT torpedoDmg  = 0;
	for (int i = 0; i < GetBeamWeapons()->GetSize(); i++)
	{
		short counter = 0;
		for (int j = 0; j < 100; j++)
		{
			if (counter == 0)
				counter = GetBeamWeapons()->GetAt(i).GetBeamLenght() 
							+ GetBeamWeapons()->GetAt(i).GetRechargeTime();
			if (counter > GetBeamWeapons()->GetAt(i).GetRechargeTime())
			{
				UINT tempBeamDmg = (UINT)GetBeamWeapons()->GetAt(i).GetBeamPower()
							* GetBeamWeapons()->GetAt(i).GetBeamNumber()
							* GetBeamWeapons()->GetAt(i).GetShootNumber();
				// besondere Beamfähigkeiten erhöhen den BeamDmg um einen selbst gewählten Mulitplikator
				// der dadurch erhaltende Schaden entspricht nicht dem wirklichen Schaden!
				if (GetBeamWeapons()->GetAt(i).GetPiercing())
					tempBeamDmg = (UINT)(tempBeamDmg * 1.5);
				if (GetBeamWeapons()->GetAt(i).GetModulating())
					tempBeamDmg *= 3;
				beamDmg += tempBeamDmg;
			}			
			counter--;
		}
	}
	beamDmg /= 3;
	for (int i = 0; i < GetTorpedoWeapons()->GetSize(); i++)
	{
		UINT tempTorpedoDmg = (UINT)(GetTorpedoWeapons()->GetAt(i).GetTorpedoPower() *
									GetTorpedoWeapons()->GetAt(i).GetNumber() * 100 *
									GetTorpedoWeapons()->GetAt(i).GetNumberOfTupes() /
									GetTorpedoWeapons()->GetAt(i).GetTupeFirerate());
		// besondere Torpedofähigkeiten erhöhen den Torpedoschaden um einen selbst gewählten Mulitplikator
		// der dadurch erhaltende Schaden entspricht nicht dem wirklichen Schaden!
		BYTE type = GetTorpedoWeapons()->GetAt(i).GetTorpedoType();
		if (CTorpedoInfo::GetPenetrating(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.5);
		if (CTorpedoInfo::GetIgnoreAllShields(type))
			tempTorpedoDmg *= 3;
		if (CTorpedoInfo::GetCollapseShields(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.25);
		if (CTorpedoInfo::GetDoubleShieldDmg(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.5);
		if (CTorpedoInfo::GetDoubleHullDmg(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 2);
		if (CTorpedoInfo::GetReduceManeuver(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.1);
		torpedoDmg += tempTorpedoDmg;
	}
	return (beamDmg + torpedoDmg);
}

/// Funktion gibt die gesamte Defensivstärke des Schiffes zurück. Dabei wird die maximale Hülle, die maximalen
/// Schilde und die Schildaufladezeit beachtet. Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum
/// Vergleich heranzuziehen.
UINT CShip::GetCompleteDefensivePower()
{
	UINT def = GetHull()->GetMaxHull() + GetShield()->GetMaxShield()
		+ (GetShield()->GetMaxShield() / 300 + 2 * GetShield()->GetShieldType()) * 100;
	if (GetHull()->GetAblative())
		def = (UINT)(def * 1.1);
	if (GetHull()->GetPolarisation())
		def = (UINT)(def * 1.1);

	return def;
}