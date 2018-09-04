#pragma once
#include "stdafx.h"
#include <array>
#include <vector>
#include<cmath>
#include "stdio.h"
#include<iostream>
// OpenPose dependencies
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include<opencv2/highgui.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/utilities/headers.hpp>

enum HumPoseEnum
{
	Normal,
	Standing,
	Sitting,
	Moving,
	Playing,
};


class ExtractFeature
{
public:
	ExtractFeature();
	~ExtractFeature();

private:
	op::Point<int>m_outputSize;
	op::Point<int>m_netInputSize;
	op::PoseModel m_poseModel;

	IplImage  *oldimg;//判断是否睡觉 ; 输入的第一帧
	op::Array<float> olddata;
	op::PoseExtractorCaffe * poseExtractorCaffe;
	op::PoseCpuRenderer* poseRenderer;

public:

public:
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年5月28日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int LoadXml(const char* xml);
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年5月26日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int InitHandle();
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年5月26日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int ProcessImage(cv::Mat inputImage, std::map<int, std::vector<int>>&personPose);
	/****************************************!
	2 *@brief  
	3 *@author mayn
	4 *@date   2018年5月26日
	5 *@param[out] 
	6 *@param[in]    
	7 *@return       
	8 ****************************************/
	int ReleaseHandle();
};


