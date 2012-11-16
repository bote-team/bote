#pragma once

#include <map>

class CShips;

class CShipArray
{
public:
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	CShipArray(void);
	virtual ~CShipArray(void);
	CShipArray(const CShipArray& o);
	CShipArray& operator=(const CShipArray& o);

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
	//appends the passed CShipArray at the end of this shiparray
	//@param other: the CShipArray to appends
	void Append(const CShipArray& other);

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////

	void Reset();
	//removes the element pointed to by the passed iterator
	//@param index: will be updated and point to the new position of the element which followed the erased one
	void RemoveAt(CShipArray::iterator& index);

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
	int index_of(const CShipArray::const_iterator& position) const;

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

	int CurrentShipsIndex() const;
	void SetCurrentShip(unsigned key);
	void SetCurrentShip(const CShipArray::iterator& position);
	const CShips& CurrentShip() const;
	CShips& CurrentShip();

	int FleetShipsIndex() const;
	void SetFleetShip(unsigned key);
	void SetFleetShip(const CShipArray::iterator& position);
	const CShips& FleetShip() const;
	CShips& FleetShip();

private:
	void UpdateSpecialShip(CShipArray::iterator& ship, CShipArray::iterator& to_erase);

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////

	//CString ToString() const;

	unsigned NextKey();

	std::map<unsigned, CShips> m_Ships;
	unsigned m_NextKey;

	CShipArray::iterator m_CurrentShip;
	CShipArray::iterator m_FleetShip;

};
