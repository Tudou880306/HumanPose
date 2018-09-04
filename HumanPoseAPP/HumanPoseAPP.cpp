// HumanPoseAPP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../HumanPose/HumanPose.h"
#include <iostream>
#include "opencv2/videoio/videoio.hpp"
//#ifdef OPENCV
#include <opencv2/opencv.hpp>			// C++
#include "opencv2/core/version.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <thread>
using namespace std;
using namespace cv;
std::map<int, std::vector<int>>resoult1, resoult2, resoult3, resoult4;


void output(int i)
{
	cout << i << endl;
	if (i == 0)
	{
		ExtractFeature *handle = HPLib_Init();
		cv::Mat cap_frame, cur_frame;
		std::thread t_detect, t_cap;
		cv::VideoCapture cap("media/test.mp4"); 
		while (true)
		{
			cap >> cur_frame;
			while (!cur_frame.empty())
			{
				if (t_cap.joinable())
				{
					t_cap.join();
					cur_frame = cap_frame.clone();
				}
				t_cap = std::thread([&]()
				{
					cap >> cap_frame;
					//std::cout << "cap_frame" << std::endl;
				});
				HPLib_StartProcess(handle, cur_frame, resoult1);
				std::string str1 = "resoult1:" + std::to_string(resoult1.size());
				cout << str1 << endl;
				cv::imshow("window name", cur_frame);
				Sleep(1);
			}
			if (t_cap.joinable()) t_cap.join();
		}
		
		/*cv::Mat img = cv::imread("media/test1.jpg", CV_LOAD_IMAGE_COLOR);
		HPLib_StartProcess(handle, img , resoult1);*/
		
		HPLib_Release(handle);

	}
	else if (i == 1)
	{
		ExtractFeature *handle2 = HPLib_Init();

		cv::Mat img1 = cv::imread("media/test2.jpg", CV_LOAD_IMAGE_COLOR);
		HPLib_StartProcess(handle2, img1, resoult2);
		cv::Mat img2 = cv::imread("media/test3.jpg", CV_LOAD_IMAGE_COLOR);
		HPLib_StartProcess(handle2, img2, resoult2);
		//cout << "resoult2:" << endl;
		std::string str= "resoult2:" + std::to_string(resoult2.size()) ;
		cout << str << endl;
		HPLib_Release(handle2);

	}
	else if (i == 2)
	{
		ExtractFeature *handle3 = HPLib_Init();

		cv::Mat img3 = cv::imread("media/test3.jpg", CV_LOAD_IMAGE_COLOR);
		HPLib_StartProcess(handle3, img3, resoult3);
		//cout << "resoult2:" << endl;
		std::string str = "resoult3:" + std::to_string(resoult3.size());
		cout << str << endl;
		HPLib_Release(handle3);

	}
	else if (i == 3)
	{
		ExtractFeature *handle4 = HPLib_Init();

		cv::Mat img4 = cv::imread("media/test4.jpg", CV_LOAD_IMAGE_COLOR);
		HPLib_StartProcess(handle4, img4, resoult4);
		//cout << "resoult2:" << endl;
		std::string str = "resoult4:" + std::to_string(resoult4.size());
		cout << str << endl;
		HPLib_Release(handle4);

	}


}
int main()
{
	//cv::VideoCapture capture("media/test.mp4");
	//if (!capture.isOpened())

	//{
	//	std::cout << "video not open." << std::endl;
	//	return 1;
	//}
	////获取当前视频帧率
	//double rate = capture.get(CV_CAP_PROP_FPS);
	////当前视频帧
	//cv::Mat frame;
	////每一帧之间的延时
	////与视频的帧率相对应
	//int delay = 1000 / rate;
	//bool stop(false);
	//while (!stop)

	//{
	//	if (!capture.read(frame))

	//	{
	//		std::cout << "no video frame" << std::endl;
	//		break;
	//	}

	//	//此处为添加对视频的每一帧的操作方法
	//	int frame_num = capture.get(CV_CAP_PROP_POS_FRAMES);
	//	std::cout << "Frame Num : " << frame_num << std::endl;
	//	/*if (frame_num == 20)

	//	{
	//		capture.set(CV_CAP_PROP_POS_FRAMES, 10);
	//	}*/

	//	cv::imshow("video", frame);
	//	//引入延时
	//	//也可通过按键停止
	//	if (cv::waitKey(delay) > 0)
	//		stop = true;
	//}


	////关闭视频，手动调用析构函数（非必须）
	//capture.release();
	//return 0;
	/*for (int i = 0; i < 1; i++)
	{
		thread t(output, i);
		t.detach();
	}*/
	ExtractFeature *handle = HPLib_Init();
	cv::Mat cap_frame, cur_frame;
	std::thread t_detect, t_cap;
	cv::VideoCapture cap("media/test.mp4");
	double rate1 = cap.get(CV_CAP_PROP_FPS);
	int delay1 = 1000 / rate1;
	cout << delay1 << endl;
	if (!cap.isOpened())

	{
		std::cout << "video not open." << std::endl;
		return 1;
	}
	bool temp = false;
	while (true)
	{
		cap.set(CV_CAP_PROP_POS_FRAMES,250);
		while(!temp)
		{
			cap >> cur_frame;
			int frame_num = cap.get(CV_CAP_PROP_POS_FRAMES);
			//cout <<"frame_num:"<< frame_num << endl;
			/*if (t_cap.joinable())
			{
				t_cap.join();
				cur_frame = cap_frame.clone();
			}*/
			//t_cap = std::thread([&]()
			//{
			//	cap >> cap_frame;
			//	//std::cout << "cap_frame" << std::endl;
			//});
			cv::imshow("dst4", cur_frame);
			clock_t starts;clock_t ends;
			
			//if (frame_num%25==0)
			{
				starts = clock();
				HPLib_StartProcess(handle, cur_frame, resoult1);
				ends = clock();
				cout << ends - starts << endl;
			}
			//std::string str1 = "resoult1:" + std::to_string(resoult1.size());
			//cout <<"timeout: "<< str1 << endl; 
			/*IplImage* img = &IplImage(cur_frame);
			cvShowImage("dst3", img);*/
			//cv::imshow("dst3", cur_frame);
			//Sleep(50);
			if (cv::waitKey(delay1) > 0)
				temp = true;
		}
		if (t_cap.joinable()) t_cap.join();
	}

	/*cv::Mat img = cv::imread("media/test1.jpg", CV_LOAD_IMAGE_COLOR);
	HPLib_StartProcess(handle, img , resoult1);*/

	HPLib_Release(handle);
	getchar();
	
    return 0;
}

