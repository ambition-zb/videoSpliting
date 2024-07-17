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
	vector<Mat> m_frames; // ���浱ǰ��ͷ������֡
};

typedef std::shared_ptr<Scene> ptrScene;