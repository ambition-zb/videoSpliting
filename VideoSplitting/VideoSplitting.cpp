// VideoSplitting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>

#include "VideoManager.h"

int main()
{
	CVideoManager videoMng;

	list<string> listVedio;
	listVedio.push_back("C:\\Users\\mei_s\\Desktop\\test\\13364733725316487.mp4");
// 	listVedio.push_back("D:\\自媒体\\带货\\20240721金银花凉茶\\2.mp4");
// 	listVedio.push_back("D:\\自媒体\\带货\\20240721金银花凉茶\\3.mp4");
// 	listVedio.push_back("D:\\自媒体\\带货\\20240721金银花凉茶\\4.mp4");
// 	listVedio.push_back("D:\\自媒体\\带货\\20240721金银花凉茶\\5.mp4");
	videoMng.setVedioList(listVedio);
	videoMng.vedioSplit();

	system("pause");

	return 0;
	
}

