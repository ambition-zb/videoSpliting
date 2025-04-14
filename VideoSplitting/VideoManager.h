#pragma once
#include <string>
#include <list>
//#include <opencv2/opencv.hpp>
/*using namespace std;*/
/*using namespace cv;*/

class CVideoManager
{
public:
	CVideoManager();
	~CVideoManager();

	void setVideoFolder(std::string strFolder);
	void setVedioList(std::list<std::string>& listVedio);
	void videoSplit();
	//double frameDifference(const Mat& frame1, const Mat& frame2);

	void changeDuration(const std::string inputPath, const std::string outputPath, int newDurationSeconds);
private:
	std::list<std::string> m_listVedio;
};

