// ShipInfo.cpp: Implementierung der Klasse CShipInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShipInfo.h"
#include "Math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShipInfo, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShipInfo::CShipInfo()
{
}

CShipInfo::~CShipInfo()
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShipInfo::CShipInfo(const CShipInfo & rhs) : CShip(rhs)
{	
	m_iRace = rhs.m_iRace;					
	// nötige Forschung
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;	
	// aktuell nötige Rohstoffe zum Bau des Schiffes
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDeritium = rhs.m_iNeededDeritium;
	// nötige Rohstoffe zum Bau, so wie die Grundkosten sind, denn durch CalculateFinalCosts werden die nötigen Rohstoffe
	// und Industrieleistung erhöht. Beim nächsten Aufruf brauchen wir aber wieder die ursprünglichen Kosten
	m_iBaseIndustry = rhs.m_iBaseIndustry;
	m_iBaseTitan = rhs.m_iBaseTitan;
	m_iBaseDeuterium = rhs.m_iBaseDeuterium;
	m_iBaseDuranium = rhs.m_iBaseDuranium;
	m_iBaseCrystal = rhs.m_iBaseCrystal;
	m_iBaseIridium = rhs.m_iBaseIridium;
	m_iBaseDeritium = rhs.m_iBaseDeritium;
	// nötiger Systemname
	m_strOnlyInSystem = rhs.m_strOnlyInSystem;
	// Schiffsklasse, welche durch diese Schiffsklasse ersetzt wird
	m_strObsoletesClass = rhs.m_strObsoletesClass;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShipInfo & CShipInfo::operator=(const CShipInfo & rhs)
{
	if (this == &rhs)
		return *this;
	
	// Basisklasse
	__super::operator=(rhs);

	m_iRace = rhs.m_iRace;					
	// nötige Forschung
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;	
	// aktuell nötige Rohstoffe zum Bau des Schiffes
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDeritium = rhs.m_iNeededDeritium;
	// nötige Rohstoffe zum Bau, so wie die Grundkosten sind, denn durch CalculateFinalCosts werden die nötigen Rohstoffe
	// und Industrieleistung erhöht. Beim nächsten Aufruf brauchen wir aber wieder die ursprünglichen Kosten
	m_iBaseIndustry = rhs.m_iBaseIndustry;
	m_iBaseTitan = rhs.m_iBaseTitan;
	m_iBaseDeuterium = rhs.m_iBaseDeuterium;
	m_iBaseDuranium = rhs.m_iBaseDuranium;
	m_iBaseCrystal = rhs.m_iBaseCrystal;
	m_iBaseIridium = rhs.m_iBaseIridium;
	m_iBaseDeritium = rhs.m_iBaseDeritium;
	// nötiger Systemname
	m_strOnlyInSystem = rhs.m_strOnlyInSystem;
	// Schiffsklasse, welche durch diese Schiffsklasse ersetzt wird
	m_strObsoletesClass = rhs.m_strObsoletesClass;

	return *this;
}


///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShipInfo::Serialize(CArchive &ar)		
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iRace;
		ar << m_iBioTech;
		ar << m_iEnergyTech;
		ar << m_iCompTech;
		ar << m_iPropulsionTech;
		ar << m_iConstructionTech;
		ar << m_iWeaponTech;	
		ar << m_iNeededIndustry;
		ar << m_iNeededTitan;
		ar << m_iNeededDeuterium;
		ar << m_iNeededDuranium;
		ar << m_iNeededCrystal;
		ar << m_iNeededIridium;
		ar << m_iNeededDeritium;
		ar << m_iBaseIndustry;
		ar << m_iBaseTitan;
		ar << m_iBaseDeuterium;
		ar << m_iBaseDuranium;
		ar << m_iBaseCrystal;
		ar << m_iBaseIridium;
		ar << m_iBaseDeritium;
		ar << m_strOnlyInSystem;
		ar << m_strObsoletesClass;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iRace;
		ar >> m_iBioTech;
		ar >> m_iEnergyTech;
		ar >> m_iCompTech;
		ar >> m_iPropulsionTech;
		ar >> m_iConstructionTech;
		ar >> m_iWeaponTech;	
		ar >> m_iNeededIndustry;
		ar >> m_iNeededTitan;
		ar >> m_iNeededDeuterium;
		ar >> m_iNeededDuranium;
		ar >> m_iNeededCrystal;
		ar >> m_iNeededIridium;
		ar >> m_iNeededDeritium;
		ar >> m_iBaseIndustry;
		ar >> m_iBaseTitan;
		ar >> m_iBaseDeuterium;
		ar >> m_iBaseDuranium;
		ar >> m_iBaseCrystal;
		ar >> m_iBaseIridium;
		ar >> m_iBaseDeritium;
		ar >> m_strOnlyInSystem;
		ar >> m_strObsoletesClass;
	}
}

// Funktion löscht die Bewaffnung
void CShipInfo::DeleteWeapons()
{
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); )
		m_TorpedoWeapons.RemoveAt(i);
	for (int i = 0; i < m_BeamWeapons.GetSize(); )
		m_BeamWeapons.RemoveAt(i);
	m_TorpedoWeapons.RemoveAll();
	m_BeamWeapons.RemoveAll();
}

