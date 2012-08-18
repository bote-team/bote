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
	std::vector<const std::string> m_vLogDomains;
	bool m_bActiveDomains;//should the m_vLogDomains be muted (false) or be the only ones that speak (true) ?

	void ParseLogDomainParamArgs(const std::string& args);

	CCommandLineParameters(void);
public:
	static CCommandLineParameters* GetInstance();
	~CCommandLineParameters(void);

	const MT::Level LogLevel() const {
		return m_nLogLevel;
	}
	const std::string LogDir() const {
		return m_sLogDir;
	}
	const std::vector<const std::string> LogDomains() const {
		return m_vLogDomains;
	}
	const bool ActiveDomains() const {
		return m_bActiveDomains;
	}

	void ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/);
};
