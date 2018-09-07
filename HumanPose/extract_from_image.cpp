#include "stdafx.h"
#include "extract_from_image.h"
#include <array>
#include <vector>
#include<cmath>
#include "stdio.h"
#include<iostream>
#include "tinyxml2.h"
#include "dllmain.cpp"
// ------------------------- OpenPose Library Tutorial - Pose - Example 1 - Extract from Image -------------------------
// This first example shows the user how to:
// 1. Load an image (`filestream` module)
// 2. Extract the pose of that image (`pose` module)
// 3. Render the pose on a resized copy of the input image (`pose` module)
// 4. Display the rendered pose (`gui` module)
// In addition to the previous OpenPose modules, we also need to use:
// 1. `core` module: for the Array<float> class that the `pose` module needs
// 2. `utilities` module: for the error & logging functions, i.e. op::error & op::log respectively

// 3rdparty dependencies
// GFlags: DEFINE_bool, _int32, _int64, _uint64, _double, _string
#include <gflags/gflags.h>
// Allow Google Flags in Ubuntu 14
#ifndef GFLAGS_GFLAGS_H_
namespace gflags = google;
#endif

//#ifdef _DEBUG
//#pragma comment(lib, "opencv_world310d.lib") 
//#pragma comment(lib, "openposed.lib") 
//#pragma comment(lib, "gflagsd.lib")
////#pragma comment(lib, "opencv_world310d.lib")   
////#pragma comment(lib, "openposed.lib") 
////#pragma comment(lib, "gflags.lib") 
//#else
//#pragma comment(lib, "opencv_world310.lib") 
//#pragma comment(lib, "openpose.lib") 
//#pragma comment(lib, "gflags.lib") 
//#endif

// See all the available parameter options withe the `--help` flag. E.g. `build/examples/openpose/openpose.bin --help`
// Note: This command will show you flags for other unnecessary 3rdparty files. Check only the flags for the OpenPose
// executable. E.g. for `openpose.bin`, look for `Flags from examples/openpose/openpose.cpp:`.
// Debugging/Other
DEFINE_int32(logging_level, 3, "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
	" 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
	" low priority messages and 4 for important ones.");
// Producer
DEFINE_string(image_path, "media/test.jpg", "Process the desired image.");
// OpenPose
DEFINE_string(model_pose, "COCO", "Model to be used. E.g. `COCO` (18 keypoints), `MPI` (15 keypoints, ~10% faster), "
	"`MPI_4_layers` (15 keypoints, even faster but less accurate).");
DEFINE_string(model_folder, "models/", "Folder path (absolute or relative) where the models (pose, face, ...) are located.");
DEFINE_string(net_resolution, "-1x368", "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
	" decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
	" closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
	" any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
	" input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
	" e.g. full HD (1980x1080) and HD (1280x720) resolutions.");
DEFINE_string(output_resolution, "-1x-1", "The image resolution (display and output). Use \"-1x-1\" to force the program to use the"
	" input image resolution.");
DEFINE_int32(num_gpu_start, -1, "GPU device start number.");
DEFINE_double(scale_gap, 0.3, "Scale gap between scales. No effect unless scale_number > 1. Initial scale is always 1."
	" If you want to change the initial scale, you actually want to multiply the"
	" `net_resolution` by your desired initial scale.");
DEFINE_int32(scale_number, 1, "Number of scales to average.");
// OpenPose Rendering
DEFINE_bool(disable_blending, false, "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
	" background, instead of being rendered into the original image. Related: `part_to_show`,"
	" `alpha_pose`, and `alpha_pose`.");
DEFINE_double(render_threshold, 0.05, "Only estimated keypoints whose score confidences are higher than this threshold will be"
	" rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
	" while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
	" more false positives (i.e. wrong detections).");
DEFINE_double(alpha_pose, 1, "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
	" hide it. Only valid for GPU rendering.");



template<typename T>
int intRound(const T a)
{
	return int(a + 0.5f);
}

