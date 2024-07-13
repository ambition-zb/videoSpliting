#include "VideoManager.h"
#include <iterator> 
#include "Common.h"
#include "Function_ColorHistogram.h"

CVideoManager::CVideoManager()
{
	
}

CVideoManager::~CVideoManager()
{

}

void CVideoManager::setVedioList(list<string>& listVedio)
{
	m_listVedio.clear();
	copy(listVedio.begin(), listVedio.end(), back_inserter(m_listVedio));
}

void CVideoManager::vedioSplit()
{
	for (auto strFile : m_listVedio)
	{
		CFunction_ColorHistogram::getInstance()->doTask(strFile);
	}
}

double CVideoManager::frameDifference(const Mat& frame1, const Mat& frame2)
{
	Mat diff;
	absdiff(frame1, frame2, diff);
	return sum(diff)[0] / (frame1.rows * frame1.cols);;
}

/*
		cv::Mat prevFrame, curFrame, grayPrevFrame, grayCurFrame;
		int frameCount = 0;
		int segmentCount = 0;
		double threshold = 30.0f;

		cap >> prevFrame;
		if (prevFrame.empty())
		{
			cerr << "Error reading first frame" << endl;
			return;
		}
		cv::cvtColor(prevFrame, grayPrevFrame, cv::COLOR_BGR2GRAY);

		while (true)
		{
			cap >> curFrame;
			if(curFrame.empty())
				break;
			cv::cvtColor(curFrame, grayCurFrame, cv::COLOR_BGR2GRAY);

			double diff = frameDifference(grayPrevFrame, grayCurFrame);
			cout << diff << endl;
			continue;
			if (diff > threshold)
			{
				//stringstream ss;
				//ss < "segment_" << segmentCount << ".avi";
				string strSegment  = "segment_" + std::to_string(segmentCount) + string(".avi");
				VideoWriter writer(strSegment, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, prevFrame.size());
				for (int i = frameCount - cap.get(CAP_PROP_POS_FRAMES); i < frameCount; ++i)
				{
					writer.write(prevFrame);
				}
				segmentCount++;
			}

			prevFrame = curFrame.clone();
			grayPrevFrame = grayCurFrame.clone();
			frameCount++;
		}
		*/