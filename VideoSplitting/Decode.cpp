#include "Decode.h"
#include "Function_ColorHistogram.h"

Decode::Decode(std::string filename)
{
	m_strFile = filename;
}

Decode::~Decode()
{

}

bool Decode::doTask()
{
	AVFormatContext* format_ctx = nullptr;
	AVCodecContext* codec_ctx = nullptr;
	const AVCodec* codec = nullptr;
	int video_stream_index = -1;

 	if (avformat_open_input(&format_ctx, m_strFile.c_str(), nullptr, nullptr) < 0) {
		std::cerr << "无法打开输入文件。\n";
		return false;
	}

	if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
		std::cerr << "无法找到流信息。\n";
		avformat_close_input(&format_ctx);
		return false;
	}

	// 查找视频流
	for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
		if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			break;
		}
	}

	if (video_stream_index == -1) {
		std::cerr << "找不到视频流。\n";
		avformat_close_input(&format_ctx);
		return false;
	}

	AVCodecParameters* codecpar = format_ctx->streams[video_stream_index]->codecpar;
	codec = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id);
	if (!codec) {
		std::cerr << "找不到解码器。\n";
		avformat_close_input(&format_ctx);
		return false;
	}

	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		std::cerr << "无法分配解码器上下文。\n";
		avformat_close_input(&format_ctx);
		return false;
	}

	if (avcodec_parameters_to_context(codec_ctx, format_ctx->streams[video_stream_index]->codecpar) < 0) {
		std::cerr << "无法复制解码器参数到上下文。\n";
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return false;
	}

	if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
		std::cerr << "无法打开解码器。\n";
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return false;
	}

	vd_msg.nWidth = codec_ctx->width;
	vd_msg.nHeight = codec_ctx->height;
	vd_msg.nFps = static_cast<int>(av_q2d(format_ctx->streams[video_stream_index]->avg_frame_rate));
	std::cout << m_strFile + " 视频帧率：" << vd_msg.nFps << std::endl;
	vd_msg.nBitRate = (int)codecpar->bit_rate;
	std::cout << m_strFile + " 视频码率：" << vd_msg.nBitRate << std::endl;
	

	// 初始化 AVPacket
	AVPacket packet = {}; 
	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		std::cerr << "无法分配帧" << std::endl;
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return false;
	}
	AVFrame* rgb_frame = av_frame_alloc();
	if (!rgb_frame) {
		std::cerr << "无法分配RGB帧" << std::endl;
		av_frame_free(&frame);
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return false;
	}
	rgb_frame->width = codec_ctx->width;
	rgb_frame->height = codec_ctx->height;

	int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
	uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
	av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);

	SwsContext* sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
		codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
		SWS_BILINEAR, nullptr, nullptr, nullptr);

	// 存储前一帧和当前帧的直方图
	std::vector<double> prev_hist, curr_hist;
	// 设置一个合理的阈值
	double threshold = HISTOGRAM_DIFFERENCE_THRESHOLD; 
	//新建一个Scene
	ScenePtr scene = std::make_shared<Scene>();
	m_scenes.push_back(scene);
	scene->setVideoMessage(&vd_msg);

	while (av_read_frame(format_ctx, &packet) >= 0) {
		if (packet.stream_index == video_stream_index) {
			if (avcodec_send_packet(codec_ctx, &packet) >= 0) {
				while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
					AVFrame* frame_copy = av_frame_clone(frame);
					if (frame_copy) {
						m_video_frames.push_back(frame_copy);
					}
					//to rgb
					sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, rgb_frame->data, rgb_frame->linesize);
					
					// 计算当前帧的直方图
					CFunction_ColorHistogram::getInstance()->calculateHistogram(rgb_frame, curr_hist); 
					// 如果前一帧的直方图存在
					if (!prev_hist.empty()) { 
						// 计算直方图之间的差异
						double diff = CFunction_ColorHistogram::getInstance()->computeHistogramDifference(prev_hist, curr_hist); 
						//std::cout << diff << std::endl;
						
						// 如果差异超过阈值
						if (diff > threshold) { 
							//创建新的scene
							scene = std::make_shared<Scene>();
							m_scenes.push_back(scene);
							scene->setVideoMessage(&vd_msg);
							
						}
					}
					prev_hist = curr_hist;
					scene->addFrame(frame);
				}
			}
		}
		av_packet_unref(&packet);
	}
	av_frame_free(&frame);
	avcodec_free_context(&codec_ctx);
	avformat_close_input(&format_ctx);
	return true;
}

bool Decode::write2file()
{
	for (size_t i = 0; i < m_scenes.size(); i++)
		m_scenes[i]->write2file();
	return true;
}

void Decode::deleteInvalidScene()
{
	for (auto itv = m_scenes.begin(); itv != m_scenes.end();)
	{
		if ((*itv)->getFrameSize() < 15) {
			std::cout << m_strFile << "[" << (*itv)->getFrameSize() << "fps--deleted]" << std::endl;
			itv = m_scenes.erase(itv);
		}
		else {
			++itv;
		}
	}
}

void Decode::randomDeleteFrame()
{
	for (ScenePtr scene : m_scenes)
		scene->randomDelete();
}

void Decode::randomResize()
{
	for (ScenePtr scene : m_scenes)
		scene->randomResize();
}

void Decode::randomSpeed()
{
	for (ScenePtr scene : m_scenes)
		scene->randomSpeed();
}
