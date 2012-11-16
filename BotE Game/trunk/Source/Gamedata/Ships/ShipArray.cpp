#include "stdafx.h"

#include <cassert>

#include "ShipArray.h"
#include "Ships.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShipArray::CShipArray(void) :
	m_Ships(),
	m_NextKey(0)
{
	m_CurrentShip = begin();
	m_FleetShip = begin();
}

CShipArray::~CShipArray(void)
{
	Reset();
}

//////////////////////////////////////////////////////////////////////
// getting iterators
//////////////////////////////////////////////////////////////////////

CShipArray::const_iterator CShipArray::begin() const {
	return m_Ships.begin();
}
CShipArray::const_iterator CShipArray::end() const {
	return m_Ships.end();
}

CShipArray::iterator CShipArray::begin() {
	return m_Ships.begin();
}
CShipArray::iterator CShipArray::end() {
	return m_Ships.end();
}

CShipArray::const_iterator CShipArray::find(unsigned key) const {
	return m_Ships.find(key);
}
CShipArray::iterator CShipArray::find(unsigned key) {
	return m_Ships.find(key);
}

CShipArray::const_iterator CShipArray::iterator_at(int index) const {
	assert(0 <= index && index < GetSize());
	CShipArray::const_iterator i = begin();
	std::advance(i, index);
	return i;
}
CShipArray::iterator CShipArray::iterator_at(int index) {
	assert(0 <= index && index < GetSize());
	CShipArray::iterator i = begin();
	std::advance(i, index);
	return i;
}

//////////////////////////////////////////////////////////////////////
// adding elements
//////////////////////////////////////////////////////////////////////

CShipArray::iterator CShipArray::Add(const CShips& ship) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("CShipArray: adding ship %s", ship.GetShipName());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}

	const unsigned key = NextKey();
	const int old_size = GetSize();
	CShipArray::iterator result = m_Ships.insert(end(), std::make_pair(key, ship));
	CShipArray::iterator temp = result;
	result->second.SetKey(key);
	assert(old_size + 1 == GetSize());
	++temp;
	assert(temp == end());
	return result;
}

void CShipArray::Append(const CShipArray& other) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("\nCShipArray: appending shiparray:\n%s\n", other.ToString());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}
	for(CShipArray::const_iterator i = other.begin(); i != other.end(); ++i)
		Add(i->second);
}

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////

void CShipArray::Reset() {
	m_Ships.clear();
	m_NextKey = 0;
	m_CurrentShip = begin();
	m_FleetShip = begin();
}
void CShipArray::RemoveAt(CShipArray::iterator& index) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("CShipArray: removing ship %s", index->second.GetShipName());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}
	UpdateSpecialShip(m_CurrentShip, index);
	UpdateSpecialShip(m_FleetShip, index);
	m_Ships.erase(index++);
}

//////////////////////////////////////////////////////////////////////
// getting elements
//////////////////////////////////////////////////////////////////////

//TODO reduce amount and calls to these, complexity linear

const CShips& CShipArray::GetAt(int index) const {
	return iterator_at(index)->second;
}
CShips& CShipArray::GetAt(int index) {
	return iterator_at(index)->second;
}

const CShips& CShipArray::ElementAt(int index) const {
	return GetAt(index);
}
CShips& CShipArray::ElementAt(int index) {
	return GetAt(index);
}

const CShips& CShipArray::operator[](int index) const {
	return GetAt(index);
}
CShips& CShipArray::operator[](int index) {
	return GetAt(index);
}

const CShips& CShipArray::at(unsigned key) const {
	const CShipArray::const_iterator i = find(key);
	assert(i != end());
	return i->second;
}
CShips& CShipArray::at(unsigned key) {
	CShipArray::iterator i = find(key);
	assert(i != end());
	return i->second;
}

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////

//TODO these could be used to get elements by an "array index" which is bad now

int CShipArray::GetUpperBound() const {
	return GetSize() - 1;
}
int CShipArray::GetSize() const {
	return m_Ships.size();
}
bool CShipArray::empty() const {
	return m_Ships.empty();
}

int CShipArray::index_of(const CShipArray::const_iterator& position) const {
	return std::distance(begin(), position);
}
//////////////////////////////////////////////////////////////////////
// private functions
//////////////////////////////////////////////////////////////////////

