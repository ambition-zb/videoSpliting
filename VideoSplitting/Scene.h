#pragma once
#include "Common.h"
#include "Proto.h"


class Scene
{
public:
	Scene();
	~Scene();

	void setVideoMessage(video_message* ptr);

	bool isEmpty();
	int getFrameSize();
	void addFrame(AVFrame* mat);
	void randomDelete();
	void randomResize();
	void randomSpeed();
	bool write2file();

private:
	int random(int min, int max);
	float random(float min, float max);
	AVFrame* interpolate_frames(AVFrame* frame1, AVFrame* frame2, float alpha);
	AVFrame* interpolate_frames_bicubic(AVFrame* frame1, AVFrame* frame2, float alpha);
	std::string m_strName;
	std::vector<AVFrame*> m_frames;
	video_message* m_vd_message = nullptr;
};

typedef std::shared_ptr<Scene> ScenePtr;