/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Shield.h: Schnittstelle f¸r die Klasse CShield.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIELD_H__FF07FA87_5FE3_427C_B743_6D9C3860B290__INCLUDED_)
#define AFX_SHIELD_H__FF07FA87_5FE3_427C_B743_6D9C3860B290__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShield : public CObject
{
	friend class CShip;
public:
	DECLARE_SERIAL (CShield)
	CShield();
	virtual ~CShield();
	CShield(const CShield & rhs);
	CShield & operator=(const CShield &);
// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	BYTE GetShieldType() const {return m_iShieldType;}
	UINT GetMaxShield() const {return m_iMaxShield;}
	UINT GetCurrentShield() const {return m_iCurrentShield;}
	BOOLEAN GetRegenerative() const {return m_bRegenerative;}

	// zum Schreiben der Membervariablen
	void SetCurrentShield(int newCurrentShield);
	void ModifyShield(UINT MaxShield, BYTE ShieldType, BOOLEAN Regenerative);

	/**
	* Diese Funktion l‰dt die Schilde entsprechend ihrem Schildtyp wieder auf. Der Multiplikator gibt an, mit
	* welcher Zahl der Wert der Aufladung multiplitiert werden soll. Standardm‰ﬂig ist hat dieser den Wert EINS.
	*/
	void RechargeShields(int nMulti = 1);

private:
	BYTE m_iShieldType;					// Typ des Schildes
	UINT m_iMaxShield;					// Maximale Schildst‰rke
	UINT m_iCurrentShield;				// Aktuellen Schilde
	BOOLEAN m_bRegenerative;			// sind es regenerative Schilde
};

#endif // !defined(AFX_SHIELD_H__FF07FA87_5FE3_427C_B743_6D9C3860B290__INCLUDED_)
