/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Hull.h: Schnittstelle für die Klasse CHull.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HULL_H__4AA4B76E_E548_45F2_BBF2_1723996202FE__INCLUDED_)
#define AFX_HULL_H__4AA4B76E_E548_45F2_BBF2_1723996202FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
typedef unsigned short USHORT;
typedef unsigned long ULONG;

class CHull : public CObject  
{
	friend class CShip;
public:
	DECLARE_SERIAL (CHull)
	CHull();
	virtual ~CHull();
	CHull(const CHull & rhs);
	CHull & operator=(const CHull &);
// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	BOOLEAN GetDoubleHull() const {return m_bDoubleHull;}
	BOOLEAN GetAblative() const {return m_bAblative;}
	BOOLEAN GetPolarisation() const {return m_bPolarisation;}
	UINT GetBaseHull() const {return m_iBaseHull;}
	UINT GetCurrentHull() const {return m_iCurrentHull;}
	UINT GetMaxHull() const {return m_iMaxHull;}
	BYTE GetHullMaterial() const {return m_iHullMaterial;}
	
	void SetCurrentHull(int add);

	void ModifyHull(BOOLEAN DoubleHull, UINT BaseHull, BYTE HullMaterial, BOOLEAN Ablative, BOOLEAN Polarisation);

	/// Diese Funktion repariert die Hülle. Die Hülle wird bei jedem Aufruf dieser Funktion um 10% repariert.
	void RepairHull();	

private:
	BOOLEAN m_bDoubleHull;
	BOOLEAN m_bAblative;		///< Der Schaden welcher immer auf die Hülle geht entfällt
	BOOLEAN m_bPolarisation;	///< 10% weniger Schaden wird genommen
	UINT m_iBaseHull;
	UINT m_iCurrentHull;
	BYTE m_iHullMaterial;
	UINT m_iMaxHull;
};

#endif // !defined(AFX_HULL_H__4AA4B76E_E548_45F2_BBF2_1723996202FE__INCLUDED_)
