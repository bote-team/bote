#pragma once

#include <vector>

class CShips;

class CShipArray
{
public:
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
	CShipArray(void);
	virtual ~CShipArray(void);

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////
	typedef std::vector<CShips>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	typedef std::vector<CShips>::iterator iterator;
	iterator begin();
	iterator end();

	const_iterator find(int index) const;
	iterator find(int index);

//////////////////////////////////////////////////////////////////////
// adding elements
//////////////////////////////////////////////////////////////////////

	//adds the passed CShips at the end of this shiparray
	//@param it: will be updated and point to the same position as before
	//@param ship: the ship to add
	void Add(CShipArray::iterator& it, const CShips& ship);
	//appends the passed CShipArray at the end of this shiparray
	//@param it: will be updated and point to the same position as before
	//@param other: the CShipArray to appends
	void Append(CShipArray::iterator& it, const CShipArray& other);

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////
	void RemoveAll();
	//removes the element pointed to by the passed iterator
	//@param index: will be updated and point to the new position of the element which followed the erased one
	void RemoveAt(CShipArray::iterator& index);
//////////////////////////////////////////////////////////////////////
// getting elements
//////////////////////////////////////////////////////////////////////
	CShips& GetAt(int index);
	const CShips& GetAt(int index) const;
	CShips& ElementAt(int index);
	const CShips& ElementAt(int index) const;
	CShips& operator[](int index);
	const CShips& operator[](int index) const;

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////
	int GetSize() const;
	int GetUpperBound() const;
	bool empty() const;

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////
	void Serialize(CArchive& ar);
	//special purpose serialization function
	void SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID);
	//special purpose serialization function
	void SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector);

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////
	CString ToString() const;

private:
	void SetSize(int size);

	std::vector<CShips> m_vShips;
};
