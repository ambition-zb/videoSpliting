#include "VideoProcessing.h"
#include "Function_ColorHistogram.h"

// 定义静态成员变量
std::shared_ptr<VideoProcessing> VideoProcessing::instance = nullptr;
std::once_flag VideoProcessing::initFlag;

bool VideoProcessing::initializeEncoderContext(EncoderContext& enc_ctx)
{
	avformat_network_init();

	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) {
		cerr << "Could not find H264 encoder\n";
		return false;
	}

	enc_ctx.codec_ctx = avcodec_alloc_context3(codec);
	if (!enc_ctx.codec_ctx) {
		cerr << "Could not allocate codec context\n";
		return false;
	}

	enc_ctx.codec_ctx->width = nWidth;
	enc_ctx.codec_ctx->height = nHeight;
	enc_ctx.codec_ctx->time_base = { 1, 30 };
	enc_ctx.codec_ctx->framerate = { 30, 1 };
	enc_ctx.codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	enc_ctx.codec_ctx->gop_size = 12;
	enc_ctx.codec_ctx->max_b_frames = 2;

	if (avcodec_open2(enc_ctx.codec_ctx, codec, nullptr) < 0) {
		cerr << "Could not open codec\n";
		return false;
	}

	enc_ctx.yuv_frame = av_frame_alloc();
	if (!enc_ctx.yuv_frame) {
		cerr << "Could not allocate frame\n";
		return false;
	}

	enc_ctx.yuv_frame->format = AV_PIX_FMT_YUV420P;
	enc_ctx.yuv_frame->width = nWidth;
	enc_ctx.yuv_frame->height = nHeight;

	if (av_frame_get_buffer(enc_ctx.yuv_frame, 32) < 0) {
		cerr << "Could not allocate frame buffer\n";
		return false;
	}

	enc_ctx.sws_ctx = sws_getContext(nWidth, nHeight, AV_PIX_FMT_RGB24, nWidth, nHeight, AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);
	if (!enc_ctx.sws_ctx) {
		cerr << "Could not initialize SWS context\n";
		return false;
	}

	return true;
}

void VideoProcessing::releaseEncoderContext(EncoderContext& enc_ctx)
{
	if (enc_ctx.sws_ctx) {
		sws_freeContext(enc_ctx.sws_ctx);
	}
	if (enc_ctx.yuv_frame) {
		av_frame_free(&enc_ctx.yuv_frame);
	}
	if (enc_ctx.codec_ctx) {
		avcodec_free_context(&enc_ctx.codec_ctx);
	}
	avformat_network_deinit();
}

