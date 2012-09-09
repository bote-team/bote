#include "StdAfx.h"
#include "Thread.h"

IMPLEMENT_DYNAMIC(CThread, CObject)

CThread::CThread(BOOL bAutoDelete) : m_bInterrupted(false), m_nExitCode(0), m_bStarted(false),
	m_bAutoDelete(bAutoDelete)
{
	m_pWinThread = AfxBeginThread(&ThreadProc, (LPVOID)this, 0, 0, CREATE_SUSPENDED, NULL);
	ASSERT(m_pWinThread);
	m_pWinThread->m_bAutoDelete = TRUE;

	m_hThreadStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(m_hThreadStoppedEvent);
}

CThread::~CThread()
{
}

BOOL CThread::Start()
{
	if (m_bStarted || !m_pWinThread) return FALSE;

	m_bStarted = true;
	DWORD dwSuspendCount = m_pWinThread->ResumeThread();
	ASSERT(dwSuspendCount == 1);
	return (dwSuspendCount != (DWORD)-1);
}

BOOL CThread::InitInstance()
{
	return TRUE;
}

UINT CThread::ExitInstance()
{
	return GetExitCode();
}

UINT CThread::ThreadProc(LPVOID pObject)
{
	ASSERT(pObject);
	ASSERT_KINDOF(CThread, (CObject *)pObject);

	CThread *pThread = (CThread *)pObject;

	// initialisieren, ausführen, aufräumen
	if (pThread->InitInstance())
		pThread->Run();	
	UINT nResult = pThread->ExitInstance();

	// den tatsächlich gelieferten Exit-Code setzen
	pThread->SetExitCode(nResult);

	pThread->m_pWinThread = NULL; // zerstört sich selbst
	HANDLE hThreadStoppedEvent = pThread->m_hThreadStoppedEvent;

	// Thread-Objekt ggf. automatisch zerstören
	if (pThread->m_bAutoDelete) delete pThread;

	// Beendigungs-Event setzen; setzt alle Threads fort, die in Join() warten
	VERIFY(SetEvent(hThreadStoppedEvent));

	return nResult;
}

BOOL CThread::Join(DWORD dwTimeout)
{
	return (WaitForSingleObject(m_hThreadStoppedEvent, dwTimeout) == WAIT_OBJECT_0);
}