#define PI 3.1415926
/****************************************!
2 *@brief  三个连续点向量角度;
3 *@author mayn
4 *@date   2018年5月25日
5 *@param[out] 
6 *@param[in]    
7 *@return       
8 ****************************************/
double  cal(int person,cv::Point p1, cv::Point p2, cv::Point p3 )
{

	double a[2] = { p2.x - p1.x,p2.y - p1.y }, b[2] = { p3.x-p2.x,p3.y-p2.y };
	double ab, a1, b1, cosr;
	ab = a[0] * b[0] + a[1] * b[1];
	a1 = sqrt(a[0] * a[0] + a[1] * a[1]);
	b1 = sqrt(b[0] * b[0] + b[1] * b[1]);
	cosr = ab / a1 / b1;
	double resoult = acos(cosr) * 180 / PI;
	
	return resoult;
	//std::cout << acos(cosr) * 180 / PI << std::endl;
}
///
//////////////////////////////////////////////////////////////////////////
/****************************************!
2 *@brief  两小臂角度;
3 *@author mayn
4 *@date   2018年5月25日
5 *@param[out] 
6 *@param[in]  p1 left hand; p2 left arm; p3 right hand ; p4right arm;  
7 *@return     left arm and right arm angle  ;
8 ****************************************/
double calArmAngle(int person, cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4)
{
	double a[2] = { p1.x - p2.x,p1.y - p2.y }, b[2] = { p3.x - p4.x,p3.y - p4.y };
	double ab, a1, b1, cosr;
	ab = a[0] * b[0] + a[1] * b[1];
	a1 = sqrt(a[0] * a[0] + a[1] * a[1]);
	b1 = sqrt(b[0] * b[0] + b[1] * b[1]);
	cosr = ab / a1 / b1;
	double resoult = acos(cosr) * 180 / PI;

	return resoult;
}


/****************************************!
2 *@brief  
3 *@author mayn
4 *@date   2018年9月4日
5 *@param[out] 
6 *@param[in]    两帧图片中的每个人的关键点;
7 *@return      检测到的每个人的匹配度 ;
8 ****************************************/
std::vector<int> TopMatchPerson(op::Array<float> olddata, op::Array<float> newdata)
{
	std::vector<int> resoult;


	int persons_n = newdata.getSize(0);
	int numberKeypoints_n = newdata.getSize(1);

	int persons_o = olddata.getSize(0);
	int numberKeypoints_o = olddata.getSize(1);
	for (int person_n = 0; person_n < persons_n; person_n++)
	{
		int topmatch = 9999;
		for (int person_o = 0; person_o < persons_o; person_o++)
		{
			int match = 0;
			for (int pair = 0u; pair < 18; pair++)
			{
				const int index_n = (person_n * numberKeypoints_n + pair) * newdata.getSize(2);
				cv::Point keypoint_n{ intRound(newdata[index_n]), intRound(newdata[index_n + 1]) };

				const int index_o = (person_o * numberKeypoints_o + pair) * olddata.getSize(2);
				cv::Point keypoint_o{ intRound(olddata[index_o]), intRound(olddata[index_o + 1]) };

				match += abs(keypoint_n.x - keypoint_o.x) + abs(keypoint_n.y - keypoint_o.y);
			}
			if (match<=topmatch)
			{
				topmatch = match;
			}
		}
		resoult.push_back(topmatch);
		
	}

	return resoult;
}
ExtractFeature::ExtractFeature()
{
//	oldimg = cvCreateImage(cvSize(400, 400), IPL_DEPTH_8U, 3);
	m_sleepthreshold = 20;
	m_sleepframe = 20;
}

