// ShipInfo.cpp: Implementierung der Klasse CShipInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShipInfo.h"

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
/*
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShipInfo::CShipInfo(const CShipInfo & rhs)
{
	m_Hull = rhs.m_Hull;
	m_Shield = rhs.m_Shield;
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		AfxMessageBox("HALLO");
		m_TorpedoWeapons.Add(rhs.m_TorpedoWeapons.GetAt(i));
	}
	for (i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
	
	m_KO = rhs.m_KO;
	for (i=0;i<4;i++)
		m_TargetKO[i] = rhs.m_TargetKO[i];
	m_sOwnerOfShip = rhs.m_sOwnerOfShip;
	m_iBuildCosts = rhs.m_iBuildCosts;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthPower = rhs.m_iStealthPower;
	m_iStorageRoom = rhs.m_iStorageRoom;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_strShipName = rhs.m_strShipName;
	m_strShipDescription = rhs.m_strShipDescription;
	m_strShipClass = rhs.m_strShipClass;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShipInfo & CShipInfo::operator=(const CShipInfo & rhs)
{
	if (this == &rhs)
		return *this;
	m_Hull = rhs.m_Hull;
	m_Shield = rhs.m_Shield;
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
		m_TorpedoWeapons.Add(rhs.m_TorpedoWeapons.GetAt(i));
	for (i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
	
	m_KO = rhs.m_KO;
	for (i=0;i<4;i++)
		m_TargetKO[i] = rhs.m_TargetKO[i];
	m_sOwnerOfShip = rhs.m_sOwnerOfShip;
	m_iBuildCosts = rhs.m_iBuildCosts;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthPower = rhs.m_iStealthPower;
	m_iStorageRoom = rhs.m_iStorageRoom;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_strShipName = rhs.m_strShipName;
	m_strShipDescription = rhs.m_strShipDescription;
	m_strShipClass = rhs.m_strShipClass;
	return *this;
}
*/

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShipInfo::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	CShip::Serialize(ar);
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
		ar << m_iNeededDilithium;
		ar << m_iBaseIndustry;
		ar << m_iBaseTitan;
		ar << m_iBaseDeuterium;
		ar << m_iBaseDuranium;
		ar << m_iBaseCrystal;
		ar << m_iBaseIridium;
		ar << m_iBaseDilithium;
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
		ar >> m_iNeededDilithium;
		ar >> m_iBaseIndustry;
		ar >> m_iBaseTitan;
		ar >> m_iBaseDeuterium;
		ar >> m_iBaseDuranium;
		ar >> m_iBaseCrystal;
		ar >> m_iBaseIridium;
		ar >> m_iBaseDilithium;
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
	m_iNeededDilithium	= 0;
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
	m_iNeededDilithium	+= m_iBaseDilithium;
	
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
	if (m_iShipType >= SCOUT)
		m_iCurrentOrder = ATTACK;
	else
		m_iCurrentOrder = AVOID;
}


