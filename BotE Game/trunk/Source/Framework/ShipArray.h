#pragma once

#include <vector>

class CShip;

class CShipArray
{
public:
	CShipArray(void);
	~CShipArray(void);

	typedef std::vector<CShip>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	typedef std::vector<CShip>::iterator iterator;
	iterator begin();
	iterator end();

	void Add(const CShip& ship, CShipArray::const_iterator& it);
	void Add(const CShip& ship);

	void RemoveAll();
	void RemoveAt(int index);

	CShip& GetAt(int index);
	const CShip& GetAt(int index) const;
	CShip& ElementAt(int index);
	const CShip& ElementAt(int index) const;
	CShip& operator[](int index);
	const CShip& operator[](int index) const;

	int GetSize() const;
	int GetUpperBound() const;

	void Serialize(CArchive& ar);
	void SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID);
	void SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector);


private:
	void SetSize(int size);

	std::vector<CShip> m_vShips;
};
