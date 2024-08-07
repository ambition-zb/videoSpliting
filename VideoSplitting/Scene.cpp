#include "Scene.h"
#include <random>

Scene::Scene()
{
	m_strName = "";
	m_frames.clear();
}

Scene::~Scene()
{

}


bool Scene::isEmpty()
{
	return m_frames.size() == 0;
}

int Scene::getFrameSize()
{
	return (int)m_frames.size();
}

void Scene::addFrame(AVFrame* mat)
{
	AVFrame* new_frame = av_frame_clone(mat);
	m_frames.push_back(new_frame);
	//Mat frame(mat);
	//m_frames.push_back(frame);
	// 分配新AVFrame
	/*
	AVFrame* dst = av_frame_alloc();
	if (!dst) {
		fprintf(stderr, "Could not allocate AVFrame\n");
		return NULL;
	}

	// 拷贝AVFrame的元数据
	if (av_frame_ref(dst, src) < 0) {
		fprintf(stderr, "Could not reference AVFrame\n");
		av_frame_free(&dst);
		return NULL;
	}

	// 为新AVFrame分配数据缓冲区
	if (av_image_alloc(dst->data, dst->linesize, src->width, src->height, src->format, 32) < 0) {
		fprintf(stderr, "Could not allocate image buffer\n");
		av_frame_free(&dst);
		return NULL;
	}

	// 拷贝图像数据
	av_image_copy(dst->data, dst->linesize, (const uint8_t**)src->data, src->linesize, src->format, src->width, src->height);

	return dst;
	*/
}

void Scene::randomDelete()
{
	/*
	int size = (int)m_frames.size();
	int delSize = size / 10;
	int random_begin = random(1, delSize);
	int random_end = random(1, delSize);
	cout << m_strName << "[" << size << "-->" << size- random_begin- random_end << "]" << endl;
	if (size > random_begin + random_end)
	{
		m_frames.erase(m_frames.begin(), m_frames.begin() + random_begin);
		m_frames.resize(m_frames.size() - random_end);
	}
	*/
}

void Scene::randomResize()
{
	/*
	float fSize = random(1.0f, 1.1f);
	cout << m_strName << "[1.0" << "-->" << fSize << "]" << endl;
	for (Mat& frame : m_frames) 
	{
		// 缩放图像
		cv::Mat resized_frame;
		cv::resize(frame, resized_frame, cv::Size(), fSize, fSize);

		// 定义目标图像的尺寸
		cv::Size targetSize(frame.cols, frame.rows);

		// 计算裁剪的起始位置
		int startX = (resized_frame.cols - targetSize.width) / 2;
		int startY = (resized_frame.rows - targetSize.height) / 2;

		// 定义裁剪区域
		cv::Rect cropRegion(startX, startY, targetSize.width, targetSize.height);

		// 裁剪图像的中心部分
		cv::Mat croppedImage = resized_frame(cropRegion);

		frame = cv::Mat(croppedImage);
	}
	*/
}

void Scene::randomSpeed()
{
	/*
	int speed = 100;
	int size = (int)m_frames.size();
// 	if (size < 30)
// 		speed = random(70, 89);
// 	else if(size > 30 && size < 50)
// 		speed = random(90, 129);
// 	else
// 		speed = random(130, 169);
	if (size < 34)
		speed = 90;
	else
		speed = 110;

	float fSpeed = speed / 100.0f;
	cout << m_strName << "[1.0" << "-->" << fSpeed << "]" << endl;
	//float fSpeed = 0.8f;
	vector<Mat> frames;
	
	//减速 加帧
	if (fSpeed < 1.0f)
	{
		//int m = int(1.0 / (1.0f - fSpeed));
		int size_new = int(size / fSpeed);
		int nAdd = size_new - size;
		if (nAdd == 0)
			return;
		int nPoint = size / nAdd;
		for (int i = 0; i < size; i++)
		{
			frames.push_back(m_frames[i]);
			if (i % nPoint == 0 && i + 1 < size)
			{
				//std::string outputFilename = "output_image1.jpg";
				//bool result1 = cv::imwrite(outputFilename, m_frames[i]);
				//outputFilename = "output_image3.jpg";
				//bool result3 = cv::imwrite(outputFilename, m_frames[i+1]);

				cv::Mat interpolatedFrame;
				//插一帧，透明度0.5
				cv::addWeighted(m_frames[i], 0.5f, m_frames[i + 1], 0.5f, 0, interpolatedFrame);
				//outputFilename = "output_image2.jpg";
				//bool result2 = cv::imwrite(outputFilename, interpolatedFrame);
				frames.push_back(interpolatedFrame);
			}
		}
	}
	//加速 删帧
	else
	{
		int index = 0;
		int index_new = 1;
		while (index < size)
		{
			frames.push_back(m_frames[index]);
			index_new++;
			index = int(fSpeed * index_new);
		}
	}
	m_frames.clear();
	copy(frames.begin(), frames.end(), back_inserter(m_frames));
	*/
}

int Scene::random(int min, int max)
{
	// 创建随机数生成器
	std::random_device rd;  // 用于种子
	std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器

	// 生成一个在 [min, max] 范围内的随机数
	std::uniform_int_distribution<int> distr(min, max);

	// 生成随机数
	int random_number = distr(gen);

	return random_number;
}

float Scene::random(float min, float max)
{
	// 创建随机数生成器
	std::random_device rd;  // 用于种子
	std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器

	// 生成一个在 [min, max] 范围内的随机数
	std::uniform_real_distribution<float> distr(min, max);

	// 生成随机数
	float random_number = distr(gen);

	return random_number;
}

bool Scene::write2file(string strFilePath)
{
	/*
	if (m_frames.size() < 1)
		return false;
	string strSegmentName = "/segment_" + m_strName + string(".avi");
	stringstream ss;
	VideoWriter writer(strFilePath + strSegmentName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, m_frames[0].size());
	for (const Mat& frame : m_frames) {
		writer.write(frame);
	}
	cout << strSegmentName << endl;
	writer.release();
	*/
	return false;
}