ExtractFeature::~ExtractFeature()
{
}
/****************************************!
2 *@brief  保存xml配置文件
3 *@author mayn
4 *@date   2018年5月30日
5 *@param[out] 
6 *@param[in]    
7 *@return       
8 ****************************************/
bool SaveXml()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLDeclaration* del = doc.NewDeclaration();
	doc.InsertFirstChild(del);
	tinyxml2::XMLElement *config = doc.NewElement("Config");

	tinyxml2::XMLElement* info = doc.NewElement("OpenPose");
	info->SetAttribute("name", "OpenPose");

	tinyxml2::XMLElement* param1 = doc.NewElement("logging_level");
	param1->SetAttribute("value", "0");
	info->InsertEndChild(param1);
	tinyxml2::XMLElement* param2 = doc.NewElement("image_path");
	param2->SetAttribute("value", "media/test.jpg");
	info->InsertEndChild(param2);
	tinyxml2::XMLElement* param3 = doc.NewElement("model_pose");
	param3->SetAttribute("value", "COCO");
	info->InsertEndChild(param3);
	tinyxml2::XMLElement* param4 = doc.NewElement("model_folder");
	param4->SetAttribute("value", "media/");
	info->InsertEndChild(param4);
	tinyxml2::XMLElement* param5 = doc.NewElement("net_resolution");
	param5->SetAttribute("value", "-1x368");
	info->InsertEndChild(param5);
	tinyxml2::XMLElement* param6 = doc.NewElement("output_resolution");
	param6->SetAttribute("value", "-1x-1");
	info->InsertEndChild(param6);
	tinyxml2::XMLElement* param7 = doc.NewElement("num_gpu_start");
	param7->SetAttribute("value", "0");
	info->InsertEndChild(param7);
	tinyxml2::XMLElement* param8 = doc.NewElement("scale_gap");
	param8->SetAttribute("value", "0.3");
	info->InsertEndChild(param8);
	tinyxml2::XMLElement* param9 = doc.NewElement("scale_number");
	param9->SetAttribute("value", "1");
	info->InsertEndChild(param9);
	tinyxml2::XMLElement* param10 = doc.NewElement("disable_blending");
	param10->SetAttribute("value", "false");
	info->InsertEndChild(param10);
	tinyxml2::XMLElement* param11 = doc.NewElement("render_threshold");
	param11->SetAttribute("value", "0.05");
	info->InsertEndChild(param11);
	tinyxml2::XMLElement* param12 = doc.NewElement("alpha_pose");
	param12->SetAttribute("value", "1");
	info->InsertEndChild(param12);
	config->InsertFirstChild(info);


	tinyxml2::XMLElement* log = doc.NewElement("LogSetting");
	log->SetAttribute("name", "LogSetting");
	config->InsertEndChild(log);

	doc.InsertEndChild(config);
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	tinyxml2::XMLError res;

	res = doc.SaveFile("D:\\test.xml");
	if (res != tinyxml2::XML_SUCCESS)
	{
		return false;
	}
	return true;
}
/****************************************!
2 *@brief  加载xml配置文件
3 *@author mayn
4 *@date   2018年5月30日
5 *@param[out] 
6 *@param[in]    
7 *@return       
8 ****************************************/
int ExtractFeature::LoadXml(const char* xml)
{
	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError res;
	res = doc.LoadFile(xml);
	if (tinyxml2::XML_SUCCESS == res)
	{
		tinyxml2::XMLElement* element = doc.FirstChildElement("Config");
		if (element != nullptr)
		{
			element = element->FirstChildElement("OpenPose");
			if (element!=nullptr)
			{
				element = element->FirstChildElement("logging_level");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(),"value")==0)
					{
						FLAGS_logging_level = atoi(element->FirstAttribute()->Value());
					}
					else return -1;
					
				}
				element = element->NextSiblingElement("image_path");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_image_path = element->FirstAttribute()->Value();
						op::log(FLAGS_image_path, op::Priority::High);
					}
					else return -1;
				}
				element = element->NextSiblingElement("model_pose");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_model_pose = element->FirstAttribute()->Value();
					}
					else return -1;
				}

				element = element->NextSiblingElement("model_folder");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_model_folder = element->FirstAttribute()->Value();
					}
					else return -1;
				}

				element = element->NextSiblingElement("net_resolution");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_net_resolution = element->FirstAttribute()->Value();
					}
					else return -1;
				}

				element = element->NextSiblingElement("output_resolution");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_output_resolution = element->FirstAttribute()->Value();
					}
					else return -1;
				}

				element = element->NextSiblingElement("num_gpu_start");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_num_gpu_start = atoi(element->FirstAttribute()->Value());
					}
					else return -1;
				}

				element = element->NextSiblingElement("scale_gap");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_scale_gap = atof(element->FirstAttribute()->Value());
					}
					else return -1;
				}

				element = element->NextSiblingElement("scale_number");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_scale_number = atof(element->FirstAttribute()->Value());
					}
					else return -1;
				}

				element = element->NextSiblingElement("disable_blending");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						std::string temp = op::toLower(element->FirstAttribute()->Value());
						if (strcmp(element->FirstAttribute()->Value(),"true")==0)
						{
							FLAGS_disable_blending = true;
						}
						else if (strcmp(element->FirstAttribute()->Value(), "false") == 0)
						{
							FLAGS_disable_blending = false;
						}
					}
					else return -1;
				}

				element = element->NextSiblingElement("render_threshold");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_render_threshold = atof(element->FirstAttribute()->Value());
					}
					else return -1;
				}
				element = element->NextSiblingElement("alpha_pose");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						FLAGS_alpha_pose = atof(element->FirstAttribute()->Value());
					}
					else
						return -1;
				}

				element = element->NextSiblingElement("sleepthreshold");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						m_sleepthreshold = atoi(element->FirstAttribute()->Value());
					}
					else
						return -1;
				}
				element = element->NextSiblingElement("sleepframe");
				if (element == nullptr)
					return -1;
				else
				{
					if (strcmp(element->FirstAttribute()->Name(), "value") == 0)
					{
						m_sleepframe = atoi(element->FirstAttribute()->Value());
					}
					else
						return -1;
				}
			}
		}

	}
	else
	{
		op::error("load xml failed!", __LINE__, __FUNCTION__, __FILE__);
		return -1;
	}
	return 0;
}
/****************************************!
2 *@brief  
3 *@author mayn
4 *@date   2018年5月26日
5 *@param[out] 
6 *@param[in]    
7 *@return       
8 ****************************************/
int ExtractFeature::InitHandle()
{
	op::log("OpenPose Library Tutorial - Example 1.", op::Priority::High);
	// ------------------------- INITIALIZATION -------------------------
	// Step 1 - Set logging level
	// - 0 will output all the logging messages
	// - 255 will output nothing
	op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
		__LINE__, __FUNCTION__, __FILE__);
	op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
	//op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
	// Step 2 - Read Google flags (user defined configuration)
	// outputSize
	 m_outputSize = op::flagsToPoint(FLAGS_output_resolution, "-1x-1");
	// netInputSize
	 m_netInputSize = op::flagsToPoint(FLAGS_net_resolution, "-1x368");
	// poseModel
	 m_poseModel = op::flagsToPoseModel(FLAGS_model_pose);
	// Check no contradictory flags enabled
	if (FLAGS_alpha_pose < 0. || FLAGS_alpha_pose > 1.)
		op::error("Alpha value for blending must be in the range [0,1].", __LINE__, __FUNCTION__, __FILE__);
	if (FLAGS_scale_gap <= 0. && FLAGS_scale_number > 1)
		op::error("Incompatible flag configuration: scale_gap must be greater than 0 or scale_number = 1.",
			__LINE__, __FUNCTION__, __FILE__);
	// Logging
	//op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
	// Step 3 - Initialize all required classes


	/*op::ScaleAndSizeExtractor scaleAndSizeExtractor(m_netInputSize, m_outputSize, FLAGS_scale_number, FLAGS_scale_gap);
	op::CvMatToOpInput cvMatToOpInput{ m_poseModel };
	op::CvMatToOpOutput cvMatToOpOutput;*/
	//op::PoseExtractorCaffe poseExtractorCaffe { m_poseModel, FLAGS_model_folder, FLAGS_num_gpu_start };
	poseExtractorCaffe = new op::PoseExtractorCaffe(m_poseModel, FLAGS_model_folder, FLAGS_num_gpu_start);
	poseRenderer = new op::PoseCpuRenderer(m_poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
		(float)FLAGS_alpha_pose);
	/*op::PoseCpuRenderer poseRenderer{ m_poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
		(float)FLAGS_alpha_pose };*/
	/*op::OpOutputToCvMat opOutputToCvMat;
	op::FrameDisplayer frameDisplayer{ "OpenPose Tutorial - Example 1", m_outputSize };*/
	// Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
	/*poseExtractorCaffe.initializationOnThread();
	poseRenderer.initializationOnThread();*/
	poseExtractorCaffe->initializationOnThread();
	poseRenderer->initializationOnThread();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/****************************************!
2 *@brief  
3 *@author mayn
4 *@date   2018年9月3日
5 *@param[out] 
6 *@param[in] src dest    
7 *@return   两幅图的帧差;    判断连续多少帧以上大于阈值，以达到检测睡觉
8 ****************************************/
int cvabs(IplImage *src,IplImage *dest)
{
	
	IplImage  *src3 = cvCreateImage(cvSize(dest->width, dest->height), IPL_DEPTH_8U, 3);;
	cvCopy(dest, src3, NULL);
	//cvShowImage("src2", src);

	cvAbsDiff(src, dest, src3);
	//cvShowImage("src3", src3);
	cvThreshold(src3, src3, 10, 255, CV_THRESH_BINARY);
	//cvShowImage("src4", src3);
	cvErode(src3, src3);
	//cvShowImage("src5", src3);
	IplImage  *dst1, *dst2, *dst3;
	//src1 = cvLoadImage("5.jpg", 1);
	dst1 = cvCreateImage(cvSize(src3->width, src3->height), IPL_DEPTH_8U, 1);
	dst2 = cvCreateImage(cvSize(src3->width, src3->height), IPL_DEPTH_8U, 1);
	dst3 = cvCreateImage(cvSize(src3->width, src3->height), IPL_DEPTH_8U, 1);
	cvSplit(src3, dst1, dst2, dst3, 0);


	cvShowImage("dst3", dst3);
	 int v = cvCountNonZero(dst3);
	 printf("cvabs nonzero count %d", v);
	 cvReleaseImage(&dst1);
	 cvReleaseImage(&dst2);
	 cvReleaseImage(&dst3);
	 cvReleaseImage(&src3);
	 return v;
}

