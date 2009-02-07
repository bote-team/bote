#include "stdafx.h"
#include "Troop.h"

IMPLEMENT_SERIAL (CTroop, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTroop::CTroop(void)
{
}

CTroop::~CTroop(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTroop::CTroop(const CTroop & rhs)
{
	m_byID = rhs.m_byID;
	m_byOwner = rhs.m_byOwner;
	m_byPower = rhs.m_byPower;
	m_iExperiance = rhs.m_iExperiance;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTroop & CTroop::operator=(const CTroop & rhs)
{
	if (this == &rhs)
		return *this;
	m_byID = rhs.m_byID;
	m_byOwner = rhs.m_byOwner;
	m_byPower = rhs.m_byPower;
	m_iExperiance = rhs.m_iExperiance;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktion
//////////////////////////////////////////////////////////////////////
void CTroop::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byID;
		ar << m_byOwner;
		ar << m_byPower;
		ar << m_iExperiance;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_byID;
		ar >> m_byOwner;
		ar >> m_byPower;
		ar >> m_iExperiance;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Mittels dieser Funktion wird eine andere Truppe, welche hier als Zeiger als Parameter <code>enemy</code>
/// übergeben werden muss, angegriffen. Es kann ein zusätzlicher Angriffsbonu <code>offenceBoni</code> übergeben
/// werden. Außerdem wird der Verteidigungsbonus in dem System, in welchem die Truppe stationiert ist im
/// Parameter <code>defenceBoni</code> übergeben. Wenn die Funktion eine 0 zurückgibt, dann hat diese Einheit gewonnen,
/// wenn sie eine 1 zurückgibt, dann hat die übergebene Einheit gewonnen, wenn sie eine 2 zurückgibt, dann haben sich
/// beide Einheiten gegenseitig zerstört.
BYTE CTroop::Attack(CTroop* enemy, BYTE offenceBoni, short defenceBoni)
{
	// Ein Angriff funktioniert ganz einfach: Es wird mit der Stärke der angreifenden Truppe gegen die
	// Stärke der verteidigenden Truppe gewürfelt. Wer den höheren Wurf gemacht hat, gewinnt diesen Kampf.
	// Bei Gleichstand haben beide verloren. Auf die Stärke wird die Erfahrung angerechnet. Die verteidigende
	// Truppe erhält zusätzlich vielleicht auch einen DefenceBonus.
	
	// der angreifenden Truppe (also dieses Objekt)
	BYTE damageBoni = 0;
	if (m_iExperiance >= 64000)
		damageBoni = 80;
	else if (m_iExperiance >= 32000)
		damageBoni = 70;
	else if (m_iExperiance >= 16000)
		damageBoni = 60;
	else if (m_iExperiance >= 8000)
		damageBoni = 50;
	else if (m_iExperiance >= 4000)
		damageBoni = 40;
	else if (m_iExperiance >= 2000)
		damageBoni = 30;
	else if (m_iExperiance >= 1000)
		damageBoni = 20;
	else if (m_iExperiance >= 500)
		damageBoni = 10;
	damageBoni += offenceBoni;
	BYTE attackPower = m_byPower + m_byPower * damageBoni / 100;
	USHORT ExpBonusForDef = attackPower * 100;
	
/*	CString t;
	t.Format("attackPower = %d\nExp: %d",attackPower, m_iExperiance);
	AfxMessageBox(t);*/

	// der verteidigenden Truppe (dem als Parameter übergebenen Objektes)
	damageBoni = 0;
	if (enemy->m_iExperiance >= 64000)
		damageBoni = 80;
	else if (enemy->m_iExperiance >= 32000)
		damageBoni = 70;
	else if (enemy->m_iExperiance >= 16000)
		damageBoni = 60;
	else if (enemy->m_iExperiance >= 8000)
		damageBoni = 50;
	else if (enemy->m_iExperiance >= 4000)
		damageBoni = 40;
	else if (enemy->m_iExperiance >= 2000)
		damageBoni = 30;
	else if (enemy->m_iExperiance >= 1000)
		damageBoni = 20;
	else if (enemy->m_iExperiance >= 500)
		damageBoni = 10;
	BYTE defencePower = enemy->m_byPower + enemy->m_byPower * damageBoni / 100;
	defencePower += defencePower * defenceBoni / 100;
	USHORT ExpBonusForAtt = defencePower * 100;

	if (attackPower > 0)
		attackPower = rand()%attackPower;
	if (defencePower > 0)
		defencePower = rand()%defencePower;
	
/*	CString s;
	s.Format("ATTACK: %d\nDEFENCE: %d",attackPower,defencePower);
	AfxMessageBox(s);*/
	if (attackPower > defencePower)
	{
		m_iExperiance += ExpBonusForAtt;
		return 0;
	}
	else if (attackPower < defencePower)
	{
		enemy->m_iExperiance += ExpBonusForDef;
		return 1;
	}
	else
		return 2;
}

