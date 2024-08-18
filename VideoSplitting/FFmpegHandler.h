extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
}
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>

bool saveFrameAsJPEG(AVFrame* frame, const std::string& filename, int width, int height) {
	// 查找MJPEG编码器
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	if (!codec) {
		std::cerr << "Codec not found\n";
		return false;
	}

	// 创建编码器上下文
	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		std::cerr << "Could not allocate codec context\n";
		return false;
	}

	codec_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
	codec_ctx->height = height;
	codec_ctx->width = width;
	codec_ctx->time_base = { 1, 25 };

	if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
		std::cerr << "Could not open codec\n";
		avcodec_free_context(&codec_ctx);
		return false;
	}

	AVPacket* pkt = av_packet_alloc();
	if (!pkt) {
		std::cerr << "Could not allocate AVPacket\n";
		avcodec_free_context(&codec_ctx);
		return false;
	}

	int ret = avcodec_send_frame(codec_ctx, frame);
	if (ret < 0) {
		std::cerr << "Error sending frame to codec context\n";
		av_packet_free(&pkt);
		avcodec_free_context(&codec_ctx);
		return false;
	}

	ret = avcodec_receive_packet(codec_ctx, pkt);
	if (ret < 0) {
		std::cerr << "Error receiving packet from codec context\n";
		av_packet_free(&pkt);
		avcodec_free_context(&codec_ctx);
		return false;
	}

	// 将数据包写入文件
	FILE* file;
	errno_t err = fopen_s(&file, filename.c_str(), "wb");
	if (err != 0) {
		std::cerr << "Could not open file\n";
		av_packet_free(&pkt);
		avcodec_free_context(&codec_ctx);
		return false;
	}
	fwrite(pkt->data, 1, pkt->size, file);
	fclose(file);

	av_packet_free(&pkt);
	avcodec_free_context(&codec_ctx);

	return true;
}
// 解码视频，将每一帧存储到 std::vector<AVFrame*> 中
std::vector<AVFrame*> decode_video(const char* filename) {
	std::vector<AVFrame*> frames;
	AVFormatContext* format_ctx = nullptr;
	AVCodecContext* codec_ctx = nullptr;
	const AVCodec* codec = nullptr;
	int video_stream_index = -1;

	if (avformat_open_input(&format_ctx, filename, nullptr, nullptr) < 0) {
		std::cerr << "无法打开输入文件。\n";
		return frames;
	}

	if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
		std::cerr << "无法找到流信息。\n";
		avformat_close_input(&format_ctx);
		return frames;
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
		return frames;
	}

	codec = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id);
	if (!codec) {
		std::cerr << "找不到解码器。\n";
		avformat_close_input(&format_ctx);
		return frames;
	}

	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		std::cerr << "无法分配解码器上下文。\n";
		avformat_close_input(&format_ctx);
		return frames;
	}

	if (avcodec_parameters_to_context(codec_ctx, format_ctx->streams[video_stream_index]->codecpar) < 0) {
		std::cerr << "无法复制解码器参数到上下文。\n";
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return frames;
	}

	if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
		std::cerr << "无法打开解码器。\n";
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return frames;
	}

	AVPacket packet = {}; // 初始化 AVPacket

	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		std::cerr << "无法分配帧。\n";
		avcodec_free_context(&codec_ctx);
		avformat_close_input(&format_ctx);
		return frames;
	}

	//int n = 0;
	while (av_read_frame(format_ctx, &packet) >= 0) {
		if (packet.stream_index == video_stream_index) {
			if (avcodec_send_packet(codec_ctx, &packet) >= 0) {
				while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
					AVFrame* frame_copy = av_frame_clone(frame);
					//av_frame_free(&frame);
					if (frame_copy) {
						frames.push_back(frame_copy);
// 						n++;
// 						string filename = "E:\\videoSpliting\\videoSpliting\\VideoSplitting\\jpg\\";
// 						saveFrameAsJPEG(frame_copy, filename + std::to_string(n) + ".JPG", frame_copy->width, frame_copy->height);
					}
				}
			}
		}
		av_packet_unref(&packet);
	}

	av_frame_free(&frame);
	avcodec_free_context(&codec_ctx);
	avformat_close_input(&format_ctx);

	return frames;
}


