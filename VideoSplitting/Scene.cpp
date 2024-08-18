#include "Scene.h"
#include <random>
#include "Encode.h"


bool save_frame_as_jpeg(AVFrame* frame, const char* filename) {
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return false;
	}

	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		fprintf(stderr, "Could not allocate video codec context\n");
		return false;
	}

	codec_ctx->bit_rate = 400000;
	codec_ctx->width = frame->width;
	codec_ctx->height = frame->height;
	codec_ctx->time_base = { 1, 25 };
	codec_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;

	if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		avcodec_free_context(&codec_ctx);
		return false;
	}
	FILE* file;
	if (fopen_s(&file, filename, "wb") != 0) 
	{
		fprintf(stderr, "Could not open %s\n", filename);
		avcodec_free_context(&codec_ctx);
		return false;
	}

	AVPacket pkt{};
	pkt.data = NULL;
	pkt.size = 0;

	int ret = avcodec_send_frame(codec_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending frame to codec context\n");
		fclose(file);
		avcodec_free_context(&codec_ctx);
		return false;
	}

	ret = avcodec_receive_packet(codec_ctx, &pkt);
	if (ret < 0) {
		fprintf(stderr, "Error receiving packet from codec context\n");
		fclose(file);
		avcodec_free_context(&codec_ctx);
		return false;
	}

	fwrite(pkt.data, 1, pkt.size, file);

	av_packet_unref(&pkt);
	fclose(file);
	avcodec_free_context(&codec_ctx);

	return true;
}


Scene::Scene()
{
	m_strName = "";
	m_frames.clear();
}

Scene::~Scene()
{

}


void Scene::setVideoMessage(video_message* ptr)
{
	m_vd_message = ptr;
}

bool Scene::isEmpty()
{
	return m_frames.size() == 0;
}

int Scene::getFrameSize()
{
	return (int)m_frames.size();
}

void Scene::addFrame(AVFrame* mat)
{
	AVFrame* new_frame = av_frame_clone(mat);
	m_frames.push_back(new_frame);
}

void Scene::randomDelete()
{
	int size = (int)m_frames.size();
	int delSize = size / 10;
	int random_begin = random(1, delSize);
	int random_end = random(1, delSize);
	std::cout << "[" << size << "-->" << size- random_begin- random_end << "]" << std::endl;
	if (size > random_begin + random_end)
	{
		m_frames.erase(m_frames.begin(), m_frames.begin() + random_begin);
		m_frames.resize(m_frames.size() - random_end);
	}
}

