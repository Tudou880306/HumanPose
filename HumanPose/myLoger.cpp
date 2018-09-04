#include "stdafx.h"
#include "myLoger.h"

#include <tchar.h>
#include <Shlwapi.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>

using namespace log4cplus;
using namespace log4cplus::helpers;


#pragma comment(lib,"shlwapi.lib")

#ifdef _DEBUG
#pragma comment(lib,"log4cplusUD.lib")
#else
#pragma comment(lib,"log4cplusU.lib")
#endif

static Logger g_loger;

LogerBase::LogerBase()
{
	if (!PathIsDirectory(_T("log")))
	{
		CreateDirectory(_T("log"), NULL);
	}
	PropertyConfigurator::doConfigure(_T("INS_log.cfg"));
	g_loger = Logger::getRoot();
#ifdef _DEBUG
	g_loger.setLogLevel(ALL_LOG_LEVEL);
#else
	g_loger.setLogLevel(ERROR_LOG_LEVEL);
#endif
}

LogerBase::~LogerBase()
{

}

void LogerBase::Log_Debug(const char* pszFormat, ...)
{
	char buf[1024] = { 0 };

	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);
	
	LOG4CPLUS_DEBUG(g_loger, buf);
}

void LogerBase::Log_Info(const char* pszFormat, ...)
{
	char buf[1024] = { 0 };

	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);

	LOG4CPLUS_INFO(g_loger, buf);
}

void LogerBase::Log_Error(const char* pszFormat, ...)
{
	char buf[1024] = { 0 };

	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);

	LOG4CPLUS_ERROR(g_loger, buf);
}