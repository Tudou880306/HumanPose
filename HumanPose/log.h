#pragma once

#ifndef _LOG_H_
#define _LOG_H_
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/initializer.h>
#include <string>

//#define DEBUG(input) DEBUG_LOG((input), (__FILE__), (__LINE__));
//#define ERROR(input) ERROR_LOG((input), (__FILE__), (__LINE__));
//#define WARN(input) WARN_LOG((input), (__FILE__), (__LINE__));



void DEBUG_LOG(const char* pszFormat, ...);
void ERROR_LOG(const char* pszFormat, ...);
void WARN_LOG(const char* pszFormat, ...);
class CLog
{
public:
	static CLog* Initialize(const std::string& strLogFile = "log4cplus.properties.in");
	
private:
	CLog(const std::string& strLogFile);
	static CLog* pInstance;
};

#endif