// Funktion berechnet die finalen Kosten zum Bau des Schiffes. Diese muß immer aufgerufen werden, wenn wir was an
// dem Schiffsobjekt geändert haben.
void CShipInfo::CalculateFinalCosts()
{
	m_iNeededIndustry	= 0;
	m_iNeededTitan		= 0;
	m_iNeededDeuterium	= 0;
	m_iNeededDuranium	= 0;
	m_iNeededCrystal	= 0;
	m_iNeededIridium	= 0;
	m_iNeededDeritium	= 0;
	// Industrieleistung, die wir zusätzlich durch bessere Schilde erbringen müssen
/*	m_iNeededIndustry += m_Shield.GetMaxShield() / 10 * m_Shield.GetShieldType();
	// Industrieleistung, die wir zusätzlich durch bessere Hülle  erbringen müssen
	// besseres Material erhöht die Industrieleistung nicht, sondern nur ob wir eine Doppelhülle haben
	m_iNeededIndustry += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()));
	// Industrieleistung die wir zusätzlich erbringen müssen, wenn wir besserer Beams dranbauen
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_iNeededIndustry += (USHORT)(10 * (m_BeamWeapons.GetAt(i).GetBeamPower() * m_BeamWeapons.GetAt(i).GetBeamType()));
	// Industrieleistung die wir zusätzlich erbringen müssen, wenn wir besserer Torpedos dranbauen
//	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
//		m_iNeededIndustry += (USHORT)((m_TorpedoWeapons.GetAt(i).GetTorpedoPower() * m_TorpedoWeapons.GetAt(i).GetNumber()) 
//							 / m_TorpedoWeapons.GetAt(i).GetTupeFirerate() * m_TorpedoWeapons.GetAt(i).GetNumberOfTupes() * 50);
*/	
	// Ab hier neue Kostenberechnung. Es geht mehr der Wert aller 100s ein. Also nicht mehr der absolute Stärkewert.
	// Es wird nur ein gewisser Anteil durch den Beamtype mit aufgerechnet, da dieser sonst keinen Einfluss auf den
	// Beamschaden hat, nur auf die Reichweite der Beams.
	USHORT beamTypeAdd = 0;
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		beamTypeAdd += (USHORT)(GetBeamWeapons()->GetAt(i).GetBeamPower()
								* GetBeamWeapons()->GetAt(i).GetBeamNumber()
								* m_BeamWeapons.GetAt(i).GetBeamType()
								* 3);
	if (m_BeamWeapons.GetSize() > 1)
		beamTypeAdd		/= m_BeamWeapons.GetSize();
	m_iNeededIndustry	+= beamTypeAdd;
	m_iNeededIndustry	+= this->GetCompleteOffensivePower();
	// Eine Doppelhülle verteuert die Industriekosten nochmals um 25%
	if (m_Hull.GetDoubleHull())
		m_iNeededIndustry	+= (UINT)(this->GetCompleteDefensivePower() / 1.5);
	else
		m_iNeededIndustry	+= this->GetCompleteDefensivePower() / 2;
	
	m_iNeededIndustry	+= GetShield()->GetMaxShield() / 200 * ((USHORT)pow((float)GetShield()->GetShieldType(), 2.5f));
	m_iNeededIndustry	/= 2;

	// Kosten erstmal wieder auf die Ausgangswerte setzen
	m_iNeededIndustry	+= m_iBaseIndustry;
	m_iNeededTitan		+= m_iBaseTitan;
	m_iNeededDeuterium	+= m_iBaseDeuterium;
	m_iNeededDuranium	+= m_iBaseDuranium;
	m_iNeededCrystal	+= m_iBaseCrystal;
	m_iNeededIridium	+= m_iBaseIridium;
	m_iNeededDeritium	+= m_iBaseDeritium;
	
	// zusätzliche Rohstoffe die wir für bessere Anbauten/Umbauten benötigen
	// wir brauchen für bessere Hüllen auch besseres Material
	switch (m_Hull.GetHullMaterial())
	{
	case TITAN: m_iNeededTitan += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	case DURANIUM: m_iNeededDuranium += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	case IRIDIUM: m_iNeededIridium += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	}
	// für bessere Torpedos (ab 500 Dmg und mehr) brauchen wir zusätzliches Deuterium
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		if (m_TorpedoWeapons.GetAt(i).GetTorpedoPower() >= 500)
			m_iNeededDeuterium += m_TorpedoWeapons.GetAt(i).GetTorpedoPower() * m_TorpedoWeapons.GetAt(i).GetNumber()
			* m_TorpedoWeapons.GetAt(i).GetNumberOfTupes();	
	}
}

// Funktion bestimmt die 1. Order des Schiffs nach dem Bau anhand dessen Typs
void CShipInfo::SetStartOrder()
{
	if (m_iShipType >= SCOUT)
		m_iCurrentOrder = ATTACK;
	else
		m_iCurrentOrder = AVOID;
}

USHORT CShipInfo::GetNeededResource(BYTE res) const
{
	switch (res)
	{
	case TITAN:		return this->GetNeededTitan();
	case DEUTERIUM: return this->GetNeededDeuterium();
	case DURANIUM:	return this->GetNeededDuranium();
	case CRYSTAL:	return this->GetNeededCrystal();
	case IRIDIUM:	return this->GetNeededIridium();
	case DERITIUM:	return this->GetNeededDeritium();
	default:		return NULL;
	}
}