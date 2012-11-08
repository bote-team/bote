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
	std::set<const std::string> m_LogDomains;//only allowed (or only muted) log domains
	//passed in the cmd line such as "graphicload"
	bool m_bActiveDomains;//should the m_LogDomains be exactly the ones which are muted (false)
	//or be the only ones that speak (true) ?
	bool m_bSeeAllOfMap; //true: komplette Map und alle Majorrassen sind sichtbar
	int m_nAutoTurns; // number of turns automaticly terminated, -1 if option is disabled

	void ParseLogDomainParamArgs(const std::string& args);

	CCommandLineParameters(void);
	CCommandLineParameters(const CCommandLineParameters& o);
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
	const std::set<const std::string> LogDomains() const {
		return m_LogDomains;
	}
	const bool ActiveDomains() const {
		return m_bActiveDomains;
	}
	const bool SeeAllOfMap() const {
		return m_bSeeAllOfMap;
	}
	const int GetAutoTurns() const {
		return m_nAutoTurns;
	}

	//called for each parameter passed in the comand line
	//supposed style is ...//BotE.exe --param-1=arg1,arg2 --param-2=arg usw
	void ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/);
};
