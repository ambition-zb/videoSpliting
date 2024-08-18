#pragma once
#include "Common.h"

class Encode
{
public:
	Encode(std::string filename);
	~Encode();

	std::string m_strFile;

	bool doTask(std::vector<AVFrame*>& video_frames, const int nWidth, const int nHeight, const int nFps);
};

