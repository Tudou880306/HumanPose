#include "stdafx.h"
#include "myLoger.h"





#pragma comment(lib,"shlwapi.lib")

//#ifdef _DEBUG
//#pragma comment(lib,"log4cplusUD.lib")
//#else
//#pragma comment(lib,"log4cplusU.lib")
//#endif
#define loglog 1
 
Logger g_loger,root;
LogerBase::LogerBase()
{
	

	/*
	log4cplus::Initializer initializer;
	LogLog::getLogLog()->setInternalDebugging(true);
	Logger root = Logger::getRoot();
	log4cplus::helpers::FileInfo fi;
	log4cplus::tstring file = LOG4CPLUS_C_STR_TO_TSTRING("log4cplus.properties.in");
	if (getFileInfo(&fi, file) != 0)
	{
		return 0;
	}
	PropertyConfigurator::doConfigure(LOG4CPLUS_C_STR_TO_TSTRING("log4cplus.properties.in"));
	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("log"));
	LOG4CPLUS_TRACE_METHOD(logger, LOG4CPLUS_TEXT("::printDebug()"));
	LOG4CPLUS_DEBUG(logger, "This is a DEBUG message");
*/
	if (!PathIsDirectory(_T("log")))
	{
		CreateDirectory(_T("log"), NULL);
	}
	log4cplus::Initializer initializer;
	g_loger =  Logger::getInstance(LOG4CPLUS_C_STR_TO_TSTRING("log"));
	root = Logger::getRoot();
	LogLog::getLogLog()->setInternalDebugging(true);
	log4cplus::tstring file = LOG4CPLUS_C_STR_TO_TSTRING("log4cplus.properties.in");
	log4cplus::helpers::FileInfo fi;
	if (getFileInfo(&fi, file) == 0)
	{
		//return ;
	}
	log4cplus::tstring temp = LOG4CPLUS_C_STR_TO_TSTRING("log4cplus.properties.in");
	PropertyConfigurator::doConfigure(temp);
	//g_loger = Logger::getRoot();
#ifdef _DEBUG
	g_loger.setLogLevel(ALL_LOG_LEVEL);
#else
	g_loger.setLogLevel(ALL_LOG_LEVEL);
#endif
	LOG4CPLUS_DEBUG(g_loger, "Log Start ....");

#if loglog
//	log4cplus::Initializer initializer;
#endif
}

LogerBase::~LogerBase()
{

}

void LogerBase::Log_Debug(const char* pszFormat, ...)
{
	//log4cplus::tstring file = LOG4CPLUS_C_STR_TO_TSTRING("log4cplus.properties.in");
	//log4cplus::helpers::FileInfo fi;
	//if (getFileInfo(&fi, file) == 0)
	//{
	//	//return ;
	//}
	//PropertyConfigurator::doConfigure(file);
	g_loger = Logger::getInstance(LOG4CPLUS_C_STR_TO_TSTRING("log"));
	root = Logger::getRoot();
	LOG4CPLUS_DEBUG(g_loger, "Log Start test....");
	LOG4CPLUS_DEBUG(root, "root Start test....");
	char buf[1024] = { 0 };

	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf_s(buf, sizeof(buf), pszFormat, arglist);
	va_end(arglist);
	
	LOG4CPLUS_DEBUG(g_loger, "Log Start test....");

#if loglog
//	LogLog::getLogLog()->debug(LOG4CPLUS_TEXT( buf ));
#endif
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
void LogerBase::Log_setQuietMode(bool b)
{
	
}