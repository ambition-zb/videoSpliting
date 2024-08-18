#pragma once
#include "Common.h"

class DownloadVideo
{
public:
	DownloadVideo();
	~DownloadVideo();
	void setFolder(std::string folder);
	std::string download(const std::string& short_url);
private:
	std::string followRedirect(const std::string& short_url);
	bool downloadFile(const std::string& url, const std::string& output_path);
	std::string getHTMLContent(const std::string& url);
	std::string extractVideoURL(const std::string& html_content);
	std::string getHTMLContentByUrl(const std::string& url);
	std::string extractUrl(const std::string& text);

	std::string m_folder;
	int m_index = 1;
};

