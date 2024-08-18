#pragma once
#include "Common.h"
#include "Decode.h"

struct EncoderContext {
	AVFormatContext* output_fmt_ctx = nullptr;
	AVCodecContext* codec_ctx = nullptr;
	AVStream* stream = nullptr;
	AVFrame* yuv_frame = nullptr;
	SwsContext* sws_ctx = nullptr;
};

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

	void spliting(std::string strFilePath);
	//ɾ����Ч��scene
	void deleteInvalidScene();
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
	}

	static std::shared_ptr<VideoProcessing> instance;
	static std::once_flag initFlag;

	//string m_strFilePath;
	//����
	std::map<std::string, DecodePtr> map_decode;
	int nWidth = 0;
	int nHeight = 0;

};

