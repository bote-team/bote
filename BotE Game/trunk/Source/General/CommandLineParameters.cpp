#include "stdafx.h"
#include "CommandLineParameters.h"

CCommandLineParameters::CCommandLineParameters(void) :
	m_nLogLevel(MT::LEVEL_INFO),
	m_sLogDir("")
{
}

CCommandLineParameters::~CCommandLineParameters(void)
{
}

void CCommandLineParameters::ParseParam(const char* pszParam, BOOL /*bFlag*/, BOOL /*bLast*/)
{
	//For some reason, the first character in each pszParam seems to miss!
	const std::string parameter_with_args(pszParam);
	const unsigned equals = parameter_with_args.find("=");
	std::string parameter(parameter_with_args);
	std::string args("");
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
}
