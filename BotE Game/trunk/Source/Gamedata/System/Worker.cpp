// Worker.cpp: Implementierung der Klasse CWorker.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "Worker.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CWorker::CWorker()
{
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::FOOD_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::INDUSTRY_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::ENERGY_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::SECURITY_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::RESEARCH_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::TITAN_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::DEUTERIUM_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::DURANIUM_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::CRYSTAL_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::IRIDIUM_WORKER, 0));
	m_AllWorkers = 0;
	m_FreeWorkers = 0;
}

CWorker::~CWorker()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CWorker::CWorker(const CWorker &rhs) :
	m_Workers(rhs.m_Workers),
	m_AllWorkers(rhs.m_AllWorkers),
	m_FreeWorkers(rhs.m_FreeWorkers)
{
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CWorker & CWorker::operator=(const CWorker & rhs)
{
	if (this == &rhs)
		return *this;
	m_Workers = rhs.m_Workers;
	m_AllWorkers = rhs.m_AllWorkers,
	m_FreeWorkers = rhs.m_FreeWorkers;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktion
//////////////////////////////////////////////////////////////////////
void CWorker::Serialize(CArchive &ar)
{
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_Workers.size();
		for(const_iterator it = m_Workers.begin(); it != m_Workers.end(); ++it)
		{
			ar << static_cast<int>(it->first);
			ar << it->second;
		}
		ar << m_AllWorkers;
		ar << m_FreeWorkers;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		m_Workers.clear();
		unsigned size;
		ar >> size;
		for(unsigned i = 0; i < size; ++i)
		{
			int key;
			ar >> key;
			int value;
			ar >> value;
			AssertBotE(WORKER::FOOD_WORKER <= key && key <= WORKER::IRIDIUM_WORKER);
			m_Workers.insert(std::pair<WORKER::Typ, int>(static_cast<WORKER::Typ>(key), value));
		}
		ar >> m_AllWorkers;
		ar >> m_FreeWorkers;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
int CWorker::GetWorker(WORKER::Typ nWorker) const
{
	if(nWorker == WORKER::ALL_WORKER)
		return m_AllWorkers;
	else if (nWorker == WORKER::FREE_WORKER)
		return m_FreeWorkers;
	const_iterator result = m_Workers.find(nWorker);
	AssertBotE(result != m_Workers.end());
	return result->second;
}

void CWorker::SetWorker(WORKER::Typ nWorker, int Value)
{
	AssertBotE(nWorker != WORKER::FREE_WORKER);
	if(nWorker == WORKER::ALL_WORKER)
	{
		m_AllWorkers = Value;
		CheckWorkers();
	}
	else
	{
		iterator worker = m_Workers.find(nWorker);
		AssertBotE(worker != m_Workers.end());
		m_FreeWorkers += worker->second - Value;
		worker->second = Value;
	}
	AssertBotE(0 <= m_FreeWorkers && m_FreeWorkers <= m_AllWorkers);
}

void CWorker::InkrementWorker(WORKER::Typ nWorker)
{
	AssertBotE(nWorker != WORKER::ALL_WORKER && nWorker != WORKER::FREE_WORKER);
	iterator worker = m_Workers.find(nWorker);
	AssertBotE(worker != m_Workers.end());
	m_FreeWorkers--;
	AssertBotE(0 <= m_FreeWorkers);
	worker->second++;
}

void CWorker::DekrementWorker(WORKER::Typ nWorker)
{
	AssertBotE(nWorker != WORKER::ALL_WORKER && nWorker != WORKER::FREE_WORKER);
	iterator worker = m_Workers.find(nWorker);
	AssertBotE(worker != m_Workers.end());
	m_FreeWorkers++;
	AssertBotE(m_FreeWorkers <= m_AllWorkers);
	worker->second--;
}

CWorker::EmployedFreeAll CWorker::Workers() const
{
	int workers_sum = 0;
	for(const_iterator it = m_Workers.begin(); it != m_Workers.end(); ++it)
		workers_sum += it->second;
	const int free_workers = m_AllWorkers - workers_sum;
	return EmployedFreeAll(workers_sum, free_workers, m_AllWorkers);
}

// Checked, ob wir zuviele Arbeiter zugewiesen haben, obwohl wir gar nicht so viele haben
// z.B. durch Nahrungsmangel kann so was passieren
void CWorker::CheckWorkers()
{
	EmployedFreeAll& workers = Workers();
	if(workers.all < workers.employed)
	{
		int diff = workers.employed - workers.all;	// Differenz der zuvielen Arbeiter
		while (diff > 0)
		{
			WORKER::Typ order[WORKER::IRIDIUM_WORKER + 1] = {
				WORKER::RESEARCH_WORKER,
				WORKER::SECURITY_WORKER,
				WORKER::INDUSTRY_WORKER,
				WORKER::TITAN_WORKER,
				WORKER::DEUTERIUM_WORKER,
				WORKER::DURANIUM_WORKER,
				WORKER::CRYSTAL_WORKER,
				WORKER::IRIDIUM_WORKER,
				WORKER::ENERGY_WORKER,
				WORKER::FOOD_WORKER };

			for(int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; ++i)
			{
				iterator current = m_Workers.find(order[i]);
				if(current->second == 0)
					continue;
				current->second--;
				diff--;
				if(diff == 0)
					return;
			}
		}
		workers = Workers();
	}
	AssertBotE(workers.free >= 0);
	m_FreeWorkers = workers.free;
}

void CWorker::FreeAll()
{
	for(iterator it = m_Workers.begin(); it != m_Workers.end(); ++it)
		it->second = 0;

	CheckWorkers();
}

int CWorker::Cap(WORKER::Typ type, int number)
{
	const int is = GetWorker(type);
	if(number < is)
	{
		SetWorker(type, number);
		return number;
	}
	return is;
}
