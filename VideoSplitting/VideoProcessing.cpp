#include "VideoProcessing.h"
#include "Function_ColorHistogram.h"

// ���徲̬��Ա����
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

	cout << "split video:" << strFilePath << endl;

	// ��ʼ������
	Mat prevFrame, currFrame;
	Mat histPrev, histCurr;
	// ֱ��ͼ������ֵ
	double threshold = 0.3; 

	ptrScene scene = make_shared<Scene>();
	scene->m_strName = "1";
	map_scene[strFilePath].push_back(scene);

	// ��ȡ��һ֡������ֱ��ͼ
	cap >> prevFrame;
	if (prevFrame.empty()) {
		cerr << "Error reading first frame" << endl;
		return;
	}
	histPrev = CFunction_ColorHistogram::getInstance()->calculateHistogram(prevFrame);
	scene->addFrame(prevFrame);

	int nCount = 1;
	// ������Ƶ��ÿһ֡
	while (true) {
		cap >> currFrame;
		if (currFrame.empty()) break;

		// ���㵱ǰ֡��ֱ��ͼ
		histCurr = CFunction_ColorHistogram::getInstance()->calculateHistogram(currFrame);

		// ����ֱ��ͼ����
		double diff = CFunction_ColorHistogram::getInstance()->histogramDifference(histPrev, histCurr);
		if (diff > threshold) {
			//�����µ�scene
			nCount++;
			scene = make_shared<Scene>();
			scene->m_strName = std::to_string(nCount);;
			map_scene[strFilePath].push_back(scene);
		}
		// ����ǰһ֡��ֱ��ͼ
		histPrev = histCurr.clone();
		prevFrame = currFrame.clone();
		scene->addFrame(currFrame.clone());
	}
	//������һ��scene��û�����ݣ�����
	if (!scene && scene->isEmpty()) 
		map_scene[strFilePath].pop_back();

	cap.release();
}

void VideoProcessing::deleteInvalidScene()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "��Ƶ[" << it->first << "]ɾ����Чscene:" << endl;
		for (auto itv = it->second.begin(); itv != it->second.end();)
		{
			if ((*itv)->getFrameSize() < 15) {
				cout << (*itv)->m_strName << "[" << (*itv)->getFrameSize() << "fps]" << endl;
				itv = it->second.erase(itv);  
			}
			else {
				++itv;  
			}
		}
	}
}

void VideoProcessing::randomDelete()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "��Ƶ[" << it->first << "]���ɾ֡:" << endl;
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
		cout << "��Ƶ[" << it->first << "]�������:" << endl;
		for (ptrScene scene : it->second)
			scene->randomResize();
	}
}

void VideoProcessing::randomSpeed()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "��Ƶ[" << it->first << "]��������ٶ�:" << endl;
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
		cout << "��Ƶ[" << it->first << "]����:" << endl;
		//int nIndex = 0;
		for (ptrScene scene : it->second)
		{
			//string strSegmentName = "/segment_" + std::to_string(nIndex + 1) + string(".avi");
			scene->write2file(strFolderPath /*+ strSegmentName*/);
			//cout << "save video:" << strSegmentName << endl;
			//nIndex++;
		}
	}
}
