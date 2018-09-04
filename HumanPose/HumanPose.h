#pragma once

#include "extract_from_image.h"

#ifndef HUMANPOSE_H_
#define HUMANPOSE_H_


#ifdef HUMANPOSE_EXPORTS
#define HUMANPOSE_API extern "C" _declspec(dllexport) 
#else
#define HUMANPOSE_API extern "C" _declspec(dllimport) 
#endif


/****************************************!
2 *@brief  
3 *@author mayn
4 *@date   2018Äê5ÔÂ26ÈÕ
5 *@param[out] 
6 *@param[in]    
7 *@return       
8 ****************************************/
HUMANPOSE_API ExtractFeature* HPLib_Init();
//HUMANPOSE_API int HPLib_StartProcess(ExtractFeature *fhandl, std::string path, std::map<int, std::vector<int>>&result);
HUMANPOSE_API int HPLib_StartProcess(ExtractFeature *fhandl, cv::Mat img, std::map<int, std::vector<int>>&result);
HUMANPOSE_API int HPLib_Release(ExtractFeature *fhandl);
#endif
