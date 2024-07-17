#pragma once
#include "Common.h"

class Scene
{
public:
	Scene();
	~Scene();

	bool isEmpty();
	int getFrameSize();
	void addFrame(const Mat& mat);
	void randomDelete();
	void randomResize();
	void randomSpeed();
	int random(int min, int max);
	float random(float min, float max);
	bool write2file(string strFilePath);

	string m_strName;
	vector<Mat> m_frames; // 保存当前镜头的所有帧
};

typedef std::shared_ptr<Scene> ptrScene;