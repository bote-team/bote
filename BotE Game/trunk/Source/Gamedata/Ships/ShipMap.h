#pragma once

#include <map>

class CShips;

class CShipMap
{
public:
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	CShipMap(void);
	virtual ~CShipMap(void);
	CShipMap(const CShipMap& o);
	CShipMap& operator=(const CShipMap& o);

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////

	typedef std::map<unsigned, CShips>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	typedef std::map<unsigned, CShips>::iterator iterator;
	iterator begin();
	iterator end();

	const_iterator find(unsigned key) const;
	iterator find(unsigned key);

	const_iterator iterator_at(int index) const;
	iterator iterator_at(int index);

//////////////////////////////////////////////////////////////////////
// adding elements
//////////////////////////////////////////////////////////////////////

	//adds the passed CShips at the end of this shiparray
	//@param ship: the ship to add
	iterator Add(const CShips& ship);
	//appends the passed CShipMap at the end of this shiparray
	//@param other: the CShipMap to appends
	void Append(const CShipMap& other);

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////

	void Reset();
	//removes the element pointed to by the passed iterator
	//@param index: will be updated and point to the new position of the element which followed the erased one
	void EraseAt(CShipMap::iterator& index);

//////////////////////////////////////////////////////////////////////
// getting elements
//////////////////////////////////////////////////////////////////////

	const CShips& GetAt(int index) const;
	CShips& GetAt(int index);

	const CShips& at(unsigned key) const;
	CShips& at(unsigned key);

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////

	int GetSize() const;
	int GetUpperBound() const;
	bool empty() const;
	int index_of(const CShipMap::const_iterator& position) const;

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

	void Serialize(CArchive& ar);
	//special purpose serialization function
	void SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID);
	//special purpose serialization function
	void SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector);

//////////////////////////////////////////////////////////////////////
// special ships
//////////////////////////////////////////////////////////////////////

	void SetCurrentShip(unsigned key);
	void SetCurrentShip(const CShipMap::iterator& position);
	const CShipMap::iterator& CurrentShip() const;

	void SetFleetShip(unsigned key);
	void SetFleetShip(const CShipMap::iterator& position);
	const CShipMap::iterator& FleetShip() const;

private:
	void UpdateSpecialShip(CShipMap::iterator& ship, const CShipMap::const_iterator& to_erase);

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////

	//CString ToString() const;

	unsigned NextKey();

	std::map<unsigned, CShips> m_Ships;
	unsigned m_NextKey;

	CShipMap::iterator m_CurrentShip;// Hilfsvariable, mit der auf ein spezielles Schiff im Array zugekriffen werden kann
	CShipMap::iterator m_FleetShip;// Das Schiff welches sozusagen die Flotte anführt

};
