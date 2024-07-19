// VideoSplitting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>

#include "VideoManager.h"

int main()
{
	CVideoManager videoMng;

	list<string> listVedio;
	listVedio.push_back("D:\\自媒体\\带货\\20240716大窑\\1.mp4");
	listVedio.push_back("D:\\自媒体\\带货\\20240716大窑\\2.mp4");
	listVedio.push_back("D:\\自媒体\\带货\\20240716大窑\\3.mp4");
	listVedio.push_back("D:\\自媒体\\带货\\20240716大窑\\4.mp4");
	listVedio.push_back("D:\\自媒体\\带货\\20240716大窑\\5.mp4");
	videoMng.setVedioList(listVedio);
	videoMng.vedioSplit();

	system("pause");

	return 0;
	
}