void VideoProcessing::spliting(string strFilePath)
{
	// 分配格式上下文
	AVFormatContext* fmt_ctx = avformat_alloc_context(); 
	if (avformat_open_input(&fmt_ctx, strFilePath.c_str(), NULL, NULL) != 0) { // 打开输入文件
		cerr << "Could not open input file" << endl;
		return;
	}
	cout << "split video:" << strFilePath << endl;

	// 获取流信息
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) { 
		cerr << "Could not find stream information" << endl;
		avformat_close_input(&fmt_ctx);
		return;
	}

	// 视频流索引
	int video_stream_index = -1; 
	for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) { // 查找视频流
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			break;
		}
	}
	if (video_stream_index == -1) { // 未找到视频流
		cerr << "Could not find video stream in the input, aborting" << endl;
		avformat_close_input(&fmt_ctx);
		return;
	}

	AVCodecParameters* codecpar = fmt_ctx->streams[video_stream_index]->codecpar; // 获取编解码参数
	const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id); // 查找解码器
	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec); // 分配编解码上下文
	avcodec_parameters_to_context(codec_ctx, codecpar); // 复制编解码参数到上下文
	avcodec_open2(codec_ctx, codec, NULL); // 打开编解码器

	nWidth = codecpar->width;   // 获取视频宽度
	nHeight = codecpar->height; // 获取视频高度

	AVFrame* frame = av_frame_alloc(); // 分配帧
	AVFrame* rgb_frame = av_frame_alloc(); // 分配RGB帧
	if (!frame || !rgb_frame) {
		cerr << "Could not allocate frames" << endl;
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&fmt_ctx);
		return;
	}
	// 设置宽度和高度
	rgb_frame->width = codec_ctx->width;
	rgb_frame->height = codec_ctx->height;

	int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 32); // 计算缓冲区大小
	uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t)); // 分配缓冲区
	if (!buffer) {
		cerr << "Could not allocate buffer" << endl;
		av_frame_free(&rgb_frame);
		av_frame_free(&frame);
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&fmt_ctx);
		return;
	}

	av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 32); // 填充RGB帧数据

	struct SwsContext* sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
		codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
		SWS_BILINEAR, NULL, NULL, NULL); // 初始化图像缩放上下文
	if (!sws_ctx) {
		cerr << "Could not initialize conversion context" << endl;
		av_free(buffer);
		av_frame_free(&rgb_frame);
		av_frame_free(&frame);
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&fmt_ctx);
		return;
	}

	vector<double> prev_hist, curr_hist; // 存储前一帧和当前帧的直方图
	double threshold = HISTOGRAM_DIFFERENCE_THRESHOLD; // 设置一个合理的阈值

	int nCount = 1;
	ptrScene scene = make_shared<Scene>();
	scene->m_strName = std::to_string(nCount);
	map_scene[strFilePath].push_back(scene);

	AVPacket packet; // 分配包
	while (av_read_frame(fmt_ctx, &packet) >= 0) { // 逐帧读取视频
		if (packet.stream_index == video_stream_index) { // 处理视频帧
			avcodec_send_packet(codec_ctx, &packet); // 发送包给解码器
			if (avcodec_receive_frame(codec_ctx, frame) == 0) { // 接收解码后的帧
				sws_scale(sws_ctx, (uint8_t const* const*)frame->data, frame->linesize, 0,
					codec_ctx->height, rgb_frame->data, rgb_frame->linesize); // 将帧转换为RGB格式

				CFunction_ColorHistogram::getInstance()->calculateHistogram(rgb_frame, curr_hist); // 计算当前帧的直方图
				if (!prev_hist.empty()) { // 如果前一帧的直方图存在
					double diff = CFunction_ColorHistogram::getInstance()->computeHistogramDifference(prev_hist, curr_hist); // 计算直方图之间的差异
					//cout << diff << endl;
					
					if (diff > threshold) { // 如果差异超过阈值
						//创建新的scene
						nCount++;
						scene = make_shared<Scene>();
						scene->m_strName = std::to_string(nCount);
						map_scene[strFilePath].push_back(scene);
					}
				}
				prev_hist = curr_hist; 
				scene->addFrame(frame);
			}
		}
		av_packet_unref(&packet); // 释放包
	}
	//saveSegment(input_filename, segment_start, frame_index - 1, segment_index); // 保存最后一个片段

	av_frame_free(&frame); // 释放帧 
	av_frame_free(&rgb_frame); // 释放RGB帧
	av_free(buffer); // 释放缓冲区
	avcodec_free_context(&codec_ctx); // 释放编解码上下文
	avformat_close_input(&fmt_ctx); // 关闭输入文件
}

void VideoProcessing::deleteInvalidScene()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "视频[" << it->first << "]删除无效scene:" << endl;
		for (auto itv = it->second.begin(); itv != it->second.end();)
		{
			if ((*itv)->getFrameSize() < 15) {
				cout << (*itv)->m_strName << "[" << (*itv)->getFrameSize() << "fps]" << endl;
				itv = it->second.erase(itv);  
			}
			else {
				++itv;  
			}
		}
	}
}

void VideoProcessing::randomDelete()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "视频[" << it->first << "]随机删帧:" << endl;
		for (ptrScene scene : it->second)
		{
			scene->randomDelete();
		}
	}
}

