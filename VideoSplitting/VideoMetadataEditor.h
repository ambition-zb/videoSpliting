#pragma once
#include "Common.h"

class VideoMetadataEditor
{
public:
	VideoMetadataEditor();
	bool modifyDuration(const std::string& inputPath, const std::string& outputPath, int newDurationSeconds);

private:
	void cleanup(AVFormatContext* inCtx, AVFormatContext* outCtx);
};

