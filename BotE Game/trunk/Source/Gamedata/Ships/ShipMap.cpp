#include "stdafx.h"

#include <cassert>

#include "ShipMap.h"
#include "Ships.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShipMap::CShipMap(void) :
	m_Ships(),
	m_NextKey(0)
{
	m_CurrentShip = begin();
	m_FleetShip = begin();
}

CShipMap::~CShipMap(void)
{
	Reset(true);
}

CShipMap::CShipMap(const CShipMap& o) :
	m_Ships(o.m_Ships),
	m_NextKey(o.m_NextKey)
{
	m_CurrentShip = begin();
	m_FleetShip = begin();
	if(!o.empty()) {
		m_CurrentShip = find(o.CurrentShip()->second->Key());
		m_FleetShip = find(o.FleetShip()->second->Key());
	}
}

CShipMap& CShipMap::operator=(const CShipMap& o)
{
	if(this == &o)
		return *this;

	m_Ships = o.m_Ships;
	m_NextKey = o.m_NextKey;
	m_CurrentShip = begin();
	m_FleetShip = begin();
	if(!o.empty()) {
		m_CurrentShip = find(o.CurrentShip()->second->Key());
		m_FleetShip = find(o.FleetShip()->second->Key());
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// getting iterators
//////////////////////////////////////////////////////////////////////

CShipMap::const_iterator CShipMap::begin() const {
	return m_Ships.begin();
}
CShipMap::const_iterator CShipMap::end() const {
	return m_Ships.end();
}

CShipMap::iterator CShipMap::begin() {
	return m_Ships.begin();
}
CShipMap::iterator CShipMap::end() {
	return m_Ships.end();
}

CShipMap::const_iterator CShipMap::find(unsigned key) const {
	return m_Ships.find(key);
}
CShipMap::iterator CShipMap::find(unsigned key) {
	return m_Ships.find(key);
}

static bool advance_by(int& advance, int size) {
	assert(0 <= advance && advance <= size);
	if(advance >= size/2) {
		advance = advance - size;
		return true;
	}
	else
		return false;
}
CShipMap::const_iterator CShipMap::iterator_at(int index) const {
	CShipMap::const_iterator i = begin();
	if(advance_by(index, GetSize()))
		i = end();
	std::advance(i, index);
	return i;
}
CShipMap::iterator CShipMap::iterator_at(int index) {
	CShipMap::iterator i = begin();
	if(advance_by(index, GetSize()))
		i = end();
	std::advance(i, index);
	return i;
}

//////////////////////////////////////////////////////////////////////
// adding elements
//////////////////////////////////////////////////////////////////////

CShipMap::iterator CShipMap::Add(CShips* ship) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("CShipMap: adding ship %s", ship.GetShipName());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}

	const unsigned key = NextKey();
	CShipMap::iterator result = m_Ships.insert(end(), std::make_pair(key, ship));
	if(GetSize() == 1)
	{
		m_CurrentShip = result;
		m_FleetShip = result;
	}
	CShipMap::iterator temp = result;
	result->second->SetKey(key);
	++temp;
	assert(temp == end());
	return result;
}

void CShipMap::Append(const CShipMap& other) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("\nCShipMap: appending shiparray:\n%s\n", other.ToString());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}
	for(CShipMap::const_iterator i = other.begin(); i != other.end(); ++i)
		Add(i->second);
}

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////

void CShipMap::Reset(bool destroy) {
	if(destroy)
		for(CShipMap::iterator i = begin(); i != end(); ++i) {
			assert(i->second);
			delete i->second;
			i->second = NULL;
		}
	m_Ships.clear();
	m_NextKey = 0;
	m_CurrentShip = begin();
	m_FleetShip = begin();
}
void CShipMap::EraseAt(CShipMap::iterator& index, bool destroy) {
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("CShipMap: removing ship %s", index->second->GetShipName());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}
	assert(!empty() && index != end());
	//need to copy the iterator, because it can come from a reference to m_CurrentShip (or m_FleetShip),
	//meaning updating m_CurrentShip/m_FleetShip would change it as well
	const CShipMap::iterator to_erase = index;
	UpdateSpecialShip(m_CurrentShip, to_erase);
	UpdateSpecialShip(m_FleetShip, to_erase);
	//now only prevent the passed iterator from becoming invalid via incrementing,
	//if the above two calls didn't already update it
	if(to_erase == index)
		++index;
	if(destroy) {
		assert(to_erase->second);
		delete to_erase->second;
		to_erase->second = NULL;
	}
	m_Ships.erase(to_erase);
	if(empty())
		Reset(true);
}

//////////////////////////////////////////////////////////////////////
// getting elements
//////////////////////////////////////////////////////////////////////

const CShips& CShipMap::at(unsigned key) const {
	const CShipMap::const_iterator i = find(key);
	assert(i != end());
	return *i->second;
}
CShips& CShipMap::at(unsigned key) {
	CShipMap::iterator i = find(key);
	assert(i != end());
	return *i->second;
}

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////

int CShipMap::GetUpperBound() const {
	return GetSize() - 1;
}
int CShipMap::GetSize() const {
	return m_Ships.size();
}
bool CShipMap::empty() const {
	return m_Ships.empty();
}

//////////////////////////////////////////////////////////////////////
// private functions
//////////////////////////////////////////////////////////////////////

