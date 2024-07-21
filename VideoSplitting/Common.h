#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

extern "C" {
#include <libavformat/avformat.h>  // FFmpeg ��ʽ�������
#include <libavcodec/avcodec.h>    // FFmpeg ��������
#include <libavutil/avutil.h>      // FFmpeg ʵ�ú���
#include <libavutil/imgutils.h>    // FFmpeg ͼ����ʵ�ú���
#include <libswscale/swscale.h>    // FFmpeg ͼ���������
}


#include <opencv2/opencv.hpp>
using namespace cv;

#define HISTOGRAM_DIFFERENCE_THRESHOLD  1.0f;

string getDirectoryPath(string filePath);
string getFileNameWithoutExtension(string filePath);
string createFolder(string strPath, string folderName);