int ExtractFeature::ProcessImage(cv::Mat inputImage, std::map<int, std::vector<int>>&personPose)
{
	op::ScaleAndSizeExtractor scaleAndSizeExtractor(m_netInputSize, m_outputSize, FLAGS_scale_number, FLAGS_scale_gap);
	op::CvMatToOpInput cvMatToOpInput{ m_poseModel };
	op::CvMatToOpOutput cvMatToOpOutput;
	/*op::PoseExtractorCaffe poseExtractorCaffe{ m_poseModel, FLAGS_model_folder, FLAGS_num_gpu_start };
	op::PoseCpuRenderer poseRenderer{ m_poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
		(float)FLAGS_alpha_pose };*/
	op::OpOutputToCvMat opOutputToCvMat;
	op::FrameDisplayer frameDisplayer{ "OpenPose Tutorial - Example 1", m_outputSize };
	//// Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
	//poseExtractorCaffe.initializationOnThread();
	//poseRenderer.initializationOnThread();

	// ------------------------- POSE ESTIMATION AND RENDERING -------------------------
	// Step 1 - Read and load image, error if empty (possibly wrong path)
	// Alternative: cv::imread(FLAGS_image_path, CV_LOAD_IMAGE_COLOR);
	//cv::Mat inputImage = cv::cvarrToMat(imgage);
	//cv::Mat inputImage = op::loadImage(path, CV_LOAD_IMAGE_COLOR);
	if (inputImage.empty())
		op::error("Could not open or find the image" , __LINE__, __FUNCTION__, __FILE__);
	const op::Point<int> imageSize{ inputImage.cols, inputImage.rows };
	// Step 2 - Get desired scale sizes
	std::vector<double> scaleInputToNetInputs;
	std::vector<op::Point<int>> netInputSizes;
	double scaleInputToOutput;
	op::Point<int> outputResolution;
	std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput, outputResolution)
		= scaleAndSizeExtractor.extract(imageSize);
	
	// Step 3 - Format input image to OpenPose input and output formats
	const auto netInputArray = cvMatToOpInput.createArray(inputImage, scaleInputToNetInputs, netInputSizes);
	auto outputArray = cvMatToOpOutput.createArray(inputImage, scaleInputToOutput, outputResolution);
	// Step 4 - Estimate poseKeypoints
	poseExtractorCaffe->forwardPass(netInputArray, imageSize, scaleInputToNetInputs);
	auto poseKeypoints = poseExtractorCaffe->getPoseKeypoints();
