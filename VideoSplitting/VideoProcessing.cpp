#include "VideoProcessing.h"
#include "Function_ColorHistogram.h"

// 定义静态成员变量
std::shared_ptr<VideoProcessing> VideoProcessing::instance = nullptr;
std::once_flag VideoProcessing::initFlag;

void VideoProcessing::spliting(string strFilePath)
{
	cv::VideoCapture cap(strFilePath);
	if (!cap.isOpened())
	{
		cerr << "Error opening video file" << endl;
		return;
	}

	// 初始化变量
	Mat prevFrame, currFrame;
	Mat histPrev, histCurr;
	// 直方图差异阈值
	double threshold = 0.3; 

	ptrScene scene = make_shared<Scene>();
	map_scene[strFilePath].push_back(scene);

	// 读取第一帧并计算直方图
	cap >> prevFrame;
	if (prevFrame.empty()) {
		cerr << "Error reading first frame" << endl;
		return;
	}
	histPrev = CFunction_ColorHistogram::getInstance()->calculateHistogram(prevFrame);
	scene->addFrame(prevFrame);

	// 遍历视频的每一帧
	while (true) {
		cap >> currFrame;
		if (currFrame.empty()) break;

		// 计算当前帧的直方图
		histCurr = CFunction_ColorHistogram::getInstance()->calculateHistogram(currFrame);

		// 计算直方图差异
		double diff = CFunction_ColorHistogram::getInstance()->histogramDifference(histPrev, histCurr);
		if (diff > threshold) {
			//创建新的scene
			scene = make_shared<Scene>();
			map_scene[strFilePath].push_back(scene);
		}
		// 更新前一帧的直方图
		histPrev = histCurr.clone();
		prevFrame = currFrame.clone();
		scene->addFrame(currFrame.clone());
	}
	//如果最后一个scene里没有数据，舍弃
	if (!scene && scene->isEmpty()) 
		map_scene[strFilePath].pop_back();

	cap.release();
}

void VideoProcessing::randomDelete()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		for (ptrScene scene : it->second)
		{
			scene->randomDelete();
		}
	}
}

void VideoProcessing::randomResize()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		for (ptrScene scene : it->second)
			scene->randomResize();
	}
}

void VideoProcessing::randomSpeed()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		for (ptrScene scene : it->second)
			scene->randomSpeed();
	}
}

void VideoProcessing::write2file()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		string strFilePath = it->first;
		string strDirectory = getDirectoryPath(strFilePath);
		string strFileName = getFileNameWithoutExtension(strFilePath);
		string strFolderPath = createFolder(strDirectory, strFileName);
		int nIndex = 0;
		for (ptrScene scene : it->second)
		{
			string strSegmentName = "/segment_" + std::to_string(nIndex + 1) + string(".avi");
			scene->write2file(strFolderPath + strSegmentName);
			nIndex++;
		}
	}
}
