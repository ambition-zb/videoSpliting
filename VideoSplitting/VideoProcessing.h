#pragma once
#include "Common.h"
#include "Scene.h"

class VideoProcessing
{
public:
	// 禁止拷贝构造和赋值操作
	VideoProcessing(const VideoProcessing&) = delete;
	VideoProcessing& operator=(const VideoProcessing&) = delete;

	// 提供一个静态方法来获取单例实例
	static std::shared_ptr<VideoProcessing> getInstance() {
		std::call_once(initFlag, []() {
			instance.reset(new VideoProcessing());
			});
		return instance;
	}

	void spliting(string strFilePath);
	//随机删除头尾几帧画面
	void randomDelete();
	//随机缩放 1.0-1.1
	void randomResize();
	//随机变速
	void randomSpeed();
	void write2file();
private:
	// 私有构造函数，禁止从外部创建实例
	VideoProcessing() {
		std::cout << "CFunction_ColorHistogram instance created." << std::endl;
	}

	static std::shared_ptr<VideoProcessing> instance;
	static std::once_flag initFlag;

	//string m_strFilePath;
	//场景
	map<string, vector<ptrScene>> map_scene;
};

