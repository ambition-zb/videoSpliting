#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;


string getDirectoryPath(string filePath);
string getFileNameWithoutExtension(string filePath);
string createFolder(string strPath, string folderName);