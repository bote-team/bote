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
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::ALL_WORKER, 0));
	m_Workers.insert(std::pair<WORKER::Typ, int>(WORKER::FREE_WORKER, 10));
}

CWorker::~CWorker()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CWorker::CWorker(const CWorker &rhs) :
	m_Workers(rhs.m_Workers)
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
			AssertBotE(WORKER::FOOD_WORKER <= key && key <= WORKER::FREE_WORKER);
			m_Workers.insert(std::pair<WORKER::Typ, int>(static_cast<WORKER::Typ>(key), value));
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
int CWorker::GetWorker(WORKER::Typ nWorker) const
{
	const_iterator result = m_Workers.find(nWorker);
	AssertBotE(result != m_Workers.end());
	return result->second;
}

void CWorker::SetWorker(WORKER::Typ nWorker, int Value)
{
	AssertBotE(0 <= Value);
	iterator worker = m_Workers.find(nWorker);
	AssertBotE(worker != m_Workers.end());
	worker->second = Value;
}

void CWorker::InkrementWorker(WORKER::Typ nWorker)
{
	iterator worker = m_Workers.find(nWorker);
	AssertBotE(worker != m_Workers.end());
	worker->second++;
}

void CWorker::DekrementWorker(WORKER::Typ nWorker)
{
	iterator worker = m_Workers.find(nWorker);
	AssertBotE(worker != m_Workers.end());
	AssertBotE(worker->second > 0);
	worker->second--;
}

CWorker::EmployedFreeAll CWorker::Workers() const
{
	int all_workers = 0;
	int workers_sum = 0;
	for(const_iterator it = m_Workers.begin(); it != m_Workers.end(); ++it)
	{
		if(it->first == WORKER::ALL_WORKER)
		{
			all_workers = it->second;
			continue;
		}
		else if(it->first == WORKER::FREE_WORKER)
			continue;
		workers_sum += it->second;
	}
	const int free_workers = all_workers - workers_sum;
	return EmployedFreeAll(workers_sum, free_workers, all_workers);
}

CWorker::EmployedFreeAll CWorker::CalculateFreeWorkers()
{
	const EmployedFreeAll& efa = Workers();
	AssertBotE(efa.free >= 0);
	m_Workers[WORKER::FREE_WORKER] = efa.free;
	return efa;
}

// Checked, ob wir zuviele Arbeiter zugewiesen haben, obwohl wir gar nicht so viele haben
// z.B. durch Nahrungsmangel kann so was passieren
void CWorker::CheckWorkers()
{
	const EmployedFreeAll& workers = Workers();
	if(workers.all >= workers.employed)
		return;
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
}

void CWorker::FreeAll()
{
	for(iterator it = m_Workers.begin(); it != m_Workers.end(); ++it)
		it->second = 0;

	EmployedFreeAll& efa = CalculateFreeWorkers();
	AssertBotE(efa.all == efa.free);
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
