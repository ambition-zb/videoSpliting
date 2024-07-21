#pragma once
#include "Common.h"



class Scene
{
public:
	Scene();
	~Scene();

	

	bool isEmpty();
	int getFrameSize();
	void addFrame(AVFrame* mat);
	void randomDelete();
	void randomResize();
	void randomSpeed();
	int random(int min, int max);
	float random(float min, float max);
	bool write2file(string strFilePath);

	string m_strName;
	vector<AVFrame*> m_frames;
};

typedef std::shared_ptr<Scene> ptrScene;