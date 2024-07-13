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
	videoMng.setVedioList(listVedio);
	videoMng.vedioSplit();

	system("pause");

	return 0;
	
}

