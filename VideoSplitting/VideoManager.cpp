#include "VideoManager.h"
#include <iterator> 
#include "Common.h"
#include "VideoProcessing.h"
#include <filesystem>

CVideoManager::CVideoManager()
{
	
}

CVideoManager::~CVideoManager()
{

}

void CVideoManager::setVideoFolder(std::string strFolder)
{
	std::list<std::string> listVedio;
	for (const auto& entry : std::filesystem::directory_iterator(strFolder)) {
		// 检查是否是文件，并且扩展名为 .mp4
		if (entry.is_regular_file() && entry.path().extension() == ".mp4") {
			listVedio.push_back(entry.path().string());
		}
	}

	std::string strInput = strFolder;
	std::string strOutput = strFolder + "\\output";
	std::string strTemp = strFolder +  + "\\temp";
	Message::getInstance()->setMessageInfo(strInput, strOutput, strTemp);

	if (!listVedio.empty())
	{
		setVedioList(listVedio);
		videoSplit();
	}
}

void CVideoManager::setVedioList(std::list<std::string>& listVedio)
{
	m_listVedio.clear();
	copy(listVedio.begin(), listVedio.end(), back_inserter(m_listVedio));
}

void CVideoManager::videoSplit()
{
	for (auto strFile : m_listVedio)
	{
		VideoProcessing::getInstance()->spliting(strFile);
	}
 	VideoProcessing::getInstance()->deleteInvalidScene();
 	VideoProcessing::getInstance()->randomDelete();
 	VideoProcessing::getInstance()->randomResize();
 	VideoProcessing::getInstance()->randomSpeed();
	VideoProcessing::getInstance()->write2file();

	std::cout << "video spliting successful!" << std::endl;
}

// double CVideoManager::frameDifference(const Mat& frame1, const Mat& frame2)
// {
// 	Mat diff;
// 	absdiff(frame1, frame2, diff);
// 	return sum(diff)[0] / (frame1.rows * frame1.cols);;
// }
