#include "Function_ColorHistogram.h"


// ���徲̬��Ա����
std::shared_ptr<CFunction_ColorHistogram> CFunction_ColorHistogram::instance = nullptr;
std::once_flag CFunction_ColorHistogram::initFlag;

/*
void CFunction_ColorHistogram::doTask(string& strFilePath)
{
	cv::VideoCapture cap(strFilePath);
	if (!cap.isOpened())
	{
		cerr << "Error opening video file" << endl;
		return;
	}

	//�����ļ���
	string strDirectory = getDirectoryPath(strFilePath);
	string strFileName = getFileNameWithoutExtension(strFilePath);
	string strFolderPath = createFolder(strDirectory, strFileName);

	// ��ʼ������
	Mat prevFrame, currFrame;
	Mat histPrev, histCurr;
	int segmentCount = 0;
	double threshold = 0.3; // ֱ��ͼ������ֵ
	vector<Mat> frames; // ���浱ǰ��ͷ������֡

	// ��ȡ��һ֡������ֱ��ͼ
	cap >> prevFrame;
	if (prevFrame.empty()) {
		cerr << "Error reading first frame" << endl;
		return;
	}
	histPrev = calculateHistogram(prevFrame);
	frames.push_back(prevFrame);

	// ������Ƶ��ÿһ֡
	while (true) {
		cap >> currFrame;
		if (currFrame.empty()) break;

		// ���㵱ǰ֡��ֱ��ͼ
		histCurr = calculateHistogram(currFrame);

		// ����ֱ��ͼ����
		double diff = histogramDifference(histPrev, histCurr);
		//cout << diff << endl;

		if (diff > threshold) {
			// ���浱ǰ��ͷ
			string strSegment = strFolderPath + "/segment_" + std::to_string(segmentCount) + string(".avi");
			VideoWriter writer(strSegment, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, prevFrame.size());
			for (const Mat& frame : frames) {
				writer.write(frame);
			}
			writer.release();
			segmentCount++;
			frames.clear();
		}

		// ����ǰһ֡��ֱ��ͼ
		histPrev = histCurr.clone();
		prevFrame = currFrame.clone();
		frames.push_back(currFrame.clone());
	}
	// �������һ����ͷ����
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
*/
// ����ͼ�����ɫֱ��ͼ
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
// ��������ֱ��ͼ֮��Ĳ���
double CFunction_ColorHistogram::histogramDifference(const Mat& hist1, const Mat& hist2) {
	return compareHist(hist1, hist2, HISTCMP_BHATTACHARYYA);
}


void CFunction_ColorHistogram::calculateHistogram(const AVFrame* frame, vector<double>& hist)
{
	int width = frame->width;          // ͼ����
	int height = frame->height;        // ͼ��߶�
	int step = frame->linesize[0];     // ÿ�����صĲ���
	uint8_t* data = frame->data[0];    // ͼ������ָ��

	hist.assign(256 * 3, 0.0);         // ��ʼ��ֱ��ͼ����

	// ����ͼ�����أ�����ÿ����ɫͨ����ֱ��ͼ
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8_t r = data[y * step + x * 3 + 0];  // ��ɫͨ��
			uint8_t g = data[y * step + x * 3 + 1];  // ��ɫͨ��
			uint8_t b = data[y * step + x * 3 + 2];  // ��ɫͨ��
			hist[r]++;
			hist[256 + g]++;
			hist[512 + b]++;
		}
	}

	// ��һ��ֱ��ͼ
	for (int i = 0; i < hist.size(); i++) {
		hist[i] /= (width * height);
	}
}

double CFunction_ColorHistogram::computeHistogramDifference(const vector<double>& hist1, const vector<double>& hist2)
{
	double diff = 0.0;
	for (int i = 0; i < hist1.size(); i++) {
		diff += fabs(hist1[i] - hist2[i]);
	}
	return diff;
}

