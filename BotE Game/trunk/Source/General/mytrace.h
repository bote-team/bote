////////////////////////////////////////////////
// MyTrace is small and simple TRACE utility. //
// Based on ATLTRACE                          //
// -=MaGGuS=- (maggus@mail.ru)   2006         //
////////////////////////////////////////////////

#ifndef __MYTRACE_H__
#define __MYTRACE_H__

#pragma once

#include <vector>
#include <string>
#include <algorithm>

/////// MT defines start ///////

#ifdef _DEBUG
#define MT_USE_ATLTRACE				// forward trace messages to ATLTRACE
#endif

//#define MT_KEEP_ATLTRACE_LEVELS	// convert MYTRACE levels to ATLTRACE levels, requres DO_ATLTRACE
//#define MT_REDIRECT_STDOUT		// redirect all output to stdout into log file. Use this carefully!

/////// MT defines end ///////

#ifdef MT_USE_ATLTRACE	// for ATLTRACE support
#include <atldef.h>
#include <atlconv.h>
#endif // MT_USE_ATLTRACE

namespace MT
{
	enum Level {LEVEL_ERROR, LEVEL_WARNING, LEVEL_INFO, LEVEL_DEBUG};		// MYTRACE message severity levels

//#ifndef _DEBUG	/*empty macroses in release mode*/
/*
#define MYTRACE_INIT(x)			__noop
#define MYTRACE_INIT_EX(x, y)	__noop
#define MYTRACE_LEVEL(x)		__noop
#define MYTRACE_DEINIT			__noop
#define MYTRACE					__noop
*/
//#else	// _DEBUG

	class CMyTrace;	// forward declaration

#define MYTRACE_INIT(x)			MT::CMyTrace::Init(x);
//#define MYTRACE_INIT_EX(x, y)	MT::CMyTrace::InitEx((x), (y));
#define MYTRACE_LEVEL(x)		MT::CMyTrace::SetLevel(x);
#define MYTRACE_DEINIT			MT::CMyTrace::Deinit();
#define MYTRACE					MT::CMyTrace(__FILE__, __LINE__)
#define MYTRACE_DOMAIN(domain)	MT::CMyTrace(__FILE__, __LINE__, domain)

	/* MyTrace class */
	class CMyTrace
	{
	public:
		CMyTrace(const char *pszFileName, int nLineNo, const std::string& domain = "general")
			: m_pszFileName(pszFileName), m_nLineNo(nLineNo),
			m_callTime(time(NULL)), m_sDomain(domain)
		{
		}

		/* Add trace messge to log with desired severity level */
		void __cdecl operator()(UINT nLevel, LPCTSTR pszFmt, ...) const
		{
			if (!IsLoggingEnabledFor(m_sDomain))
				return;
			va_list ptr; va_start(ptr, pszFmt);
			if((nLevel <= (UINT)traceLevel))
			{
				CString strFormat(pszFmt);
				decorateMessage(strFormat, nLevel);
				if(pLOG_FILE != NULL)
					printMessageV(pLOG_FILE, strFormat, ptr);
				if(pLOG_FILE_ERRORS != NULL && (nLevel == MT::LEVEL_ERROR || nLevel == MT::LEVEL_WARNING) )
					printMessageV(pLOG_FILE_ERRORS, strFormat, ptr);
#ifdef MT_USE_ATLTRACE
				ATL::CTrace::s_trace.TraceV(m_pszFileName, m_nLineNo, atlTraceGeneral,
#ifdef MT_KEEP_ATLTRACE_LEVELS
					nLevel
#else	// MT_KEEP_ATLTRACE_LEVELS
					0,
#endif	// MT_KEEP_ATLTRACE_LEVELS
					strFormat, ptr);
#endif	// MT_USE_ATLTRACE
			}
			va_end(ptr);
		};

	private:
		/* Add trace messge to log with default INFO severity level */
		void __cdecl operator()( LPCTSTR pszFmt, ...) const
		{
			if (!IsLoggingEnabledFor(m_sDomain))
				return;
			va_list ptr; va_start(ptr, pszFmt);
			CString strFormat(pszFmt);
			decorateMessage(strFormat);
			if(pLOG_FILE != NULL)
				printMessageV(pLOG_FILE, strFormat, ptr);
#ifdef MT_USE_ATLTRACE
			ATL::CTrace::s_trace.TraceV(m_pszFileName, m_nLineNo, atlTraceGeneral, 0, strFormat, ptr);
#endif	// MT_USE_ATLTRACE
			va_end(ptr);
		};
	public:

		/* Open main log file */
		static bool Init(LPCTSTR fName,
			const std::vector<const std::string>& domains = std::vector<const std::string>(),
			const bool active_domains = false)
		{
			m_vDomains = domains;
			m_bActiveDomains = active_domains;
			bool bRes1= false;
			if( (pLOG_FILE = fopen( fName, "w" )) == NULL )
			{
				::MessageBox(NULL, "Failed to open LOG file", "Notice", 0x10000);
				MYTRACE_DOMAIN("logging")(_T("\t\t\t FAILED TO OPEN LOG FILE: %s"), fName);
				bRes1 = false;
			}
			else
			{
#ifdef MT_REDIRECT_STDOUT
				// redirect stdout to enable simple "printf" output to file
				*stdout = *pLOG_FILE;
				setvbuf(stdout,NULL,_IONBF,0);	// turn off cashing
				//sync_with_stdio();	// check
#endif	// MT_REDIRECT_STDOUT
				MYTRACE_DOMAIN("logging")(_T("\t\t*** Log Started at %s ***\n\n"), CTime(time(NULL)).Format("%c"));
				bRes1 = true;
			}
			return bRes1;
		};

