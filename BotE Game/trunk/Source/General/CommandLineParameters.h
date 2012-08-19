#pragma once
#include "afxwin.h"
#include "mytrace.h"

#include <string>

class CCommandLineParameters :
	public CCommandLineInfo
{
private:
	MT::Level m_nLogLevel;//error, warning, info or debug passed in cmd line
	std::string m_sLogDir;//directory with BotE.log passed in the cmd line
	std::vector<const std::string> m_vLogDomains;//only allowed (or only muted) log domains
	//passed in the cmd line such as "graphicload"
	bool m_bActiveDomains;//should the m_vLogDomains be exactly the ones which are muted (false)
	//or be the only ones that speak (true) ?

	void ParseLogDomainParamArgs(const std::string& args);

	CCommandLineParameters(void);
public:
	//there is only a single instance of this class supposed to be created
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

	//called for each parameter passed in the comand line
	//supposed style is ...//BotE.exe --param-1=arg1,arg2 --param-2=arg usw
	void ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/);
};
