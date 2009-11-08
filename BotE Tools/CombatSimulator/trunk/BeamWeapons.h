/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// BeamWeapons.h: Schnittstelle für die Klasse CBeamWeapons.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEAMWEAPONS_H__B441285A_53B4_4C2A_9F9F_0C99ABEB1584__INCLUDED_)
#define AFX_BEAMWEAPONS_H__B441285A_53B4_4C2A_9F9F_0C99ABEB1584__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FireArc.h"

class CBeamWeapons : public CObject  
{
	friend class CShip;
public:
	DECLARE_SERIAL (CBeamWeapons)
	CBeamWeapons();
	virtual ~CBeamWeapons();
	CBeamWeapons(const CBeamWeapons & rhs);
	CBeamWeapons & operator=(const CBeamWeapons &);
// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	
	const CString& GetBeamName() const {return m_strBeamName;}
	USHORT GetBeamPower() const {return m_iBeamPower;}
	BYTE GetBeamType() const {return m_iBeamType;}
	BYTE GetBeamNumber() const {return m_iBeamNumber;}
	BYTE GetShootNumber() const {return m_byShootNumber;}
	BYTE GetBonus() const {return m_byBonus;}
	BYTE GetBeamLenght() const {return m_byBeamLenght;}
	BYTE GetRechargeTime() const {return m_byRechargeTime;}
	BOOLEAN GetPiercing() const {return m_bPiercing;}
	BOOLEAN GetModulating() const {return m_bModulating;}

	/// Funktion gibt Zeiger auf das Schussfeld der Waffe zurück.
	/// @return Zeiger auf Schussfeld
	CFireArc* GetFirearc(void) {return &m_Firearc;}

	void SetBeamPower(USHORT power) {m_iBeamPower = power;}
	void SetRechargeTime(BYTE time) {m_byRechargeTime = time;}

	void ModifyBeamWeapon(BYTE BeamType, USHORT BeamPower, BYTE BeamNumber, const CString& BeamName, BOOLEAN Modulating,
		BOOLEAN Piercing, BYTE Bonus, BYTE BeamLenght, BYTE RechargeTime, BYTE ShootNumber);
	
private:
	CString m_strBeamName;			// Name der Beamart
	USHORT m_iBeamPower;			// Stärke eines Beams
	BYTE m_iBeamType;				// Typ des/der Beams
	BYTE m_iBeamNumber;				// Anzahl der einzelnen Beamsysteme
	BYTE m_byShootNumber;			// Wieviele Bolzen/Strahlen werden von einem Beamsystem pro Schuss verschossen
	BYTE m_byBonus;					// Bonus des Beams duruch das Feuersystem/Waffengattung (also Cannon, Array usw.)
	BYTE m_byBeamLenght;			// Beamdauer
	BYTE m_byRechargeTime;			// Nachladezeit, wenn gefeuert wurde
	BOOLEAN m_bPiercing;			// schilddurchdringende Beamwaffen, außer bei regenerativen Schilden
	BOOLEAN m_bModulating;			// modulierende Beams -> immer 50% des Schadens auf die Hülle
	CFireArc m_Firearc;				// möglicher Feuerwinkel und Anbringung der Waffe
};

#endif // !defined(AFX_BEAMWEAPONS_H__B441285A_53B4_4C2A_9F9F_0C99ABEB1584__INCLUDED_)