void Scene::randomResize()
{
	float fSize = random(1.0f, 1.1f);
	//fSize = 1.5f;
	std::cout << m_strName << "[1.0" << "-->" << fSize << "]" << std::endl;

	std::vector<AVFrame*> m_frames_1;
	for (AVFrame* src_frame : m_frames)
	{
		int src_width = src_frame->width;
		int src_height = src_frame->height;
		int scale_width = static_cast<int>(src_width * fSize);
		int scale_height = static_cast<int>(src_height * fSize);

		// Allocate the scaled frame
		AVFrame* scaled_frame = av_frame_alloc();
		if (!scaled_frame) {
			fprintf(stderr, "Could not allocate scaled frame\n");
			continue;
		}

		scaled_frame->format = (AVPixelFormat)src_frame->format;
		scaled_frame->width = scale_width;
		scaled_frame->height = scale_height;

		// Allocate buffer for the scaled frame
		if (av_image_alloc(scaled_frame->data, scaled_frame->linesize, scale_width, scale_height, (AVPixelFormat)src_frame->format, 1) < 0) {
			fprintf(stderr, "Could not allocate scaled image buffer\n");
			av_frame_free(&scaled_frame);
			continue;
		}

		// Create scaling context for scaling up
		struct SwsContext* sws_ctx = sws_getContext(
			src_width, src_height, (AVPixelFormat)src_frame->format,
			scale_width, scale_height, (AVPixelFormat)src_frame->format,
			SWS_BILINEAR, NULL, NULL, NULL
		);

		if (!sws_ctx) {
			fprintf(stderr, "Could not create scaling context\n");
			av_freep(&scaled_frame->data[0]);
			av_frame_free(&scaled_frame);
			continue;
		}

		// Scale the image
		sws_scale(
			sws_ctx, src_frame->data, src_frame->linesize, 0, src_height,
			scaled_frame->data, scaled_frame->linesize
		);

		// Free the scaling context
		sws_freeContext(sws_ctx);

		// Allocate the cropped frame
		AVFrame* cropped_frame = av_frame_alloc();
		if (!cropped_frame) {
			fprintf(stderr, "Could not allocate cropped frame\n");
			av_freep(&scaled_frame->data[0]);
			av_frame_free(&scaled_frame);
			continue;
		}

		cropped_frame->format = (AVPixelFormat)src_frame->format;
		cropped_frame->width = src_width;
		cropped_frame->height = src_height;

		// Allocate buffer for the cropped frame
		if (av_image_alloc(cropped_frame->data, cropped_frame->linesize, src_width, src_height, (AVPixelFormat)src_frame->format, 1) < 0) {
			fprintf(stderr, "Could not allocate cropped image buffer\n");
			av_freep(&scaled_frame->data[0]);
			av_frame_free(&scaled_frame);
			av_frame_free(&cropped_frame);
			continue;
		}

		// Create a new scaling context for cropping
		struct SwsContext* crop_sws_ctx = sws_getContext(
			scale_width, scale_height, (AVPixelFormat)scaled_frame->format,
			src_width, src_height, (AVPixelFormat)cropped_frame->format,
			SWS_BILINEAR, NULL, NULL, NULL
		);

		if (!crop_sws_ctx) {
			fprintf(stderr, "Could not create cropping scaling context\n");
			av_freep(&scaled_frame->data[0]);
			av_frame_free(&scaled_frame);
			av_freep(&cropped_frame->data[0]);
			av_frame_free(&cropped_frame);
			continue;
		}

		// Calculate the top-left corner of the cropping rectangle based on the center
		int crop_x = (scale_width - src_width) / 2;
		int crop_y = (scale_height - src_height) / 2;

		// Copy the cropped portion from the source frame to the cropped frame
		// Copy Y plane
		for (int y = 0; y < src_height; y++) {
			memcpy(cropped_frame->data[0] + y * cropped_frame->linesize[0],
				scaled_frame->data[0] + (y + crop_y) * scaled_frame->linesize[0] + crop_x,
				src_width);
		}

		// Copy U and V planes
		crop_x /= 2;
		crop_y /= 2;
		src_width /= 2;
		src_height /= 2;

		for (int y = 0; y < src_height; y++) {
			memcpy(cropped_frame->data[1] + y * cropped_frame->linesize[1],
				scaled_frame->data[1] + (y + crop_y) * scaled_frame->linesize[1] + crop_x,
				src_width);

			memcpy(cropped_frame->data[2] + y * cropped_frame->linesize[2],
				scaled_frame->data[2] + (y + crop_y) * scaled_frame->linesize[2] + crop_x,
				src_width);
		}

		//save_frame_as_jpeg(src_frame, "1.jpg");
		//save_frame_as_jpeg(cropped_frame, "2.jpg");
		//save_frame_as_jpeg(scaled_frame, "3.jpg");
		//av_freep(&src_frame->data[0]);
		//src_frame = av_frame_clone(cropped_frame);
		m_frames_1.push_back(cropped_frame); 
		// Free the scaled frame
		av_freep(&scaled_frame->data[0]);
		av_frame_free(&scaled_frame);
	}
	m_frames.swap(m_frames_1);
	for (AVFrame* frame : m_frames_1)
	{
		av_frame_unref(frame);   
		av_frame_free(&frame);   
	}
}

