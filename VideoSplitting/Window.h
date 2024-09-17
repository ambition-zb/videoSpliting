#pragma once
#include "Common.h"
//#include "DownloadVideo.h"

class Window
{
public:
	Window();
	~Window();

	void Init();
	void CreateInput(std::string strName, std::string& strValue);
	std::string SelectFolder();
	bool IsUrlsValid();

	std::vector<std::string> m_vUrls = std::vector<std::string>(5);
	std::string m_strFolderPath;
	std::string m_strPath_Url = "D://media";
	std::string m_strFolder_Url = getCurrentDate();

	//DownloadVideo videoDownload;
};