#if 1
	//cv::namedWindow("test1", 0);
	IplImage *imgage1 = cvCreateImage(CvSize(333, 500), IPL_DEPTH_8U, 3);
	cvZero(imgage1);
	cv::Mat mat = cv::cvarrToMat(imgage1);
	auto s = cvMatToOpOutput.createArray(mat, scaleInputToOutput, outputResolution);
	auto frame = s.getCvMat();
	const auto width = frame.size[2];
	const auto height = frame.size[1];
	const auto area = width * height;
	const auto channelOffset = area * sizeof(float) / sizeof(uchar);
	cv::Mat frameB(height, width, CV_32FC1, &frame.data[0]);
	cv::Mat frameG(height, width, CV_32FC1, &frame.data[channelOffset]);
	cv::Mat frameR(height, width, CV_32FC1, &frame.data[2 * channelOffset]);
	
	const auto numberPeopleDetected = poseKeypoints.getSize(0);
	const auto numberBodyParts = poseKeypoints.getSize(1);
	printf("people:%d,parts:%d.\n", numberPeopleDetected, numberBodyParts);
	cvReleaseImage(&imgage1);

#endif

#if 1
	int persons = poseKeypoints.getSize(0);
	for (int person = 0; person < persons; person++)
	{
		//const auto personRectangle = getKeypointsRectangle(poseKeypoints, person, thresholdRectangle);
		//if (personRectangle.area() > 0)
		{
			const auto lineType = 8;
			const auto shift = 0;
			const auto ratioAreas = 1;
			// Size-dependent variables
			const auto thicknessRatio = 1;
			// Negative thickness in cv::circle means that a filled circle is to be drawn.
			const auto thicknessCircle = 1;
			const auto thicknessLine = 1;
			const auto radius = thicknessRatio / 2;
			std::vector<unsigned int> pairs = { 1,2,   1,5,   2,3,   3,4,   5,6,   6,7,   1,8,   8,9,   9,10,  1,11,  11,12, 12,13,  1,0,   0,14, 14,16,  0,15, 15,17 };
			int numberKeypoints = poseKeypoints.getSize(1);
			int threshold = 0.05;
			// Draw lines
			int sizes = pairs.size();
			char ch[256];
			sprintf_s(ch, sizeof(ch), "%d", sizes);
			op::log(ch, op::Priority::High);
			cv::Point LeftLegkeypointF, LeftLegkeypointS, LeftLegkeypointT, RightLegkeypointF, RightLegkeypointS, RightLegkeypointT, \
				LeftArmkeypointF, LeftArmkeypointS, RightArmkeypointF, RightArmkeypointS;

			for (int pair = 0u; pair < sizes; pair += 2)
			{
				const int index1 = (person * numberKeypoints + pairs[pair]) * poseKeypoints.getSize(2);
				const int index2 = (person * numberKeypoints + pairs[pair + 1]) * poseKeypoints.getSize(2);
				if (poseKeypoints[person, index1 + 2] > threshold && poseKeypoints[person, index2 + 2] > threshold)
				{
					const auto thicknessLineScaled = 1;
					const auto colorIndex = pairs[pair + 1] * 3; // Before: colorIndex = pair/2*3;
					const cv::Scalar color{ 0,
						255,
						0 };
					sprintf_s(ch, sizeof(ch), "%d", person);
					//if (poseKeypoints[{person, index1, 0}]>0&& poseKeypoints[{person, index1, 1}]>0&& poseKeypoints[{person, index2, 0}]>0&& poseKeypoints[{person, index2, 1}]>0)
					{
						/*			const cv::Point keypoint1{ intRound(poseKeypoints[{person,index1,0}]), intRound(poseKeypoints[{person,index1 ,1}]) };
						const cv::Point keypoint2{ intRound(poseKeypoints[{person,index2 ,0}]), intRound(poseKeypoints[{person,index2 ,1}]) };*/
						const cv::Point keypoint1{ intRound(poseKeypoints[index1]), intRound(poseKeypoints[index1 + 1]) };

						const cv::Point keypoint2{ intRound(poseKeypoints[index2]), intRound(poseKeypoints[index2 + 1]) };

						//left arm 
						if (pair == 6)
						{
							LeftArmkeypointF = keypoint1;
							LeftArmkeypointS = keypoint2;
						}
						//rihgt arm
						if (pair == 10)
						{
							RightArmkeypointF = keypoint1;
							RightArmkeypointS = keypoint2;
						}
						//left leg
						if (pair == 12)
						{
							LeftLegkeypointF = keypoint1;
							LeftLegkeypointS = keypoint2;
						}
						if (pair == 14)
						{
							LeftLegkeypointT = keypoint2;
						}
						//right leg
						if (pair == 18)
						{
							RightLegkeypointF = keypoint1;
							RightLegkeypointS = keypoint2;
						}
						if (pair == 20)
						{
							RightLegkeypointT = keypoint2;
						}

						char sh[256];
						sprintf_s(sh, sizeof(sh), "index:%d  == start.x :%d  ==== start.y: %d ", pair / 2, keypoint1.x, keypoint1.y);
						//op::log(sh, op::Priority::High);
						//char ch[128];
						sprintf_s(sh, sizeof(sh), "index:%d  == end.x :%d  ==== end.y: %d ", pair / 2, keypoint2.x, keypoint2.y);
						//op::log(sh, op::Priority::High);

						/*cv::line(frameR, keypoint1, keypoint2, color[0], thicknessLineScaled, lineType, shift);
						cv::line(frameG, keypoint1, keypoint2, color[1], thicknessLineScaled, lineType, shift);
						cv::line(frameB, keypoint1, keypoint2, color[2], thicknessLineScaled, lineType, shift);
						if (pair == 0)
						{
							sprintf_s(ch, sizeof(ch), "%d ", person);
							cv::putText(frameR, ch, keypoint1, 0, 2, cv::Scalar(0, 255, 255), 2, 8, 0);
							cv::putText(frameG, ch, keypoint1, 0, 2, cv::Scalar(0, 255, 255), 2, 8, 0);
							cv::putText(frameB, ch, keypoint1, 0, 2, cv::Scalar(0, 255, 255), 2, 8, 0);
						}*/
					}



				}
			}
			double LeftLegAngel, RightLegAngle, armAngle;
			LeftLegAngel = cal(person, LeftLegkeypointF, LeftLegkeypointS, LeftLegkeypointT);
			//char ch[128];
			sprintf_s(ch, sizeof(ch), "person num :%d  left leg angle:%0.3f  point1 x:%d y:%d point2 x:%d y:%d point3 x:%d y:%d", person, LeftLegAngel, LeftLegkeypointF.x, LeftLegkeypointF.y, LeftLegkeypointS.x, LeftLegkeypointS.y, LeftLegkeypointT.x, LeftLegkeypointT.y);
			//op::log(ch, op::Priority::High);
			RightLegAngle = cal(person, RightLegkeypointF, RightLegkeypointS, RightLegkeypointT);
			//char ch[128];
			sprintf_s(ch, sizeof(ch), "person num :%d  right leg angle:%0.3f  point1 x:%d y:%d point2 x:%d y:%d point3 x:%d y:%d", person, RightLegAngle, RightLegkeypointF.x, RightLegkeypointF.y, RightLegkeypointS.x, RightLegkeypointS.y, RightLegkeypointT.x, RightLegkeypointT.y);
			//op::log(ch, op::Priority::High);

			armAngle = calArmAngle(person, LeftArmkeypointS, LeftArmkeypointF, RightArmkeypointS, RightArmkeypointF);
			sprintf_s(ch, sizeof(ch), "person num :%d  arm angle:%0.3f  LeftArmkeypointS x:%d y:%d LeftArmkeypointF x:%d y:%d RightArmkeypointF x:%d y:%d  RightArmkeypointS  x:%d y:%d", person, armAngle, LeftArmkeypointS.x, LeftArmkeypointS.y, LeftArmkeypointF.x, LeftArmkeypointF.y, RightArmkeypointF.x, RightArmkeypointF.y, RightArmkeypointS.x, RightArmkeypointS.y);
			//op::log(ch, op::Priority::High);

			

			//大腿和躯干之间角度大于45° 坐下姿势;
			if (LeftLegAngel>45 || RightLegAngle>45)
			{
				//char ch[128];
				sprintf_s(ch, sizeof(ch), "No.%d person is sitting ", person);
				op::log(ch, op::Priority::High);
				personPose[person].push_back(int(HumPoseEnum::Sitting));
			}
			else
			{
				char ch[128];
				sprintf_s(ch, sizeof(ch), "No.%d person is standing. ", person);
				op::log(ch, op::Priority::High);
				personPose[person].push_back(int(HumPoseEnum::Standing));
			}
			//双臂之间角度大于45°说明双臂交叉
			if (armAngle<45)
			{
				sprintf_s(ch, sizeof(ch), "No.%d person is working ", person);
				op::log(ch, op::Priority::High);
				personPose[person].push_back(int(HumPoseEnum::Normal));
			}
			else
			{
				sprintf_s(ch, sizeof(ch), "No.%d person is playing ", person);
				op::log(ch, op::Priority::High);
				personPose[person].push_back(int(HumPoseEnum::Playing));
			}
			/*auto m = personPose.find(person);
			if (m!=personPose.end())
			{
				std::vector<int> item = m->second;
				int count = item.size();
				for (int i = 0; i < count; i++)
				{
					int temp = item[i];
					op::log("111", op::Priority::High);
				}
			}*/
			
			// Draw circles
			/*for (auto part = 0; part < numberKeypoints; part++)
			{
			const auto faceIndex = (person * numberKeypoints + part) * poseKeypoints.getSize(2);
			if (poseKeypoints[faceIndex + 2] > threshold)
			{
			const auto radiusScaled = radius * poseScales[part % numberScales];
			const auto thicknessCircleScaled = thicknessCircle * poseScales[part % numberScales];
			const auto colorIndex = part * 3;
			const cv::Scalar color{ colors[colorIndex % numberColors],
			colors[(colorIndex + 1) % numberColors],
			colors[(colorIndex + 2) % numberColors] };
			const cv::Point center{ intRound(keypoints[faceIndex]),
			intRound(keypoints[faceIndex + 1]) };
			cv::circle(frameR, center, radiusScaled, color[0], thicknessCircleScaled, lineType,
			shift);
			cv::circle(frameG, center, radiusScaled, color[1], thicknessCircleScaled, lineType,
			shift);
			cv::circle(frameB, center, radiusScaled, color[2], thicknessCircleScaled, lineType,
			shift);
			}
			}*/
		}
	}
	auto outputImage1 = opOutputToCvMat.formatToCvMat(s);
	for (int i = 0; i<numberPeopleDetected; i++)
		for (int j = 0; j < numberBodyParts; j++)
		{
			const auto x = poseKeypoints[{i, j, 0}];
			const auto y = poseKeypoints[{i, j, 1}];
			const auto score = poseKeypoints[{i, j, 2}];
			//circle(inputImage, Point(x, y), 2, CV_RGB(255, 0, 0), 2);
			char ch1[128];
			sprintf_s(ch1, sizeof(ch1), "index %d  x  %d   y %d", (int)j, (int)x, (int)y);
			//op::log(ch1, op::Priority::High);
			if (j == 0)
			{
				cv::Point test(x, y);
				sprintf_s(ch1, sizeof(ch1), " %d ", (int)i);
				cv::putText(outputImage1, ch1, test, 0, 1, cv::Scalar(0, 255, 0), 2, 8, 0);
			}
			/*else
			{
			cv::Point test(x, y);
			sprintf_s(ch1, sizeof(ch1), " x%d y%d ", (int)x, (int)y);
			cv::putText(outputImage1, ch1, test, 0, 0.4, cv::Scalar(0, 255, 255), 1, 8, 0);
			}*/
		}