		/* Open main log file and extra log file for errors report only */
		//static bool InitEx(LPCTSTR fName, LPCTSTR fNameEx)
		//{
		//	bool bRes1 = Init(fName), bRes2 = false;
		//	// open extra log file for errors only
		//	if( (pLOG_FILE_ERRORS = fopen( fNameEx, "w" )) == NULL )
		//	{
		//		//::MessageBox(NULL, "Failed to open Extra LOG file", "Notice", 0x10000);
		//		MYTRACE(_T("\t\t\t FAILED TO OPEN EXTRA LOG FILE: %s"), fNameEx);
		//		bRes2 = false;
		//	}
		//	else
		//	{
		//		MYTRACE(_T("\t\t Extra Log file opened"));
		//		bRes2 = true;
		//	}
		//	return (bRes1 && bRes2);
		//};

		/* Close all log files */
		static void Deinit()
		{
			if(pLOG_FILE_ERRORS != NULL)	// close extra log file
			{
				MYTRACE_DOMAIN("logging")(_T("\t\t Extra Log file closed"));
				fflush(pLOG_FILE_ERRORS);
				fclose(pLOG_FILE_ERRORS);
				pLOG_FILE_ERRORS = NULL;
			}
			if(pLOG_FILE != NULL)	// close main log file
			{
				MYTRACE_DOMAIN("logging")(_T("\n\t\t*** Log Finished at %s ***"), CTime(time(NULL)).Format("%c"));
				fflush(pLOG_FILE);
				fclose(pLOG_FILE);
				pLOG_FILE = NULL;
			}
		};

		static void SetLevel(Level level)
		{
			traceLevel = level;
		};

		static void SetDomains(const std::vector<const std::string>& domains)
		{
			m_vDomains = domains;
		}

		static void SetActiveDomains(const bool active)
		{
			m_bActiveDomains = active;
		}

	private:
		/* Change message format into desired form */
		void __cdecl decorateMessage(CString &strFmt, int nLevel = LEVEL_INFO) const
		{
			CString strLevel;
			switch(nLevel)
			{
			case LEVEL_ERROR:
				strLevel = _T("ERROR");
				break;
			case LEVEL_WARNING:
				strLevel = _T("WARNING");
				break;
			case LEVEL_INFO:
				strLevel = _T("INFO");
				break;
			case LEVEL_DEBUG:
				strLevel = _T("DEBUG");
				break;
			default:
				strLevel = _T("UNKNOWN");
				break;
			}
			CString decMsg;
			//decMsg.Format("%s\t%s(%d)\t%s:\n%s%s", strLevel, m_pszFileName, m_nLineNo, m_callTime.Format("%c"),
			//	strFmt, strFmt.Right(1) != _T("\n") ? _T("\n") : "" );	// add line end if requred
			decMsg.Format("%s %s\t%s%s", strLevel, CString(m_sDomain.c_str()), strFmt, strFmt.Right(1) != _T("\n") ? _T("\n") : "" );	// add line end if requred
			strFmt = decMsg;
		};

		/* Printout message into file */
		void __cdecl printMessageV(FILE *pFile,  CString &strFmt, va_list args) const
		{
			if(!pFile) return;
			CString strMsg;
			strMsg.FormatV(strFmt, args);
			if(_ftprintf(pFile, strMsg) < 0)
			{
				// No luck with writing into log file?
				// DO NOT call MYTRACE from here to report that error, that can cause an infinite loop!
			}
			else
				fflush(pLOG_FILE);
		}
	public:
		static bool __cdecl IsLoggingEnabledFor(const std::string& domain) {
			if(m_vDomains.empty())
				return !m_bActiveDomains;
			const bool found = std::find(m_vDomains.begin(), m_vDomains.end(), domain) != m_vDomains.end();
			if(m_bActiveDomains)
				return found;
			return !found;
		}
	private:

		const char *const m_pszFileName;	// curent source file name
		const int m_nLineNo;		// curent source line number
		const CTime m_callTime;	// time of call
		const std::string m_sDomain;
		static FILE *pLOG_FILE;	// trace log file
		static FILE *pLOG_FILE_ERRORS;	// trace errors file
		static Level traceLevel;	// minimum severity level to report

		static std::vector<const std::string> m_vDomains;
		static bool m_bActiveDomains;
	};

	__declspec( selectany ) FILE *CMyTrace::pLOG_FILE = NULL;
	__declspec( selectany ) FILE *CMyTrace::pLOG_FILE_ERRORS = NULL;
	__declspec( selectany ) Level CMyTrace::traceLevel = LEVEL_INFO;
	__declspec( selectany ) std::vector<const std::string> CMyTrace::m_vDomains;
	__declspec( selectany ) bool CMyTrace::m_bActiveDomains = false;


//#endif	// _DEBUG

};	// namespace MT

#endif  // __MYTRACE_H__
