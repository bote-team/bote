#include "stdafx.h"
#include "Test.h"

#include "botf2Doc.h"
#include "Ships/Ships.h"
#include "Races/RaceController.h"

#include <cassert>

CTest::CTest(const CBotf2Doc& doc) :
	m_Doc(doc)
{
}

CTest::~CTest(void)
{
}

const CTest* const CTest::GetInstance(const CBotf2Doc& doc) {
	static CTest instance(doc);
	return &instance;
}

void CTest::Run() const {
	TestShipMap();
	TestGenShipname();
}

void CTest::TestGenShipname() const {
	CGenShipName gsn(m_Doc.m_GenShipName);

	//check uniqueness of generated shipnames
	const std::map<CString, CRace*>& races = *m_Doc.m_pRaceCtrl->GetRaces();
	CString race;
	for(int j = 0; j < 10; ++j)
	{
		const unsigned at = rand()%races.size();
		unsigned index = 0;
		for(std::map<CString, CRace*>::const_iterator i = races.begin(); i != races.end(); ++i)
		{
			if(at != index) {
				++index;
				continue;
			}
			race = i->first;
			break;
		}
		std::set<CString> seen;
		std::vector<CString> seen_v;
		for(unsigned i = 0; i < 10000; ++i)
		{
			const CString& name = gsn.GenerateShipName(race, false);
			if(seen.find(name) != seen.end()) {
				CString s;
				s.Format("repeated name: %s", name);
				MYTRACE("test_genshipname")(MT::LEVEL_ERROR, s);
				break;
			}
			seen.insert(name);
			seen_v.push_back(name);
		}
	}
}

void CTest::TestShipMap() const {

	CShipMap shipmap(m_Doc.m_ShipMap);

	//////////////////////////////////////////////////////////////////////
	// special ships
	//////////////////////////////////////////////////////////////////////
	//special ships should be at the beginning
	if(shipmap.CurrentShip() != shipmap.begin())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "CurrentShip not at begin()");
	if(shipmap.FleetShip() != shipmap.begin())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "FleetShip not at begin()");

	//////////////////////////////////////////////////////////////////////
	// iterator_at
	//////////////////////////////////////////////////////////////////////
	//directly after creation, keys are the same as indices
	//check iterator_at(index) against at(key)
	const CShipMap::const_iterator& ci = shipmap.iterator_at(0);
	if(shipmap.at(0).GetShipName() != ci->second.GetShipName())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "iterator at index 0 should point to ship with key 0");

	const CShipMap::const_iterator& ci2 = shipmap.iterator_at(m_Doc.m_ShipMap.GetUpperBound());
	if(shipmap.at(m_Doc.m_ShipMap.GetUpperBound()).GetShipName() != ci2->second.GetShipName())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "iterator at max index should point to ship with key max index");

	//check iterator_at(index) against index_of()
	//should allow getting an end() iterator
	for(CShipMap::const_iterator i = shipmap.begin();;) {
		if(i == shipmap.iterator_at(shipmap.index_of(i))) {
			if(i == shipmap.end())
				break;
			++i;
			continue;
		}
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "i != shipmap.iterator_at(shipmap.index_of(i)");
		if(i == shipmap.end())
			break;
		++i;
	}

	//////////////////////////////////////////////////////////////////////
	// empty shipmap
	//////////////////////////////////////////////////////////////////////

	shipmap = CShipMap();
	if(shipmap.end() != shipmap.begin())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "shipmap.end() != shipmap.begin()");

	const CShipMap::const_iterator& i4 = shipmap.iterator_at(0);
	if(i4 != shipmap.begin())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "i4 != shipmap.begin()");
	const CShipMap::const_iterator& i5 = shipmap.iterator_at(0);
	if(i5 != shipmap.end())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "i5 != shipmap.end()");

	//////////////////////////////////////////////////////////////////////
	// one element
	//////////////////////////////////////////////////////////////////////

	shipmap.Add(m_Doc.m_ShipInfoArray.GetAt(0));

	const CShips& ship = shipmap.GetAt(0);
	if(&ship != &shipmap.begin()->second)
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "&ship != &shipmap.begin()->second");
	const CShipMap::const_iterator& i6 = shipmap.iterator_at(1);
	if(i6 != shipmap.end())
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "i6 != shipmap.end()");
	if(&shipmap.CurrentShip()->second != &shipmap.begin()->second)
		MYTRACE("test_shipmap")(MT::LEVEL_ERROR, "&shipmap.CurrentShip()->second != &shipmap.begin()->second");
}
