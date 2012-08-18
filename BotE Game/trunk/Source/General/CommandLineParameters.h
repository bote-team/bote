#pragma once
#include "afxwin.h"
#include "mytrace.h"

#include <string>

class CCommandLineParameters :
	public CCommandLineInfo
{
private:
	MT::Level m_nLogLevel;
	std::string m_sLogDir;

public:
	CCommandLineParameters(void);
	~CCommandLineParameters(void);

	MT::Level LogLevel() const {
		return m_nLogLevel;
	}
	std::string LogDir() const {
		return m_sLogDir;
	}

	void ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/);
};
