#include "VideoMetadataEditor.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
uint32_t read_be_uint32(const std::vector<uint8_t>& data, size_t offset) {
	return (data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3];
}

void write_be_uint32(std::vector<uint8_t>& data, size_t offset, uint32_t value) {
	data[offset] = (value >> 24) & 0xFF;
	data[offset + 1] = (value >> 16) & 0xFF;
	data[offset + 2] = (value >> 8) & 0xFF;
	data[offset + 3] = value & 0xFF;
}

VideoMetadataEditor::VideoMetadataEditor()
{

}

bool VideoMetadataEditor::modifyDuration(const std::string& inputPath, const std::string& outputPath, int newDurationSeconds)
{
	/*
	std::ifstream in(inputPath, std::ios::binary);
	if (!in) return false;

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(in)), {});
	in.close();

	// Find 'mvhd'
	for (size_t i = 0; i + 8 < buffer.size(); ++i) {
		if (buffer[i] == 'm' && buffer[i + 1] == 'v' &&
			buffer[i + 2] == 'h' && buffer[i + 3] == 'd') {

			size_t mvhd_offset = i;
			uint8_t version = buffer[mvhd_offset + 4];
			size_t timescale_offset = 0;
			size_t duration_offset = 0;
			if (version == 1) {
				timescale_offset = mvhd_offset + 28;
				duration_offset = mvhd_offset + 32;
				std::cerr << "[WARN] mvhd version 1 not fully supported in 32-bit write!\n";
			}
			else {
				timescale_offset = mvhd_offset + 16;
				duration_offset = mvhd_offset + 20;
			}


			uint32_t timescale = read_be_uint32(buffer, timescale_offset);
			uint32_t new_duration = static_cast<uint32_t>(newDurationSeconds * timescale);

			std::cout << "Found mvhd at offset " << mvhd_offset
				<< ", timescale=" << timescale
				<< ", new_duration=" << new_duration << "\n";

			write_be_uint32(buffer, duration_offset, new_duration);

			std::ofstream out(outputPath, std::ios::binary);
			out.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
			out.close();
			return true;
		}
	}
	return false;
	*/

	std::ifstream in(inputPath, std::ios::binary);
	std::ofstream out(outputPath, std::ios::binary);
	if (!in || !out) return false;

	// 拷贝文件内容（边读边写，不占内存）
	out << in.rdbuf();
	in.close();
	out.close();

	// 打开写文件用于随机修改
	std::fstream file(outputPath, std::ios::in | std::ios::out | std::ios::binary);
	if (!file) return false;

	const size_t searchLimit = 1024 * 1024; // 限制搜索范围，比如前1MB（moov 通常在前面）
	std::vector<char> buffer(searchLimit);
	file.read(buffer.data(), buffer.size());

	for (size_t i = 0; i + 8 < buffer.size(); ++i) {
		if (buffer[i] == 'm' && buffer[i + 1] == 'v' &&
			buffer[i + 2] == 'h' && buffer[i + 3] == 'd') {

			size_t mvhd_offset = i;
			uint8_t version = buffer[mvhd_offset + 4];

			size_t timescale_offset = (version == 1) ? mvhd_offset + 28 : mvhd_offset + 16;
			size_t duration_offset = (version == 1) ? mvhd_offset + 32 : mvhd_offset + 20;

			// 读取 timescale
			file.seekg(timescale_offset, std::ios::beg);
			uint8_t ts_buf[4];
			file.read(reinterpret_cast<char*>(ts_buf), 4);
			uint32_t timescale = (ts_buf[0] << 24) | (ts_buf[1] << 16) | (ts_buf[2] << 8) | ts_buf[3];

			uint32_t new_duration = static_cast<uint32_t>(newDurationSeconds * timescale);

			std::cout << "Found mvhd at " << mvhd_offset
				<< ", timescale = " << timescale
				<< ", new_duration = " << new_duration << "\n";

			// 写 duration
			uint8_t duration_buf[4] = {
				static_cast<uint8_t>((new_duration >> 24) & 0xFF),
				static_cast<uint8_t>((new_duration >> 16) & 0xFF),
				static_cast<uint8_t>((new_duration >> 8) & 0xFF),
				static_cast<uint8_t>(new_duration & 0xFF)
			};

			file.seekp(duration_offset, std::ios::beg);
			file.write(reinterpret_cast<char*>(duration_buf), 4);

			file.close();
			return true;
		}
	}

	file.close();
	return false;
}

void VideoMetadataEditor::cleanup(AVFormatContext* inCtx, AVFormatContext* outCtx)
{
	if (inCtx) {
		avformat_close_input(&inCtx);
	}
	if (outCtx) {
		if (!(outCtx->oformat->flags & AVFMT_NOFILE)) {
			avio_closep(&outCtx->pb);
		}
		avformat_free_context(outCtx);
	}
}
