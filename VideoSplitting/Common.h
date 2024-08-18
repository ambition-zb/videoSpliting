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
#include <libavformat/avformat.h>  // FFmpeg 格式处理相关
#include <libavcodec/avcodec.h>    // FFmpeg 编解码相关
#include <libavutil/avutil.h>      // FFmpeg 实用函数
#include <libavutil/imgutils.h>    // FFmpeg 图像处理实用函数
#include <libswscale/swscale.h>    // FFmpeg 图像缩放相关
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