// 编码视频帧到文件
bool encode_video(const char* filename, std::vector<AVFrame*>& frames, int width, int height, int fps) {
	AVFormatContext* format_ctx = nullptr;
	AVCodecContext* codec_ctx = nullptr;
	AVStream* stream = nullptr;

	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) {
		std::cerr << "Codec not found\n";
		return false;
	}

	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		std::cerr << "Could not allocate codec context\n";
		return false;
	}

	codec_ctx->codec_id = codec->id;
	codec_ctx->bit_rate = 4000000;
	codec_ctx->width = width;
	codec_ctx->height = height;
	codec_ctx->time_base = AVRational{ 1, fps };
	codec_ctx->framerate = AVRational{ fps, 1 };
	codec_ctx->gop_size = 10;
	codec_ctx->max_b_frames = 1;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
		std::cerr << "Could not open codec\n";
		avcodec_free_context(&codec_ctx);
		return false;
	}

	avformat_alloc_output_context2(&format_ctx, nullptr, nullptr, filename);
	if (!format_ctx) {
		std::cerr << "Could not create output context\n";
		avcodec_free_context(&codec_ctx);
		return false;
	}

	stream = avformat_new_stream(format_ctx, nullptr);
	if (!stream) {
		std::cerr << "Could not create new stream\n";
		avcodec_free_context(&codec_ctx);
		avformat_free_context(format_ctx);
		return false;
	}

	if (avcodec_parameters_from_context(stream->codecpar, codec_ctx) < 0) {
		std::cerr << "Could not copy codec parameters to stream\n";
		avcodec_free_context(&codec_ctx);
		avformat_free_context(format_ctx);
		return false;
	}

	stream->time_base = codec_ctx->time_base;

	if (!(format_ctx->oformat->flags & AVFMT_NOFILE)) {
		if (avio_open(&format_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
			std::cerr << "Could not open output file\n";
			avcodec_free_context(&codec_ctx);
			avformat_free_context(format_ctx);
			return false;
		}
	}

	if (avformat_write_header(format_ctx, nullptr) < 0) {
		std::cerr << "Could not write file header\n";
		avcodec_free_context(&codec_ctx);
		avformat_free_context(format_ctx);
		return false;
	}


	int frame_pts = 0;
	for (AVFrame* frame : frames) {
		frame->pts = frame_pts++;

		if (avcodec_send_frame(codec_ctx, frame) < 0) {
			std::cerr << "Could not send frame to encoder\n";
			continue;
		}

		AVPacket pkt{};
		//av_init_packet(&pkt);
		pkt.data = nullptr;
		pkt.size = 0;

		while (avcodec_receive_packet(codec_ctx, &pkt) == 0) {
			av_packet_rescale_ts(&pkt, codec_ctx->time_base, stream->time_base);
			pkt.stream_index = stream->index;

			if (av_interleaved_write_frame(format_ctx, &pkt) < 0) {
				std::cerr << "Could not write frame to file\n";
				av_packet_unref(&pkt);
				continue;
			}
			av_packet_unref(&pkt);
		}
	}
	// 发送空帧以刷新编码器缓冲区
	if (avcodec_send_frame(codec_ctx, nullptr) == 0) {
		AVPacket pkt{};
		pkt.data = nullptr;
		pkt.size = 0;

		while (avcodec_receive_packet(codec_ctx, &pkt) == 0) {
			av_packet_rescale_ts(&pkt, codec_ctx->time_base, stream->time_base);
			pkt.stream_index = stream->index;

			if (av_interleaved_write_frame(format_ctx, &pkt) < 0) {
				std::cerr << "Could not write frame to file\n";
				av_packet_unref(&pkt);
				continue;
			}
			av_packet_unref(&pkt);
		}
	}

	av_write_trailer(format_ctx);

	if (!(format_ctx->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&format_ctx->pb);
	}
	avcodec_free_context(&codec_ctx);
	avformat_free_context(format_ctx);
	return true;
}
