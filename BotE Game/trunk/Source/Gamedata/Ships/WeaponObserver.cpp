#include "stdafx.h"
#include "WeaponObserver.h"

IMPLEMENT_SERIAL (CWeaponObserver, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CWeaponObserver::CWeaponObserver(void)
{
	for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
		m_BuildableTorpedos[i] = FALSE;
	m_MaxShieldLevel = 0;
}

CWeaponObserver::~CWeaponObserver(void)
{
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.RemoveAt(i--);
	for (int i = 0; i < m_TupeWeapons.GetSize(); i++)
		m_TupeWeapons.RemoveAt(i--);
	m_BeamWeapons.RemoveAll();
	m_TupeWeapons.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CWeaponObserver::CWeaponObserver(const CWeaponObserver & rhs)
{
	for (int i = 0; i < rhs.m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
	for (int i = 0; i < rhs.m_TupeWeapons.GetSize(); i++)
		m_TupeWeapons.Add(rhs.m_TupeWeapons.GetAt(i));
	for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
		m_BuildableTorpedos[i] = rhs.m_BuildableTorpedos[i];
	m_MaxShieldLevel = rhs.m_MaxShieldLevel;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CWeaponObserver & CWeaponObserver::operator=(const CWeaponObserver & rhs)
{
	if (this == &rhs)
		return *this;
	for (int i = 0; i < rhs.m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
	for (int i = 0; i < rhs.m_TupeWeapons.GetSize(); i++)
		m_TupeWeapons.Add(rhs.m_TupeWeapons.GetAt(i));
	for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
		m_BuildableTorpedos[i] = rhs.m_BuildableTorpedos[i];
	m_MaxShieldLevel = rhs.m_MaxShieldLevel;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CWeaponObserver::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
			ar << m_BuildableTorpedos[i];
		ar << m_MaxShieldLevel;
		ar << m_BeamWeapons.GetSize();
		for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		{
			ar << m_BeamWeapons.GetAt(i).maxLevel;
			ar << m_BeamWeapons.GetAt(i).WeaponName;
		}
		ar << m_TupeWeapons.GetSize();
		for (int i = 0; i < m_TupeWeapons.GetSize(); i++)
		{
			ar << m_TupeWeapons.GetAt(i).fireRate;
			ar << m_TupeWeapons.GetAt(i).number;
			ar << m_TupeWeapons.GetAt(i).onlyMicro;
			ar << m_TupeWeapons.GetAt(i).TupeName;
			ar << m_TupeWeapons.GetAt(i).fireAngle;
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
			ar >> m_BuildableTorpedos[i];
		ar >> m_MaxShieldLevel;	
		ar >> number;
		BeamWeaponsObserverStruct bwos;
		m_BeamWeapons.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ar >> bwos.maxLevel;
			ar >> bwos.WeaponName;
			m_BeamWeapons.Add(bwos);
		}
		ar >> number;
		TupeWeaponsObserverStruct twos;
		m_TupeWeapons.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ar >> twos.fireRate;
			ar >> twos.number;
			ar >> twos.onlyMicro;
			ar >> twos.TupeName;
			ar >> twos.fireAngle;
			m_TupeWeapons.Add(twos);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

// Funktion gibt uns den maximal baubaren Typ einer Beamwaffe zurück. Dafür übergeben wir der Funktion
// den Namen der Beamwaffe. Ist der Rückgabewert NULL, so können wir den Typ nicht bauen
BYTE CWeaponObserver::GetMaxBeamType(CString NameOfBeamWeapon)
{
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		if (m_BeamWeapons.GetAt(i).WeaponName == NameOfBeamWeapon)
			return m_BeamWeapons.GetAt(i).maxLevel;
	return 0;
}

// Funktion checkt alle baubaren Beamwaffen und setzt alle Variablen auf den richtigen Wert.
// Der Parameter <code>info</code> muß ein derzeit baubares Schiff des Imperium sein. Die Funktion
// checkt gleichzeitig auch den max. Schildtyp.
void CWeaponObserver::CheckBeamWeapons(CShipInfo* info)
{
	// Erstmal den Namen der Beamwaffe checken und den maximalen Typ eintragen
	for (int j = 0; j < info->GetBeamWeapons()->GetSize(); j++)
	{
		BOOLEAN foundBeamName = FALSE;
		for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		{
			// Wir haben in unserem Feld so einen Namen der Beamwaffe gefunden
			if (m_BeamWeapons.GetAt(i).WeaponName == info->GetBeamWeapons()->GetAt(j).GetBeamName())
			{
				foundBeamName = TRUE;
				if (m_BeamWeapons.GetAt(i).maxLevel < info->GetBeamWeapons()->GetAt(j).GetBeamType())
				{
					BeamWeaponsObserverStruct bwos;
					bwos.WeaponName = info->GetBeamWeapons()->GetAt(j).GetBeamName();
					bwos.maxLevel   = info->GetBeamWeapons()->GetAt(j).GetBeamType();
					m_BeamWeapons.SetAt(i,bwos);
				}
			}
		}
		// Wenn wir in dem Feld nicht diesen Namen der Beamwaffe gefunden haben, dann fügen wir ihn hinzu
		if (foundBeamName == FALSE)
		{
			BeamWeaponsObserverStruct bwos;
			bwos.WeaponName = info->GetBeamWeapons()->GetAt(j).GetBeamName();
			bwos.maxLevel   = info->GetBeamWeapons()->GetAt(j).GetBeamType();
			m_BeamWeapons.Add(bwos);
		}
	}
	// Hier wird auch gleich der maximale Schildtyp berechnet
	if (info->GetShield()->GetShieldType() > m_MaxShieldLevel)
		m_MaxShieldLevel = info->GetShield()->GetShieldType();
}

// Funktion checkt alle Torpedowaffen (Torpedowerfer, Torpedoart) die wir bauen können und übernimmt dafür einen
// Zeiger auf ein Schiffsinfoobjekt. Das Schiff muß zur gleichen Rasse gehören, zu der auch das
// Observerobjekt gehört. Der Parameter <code>info</code> muß ein derzeit baubares Schiff des Imperium sein.
void CWeaponObserver::CheckTorpedoWeapons(CShipInfo* info)
{
	for (int j = 0; j < info->GetTorpedoWeapons()->GetSize(); j++)
	{
		// Gleich die gefundene Torpedoart im Feld auf TRUE setzen
		m_BuildableTorpedos[info->GetTorpedoWeapons()->GetAt(j).GetTorpedoType()] = TRUE;
		// Jetzt noch nach baubaren Torpedowerfern suchen
		BOOLEAN foundTupeName = FALSE;
		for (int k = 0; k < m_TupeWeapons.GetSize(); k++)
		{	// Wir haben in dem Feld schon einen Werfer mit dem Namen gefunden
			if (m_TupeWeapons.GetAt(k).TupeName == info->GetTorpedoWeapons()->GetAt(j).GetTupeName())
			{
				foundTupeName = TRUE;
				break;
			}
		}
		// Wenn wir in dem Feld nicht den Namen des Torpedowerfers gefunden haben, dann fügen wir ihn hinzu
		if (foundTupeName == FALSE)
		{
			TupeWeaponsObserverStruct twos;
			twos.fireAngle = info->GetTorpedoWeapons()->GetAt(j).GetFirearc()->GetAngle();
			twos.fireRate  = info->GetTorpedoWeapons()->GetAt(j).GetTupeFirerate();
			twos.number	   = info->GetTorpedoWeapons()->GetAt(j).GetNumber();
			twos.onlyMicro = info->GetTorpedoWeapons()->GetAt(j).GetOnlyMicroPhoton();
			twos.TupeName  = info->GetTorpedoWeapons()->GetAt(j).GetTupeName();			
			m_TupeWeapons.Add(twos);
		}
	}	
}

// Funktion sucht einen weiteren baubaren Torpedo und übernimmt dafür den aktuell angebauten Torpedotyp und
// ob der aktuelle Werfer nur Micro-Torpedos verschießen kann
BYTE CWeaponObserver::GetNextTorpedo(BYTE currentTorpedoType, BOOLEAN onlyMicroTupe)
{
	int i = currentTorpedoType + 1;
	if (i == DIFFERENT_TORPEDOS)
		i = 0;
	while (i < DIFFERENT_TORPEDOS)
	{
		if (m_BuildableTorpedos[i] == TRUE)
		{
			// Wenn der aktuell angebaute Torpedowerfer nur Micro-Torpedos verschießen kann, dann hier überprüfen,
			// ob es sich auch um einen Micro-Torpedotyp handelt
			if (onlyMicroTupe == TRUE && CTorpedoInfo::GetMicro(i))
				return i;
			else if (onlyMicroTupe == FALSE)
				return i;
			else
				return currentTorpedoType;
		}
		i++;
		if (i == DIFFERENT_TORPEDOS)
			i = 0;
	}
	return currentTorpedoType;
}

// Funktion sucht einen weiteren Torpedowerfer, denn wir an das Schiff montieren können und übernimmt dafür
// den Namen des aktuell angebauten Werfers und den aktuell angebauten Torpedotyp
TupeWeaponsObserverStruct CWeaponObserver::GetNextTupe(CString currentTupeName, BYTE currentTorpedoType)
{
	TupeWeaponsObserverStruct twos;
	USHORT count = 0;
	// Nummer des aktuellen Werfers suchen
	for (int i = 0; i < m_TupeWeapons.GetSize(); i++)
		if (m_TupeWeapons.GetAt(i).TupeName == currentTupeName)
		{
			count = i;
			twos = m_TupeWeapons.GetAt(i);
			break;
		}
	// Jetzt Nachfolger des aktuellen Werfers im Feld suchen
	for (int i = (count+1); i <= m_TupeWeapons.GetSize(); i++)
	{
		if (i == m_TupeWeapons.GetSize())
			i = 0;
		// Wenn wir mit dem neuen Werfer nur Mirco-Torpedos verschießen können, dann auch schauen, dass ein
		// Micro-Torpedo eingestellt ist
		if (m_TupeWeapons.GetAt(i).onlyMicro == TRUE && CTorpedoInfo::GetMicro(currentTorpedoType))			
		{
			twos = m_TupeWeapons.GetAt(i);
			return twos;
		}
		else if (m_TupeWeapons.GetAt(i).onlyMicro == FALSE)
		{
			twos = m_TupeWeapons.GetAt(i);
			return twos;
		}		
	}
	return twos;
}

// Resetfunktion für die Klasse CWeaponObserver
void CWeaponObserver::Reset()
{
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.RemoveAt(i--);
	for (int i = 0; i < m_TupeWeapons.GetSize(); i++)
		m_TupeWeapons.RemoveAt(i--);
	m_BeamWeapons.RemoveAll();
	m_TupeWeapons.RemoveAll();
	for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
		m_BuildableTorpedos[i] = FALSE;
	m_MaxShieldLevel = 0;
}
