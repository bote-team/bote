#include "stdafx.h"
#include "TorpedoInfo.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTorpedoInfo::CTorpedoInfo(void)
{
	/*
	EFFECTS: 
			Micro..................Microtorpedo (nur für Microwerfer)
			Penetrating............schilddurchschlagender Torpedo 
			DoubleShieldDmg........Torpedo macht doppelten Schaden an den Schilden 
			DoubleHullDmg..........Torpedo macht doppelten Schaden an der Hülle 
			IgnoreAllShields.......Torpedo ignoriert alle Arten von Schilden 
			CollapseShields........Torpedotreffer kann Schilde kollabieren lassen 
			ReduceManeuver........ Bei einem Treffer besteht die Chance, die Manövrierbarkeit des Zieles auf NULL zu verringern.
	*/

// a.) Nuclear Torpedos
	m_vInfos.push_back(TORPEDOINFO("Fusionstorpedo", 44));
	m_vInfos.push_back(TORPEDOINFO("Nucleartorpedo", 75));
// b.) Photon Torpedos
	m_vInfos.push_back(TORPEDOINFO("Prototorpedo", 150));
	m_vInfos.push_back(TORPEDOINFO("Photontorpedo", 250));
	m_vInfos.push_back(TORPEDOINFO("Microphotontorpedo", 5, MICROTORPEDO | PENETRATING));	
// c.) Plasma Torpedos
	m_vInfos.push_back(TORPEDOINFO("Plasmatorpedo", 300));
	m_vInfos.push_back(TORPEDOINFO("Plasmatorpedo I", 400));
	m_vInfos.push_back(TORPEDOINFO("Plasmatorpedo II", 600));
	m_vInfos.push_back(TORPEDOINFO("Plasmatorpedo III", 800));
	m_vInfos.push_back(TORPEDOINFO("Microplasmatorpedo", 6, MICROTORPEDO | PENETRATING));
// d.) Ion Torpedos
	m_vInfos.push_back(TORPEDOINFO("Iontorpedo", 225));
	m_vInfos.push_back(TORPEDOINFO("Iontorpedo II", 450));
// e.) Quantum Torpedos
	m_vInfos.push_back(TORPEDOINFO("Quantumtorpedo", 575));
	m_vInfos.push_back(TORPEDOINFO("Myonictorpedo", 646));
// f.) Mytronic Torpedos
	m_vInfos.push_back(TORPEDOINFO("Mytronictorpedo I", 200));
	m_vInfos.push_back(TORPEDOINFO("Mytronictorpedo II", 500));
	m_vInfos.push_back(TORPEDOINFO("Micromytrontic", 4, MICROTORPEDO | PENETRATING));
// g.) Polaron Torpedos
	m_vInfos.push_back(TORPEDOINFO("Polarontorpedo", 288, PENETRATING));	
// h.)Gravimetric Torpedos
	m_vInfos.push_back(TORPEDOINFO("Gravimetrictorpedo", 600, PENETRATING));	
// i.) Trilithium Torpedos
	m_vInfos.push_back(TORPEDOINFO("Trilithiumtorpedo", 1000, DOUBLESHIELDDMG));	
// j.) Tricobalt Torpedos
	m_vInfos.push_back(TORPEDOINFO("Tricobalttorpedo", 900, PENETRATING));	
// k.) Ultritium Torpedos
	m_vInfos.push_back(TORPEDOINFO("Ultritiumtorpedo", 800, COLLAPSESHIELDS));
// l.) Positron Torpedos
	m_vInfos.push_back(TORPEDOINFO("Positrontorpedo", 400, REDUCEMANEUVER));	
// m.) Chroniton Torpedos
	m_vInfos.push_back(TORPEDOINFO("Chronitontorpedo", 300, PENETRATING | DOUBLEHULLDMG));
// n.) Transphasic Torpedos
	m_vInfos.push_back(TORPEDOINFO("Transquantumtorpedo", 500, IGNOREALLSHIELDS));	
// zusätzlicher d.) Ion Torpedo
	m_vInfos.push_back(TORPEDOINFO("Microiontorpedo", 8, MICROTORPEDO | PENETRATING));
// o.) Energy Dampening Weapons
	m_vInfos.push_back(TORPEDOINFO("Energy Dissipator 1", 50, REDUCEMANEUVER));
	m_vInfos.push_back(TORPEDOINFO("Energy Dissipator 2", 100, REDUCEMANEUVER | PENETRATING));
	m_vInfos.push_back(TORPEDOINFO("Energy Dissipator 3", 150, REDUCEMANEUVER | COLLAPSESHIELDS | IGNOREALLSHIELDS));
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
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CTorpedoInfo* CTorpedoInfo::GetInstance(void)
{
	static CTorpedoInfo instance; 
    return &instance;
}