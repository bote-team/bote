#pragma once

class CBotEDoc;

class CTest
{
	private:
		CTest(void);
		CTest(const CBotEDoc& doc);
	public:
		~CTest(void);

	static const CTest* const GetInstance(const CBotEDoc& doc);
	void Run() const;


	void TestGenShipname() const;
	void TestShipMap() const;

	const CBotEDoc& m_Doc;
};