#endif
	//////////////////////////////////////////////////////////////////////////
#if 0
	static int temp = 0;
	static bool first = true;
	if (first)
	{
		olddata = poseKeypoints;
		first = false;
	}
	else
	{
		
		std::vector<int> result = TopMatchPerson(olddata, poseKeypoints);
		bool somesleep = false;
		for (std::vector<int>::iterator it = result.begin();it!=result.end();it++)
		{
			if (*it < m_sleepthreshold)
			{
				somesleep = true;
			}
		}
		if (somesleep)
		{
			temp++;
		}
		else
		{
			olddata = poseKeypoints;
		}
	}
	if (temp >m_sleepframe)
	{
		op::log("someone is sleeping", op::Priority::High);
		temp = 0;
		olddata = poseKeypoints;
	}
#endif
#if 0
	IplImage* img1 = cvCreateImage(cvSize(inputImage.cols, inputImage.rows), IPL_DEPTH_8U, 3);
	cvCopy(&IplImage(inputImage), img1, NULL);
	
	static int temp = 0;
	static bool first = true;
	if (first)
	{
		oldimg = cvCreateImage(cvSize(img1->width, img1->height), IPL_DEPTH_8U, 3);
		//oldimg = img;
		cvCopy(img1, oldimg, NULL);
		first = false;
	}
	if (cvabs(oldimg, img1)>100)
	{
		//oldimg = img;
		cvCopy(img1, oldimg, NULL);
		temp = 0;
	}
	else
	{
		temp++;
		printf("temp: %d", temp);
		op::log(temp, op::Priority::High);
	}
	if (temp>15) //fps == 25// 25*60*10
	{
		
		op::log("temp>15", op::Priority::High);
		temp = 0;
		//oldimg = img;
		cvCopy(img1, oldimg, NULL);
	}
	cvReleaseImage(&img1);
#endif

	//cvShowImage("test1", (CvArr*)&outputImage1);
	//cv::imshow("test1", outputImage1);
	//frameDisplayer.displayFrame(outputImage1, 0);
	// Step 5 - Render poseKeypoints
	//poseRenderer->renderPose(outputArray, poseKeypoints, scaleInputToOutput);
	// Step 6 - OpenPose output format to cv::Mat
	//auto outputImage = opOutputToCvMat.formatToCvMat(outputArray);
	//return 0;
	// ------------------------- SHOWING RESULT AND CLOSING -------------------------
	// Step 1 - Show results
	//frameDisplayer.displayFrame(outputImage, 0); // Alternative: cv::imshow(outputImage) + cv::waitKey(0)
												 // Step 2 - Logging information message
	op::log("Example 1 successfully finished.", op::Priority::High);
	// Return successful message
	return 0;

}

int ExtractFeature::ReleaseHandle()
{
	delete poseExtractorCaffe;
	delete poseRenderer;
	return 0;
}