// Funktion zeichnet wichtige Informationen zu dem Schiff
void CShipInfo::DrawShipInformation(Graphics* g, CRect rect, Gdiplus::Font* font, Gdiplus::Color clrNormal, Gdiplus::Color clrMark, CResearch* research)
{
	CRect r;
	CString s;	
	r.SetRect(rect.left,rect.top,rect.right,rect.top+25);
	s.Format("%s",this->GetShipTypeAsString());
	
	SolidBrush fontBrush(clrMark);
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	
	r.SetRect(rect.left,rect.top+30,rect.right,rect.top+52);
	CString speed = CResourceManager::GetString("SPEED");
	CString range = CResourceManager::GetString("RANGE");
	BYTE tmpRange = m_iRange;
	BYTE tmpSpeed = m_iSpeed;
	if (research->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(1) == RESEARCHED)
	{
		if (m_iRange == RANGE_SHORT)
			tmpRange = (BYTE)(research->GetResearchInfo()->GetResearchComplex(2)->GetBonus(1));
	}
	else if (research->GetResearchInfo()->GetResearchComplex(2)->GetFieldStatus(2) == RESEARCHED)
	{
		if (m_iSpeed == 1)
			tmpSpeed = (BYTE)(research->GetResearchInfo()->GetResearchComplex(2)->GetBonus(2));
	}
	switch (tmpRange)
	{
	case RANGE_SHORT: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CResourceManager::GetString("SHORT"));break;
	case RANGE_MIDDLE: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CResourceManager::GetString("MIDDLE")); break;
	case RANGE_LONG: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CResourceManager::GetString("LONG")); break;
	}
	fontBrush.SetColor(clrNormal);
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	fontBrush.SetColor(clrMark);
	r.SetRect(rect.left,rect.top+52,rect.right,rect.top+74);
	s = CResourceManager::GetString("ARMAMENT");
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	
	CString weapons = "";
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
	{
		s.Format("%d x %s %d %s",m_BeamWeapons.GetAt(i).GetBeamNumber(), CResourceManager::GetString("TYPE"),
			m_BeamWeapons.GetAt(i).GetBeamType(),m_BeamWeapons.GetAt(i).GetBeamName());
		weapons.Insert(weapons.GetLength(),s);
		if ((i+1) < m_BeamWeapons.GetSize())
			weapons += ", ";
	}
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		if (i == 0 && weapons.IsEmpty() != TRUE)
			weapons += ", ";
		s.Format("%d x %s (%s)",m_TorpedoWeapons.GetAt(i).GetNumberOfTupes(),m_TorpedoWeapons.GetAt(i).GetTupeName(),
			m_TorpedoWeapons.GetAt(i).GetTorpedoName());
		weapons.Insert(weapons.GetLength(),s);
		if ((i+1) < m_TorpedoWeapons.GetSize())
			weapons += ", ";
	}
	if (weapons == "")
		weapons = CResourceManager::GetString("NONE");
	r.SetRect(rect.left,rect.top+74,rect.right,rect.top+142);
	fontBrush.SetColor(clrNormal);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(weapons.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
			
	// Wenn das Rechteck, welches wir an diese Funktion übergeben haben breiter als 500 ist, dann wird der Abstand
	// zwischen Bewaffnung und "Schilde und Hülle" um eine Zeile verkürzt
	USHORT sub = 0;
	if (rect.Width() > 500)
		sub = 20;
	r.SetRect(rect.left,rect.top+142-sub,rect.right,rect.top+164-sub);
	s = CResourceManager::GetString("SHIELDS")+" "+CResourceManager::GetString("AND")+" "+CResourceManager::GetString("HULL");
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(clrMark);
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	
	r.SetRect(rect.left,rect.top+164-sub,rect.right,rect.top+186-sub);
	float shieldBoni = 1.0f;
	if (research->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(1) == RESEARCHED)
		shieldBoni += (float)(research->GetResearchInfo()->GetResearchComplex(1)->GetBonus(1)) / 100;

	s.Format("%s %d %s: %s %d",CResourceManager::GetString("TYPE"),m_Shield.GetShieldType(),
		CResourceManager::GetString("SHIELDS"), CResourceManager::GetString("CAPACITY"), (UINT)(m_Shield.GetMaxShield() * shieldBoni));
	fontBrush.SetColor(clrNormal);
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	r.SetRect(rect.left,rect.top+186-sub,rect.right,rect.top+230-sub);
	CString material;
	switch (m_Hull.GetHullMaterial())
	{
	case TITAN: material = CResourceManager::GetString("TITAN");; break;
	case DURANIUM: material = CResourceManager::GetString("DURANIUM");; break;
	case IRIDIUM: material = CResourceManager::GetString("IRIDIUM");; break;
	default: material = "";
	}
	float hullBoni = 1.0f;
	if (research->GetResearchInfo()->GetResearchComplex(1)->GetFieldStatus(2) == RESEARCHED)
		hullBoni += (float)(research->GetResearchInfo()->GetResearchComplex(1)->GetBonus(2)) / 100;
	
	if (m_Hull.GetDoubleHull() == TRUE)
		s.Format("%s%s: %s %d",material, CResourceManager::GetString("DOUBLE_HULL_ARMOUR"),
			CResourceManager::GetString("INTEGRITY"), (int)(m_Hull.GetMaxHull()*hullBoni));
	else
		s.Format("%s%s: %s %d",material, CResourceManager::GetString("HULL_ARMOR"),
			CResourceManager::GetString("INTEGRITY"), (int)(m_Hull.GetMaxHull()*hullBoni));
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		
	if (rect.Width() >= 501)
	{
		// Manövrierbarkeit anzeigen
		fontBrush.SetColor(clrMark);
		r.SetRect(rect.left,rect.top+275,rect.right,rect.top+300);
		g->DrawString(CResourceManager::GetString("MANEUVERABILITY").AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		switch (m_byManeuverability)
		{
		case 8:	s = CResourceManager::GetString("EXCELLENT");	break;
		case 7:	s = CResourceManager::GetString("VERYGOOD");	break;
		case 6:	s = CResourceManager::GetString("GOOD");		break;
		case 5:	s = CResourceManager::GetString("NORMAL");		break;
		case 4:	s = CResourceManager::GetString("ADEQUATE");	break;
		case 3:	s = CResourceManager::GetString("BAD");			break;
		case 2:	s = CResourceManager::GetString("VERYBAD");		break;
		case 1:	s = CResourceManager::GetString("MISERABLE");	break;
		default:s = CResourceManager::GetString("NONE");
		}
		fontBrush.SetColor(clrNormal);
		r.SetRect(rect.left,rect.top+300,rect.right,rect.top+325);
		g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

		// Spezialfähigkeiten anzeigen
		fontBrush.SetColor(clrMark);
		r.SetRect(rect.left,rect.top+325,rect.right,rect.top+350);
		g->DrawString(CResourceManager::GetString("SPECIAL_ABILITIES").AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		s = "";
		fontBrush.SetColor(clrNormal);
		if (m_bySpecial[0] == ASSULTSHIP || m_bySpecial[1] == ASSULTSHIP)
			s += CResourceManager::GetString("ASSAULTSHIP")+" \n";
		if (m_bySpecial[0] == BLOCKADESHIP || m_bySpecial[1] == BLOCKADESHIP)
			s += CResourceManager::GetString("BLOCKADESHIP")+" \n";
		if (m_bySpecial[0] == COMMANDSHIP || m_bySpecial[1] == COMMANDSHIP)
			s += CResourceManager::GetString("COMMANDSHIP")+" \n";
		if (m_bySpecial[0] == DOGFIGHTER || m_bySpecial[1] == DOGFIGHTER)
			s += CResourceManager::GetString("DOGFIGHTER")+" \n";
		if (m_bySpecial[0] == DOGKILLER || m_bySpecial[1] == DOGKILLER)
			s += CResourceManager::GetString("DOGKILLER")+" \n";
		if (m_bySpecial[0] == PATROLSHIP || m_bySpecial[1] == PATROLSHIP)
			s += CResourceManager::GetString("PATROLSHIP")+" \n";
		if (m_bySpecial[0] == RAIDER || m_bySpecial[1] == RAIDER)
			s += CResourceManager::GetString("RAIDER")+" \n";
		if (m_bySpecial[0] == SCIENCEVESSEL || m_bySpecial[1] == SCIENCEVESSEL)
			s += CResourceManager::GetString("SCIENCESHIP")+" \n";
		if (m_Shield.GetRegenerative())
			s += CResourceManager::GetString("REGENERATIVE_SHIELDS")+" \n";
		if (m_Hull.GetAblative())
			s += CResourceManager::GetString("ABLATIVE_ARMOR")+" \n";
		if (m_Hull.GetPolarisation())
			s += CResourceManager::GetString("HULLPOLARISATION")+" \n";
		if (m_iStealthPower > 3)
			s += CResourceManager::GetString("CAN_CLOAK")+" \n";
		if (s.IsEmpty())
			s = CResourceManager::GetString("NONE");
		r.SetRect(rect.left,rect.top+350,rect.right,rect.top+450);
		fontBrush.SetColor(clrNormal);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		g->DrawString(s.AllocSysString(), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);		
	}
}

/// Funktion gibt zurück, ob das Schiff mit der aktuellen Forschung einer Rasse baubar ist.
/// @param researchLevels Forschungsstufen der Rasse
/// @return Wahrheitswert
bool CShipInfo::IsThisShipBuildableNow(const BYTE reserachLevels[6]) const
{
	// zuerstmal die Forschungsstufen checken
	if (reserachLevels[5] < this->GetWeaponTech())
		return false;
	if (reserachLevels[4] < this->GetConstructionTech())
		return false;
	if (reserachLevels[3] < this->GetPropulsionTech())
		return false;	
	if (reserachLevels[2] < this->GetComputerTech())
		return false;
	if (reserachLevels[1] < this->GetEnergyTech())
		return false;
	if (reserachLevels[0] < this->GetBioTech())
		return false;
	
	return true;
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
	case DILITHIUM:	return this->GetNeededDilithium();
	default:		return NULL;
	}
}
