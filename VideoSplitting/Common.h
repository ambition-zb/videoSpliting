#pragma once
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;


string getDirectoryPath(string filePath);
string getFileNameWithoutExtension(string filePath);
string createFolder(string strPath, string folderName);