unsigned CShipArray::NextKey() {
	return m_NextKey++;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

void CShipArray::Serialize(CArchive& ar) {
	if(ar.IsStoring()) {
		ar << GetSize();
		for(CShipArray::iterator i = begin(); i != end(); ++i)
			i->second.Serialize(ar);
	}
	else if(ar.IsLoading()) {
		int size;
		ar >> size;
		//We need to call clear(), since resize() calls destructors only for elements from after the new
		//lenght (if new size is smaller), and apparently the >> operator allocates stuff with new
		//with the CString members of the ship class in their Serialize functions above.
		//Thus, call the destructors of all the current elements explicitely and manually.
		//(This comment was appropriate for std::vector)
		Reset();
		for(int i = 0; i < size; ++i)
		{
			CShips ship;
			ship.Serialize(ar);
			Add(ship);
		}
	}
	else
		assert(false);
}

void CShipArray::SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID) {
	if(ar.IsStoring()) {
		CShipArray ships;
		for(CShipArray::const_iterator i = begin(); i != end(); ++i)
			if (i->second.GetOwnerOfShip() == sMajorID)
				ships.Add(i->second);
		ships.Serialize(ar);
	}
	else if(ar.IsLoading()) {
		int count = 0;
		ar >> count;
		for(CShipArray::iterator i = begin(); i != end();) {
			if (i->second.GetOwnerOfShip() == sMajorID) {
				RemoveAt(i);
				continue;
			}
			++i;
		}
		const int newsize = GetSize() + count;
		for (int i = GetSize(); i < newsize; ++i) {
			CShips ship;
			ship.Serialize(ar);
			const CShipArray::iterator j = Add(ship);
			assert(j->second.GetOwnerOfShip() == sMajorID);
		}
	}
	else
		assert(false);
}

void CShipArray::SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector) {
	if(ar.IsStoring()) {
		int nCount = 0;
		for(CShipArray::const_iterator i = begin(); i != end(); ++i)
			if (i->second.GetKO() == ptCurrentCombatSector)
				nCount++;
		ar << nCount;
		// nur Schiffe aus diesem Sektor senden
		for(CShipArray::iterator i = begin(); i != end(); ++i)
			if (i->second.GetKO() == ptCurrentCombatSector)
				i->second.Serialize(ar);
	}
	else if(ar.IsLoading()) {
		// Es werden nur Schiffe aus dem aktuellen Kampfsektor empfangen
		int count = 0;
		ar >> count;
		// alle Schiffe aus dem Kampfsektor entfernen
		for(CShipArray::iterator i = begin(); i != end();) {
			if (i->second.GetKO() == ptCurrentCombatSector) {
				RemoveAt(i);
				continue;
			}
			++i;
		}
		// empfangene Schiffe wieder hinzufügen
		const int newsize = GetSize() + count;
		for (int i = GetSize(); i < newsize; ++i) {
			CShips ship;
			ship.Serialize(ar);
			const CShipArray::iterator j = Add(ship);
			assert(j->second.GetKO() == ptCurrentCombatSector);
		}
	}
	else
		assert(false);
}

//////////////////////////////////////////////////////////////////////
// special ships
//////////////////////////////////////////////////////////////////////
//current ship

int CShipArray::CurrentShipsIndex() const {
	assert(begin() != m_CurrentShip || m_CurrentShip != end());
	return index_of(m_CurrentShip);
}

void CShipArray::SetCurrentShip(unsigned key) {
	const CShipArray::iterator i = find(key);
	assert(i != end());
	m_CurrentShip = i;
}

void CShipArray::SetCurrentShip(const CShipArray::iterator& position) {
	assert(position != end());
	m_CurrentShip = position;
}

const CShips& CShipArray::CurrentShip() const {
	assert(begin() != m_CurrentShip || m_CurrentShip != end());
	return m_CurrentShip->second;
}

CShips& CShipArray::CurrentShip() {
	assert(begin() != m_CurrentShip || m_CurrentShip != end());
	return m_CurrentShip->second;
}

//fleet ship

int CShipArray::FleetShipsIndex() const {
	assert(begin() != m_FleetShip || m_FleetShip != end());
	return index_of(m_FleetShip);
}

void CShipArray::SetFleetShip(unsigned key) {
	const CShipArray::iterator i = find(key);
	assert(i != end());
	m_FleetShip = i;
}

void CShipArray::SetFleetShip(const CShipArray::iterator& position) {
	assert(position != end());
	m_FleetShip = position;
}

const CShips& CShipArray::FleetShip() const {
	assert(begin() != m_FleetShip || m_FleetShip != end());
	return m_FleetShip->second;
}


CShips& CShipArray::FleetShip() {
	assert(begin() != m_FleetShip || m_FleetShip != end());
	return m_FleetShip->second;
}

//updating

void CShipArray::UpdateSpecialShip(CShipArray::iterator& ship, CShipArray::iterator& to_erase) {
	if(ship != to_erase)
		return;
	if(ship != begin())
		--ship;
	else
		++ship;
	//TODO unsolved; there should remain at least one ship in the container
	assert(ship != end());
}

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////

//CString CShipArray::ToString() const {
//	CString s("SHIPARRAY BEGINN\n");
//	unsigned index = 0;
//	for(CShipArray::const_iterator i = begin(); i != end(); ++i)
//		s.Format("%s%u: %s at (%u,%u)\n", s, index, i->GetShipName(), i->GetKO().x, i->GetKO().y);
//	s.Format("%s%s", s, "SHIPARRAY END");
//	return s;
//}