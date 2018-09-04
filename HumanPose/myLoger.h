#pragma  once
//
class  LogerBase
{

public:
	LogerBase();
	~LogerBase();

public:

	void Log_Debug(const char* pszFormat, ...);

	void Log_Info(const char* pszFormat, ...);

	void Log_Error(const char* pszFormat, ...);
	
};