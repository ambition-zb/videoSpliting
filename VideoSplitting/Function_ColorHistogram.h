#pragma once
#include "Common.h"


class CFunction_ColorHistogram
{
public:
	// ��ֹ��������͸�ֵ����
	CFunction_ColorHistogram(const CFunction_ColorHistogram&) = delete;
	CFunction_ColorHistogram& operator=(const CFunction_ColorHistogram&) = delete;

	// �ṩһ����̬��������ȡ����ʵ��
	static std::shared_ptr<CFunction_ColorHistogram> getInstance() {
		std::call_once(initFlag, []() {
			instance.reset(new CFunction_ColorHistogram());
			});
		return instance;
	}
	
	void doTask(string& strFilePath);

private:
	// ˽�й��캯������ֹ���ⲿ����ʵ��
	CFunction_ColorHistogram() {
		std::cout << "CFunction_ColorHistogram instance created." << std::endl;
	}

	static std::shared_ptr<CFunction_ColorHistogram> instance;
	static std::once_flag initFlag;

	Mat calculateHistogram(const Mat& frame);
	double histogramDifference(const Mat& hist1, const Mat& hist2);
};



