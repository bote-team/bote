#pragma once

class CBotf2Doc;

class CTest
{
	private:
		CTest(void);
		CTest(const CBotf2Doc& doc);
	public:
		~CTest(void);

	static const CTest* const GetInstance(const CBotf2Doc& doc);
	void Run() const;


	void TestGenShipname() const;
	void TestShipMap() const;

	const CBotf2Doc& m_Doc;
};
