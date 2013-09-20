// Hull.cpp: Implementierung der Klasse CHull.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Constants.h"
#include "Hull.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CHull, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CHull::CHull() :
	m_iBaseHull(0),
	m_iCurrentHull(0),
	m_iHullMaterial(0),
	m_iMaxHull(0)
{
	m_bDoubleHull			= FALSE;
	m_bAblative				= FALSE;
	m_bPolarisation			= FALSE;
}

CHull::~CHull()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CHull::CHull(const CHull & rhs)
{
	m_bDoubleHull = rhs.m_bDoubleHull;
	m_bAblative = rhs.m_bAblative;
	m_bPolarisation = rhs.m_bPolarisation;
	m_iBaseHull = rhs.m_iBaseHull;
	m_iCurrentHull = rhs.m_iCurrentHull;
	m_iHullMaterial = rhs.m_iHullMaterial;
	m_iMaxHull = rhs.m_iMaxHull;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CHull & CHull::operator=(const CHull & rhs)
{
	if (this == &rhs)
		return *this;
	m_bDoubleHull = rhs.m_bDoubleHull;
	m_bAblative = rhs.m_bAblative;
	m_bPolarisation = rhs.m_bPolarisation;
	m_iBaseHull = rhs.m_iBaseHull;
	m_iCurrentHull = rhs.m_iCurrentHull;
	m_iHullMaterial = rhs.m_iHullMaterial;
	m_iMaxHull = rhs.m_iMaxHull;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CHull::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_bDoubleHull;
		ar << m_bAblative;
		ar << m_bPolarisation;
		ar << m_iBaseHull;
		ar << m_iCurrentHull;
		ar << m_iHullMaterial;
		ar << m_iMaxHull;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_bDoubleHull;
		ar >> m_bAblative;
		ar >> m_bPolarisation;
		ar >> m_iBaseHull;
		ar >> m_iCurrentHull;
		ar >> m_iHullMaterial;
		ar >> m_iMaxHull;
	}
}

void CHull::ModifyHull(BOOLEAN DoubleHull, UINT BaseHull, BYTE HullMaterial, BOOLEAN Ablative, BOOLEAN Polarisation)
{
	m_bDoubleHull = DoubleHull;
	m_bAblative = Ablative;
	m_bPolarisation = Polarisation;
	m_iBaseHull = BaseHull;
	m_iHullMaterial = HullMaterial;
	float fMulti = 1.0f;
	switch(HullMaterial)
	{
	case RESOURCES::DURANIUM:	fMulti = 1.5f; break;
	case RESOURCES::IRIDIUM:	fMulti = 2.0f; break;
	}
	m_iMaxHull = (UINT)(m_iBaseHull * fMulti);
	if (m_bDoubleHull == TRUE)
		m_iMaxHull = (UINT)(m_iMaxHull * 1.5f);
	m_iCurrentHull = m_iMaxHull;
}

void CHull::SetCurrentHull(int add, bool bIgnoreHullSpecials/* = false*/)
{
	// wenn die Hülle die Polarisationseigenschaft hat, dann werden 10% des Schadens abgezogen

	if (bIgnoreHullSpecials == false)
	{
		double multi = 0.0f;
		if (GetPolarisation() && add < 0)
			{
			multi += 0.1;
//MYTRACE("general")(MT::LEVEL_DEBUG, "Ship has Polarisation: multi = %d\n", multi);
			}
		if (GetAblative() && add < 0)
			{
			multi += 0.25;
// MYTRACE("general")(MT::LEVEL_DEBUG, "Ship has Ablative: multi = %d\n", multi);
			}
		add -= int(add * multi);
//		MYTRACE("general")(MT::LEVEL_DEBUG, "Ship has multi = %d\n", add);
	}

	// Überprüfen, das die Werte auch im Rahmen bleiben
	if ((long)(add + m_iCurrentHull) >= (long)m_iMaxHull)
		m_iCurrentHull = m_iMaxHull;
	else if ((long)(add + m_iCurrentHull) <= 0)
		m_iCurrentHull = 0;
	else
		m_iCurrentHull += add;

//MYTRACE("general")(MT::LEVEL_DEBUG, "m_iCurrentHull = %d\n", m_iCurrentHull);
}

/// Diese Funktion repariert die Hülle
void CHull::RepairHull()
{
	// Die Hülle wird bei jedem Aufruf dieser Funktion um 10% repariert.
	int maxRepair = m_iMaxHull - m_iCurrentHull;
	int Repair = m_iMaxHull / 10;
	// Minimum werden aber immer 20 repariert
	if (Repair < 20)
		Repair = 20;
	if (Repair > maxRepair)
		Repair = maxRepair;
	m_iCurrentHull += Repair;
MYTRACE("general")(MT::LEVEL_DEBUG, "m_iCurrentHull = %d, Repair = %d\n", m_iCurrentHull, Repair);
}
