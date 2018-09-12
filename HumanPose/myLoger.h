#pragma  once
//
#include <tchar.h>
#include <Shlwapi.h>
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

using namespace log4cplus;
using namespace log4cplus::helpers;
class  LogerBase
{

public:
	LogerBase();
	~LogerBase();
private:

public:

	void Log_Debug(const char* pszFormat, ...);

	void Log_Info(const char* pszFormat, ...);

	void Log_Error(const char* pszFormat, ...);
	void Log_setQuietMode(bool b);
};