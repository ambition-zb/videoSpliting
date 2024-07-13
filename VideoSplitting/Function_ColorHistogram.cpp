#include "Function_ColorHistogram.h"


// 定义静态成员变量
std::shared_ptr<CFunction_ColorHistogram> CFunction_ColorHistogram::instance = nullptr;
std::once_flag CFunction_ColorHistogram::initFlag;


void CFunction_ColorHistogram::doTask(string& strFilePath)
{
	cv::VideoCapture cap(strFilePath);
	if (!cap.isOpened())
	{
		cerr << "Error opening video file" << endl;
		return;
	}

	//创建文件夹
	string strDirectory = getDirectoryPath(strFilePath);
	string strFileName = getFileNameWithoutExtension(strFilePath);
	string strFolderPath = createFolder(strDirectory, strFileName);

	// 初始化变量
	Mat prevFrame, currFrame;
	Mat histPrev, histCurr;
	int segmentCount = 0;
	double threshold = 0.3; // 直方图差异阈值
	vector<Mat> frames; // 保存当前镜头的所有帧

	// 读取第一帧并计算直方图
	cap >> prevFrame;
	if (prevFrame.empty()) {
		cerr << "Error reading first frame" << endl;
		return;
	}
	histPrev = calculateHistogram(prevFrame);
	frames.push_back(prevFrame);

	// 遍历视频的每一帧
	while (true) {
		cap >> currFrame;
		if (currFrame.empty()) break;

		// 计算当前帧的直方图
		histCurr = calculateHistogram(currFrame);

		// 计算直方图差异
		double diff = histogramDifference(histPrev, histCurr);
		//cout << diff << endl;

		if (diff > threshold) {
			// 保存当前镜头
			string strSegment = strFolderPath + "/segment_" + std::to_string(segmentCount) + string(".avi");
			VideoWriter writer(strSegment, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, prevFrame.size());
			for (const Mat& frame : frames) {
				writer.write(frame);
			}
			writer.release();
			segmentCount++;
			frames.clear();
		}

		// 更新前一帧的直方图
		histPrev = histCurr.clone();
		prevFrame = currFrame.clone();
		frames.push_back(currFrame.clone());
	}
	// 保存最后一个镜头段落
	if (!frames.empty()) {
		stringstream ss;
		string strSegment = strFolderPath + "/segment_" + std::to_string(segmentCount + 1) + string(".avi");
		VideoWriter writer(strSegment, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, prevFrame.size());
		for (const Mat& frame : frames) {
			writer.write(frame);
		}
		writer.release();
	}
	cap.release();
}

// 计算图像的颜色直方图
Mat CFunction_ColorHistogram::calculateHistogram(const Mat& frame) {
	Mat hsv;
	cvtColor(frame, hsv, COLOR_BGR2HSV);

	int hBins = 50, sBins = 60;
	int histSize[] = { hBins, sBins };
	float hRanges[] = { 0, 180 };
	float sRanges[] = { 0, 256 };
	const float* ranges[] = { hRanges, sRanges };
	int channels[] = { 0, 1 };

	Mat hist;
	calcHist(&hsv, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
	normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat());
	return hist;
}

// 计算两个直方图之间的差异
double CFunction_ColorHistogram::histogramDifference(const Mat& hist1, const Mat& hist2) {
	return compareHist(hist1, hist2, HISTCMP_BHATTACHARYYA);
}
