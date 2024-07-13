#include "Common.h"
#include <iostream>
#include <filesystem>

enum class CreateDirectoryStatus {
	Success,
	AlreadyExists,
	Failed
};

CreateDirectoryStatus createDirectory(const filesystem::path& dirPath) {
	std::error_code ec;
	if (filesystem::create_directory(dirPath, ec)) {
		return CreateDirectoryStatus::Success;
	}
	else {
		if (filesystem::exists(dirPath)) {
			return CreateDirectoryStatus::AlreadyExists;
		}
		else {
			return CreateDirectoryStatus::Failed;
		}
	}
}

string getDirectoryPath(string filePath)
{
	filesystem::path p(filePath);
	return p.parent_path().string();
}
string getFileNameWithoutExtension(string filePath)
{
	filesystem::path p(filePath);
	return p.stem().string();
}
string createFolder(string strPath, string folderName)
{
	string strFolderPath = strPath + "/" + folderName;
	filesystem::path path(strFolderPath);
	CreateDirectoryStatus status = createDirectory(path);
	if (status == CreateDirectoryStatus::Failed)
		return "";
	return strFolderPath;
}

