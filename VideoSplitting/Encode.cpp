#include "Encode.h"

Encode::Encode(std::string filename)
{
	m_strFile = filename;
}

Encode::~Encode()
{

}

bool Encode::doTask(std::vector<AVFrame*>& video_frames, const int nWidth, const int nHeight, const int nFps)
{
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
	codec_ctx->width = nWidth;
	codec_ctx->height = nHeight;
	codec_ctx->time_base = AVRational{ 1, nFps };
	codec_ctx->framerate = AVRational{ nFps, 1 };
	codec_ctx->gop_size = 10;
	codec_ctx->max_b_frames = 1;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
		std::cerr << "Could not open codec\n";
		avcodec_free_context(&codec_ctx);
		return false;
	}

	avformat_alloc_output_context2(&format_ctx, nullptr, nullptr, m_strFile.c_str());
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
		if (avio_open(&format_ctx->pb, m_strFile.c_str(), AVIO_FLAG_WRITE) < 0) {
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
	for (AVFrame* frame : video_frames) {
		frame->pts = frame_pts++;

		if (avcodec_send_frame(codec_ctx, frame) < 0) {
			std::cerr << "Could not send frame to encoder\n";
			continue;
		}

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
	// ·¢ËÍ¿ÕÖ¡ÒÔË¢ÐÂ±àÂëÆ÷»º³åÇø
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
