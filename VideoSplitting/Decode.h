#pragma once
#include "Common.h"
#include "Scene.h"
#include "Proto.h"
class Decode
{
public:
	Decode(std::string filename);
	~Decode();

	bool doTask();
	bool write2file();
	void deleteInvalidScene();
	void randomDeleteFrame();
	void randomResize();
	void randomSpeed();
private:
	std::string m_strFile;

	video_message vd_msg;
	std::vector<AVFrame*> m_video_frames;
	std::vector<ScenePtr> m_scenes;
};

typedef std::shared_ptr<Decode> DecodePtr;