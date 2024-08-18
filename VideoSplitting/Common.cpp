#include "Common.h"
#include <iostream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

enum class CreateDirectoryStatus {
	Success,
	AlreadyExists,
	Failed
};

CreateDirectoryStatus createDirectory(const std::filesystem::path& dirPath) {
	std::error_code ec;
	if (std::filesystem::create_directory(dirPath, ec)) {
		return CreateDirectoryStatus::Success;
	}
	else {
		if (std::filesystem::exists(dirPath)) {
			return CreateDirectoryStatus::AlreadyExists;
		}
		else {
			return CreateDirectoryStatus::Failed;
		}
	}
}

std::string getDirectoryPath(std::string filePath)
{
	std::filesystem::path p(filePath);
	return p.parent_path().string();
}
std::string getFileNameWithoutExtension(std::string filePath)
{
	std::filesystem::path p(filePath);
	return p.stem().string();
}

std::string createFolder(std::string strPath)
{
	std::filesystem::path path(strPath);
	CreateDirectoryStatus status = createDirectory(path);
	if (status == CreateDirectoryStatus::Failed)
		return "";
	return strPath;
}

std::string createFolder(std::string strPath, std::string folderName)
{
	std::string strFolderPath = strPath + "/" + folderName;
	std::filesystem::path path(strFolderPath);
	CreateDirectoryStatus status = createDirectory(path);
	if (status == CreateDirectoryStatus::Failed)
		return "";
	return strFolderPath;
}

std::string getCurrentDate()
{
	// 获取当前时间
	std::time_t now = std::time(nullptr);  
	std::tm localTime;
	localtime_s(&localTime, &now);         

	// 使用字符串流进行格式化
	std::ostringstream oss;
	oss << std::put_time(&localTime, "%Y-%m-%d");

	return oss.str();
}

