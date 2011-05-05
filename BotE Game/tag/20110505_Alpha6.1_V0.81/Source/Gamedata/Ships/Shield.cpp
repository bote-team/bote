// Shield.cpp: Implementierung der Klasse CShield.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shield.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShield, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShield::CShield()
{
	m_iShieldType = 0;
	m_iMaxShield = 0;
	m_iCurrentShield = 0;
	m_bRegenerative = FALSE;
}

CShield::~CShield()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShield::CShield(const CShield & rhs)
{
	m_iShieldType = rhs.m_iShieldType;
	m_iCurrentShield = rhs.m_iCurrentShield;
	m_iMaxShield = rhs.m_iMaxShield;
	m_bRegenerative = rhs.m_bRegenerative;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShield & CShield::operator=(const CShield & rhs)
{
	if (this == &rhs)
		return *this;
	m_iShieldType = rhs.m_iShieldType;
	m_iCurrentShield = rhs.m_iCurrentShield;
	m_iMaxShield = rhs.m_iMaxShield;
	m_bRegenerative = rhs.m_bRegenerative;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShield::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iShieldType;
		ar << m_iMaxShield;
		ar << m_iCurrentShield;
		ar << m_bRegenerative;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iShieldType;
		ar >> m_iMaxShield;
		ar >> m_iCurrentShield;
		ar >> m_bRegenerative;
	}
}

void CShield::SetCurrentShield(int newCurrentShield)
{
	if (newCurrentShield < 0)
		m_iCurrentShield = 0;
	else if ((UINT)newCurrentShield > m_iCurrentShield)
		m_iCurrentShield = 0;
	else
		m_iCurrentShield = newCurrentShield;
}

void CShield::ModifyShield(UINT MaxShield, BYTE ShieldType, BOOLEAN Regenerative)
{
	m_iShieldType = ShieldType;
	m_iCurrentShield = MaxShield;
	m_iMaxShield = MaxShield;
	m_bRegenerative = Regenerative;
}

// Diese Funktion lädt die Schilde entsprechend ihrem Schildtyp wieder auf
void CShield::RechargeShields(int nMulti)
{
	// Die Schilde werden um multi * Schildtyp / 300 + 2*Schildtyp aufgeladen.
	int maxRecharge = m_iMaxShield - m_iCurrentShield;
	//int Recharge = multi * m_iMaxShield / 300 + 2 * m_iShieldType;
	int Recharge = nMulti * (int)(max(m_iMaxShield * 0.001f, 1) * m_iShieldType);
	if (Recharge > maxRecharge)
		Recharge = maxRecharge;
	m_iCurrentShield += Recharge;

/*	if (maxRecharge > 0)
	{
		CString s;
		s.Format("Recharge %d",Recharge);
		AfxMessageBox(s);
	}*/
}
