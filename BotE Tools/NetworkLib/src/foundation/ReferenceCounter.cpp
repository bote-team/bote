#include "StdAfx.h"
#include "ReferenceCounter.h"

CReferenceCounter::CReferenceCounter() : m_nCounter(0)
{
}

CReferenceCounter::~CReferenceCounter()
{
	ASSERT(m_nCounter == 0);
}

void CReferenceCounter::Pin()
{
	CSingleLock singleLock(&m_CriticalSection);
	singleLock.Lock();

	m_nCounter++;
}

void CReferenceCounter::Release()
{
	CSingleLock singleLock(&m_CriticalSection);
	singleLock.Lock();

	if (!(--m_nCounter))
	{
		singleLock.Unlock();
		delete this;
	}
}
