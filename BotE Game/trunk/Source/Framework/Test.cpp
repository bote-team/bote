#include "stdafx.h"
#include "Test.h"

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


}
