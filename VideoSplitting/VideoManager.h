#pragma once
#include <string>
#include <list>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class CVideoManager
{
public:
	CVideoManager();
	~CVideoManager();

	void setVedioList(list<string>& listVedio);
	void vedioSplit();
	double frameDifference(const Mat& frame1, const Mat& frame2);

	
private:
	list<string> m_listVedio;
};

