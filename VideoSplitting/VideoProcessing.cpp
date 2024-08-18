#include "VideoProcessing.h"
#include "Function_ColorHistogram.h"
#include "Decode.h"

// ���徲̬��Ա����
std::shared_ptr<VideoProcessing> VideoProcessing::instance = nullptr;
std::once_flag VideoProcessing::initFlag;


void VideoProcessing::spliting(std::string strFilePath)
{
	av_log_set_level(AV_LOG_QUIET); // ������־����Ϊ��Ĭ�����ر���־���

	DecodePtr pDecode = std::make_shared<Decode>(strFilePath.c_str());
	map_decode[strFilePath] = pDecode;
	pDecode->doTask();
}

void VideoProcessing::deleteInvalidScene()
{
	std::map<std::string, DecodePtr>::iterator it;
	for (it = map_decode.begin(); it != map_decode.end(); ++it)
	{
		std::cout << "��Ƶ[" << it->first << "]ɾ����Чscene:" << std::endl;
		it->second->deleteInvalidScene();
	}
}

void VideoProcessing::randomDelete()
{
	std::map<std::string, DecodePtr>::iterator it;
	for (it = map_decode.begin(); it != map_decode.end(); ++it)
	{
		std::cout << "��Ƶ[" << it->first << "]���ɾ֡:" << std::endl;
		it->second->randomDeleteFrame();
	}
}

void VideoProcessing::randomResize()
{
	std::map<std::string, DecodePtr>::iterator it;
	for (it = map_decode.begin(); it != map_decode.end(); ++it)
	{
		std::cout << "��Ƶ[" << it->first << "]�������:" << std::endl;
		it->second->randomResize();
	}
}

void VideoProcessing::randomSpeed()
{
	std::map<std::string, DecodePtr>::iterator it;
	for (it = map_decode.begin(); it != map_decode.end(); ++it)
	{
		std::cout << "��Ƶ[" << it->first << "]��������ٶ�:" << std::endl;
		it->second->randomSpeed();
	}
}


void VideoProcessing::write2file()
{
	std::map<std::string, DecodePtr>::iterator it;
	for (it = map_decode.begin(); it != map_decode.end(); ++it)
	{
		it->second->write2file();
	}
}
