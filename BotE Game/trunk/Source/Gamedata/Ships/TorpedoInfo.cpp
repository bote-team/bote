#include "stdafx.h"
#include "TorpedoInfo.h"

// statische Variable initialisieren
BYTE	CTorpedoInfo::m_byType				= 255;
CString CTorpedoInfo::m_strTorpedoName		= "";
USHORT  CTorpedoInfo::m_iTorpedoPower		= 0;
BOOLEAN CTorpedoInfo::m_bMicro				= false;
BOOLEAN CTorpedoInfo::m_bPenetrating		= false;
BOOLEAN CTorpedoInfo::m_bDoubleShieldDmg	= false;
BOOLEAN CTorpedoInfo::m_bDoubleHullDmg		= false;
BOOLEAN CTorpedoInfo::m_bIgnoreAllShields	= false;
BOOLEAN CTorpedoInfo::m_bCollapseShields	= false;
BOOLEAN CTorpedoInfo::m_bReduceManeuver		= false;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTorpedoInfo::CTorpedoInfo(void)
{
}

CTorpedoInfo::~CTorpedoInfo(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
void CTorpedoInfo::SetValues(BYTE type)
{
	// Wenn wieder der selbe Torpedotyp abgefragt wird, dann haben sich die Werte nicht verändert und die
	// Funktion braucht nix zu machen.
	if (type == m_byType)
		return;

	m_byType = type;
	m_strTorpedoName = "";
	m_iTorpedoPower = 0;
	// Eigenschaften der Torpedos zuerst alle auf FALSE, wird bei der Auswahl eines Torepdos dann eventuell auf
	// TRUE gestellt
	m_bMicro			= FALSE;
	m_bPenetrating		= FALSE;
	m_bDoubleShieldDmg	= FALSE;
	m_bDoubleHullDmg	= FALSE;
	m_bIgnoreAllShields	= FALSE;
	m_bCollapseShields	= FALSE;
	m_bReduceManeuver	= FALSE;
	
	// !!!!!!!!!!!!!
	// Bei Änderungen in der Reihenfolge der Torpedos auch die Funktion in der CWeaponObserver-Klasse anpassen!
	// !!!!!!!!!!!!!

	switch (type)
	{
// a.) Nuclear Torpedos
	case 0: m_strTorpedoName	= "Fusionstorpedo";		m_iTorpedoPower = 44;  break;
	case 1: m_strTorpedoName	= "Nucleartorpedo";		m_iTorpedoPower = 75;  break;
// b.) Photon Torpedos
	case 2: m_strTorpedoName	= "Prototorpedo";		m_iTorpedoPower = 150; break;
	case 3: m_strTorpedoName	= "Photontorpedo";		m_iTorpedoPower = 250; break;
	case 4: m_strTorpedoName	= "Microphotontorpedo"; m_iTorpedoPower = 5;   m_bMicro = TRUE;	m_bPenetrating = TRUE;		break;
// c.) Plasma Torpedos
	case 5: m_strTorpedoName	= "Plasmatorpedo";		m_iTorpedoPower = 300; break;
	case 6: m_strTorpedoName	= "Plasmatorpedo I";	m_iTorpedoPower = 400; break;
	case 7: m_strTorpedoName	= "Plasmatorpedo II";	m_iTorpedoPower = 600; break;
	case 8: m_strTorpedoName	= "Plasmatorpedo III";	m_iTorpedoPower = 800; break;
	case 9: m_strTorpedoName	= "Microplasmatorpedo";	m_iTorpedoPower = 6;   m_bMicro = TRUE; m_bPenetrating = TRUE;		break;
// d.) Ion Torpedos
	case 10: m_strTorpedoName	= "Iontorpedo";			m_iTorpedoPower = 225; break;
	case 11: m_strTorpedoName	= "Iontorpedo II";		m_iTorpedoPower = 450; break;
// e.) Quantum Torpedos
	case 12: m_strTorpedoName	= "Quantumtorpedo";		m_iTorpedoPower = 575; break;
//	case 13: m_strTorpedoName	= "Merculittorpedo";	m_iTorpedoPower = 646; break;
	case 13: m_strTorpedoName	= "Myonictorpedo";		m_iTorpedoPower = 646; break;
// f.) Mytronic Torpedos
	case 14: m_strTorpedoName	= "Mytronictorpedo I";	m_iTorpedoPower = 200; break;
	case 15: m_strTorpedoName	= "Mytronictorpedo II";	m_iTorpedoPower = 500; break;
	case 16: m_strTorpedoName	= "Micromytrontic";		m_iTorpedoPower = 4;   m_bMicro = TRUE; m_bPenetrating = TRUE;		break;
// g.) Polaron Torpedos
	case 17: m_strTorpedoName	= "Polarontorpedo";		m_iTorpedoPower = 288; m_bPenetrating = TRUE;		break;
// h.)Gravimetric Torpedos
	case 18: m_strTorpedoName	= "Gravimetrictorpedo";	m_iTorpedoPower = 600; m_bPenetrating = TRUE;		break;
// i.) Trilithium Torpedos
	case 19: m_strTorpedoName	= "Trilithiumtorpedo";	m_iTorpedoPower = 1000;m_bDoubleShieldDmg = TRUE;	break;
// j.) Tricobalt Torpedos
	case 20: m_strTorpedoName	= "Tricobalttorpedo";	m_iTorpedoPower = 900; m_bPenetrating = TRUE;		break;
// k.) Ultritium Torpedos
	case 21: m_strTorpedoName	= "Ultritiumtorpedo";	m_iTorpedoPower = 800; m_bCollapseShields = TRUE;	break;
// l.) Positron Torpedos
	case 22: m_strTorpedoName	= "Positrontorpedo";	m_iTorpedoPower = 400; m_bReduceManeuver = TRUE;	break;
// m.) Chroniton Torpedos
	case 23: m_strTorpedoName	= "Chronitontorpedo";	m_iTorpedoPower = 300; m_bPenetrating = TRUE; m_bDoubleHullDmg = TRUE;	break;
// n.) Transphasic Torpedos
//	case 24: m_strTorpedoName	= "Transphasictorpedo";	m_iTorpedoPower = 500; m_bIgnoreAllShields = TRUE;	break;
	case 24: m_strTorpedoName	= "Transquantumtorpedo";m_iTorpedoPower = 500; m_bIgnoreAllShields = TRUE;	break;
// zusätzlicher d.) Ion Torpedo
	case 25: m_strTorpedoName	= "Microiontorpedo";	m_iTorpedoPower = 8;   m_bMicro = TRUE;	 m_bPenetrating = TRUE;		break;
// o.) Energy Dampening Weapons
	case 26: m_strTorpedoName	= "Energy Dissipator 1";m_iTorpedoPower = 50;  m_bReduceManeuver = TRUE;	break;
	case 27: m_strTorpedoName	= "Energy Dissipator 2";m_iTorpedoPower = 100; m_bReduceManeuver = TRUE; m_bPenetrating = TRUE;	break;
	case 28: m_strTorpedoName	= "Energy Dissipator 3";m_iTorpedoPower = 150; m_bReduceManeuver = TRUE; m_bIgnoreAllShields = TRUE; m_bCollapseShields = TRUE;	break;
	default: AfxMessageBox("Error with torpedotype!\n\nPlease make a bugreport at www.birth-of-the-empires.de.vu");
	}

	/*	NAME

		a.) Nuclear Torpedos 
		-Human Fusion Torpedo 
		acc: 
		bdmg: 44 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 0 
		-Romulan Nuclear Torpedo 
		acc: 
		bdmg: 75 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 

		b.) Photon Torpedos 
		-Vulcan Photonic Torpedo 
		acc: 
		bdmg: 150 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Federation Photon Torpedo 
		acc: 
		bdmg: 250 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Federation Micro Photon Torpedo 
		acc: 
		bdmg: 5 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		c.) Plasma Torpedos 
		-Cardassian Plasma Torpedo 
		acc: 
		bdmg: 300 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Romulan Plasma Torpedo I 
		acc: 
		bdmg: 400 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Romulan Plasma Torpedo II 
		acc: 
		bdmg: 600 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Romulan Plasma Torpedo III 
		acc: 
		bdmg: 800 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Romulan Micro Plasma Torpedo 
		acc: 
		bdmg: 6 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		d.) Ion Torpedos 
		-Klingon Ion Torpedo I 
		acc: 
		bdmg: 225 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Klingon Ion Torpedo II 
		acc: 
		bdmg: 450 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Klingon Micro Ion Torpedo 
		acc: 
		bdmg: 8 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		e.) Quantum Torpedos 
		-Federation Quantum Torpedo 
		acc: 
		bdmg: 575 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Cardassian Merculit Torpedo 
		acc: 
		bdmg: 646 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 

		f.) Mytronic Torpedos 
		-Ferengi Mytronic Torpedo I 
		acc: 
		bdmg: 220 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Ferengi Mytronic Torpedo II 
		acc: 
		bdmg: 500 
		min_range: 3 
		max_range: 200 
		effects: 
		techlevel: 
		-Ferengi Micro Mytronic Torpedo 
		acc: 
		bdmg: 4 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		g.) Polaron Torpedos 
		-Dominion Polaron Torpedo 
		acc: 
		bdmg: 288 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		h.)Gravimetric Torpedos 
		-Borg Gravimetric Torpedo 
		acc: 
		bdmg: 600 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		i.) Trilithium Torpedos 
		- Trilithium Torpedo 
		acc: 
		bdmg: 1000 
		min_range: 3 
		max_range: 200 
		effects: Double damage to shields 
		techlevel: 

		j.) Tricobalt Torpedos 
		- Tricobalt Torpedo 
		acc: 
		bdmg: 900 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular shields 
		techlevel: 

		k.) Ultritium Torpedos 
		- Ultritium Torpedo 
		acc: 
		bdmg: 800 
		min_range: 3 
		max_range: 200 
		effects: Can collapse shields 
		techlevel: 

		l.) Positron Torpedos 
		- Positron Torpedo 
		acc: 
		bdmg: 400 
		min_range: 3 
		max_range: 200 
		effects: Can set maneuverabilty to 0 
		techlevel: 

		m.) Chroniton Torpedos 
		- Chroniton Torpedo 
		acc: 
		bdmg: 300 
		min_range: 3 
		max_range: 200 
		effects: Ignores regular Shields, double damage to Hull 
		techlevel: 

		n.) Transphasic Torpedos 
		- Transphasic Torpedo 
		acc: 
		bdmg: 500 
		min_range: 3 
		max_range: 200 
		effects: Ignores all Shields 
		techlevel: 

		o.) Energy Dampening Weapons 
		- Breen Energy Dampener Level 1 
		acc: 
		bdmg: 50 
		min_range: 3 
		max_range: 200 
		effects: ReduceManeuver 
		techlevel: 
		- Breen Energy Dampener Level 2 
		acc: 
		bdmg: 100 
		min_range: 3 
		max_range: 200 
		effects: Penetrating, ReduceManeuver 
		techlevel: 
		- Breen Energy Dampener Level 3 
		acc: 
		bdmg: 150 
		min_range: 3 
		max_range: 200 
		effects: IgnoreAllShields, ReduceManeuver, CollapseShields 
		techlevel:



		EFFECTS: 
			Penetrating .............. schilddurchschlagender Torpedo 
			DoubleShieldDmg ..... Torpedo macht doppelten Schaden an den Schilden 
			DoubleHullDmg ......... Torpedo macht doppelten Schaden an der Hülle 
			IgnoreAllShields ........ Torpedo ignoriert alle Arten von Schilden 
			CollapseShields ......... Torpedotreffer kann Schilde kollabieren lassen 
			ReduceManeuver ....... Bei einem Treffer besteht die Chance, die Manövrierbarkeit des Zieles auf NULL zu verringern.

	*/
}

