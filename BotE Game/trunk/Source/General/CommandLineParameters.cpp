#include "stdafx.h"
#include "CommandLineParameters.h"
#include "resources.h"

CCommandLineParameters::CCommandLineParameters(void) :
	m_nLogLevel(MT::LEVEL_INFO),
	m_sLogDir(""),
	m_LogDomains(),
	m_bActiveDomains(true),
	m_bSeeAllOfMap(false),
	m_nAutoTurns(-1),
	m_bTest(false)
{
	resources::pClp = this;
}

CCommandLineParameters::~CCommandLineParameters(void)
{
	resources::pClp = NULL;
}

CCommandLineParameters* CCommandLineParameters::GetInstance() {
	static CCommandLineParameters instance;
	return &instance;
}

static void ParseParamArgs(const std::string& args, std::set<const std::string>& result) {
	std::string::const_iterator start = args.begin();
	std::string::const_iterator end = args.begin();
	while(end != args.end()) {
		if(*end == ',') {
			result.insert(std::string(start, end));
			++end;
			start = end;
		}
		else
			++end;
	}
	result.insert(std::string(start, end));
}

void CCommandLineParameters::ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/)
{
	//For some reason, the first character in each pszParam seems to miss!
	const std::string parameter_with_args(pszParam);
	const unsigned equals = parameter_with_args.find("=");
	std::string parameter(parameter_with_args);
	std::string args;
	if(equals != std::string::npos) {
		parameter = parameter_with_args.substr(0, equals);
		args = parameter_with_args.substr(equals + 1);
	}
	if(strcmp(parameter.c_str(), "-log-dir") == 0) {
		int max_index = args.length() - 1;
		if(max_index >= 0 && args.substr(max_index) != "\\")
			args = args + "\\";
		m_sLogDir = args;
	}
	else if(strcmp(parameter.c_str(), "-log-level") == 0) {
		if(strcmp(args.c_str(), "error") == 0)
			m_nLogLevel = MT::LEVEL_ERROR;
		else if(strcmp(args.c_str(), "warning") == 0)
			m_nLogLevel = MT::LEVEL_WARNING;
		//default case
		//else if(strcmp(args.c_str(), "info") == 0)
		//	m_nLogLevel = MT::LEVEL_INFO;
		else if(strcmp(args.c_str(), "debug") == 0)
			m_nLogLevel = MT::LEVEL_DEBUG;
	}
	else if(strcmp(parameter.c_str(), "-log-domains") == 0) {
		if(!args.empty())
			ParseParamArgs(args, m_LogDomains);
	}
	else if(strcmp(parameter.c_str(), "-passive-domains") == 0) {
		if(strcmp(args.c_str(), "no") == 0 || strcmp(args.c_str(), "false") == 0)
			m_bActiveDomains = true;
		else
			m_bActiveDomains = false;
	}
	else if(strcmp(parameter.c_str(), "-see-all") == 0) {
		if(strcmp(args.c_str(), "no") == 0 || strcmp(args.c_str(), "false") == 0)
			m_bSeeAllOfMap = false;
		else
			m_bSeeAllOfMap = true;
	}
	else if(strcmp(parameter.c_str(), "-autoturn") == 0) {
		if (args.length() > 0)
			m_nAutoTurns = atoi(args.c_str());
	}
	else if(strcmp(parameter.c_str(), "-test") == 0) {
		m_bTest = true;
		if(args.empty()) {
			m_LogDomains.insert("shipmap");
			m_LogDomains.insert("genshipname");
		}
		else
			ParseParamArgs(args, m_LogDomains);
	}
}