unsigned CShipMap::NextKey() {
	return m_NextKey++;
}

int CShipMap::index_of(const CShipMap::const_iterator& position) const {
	return std::distance(begin(), position);
}

const CShips& CShipMap::GetAt(int index) const {
	assert(index < GetSize());
	return *iterator_at(index)->second;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

void CShipMap::Serialize(CArchive& ar) {
	if(ar.IsStoring()) {
		ar << GetSize();
		for(CShipMap::iterator i = begin(); i != end(); ++i)
			i->second->Serialize(ar);
	}
	else if(ar.IsLoading()) {
		int size;
		ar >> size;
		//We need to call clear(), since resize() calls destructors only for elements from after the new
		//lenght (if new size is smaller), and apparently the >> operator allocates stuff with new
		//with the CString members of the ship class in their Serialize functions above.
		//Thus, call the destructors of all the current elements explicitely and manually.
		//(This comment was appropriate for std::vector)
		Reset(true);
		for(int i = 0; i < size; ++i)
		{
			CShips* ship = new CShips();
			ship->Serialize(ar);
			Add(ship);
		}
	}
	else
		assert(false);
}

void CShipMap::SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID) {
	if(ar.IsStoring()) {
		std::vector<CShips*> ships;
		for(CShipMap::const_iterator i = begin(); i != end(); ++i)
			if (i->second->GetOwnerOfShip() == sMajorID)
				ships.push_back(i->second);
		ar << ships.size();
		for(std::vector<CShips*>::iterator i = ships.begin(); i != ships.end(); ++i)
			(*i)->Serialize(ar);
	}
	else if(ar.IsLoading()) {
		int count = 0;
		ar >> count;
		for(CShipMap::iterator i = begin(); i != end();) {
			if (i->second->GetOwnerOfShip() == sMajorID) {
				EraseAt(i, true);
				continue;
			}
			++i;
		}
		const int newsize = GetSize() + count;
		for (int i = GetSize(); i < newsize; ++i) {
			CShips* ship = new CShips();
			ship->Serialize(ar);
			const CShipMap::iterator j = Add(ship);
			assert(j->second->GetOwnerOfShip() == sMajorID);
		}
	}
	else
		assert(false);
}

void CShipMap::SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector) {
	if(ar.IsStoring()) {
		int nCount = 0;
		for(CShipMap::const_iterator i = begin(); i != end(); ++i)
			if (i->second->GetKO() == ptCurrentCombatSector)
				nCount++;
		ar << nCount;
		// nur Schiffe aus diesem Sektor senden
		for(CShipMap::iterator i = begin(); i != end(); ++i)
			if (i->second->GetKO() == ptCurrentCombatSector)
				i->second->Serialize(ar);
	}
	else if(ar.IsLoading()) {
		// Es werden nur Schiffe aus dem aktuellen Kampfsektor empfangen
		int count = 0;
		ar >> count;
		// alle Schiffe aus dem Kampfsektor entfernen
		for(CShipMap::iterator i = begin(); i != end();) {
			if (i->second->GetKO() == ptCurrentCombatSector) {
				EraseAt(i, true);
				continue;
			}
			++i;
		}
		// empfangene Schiffe wieder hinzufügen
		const int newsize = GetSize() + count;
		for (int i = GetSize(); i < newsize; ++i) {
			CShips* ship = new CShips();
			ship->Serialize(ar);
			const CShipMap::iterator j = Add(ship);
			assert(j->second->GetKO() == ptCurrentCombatSector);
		}
	}
	else
		assert(false);
}

//////////////////////////////////////////////////////////////////////
// special ships
//////////////////////////////////////////////////////////////////////
//current ship

void CShipMap::SetCurrentShip(unsigned key) {
	const CShipMap::iterator i = find(key);
	assert(i != end());
	m_CurrentShip = i;
}

void CShipMap::SetCurrentShip(const CShipMap::iterator& position) {
	assert(!empty() && position != end());
	m_CurrentShip = position;
}

const CShipMap::iterator& CShipMap::CurrentShip() const {
	assert(!empty() && m_CurrentShip != end());
	return m_CurrentShip;
}

//fleet ship

void CShipMap::SetFleetShip(unsigned key) {
	const CShipMap::iterator i = find(key);
	assert(i != end());
	m_FleetShip = i;
}

void CShipMap::SetFleetShip(const CShipMap::iterator& position) {
	assert(!empty() && position != end());
	m_FleetShip = position;
}

const CShipMap::iterator& CShipMap::FleetShip() const {
	assert(!empty() && m_FleetShip != end());
	return m_FleetShip;
}

//updating

void CShipMap::UpdateSpecialShip(CShipMap::iterator& ship, const CShipMap::const_iterator& to_erase) {
	if(ship != to_erase)
		return;
	if(ship != begin())
		--ship;
	else if (ship != end())
		++ship;
}

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////

//CString CShipMap::ToString() const {
//	CString s("SHIPARRAY BEGINN\n");
//	unsigned index = 0;
//	for(CShipMap::const_iterator i = begin(); i != end(); ++i)
//		s.Format("%s%u: %s at (%u,%u)\n", s, index, i->GetShipName(), i->GetKO().x, i->GetKO().y);
//	s.Format("%s%s", s, "SHIPARRAY END");
//	return s;
//}