void VideoProcessing::randomResize()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "视频[" << it->first << "]随机缩放:" << endl;
		for (ptrScene scene : it->second)
			scene->randomResize();
	}
}

void VideoProcessing::randomSpeed()
{
	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		cout << "视频[" << it->first << "]随机播放速度:" << endl;
		for (ptrScene scene : it->second)
			scene->randomSpeed();
	}
}

void VideoProcessing::write2file()
{
	EncoderContext enc_ctx;
	if (!initializeEncoderContext(enc_ctx)) {
		cerr << "Failed to initialize encoder context" << endl;
		return;
	}

	std::map<string, vector<ptrScene>>::iterator it;
	for (it = map_scene.begin(); it != map_scene.end(); ++it)
	{
		string strFilePath = it->first;
		string strDirectory = getDirectoryPath(strFilePath);
		string strFileName = getFileNameWithoutExtension(strFilePath);
		string strFolderPath = createFolder(strDirectory, strFileName);
		cout << "视频[" << it->first << "]保存:" << endl;
		
		for (ptrScene scene : it->second)
		{
			string output_filename = "/segment_" + scene->m_strName + string(".mp4");
			//cout << "save video:" << strSegmentName << endl;

			avformat_alloc_output_context2(&enc_ctx.output_fmt_ctx, nullptr, nullptr, output_filename.c_str());
			if (!enc_ctx.output_fmt_ctx) {
				cerr << "Could not create output context\n";
				continue;
			}

			enc_ctx.stream = avformat_new_stream(enc_ctx.output_fmt_ctx, nullptr);
			if (!enc_ctx.stream) {
				cerr << "Could not create new stream\n";
				continue;
			}

			if (avcodec_parameters_from_context(enc_ctx.stream->codecpar, enc_ctx.codec_ctx) < 0) {
				cerr << "Could not copy codec parameters to stream\n";
				continue;
			}

			if (!(enc_ctx.output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
				if (avio_open(&enc_ctx.output_fmt_ctx->pb, output_filename.c_str(), AVIO_FLAG_WRITE) < 0) {
					cerr << "Could not open output file\n";
					continue;
				}
			}

			if (avformat_write_header(enc_ctx.output_fmt_ctx, nullptr) < 0) {
				cerr << "Could not write file header\n";
				continue;
			}

			int pts = 0;
			for (AVFrame* frame : scene->m_frames) {
				sws_scale(enc_ctx.sws_ctx, frame->data, frame->linesize, 0, nHeight, enc_ctx.yuv_frame->data, enc_ctx.yuv_frame->linesize);
				enc_ctx.yuv_frame->pts = pts++;

				if (avcodec_send_frame(enc_ctx.codec_ctx, enc_ctx.yuv_frame) < 0) {
					cerr << "Could not send frame to encoder\n";
					continue;
				}

				AVPacket* pkt = av_packet_alloc();
				if (!pkt) {
					cerr << "Could not allocate packet\n";
					continue;
				}

				while (avcodec_receive_packet(enc_ctx.codec_ctx, pkt) == 0) {
					av_packet_rescale_ts(pkt, enc_ctx.codec_ctx->time_base, enc_ctx.stream->time_base);
					pkt->stream_index = enc_ctx.stream->index;

					if (av_interleaved_write_frame(enc_ctx.output_fmt_ctx, pkt) < 0) {
						cerr << "Could not write frame to file\n";
						av_packet_unref(pkt);
						continue;
					}
					av_packet_unref(pkt);
				}
				av_packet_free(&pkt);
			}

			av_write_trailer(enc_ctx.output_fmt_ctx);

			if (!(enc_ctx.output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
				avio_closep(&enc_ctx.output_fmt_ctx->pb);
			}
			avformat_free_context(enc_ctx.output_fmt_ctx);
		}
	}

	releaseEncoderContext(enc_ctx);
}
