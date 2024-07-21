#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

extern "C" {
#include <libavformat/avformat.h>  // FFmpeg 格式处理相关
#include <libavcodec/avcodec.h>    // FFmpeg 编解码相关
#include <libavutil/avutil.h>      // FFmpeg 实用函数
#include <libavutil/imgutils.h>    // FFmpeg 图像处理实用函数
#include <libswscale/swscale.h>    // FFmpeg 图像缩放相关
}


#include <opencv2/opencv.hpp>
using namespace cv;

#define HISTOGRAM_DIFFERENCE_THRESHOLD  1.0f;

string getDirectoryPath(string filePath);
string getFileNameWithoutExtension(string filePath);
string createFolder(string strPath, string folderName);