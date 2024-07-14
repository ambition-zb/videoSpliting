#pragma once
#include "Common.h"
#include "Scene.h"

class VideoProcessing
{
public:
	// ��ֹ��������͸�ֵ����
	VideoProcessing(const VideoProcessing&) = delete;
	VideoProcessing& operator=(const VideoProcessing&) = delete;

	// �ṩһ����̬��������ȡ����ʵ��
	static std::shared_ptr<VideoProcessing> getInstance() {
		std::call_once(initFlag, []() {
			instance.reset(new VideoProcessing());
			});
		return instance;
	}

	void spliting(string strFilePath);
	//���ɾ��ͷβ��֡����
	void randomDelete();
	//������� 1.0-1.1
	void randomResize();
	//�������
	void randomSpeed();
	void write2file();
private:
	// ˽�й��캯������ֹ���ⲿ����ʵ��
	VideoProcessing() {
		std::cout << "CFunction_ColorHistogram instance created." << std::endl;
	}

	static std::shared_ptr<VideoProcessing> instance;
	static std::once_flag initFlag;

	//string m_strFilePath;
	//����
	map<string, vector<ptrScene>> map_scene;
};

