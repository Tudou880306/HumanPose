#include <sstream>
#include <memory>

#include "stdafx.h"
#include "log.h"
#include <tchar.h>
#include <Shlwapi.h>


using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;
#pragma comment(lib,"shlwapi.lib")

static Logger global_pLogger;

CLog* CLog::pInstance = NULL;

CLog* CLog::Initialize(const string& strLogFile)
{
	if (NULL == pInstance)
	{
		pInstance = new CLog(strLogFile);
	}
	return pInstance;
}

CLog::CLog(const string& strLogFile)
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_C_STR_TO_TSTRING(strLogFile));
	LogLog::getLogLog()->setInternalDebugging(true);
	global_pLogger = Logger::getRoot();
	global_pLogger.setLogLevel(ALL_LOG_LEVEL);
}


void DEBUG_LOG(const char* pszFormat, ...)
{
	/*stringstream ssLogData;
	ssLogData << "[" << strFile << ":" << iLine << "] " << strInfo;*/
	char buf[1024] = { 0 };
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);

	LOG4CPLUS_DEBUG(global_pLogger, buf);
}

void ERROR_LOG(const char* pszFormat, ...)
{
	char buf[1024] = { 0 };
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);
	LOG4CPLUS_ERROR(global_pLogger, buf);
}

void WARN_LOG(const char* pszFormat, ...)
{
	char buf[1024] = { 0 };
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);
	LOG4CPLUS_WARN(global_pLogger, buf);
}
