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
	void CreateInput(std::string strName, int& nValue);
	std::string SelectFolder();
	std::string SelectFile();
	bool IsUrlsValid();

	std::vector<std::string> m_vUrls = std::vector<std::string>(5);
	std::string m_strFolderPath;
	std::string m_strPath_Url = "D://media";
	std::string m_strFolder_Url = getCurrentDate();

	int m_nChangeDuration = 5;//±¬¸ÄÊ±³¤
	//DownloadVideo videoDownload;
};

