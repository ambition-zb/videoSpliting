#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <algorithm>
//using namespace std;

extern "C" {
#include <libavformat/avformat.h>  // FFmpeg ��ʽ�������
#include <libavcodec/avcodec.h>    // FFmpeg ��������
#include <libavutil/avutil.h>      // FFmpeg ʵ�ú���
#include <libavutil/imgutils.h>    // FFmpeg ͼ����ʵ�ú���
#include <libswscale/swscale.h>    // FFmpeg ͼ���������
}


// #include <opencv2/opencv.hpp>
// using namespace cv;


#include "Message.h"

#define HISTOGRAM_DIFFERENCE_THRESHOLD  1.0f;

std::string getDirectoryPath(std::string filePath);
std::string getFileNameWithoutExtension(std::string filePath);
std::string createFolder(std::string strPath);
std::string createFolder(std::string strPath, std::string folderName);
std::string getCurrentDate();