#pragma once
#include "Common.h"
#include "Scene.h"


class CFunction_ColorHistogram
{
public:
	// 禁止拷贝构造和赋值操作
	CFunction_ColorHistogram(const CFunction_ColorHistogram&) = delete;
	CFunction_ColorHistogram& operator=(const CFunction_ColorHistogram&) = delete;

	// 提供一个静态方法来获取单例实例
	static std::shared_ptr<CFunction_ColorHistogram> getInstance() {
		std::call_once(initFlag, []() {
			instance.reset(new CFunction_ColorHistogram());
			});
		return instance;
	}
	
	//void doTask(string& strFilePath);
	Mat calculateHistogram(const Mat& frame);
	double histogramDifference(const Mat& hist1, const Mat& hist2);

	void calculateHistogram(const AVFrame* frame, vector<double>& hist);
	double computeHistogramDifference(const vector<double>& hist1, const vector<double>& hist2);
private:
	// 私有构造函数，禁止从外部创建实例
	CFunction_ColorHistogram() {
		std::cout << "CFunction_ColorHistogram instance created." << std::endl;
	}

	static std::shared_ptr<CFunction_ColorHistogram> instance;
	static std::once_flag initFlag;
};