void Scene::randomSpeed()
{
	int speed = 100;
	int size = (int)m_frames.size();
	if (size < 34)
		speed = 90;
	else
		speed = 110;

	float fSpeed = speed / 100.0f;
	std::cout << m_strName << "[1.0" << "-->" << fSpeed << "]";
	//float fSpeed = 0.8f;
	std::vector<AVFrame*> frames;
	
	//减速 加帧
	if (fSpeed < 1.0f)
	{
		//int m = int(1.0 / (1.0f - fSpeed));
		int size_new = int(size / fSpeed);
		int nAdd = size_new - size;
		if (nAdd == 0)
			return;
		int nPoint = size / nAdd;
		for (int i = 0; i < size; i++)
		{
			frames.push_back(m_frames[i]);
			if (i % nPoint == 0 && i + 1 < size)
			{
				//cv::Mat interpolatedFrame;
				//插一帧，透明度0.5
				AVFrame* interpolatedFrame = interpolate_frames_bicubic(m_frames[i], m_frames[i + 1], 0.5f);
				//AVFrame* interpolatedFrame1 = interpolate_frames(m_frames[i], m_frames[i + 1], 0.5f);
				//cv::addWeighted(m_frames[i], 0.5f, m_frames[i + 1], 0.5f, 0, interpolatedFrame);
				//outputFilename = "output_image2.jpg";
				//bool result2 = cv::imwrite(outputFilename, interpolatedFrame);
				frames.push_back(interpolatedFrame);

				//save_frame_as_jpeg(m_frames[i], "1.jpg");
				//save_frame_as_jpeg(interpolatedFrame, "2.jpg");
				//save_frame_as_jpeg(m_frames[i + 1], "3.jpg");
				//save_frame_as_jpeg(interpolatedFrame1, "4.jpg");
			}
		}
		m_frames = std::move(frames);
	}
	//加速 删帧
	else
	{
		int index = 0;
		int index_new = 1;
		while (index < size)
		{
			frames.push_back(m_frames[index]);
			index_new++;
			index = int(fSpeed * index_new);
		}
		//删掉不用的帧数据
		for (int i = size - 1; i >= 0; i--)
		{
			if (std::find(frames.begin(), frames.end(), m_frames[i]) != frames.end())
				continue;
			av_frame_unref(m_frames[i]);
			av_frame_free(&m_frames[i]);
			m_frames[i] = nullptr;
		}
		m_frames = std::move(frames);
	}
	std::cout  << "[" << size << "-->" << m_frames.size() << "]" << std::endl;
}

int Scene::random(int min, int max)
{
	// 创建随机数生成器
	std::random_device rd;  // 用于种子
	std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器

	// 生成一个在 [min, max] 范围内的随机数
	std::uniform_int_distribution<int> distr(min, max);

	// 生成随机数
	int random_number = distr(gen);

	return random_number;
}

float Scene::random(float min, float max)
{
	// 创建随机数生成器
	std::random_device rd;  // 用于种子
	std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器

	// 生成一个在 [min, max] 范围内的随机数
	std::uniform_real_distribution<float> distr(min, max);

	// 生成随机数
	float random_number = distr(gen);

	return random_number;
}

bool Scene::write2file()
{
	if (m_vd_message == nullptr)
		return false;

	std::string strName = Message::getInstance()->getTempFolder() + "\\" + std::to_string(Message::getInstance()->getSceneIndex()) + ".mp4";
	Encode encode(strName);
	if(encode.doTask(m_frames, m_vd_message->nWidth, m_vd_message->nHeight, m_vd_message->nFps))
		std::cout << "save video: " + strName << std::endl;

	return true;
}

