// HumanPose.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "HumanPose.h"
#include "myLoger.h"
#include "log.h"
//#include <log4cplus/helpers/loglog.h>
//#include <log4cplus/logger.h>
//#include <log4cplus/initializer.h>
//using namespace log4cplus::helpers;
//LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
//{
//
//	return	EXCEPTION_EXECUTE_HANDLER;
//}

HUMANPOSE_API ExtractFeature* HPLib_Init()
{
	CLog::Initialize();
	DEBUG_LOG("HPLib_Init");
	ExtractFeature *fhandl = new ExtractFeature();
	if (0==fhandl->LoadXml("PoseConfig.xml")&&0== fhandl->InitHandle())
	{
		return  fhandl;
	}
	else return nullptr;
}
HUMANPOSE_API int HPLib_StartProcess(ExtractFeature *fhandl, cv::Mat img, std::map<int, std::vector<int>>&result)
{
	fhandl->ProcessImage(img, result);
	return 0;
}
//HUMANPOSE_API int HPLib_StartProcess(ExtractFeature *fhandl,std::string path, std::map<int, std::vector<int>>&result)
//{
//	cv::Mat img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
//	 fhandl->ProcessImage(img, result);
//	 return 0;
//}

HUMANPOSE_API int HPLib_Release(ExtractFeature *fhandl)
{
	fhandl->ReleaseHandle();
	delete fhandl;
	return 0;
}
