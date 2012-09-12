// TorpedoWeapons.cpp: Implementierung der Klasse CTorpedoWeapons.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TorpedoWeapons.h"
#include "Options.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CTorpedoWeapons, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CTorpedoWeapons::CTorpedoWeapons()
{
	m_iTorpedoType		= 0;
	m_iNumber			= 0;
	m_iTupeFirerate		= 0;
	m_iNumberOfTupes	= 0;
	m_byAccuracy		= 0;
	m_strTupeName		= "";
	m_bOnlyMicroPhoton  = FALSE;
}

CTorpedoWeapons::~CTorpedoWeapons()
{

}
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTorpedoWeapons::CTorpedoWeapons(const CTorpedoWeapons & rhs) :
	m_strTupeName(rhs.m_strTupeName),
	m_Firearc(rhs.m_Firearc)
{
	m_iTorpedoType		= rhs.m_iTorpedoType;
	m_iNumber			= rhs.m_iNumber;
	m_iTupeFirerate		= rhs.m_iTupeFirerate;
	m_iNumberOfTupes	= rhs.m_iNumberOfTupes;
	m_byAccuracy		= rhs.m_byAccuracy;
	m_bOnlyMicroPhoton  = rhs.m_bOnlyMicroPhoton;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTorpedoWeapons & CTorpedoWeapons::operator=(const CTorpedoWeapons & rhs)
{
	if (this == &rhs)
		return *this;
	m_iTorpedoType		= rhs.m_iTorpedoType;
	m_iNumber			= rhs.m_iNumber;
	m_iTupeFirerate		= rhs.m_iTupeFirerate;
	m_iNumberOfTupes	= rhs.m_iNumberOfTupes;
	m_byAccuracy		= rhs.m_byAccuracy;
	m_strTupeName		= rhs.m_strTupeName;
	m_bOnlyMicroPhoton  = rhs.m_bOnlyMicroPhoton;
	m_Firearc		= rhs.m_Firearc;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CTorpedoWeapons::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	m_Firearc.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iTorpedoType;
		ar << m_iNumber;
		ar << m_iTupeFirerate;
		ar << m_iNumberOfTupes;
		ar << m_byAccuracy;
		ar << m_strTupeName;
		ar << m_bOnlyMicroPhoton;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iTorpedoType;
		ar >> m_iNumber;
		ar >> m_iTupeFirerate;
		ar >> m_iNumberOfTupes;
		ar >> m_byAccuracy;
		ar >> m_strTupeName;
		ar >> m_bOnlyMicroPhoton;
	}
}

// Funktion ändert die Werte dieser Topredowaffe
void CTorpedoWeapons::ModifyTorpedoWeapon(BYTE TorpedoType, BYTE Number, BYTE TupeFirerate, BYTE NumberOfTupes,
										  const CString& TupeName, BOOLEAN OnlyMicroPhoton, BYTE Acc)
{
	m_iTorpedoType		= TorpedoType;
	m_iNumber			= Number;
	m_iTupeFirerate		= TupeFirerate;
	m_iNumberOfTupes	= NumberOfTupes;
	m_strTupeName		= TupeName;
	m_bOnlyMicroPhoton  = OnlyMicroPhoton;
	m_byAccuracy		= Acc;
}