AVFrame* Scene::interpolate_frames(AVFrame* frame1, AVFrame* frame2, float alpha) 
{
	if (frame1->format != frame2->format || frame1->width != frame2->width || frame1->height != frame2->height) {
		std::cerr << "Frames must have the same format and dimensions for interpolation." << std::endl;
		return nullptr;
	}

	// Allocate the new frame
	AVFrame* interpolated_frame = av_frame_alloc();
	if (!interpolated_frame) {
		std::cerr << "Could not allocate interpolated frame." << std::endl;
		return nullptr;
	}

	interpolated_frame->format = frame1->format;
	interpolated_frame->width = frame1->width;
	interpolated_frame->height = frame1->height;

	// Allocate buffer for the interpolated frame
	if (av_image_alloc(interpolated_frame->data, interpolated_frame->linesize, frame1->width, frame1->height, (AVPixelFormat)frame1->format, 1) < 0) {
		std::cerr << "Could not allocate interpolated image buffer." << std::endl;
		av_frame_free(&interpolated_frame);
		return nullptr;
	}

	// Perform linear interpolation between frame1 and frame2
	for (int plane = 0; plane < 3; plane++) {
		int height = (plane == 0) ? frame1->height : frame1->height / 2;
		int width = (plane == 0) ? frame1->width : frame1->width / 2;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				interpolated_frame->data[plane][y * interpolated_frame->linesize[plane] + x] =
					static_cast<uint8_t>((1.0 - alpha) * frame1->data[plane][y * frame1->linesize[plane] + x] +
						alpha * frame2->data[plane][y * frame2->linesize[plane] + x]);
			}
		}
	}

	return interpolated_frame;
}

// Function to create an interpolated frame using bicubic interpolation
AVFrame* Scene::interpolate_frames_bicubic(AVFrame* frame1, AVFrame* frame2, float alpha) 
{
	if (frame1->format != frame2->format || frame1->width != frame2->width || frame1->height != frame2->height) {
		std::cerr << "Frames must have the same format and dimensions for interpolation." << std::endl;
		return nullptr;
	}

	// Allocate the new frame
	AVFrame* interpolated_frame = av_frame_alloc();
	if (!interpolated_frame) {
		std::cerr << "Could not allocate interpolated frame." << std::endl;
		return nullptr;
	}

	interpolated_frame->format = frame1->format;
	interpolated_frame->width = frame1->width;
	interpolated_frame->height = frame1->height;

	// Allocate buffer for the interpolated frame
	if (av_image_alloc(interpolated_frame->data, interpolated_frame->linesize, frame1->width, frame1->height, (AVPixelFormat)frame1->format, 1) < 0) {
		std::cerr << "Could not allocate interpolated image buffer." << std::endl;
		av_frame_free(&interpolated_frame);
		return nullptr;
	}

	// Create scaling context for bicubic interpolation
	struct SwsContext* sws_ctx = sws_getContext(
		frame1->width, frame1->height, (AVPixelFormat)frame1->format,
		frame1->width, frame1->height, (AVPixelFormat)frame1->format,
		SWS_BICUBIC, NULL, NULL, NULL
	);

	if (!sws_ctx) {
		std::cerr << "Could not create scaling context." << std::endl;
		av_freep(&interpolated_frame->data[0]);
		av_frame_free(&interpolated_frame);
		return nullptr;
	}

	// Perform bicubic interpolation between frame1 and frame2
	for (int plane = 0; plane < 3; plane++) {
		int height = (plane == 0) ? frame1->height : frame1->height / 2;
		int width = (plane == 0) ? frame1->width : frame1->width / 2;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int val1 = frame1->data[plane][y * frame1->linesize[plane] + x];
				int val2 = frame2->data[plane][y * frame2->linesize[plane] + x];
				int interpolated_val = static_cast<int>((1.0 - alpha) * val1 + alpha * val2);

				interpolated_frame->data[plane][y * interpolated_frame->linesize[plane] + x] = interpolated_val;
			}
		}
	}

	// Scale the interpolated frame to enhance quality using bicubic interpolation
	sws_scale(sws_ctx, interpolated_frame->data, interpolated_frame->linesize, 0, interpolated_frame->height, interpolated_frame->data, interpolated_frame->linesize);

	// Free the scaling context
	sws_freeContext(sws_ctx);

	return interpolated_frame;
}