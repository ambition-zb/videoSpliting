#include "DownloadVideo.h"
#include <regex>

#define CURL_STATICLIB
#define HTTP_ONLY
#include <curl.h>
#include "../jsoncpp/json.h"
#include <fstream>

DownloadVideo::DownloadVideo()
{

}

DownloadVideo::~DownloadVideo()
{

}

void DownloadVideo::setFolder(std::string folder)
{
	m_folder = folder;
	//reset index
	m_index = 1;
	createFolder(folder);
}

std::string DownloadVideo::download(const std::string& short_url)
{
	std::string output_file = "";
	if (short_url.empty())
		return output_file;
	std::string url = extractUrl(short_url);
	if (url.empty())
		return output_file;
	/*
	// Step 1: Follow the redirect to get the real video page URL
	std::string real_video_url = followRedirect(short_url);
	if (real_video_url.empty()) 
	{
		std::cerr << "Failed to resolve the short URL!" << std::endl;
		return output_file;
	}*/

	// Step 2: Get the HTML content of the real video page
	std::string html_content = getHTMLContentByUrl(url);
	//std::string html_content = "";
	// Step 3: Extract the real video URL
	std::string video_url = extractVideoURL(html_content);

	if (!video_url.empty()) 
	{
		//std::cout << "Video URL: " << video_url << std::endl;

		// Step 4: The output file path where the video will be saved
		output_file = m_folder + "//" + std::to_string(m_index) + ".mp4";

		// Download the video file
		if (downloadFile(video_url, output_file)) 
		{
			m_index++;
			std::cout << "Download successful: " << output_file << std::endl;
		}
		else 
		{
			std::cout << "Download failed!" << std::endl;
			return "";
		}
	}
	else 
	{
		std::cout << "Failed to extract video URL!" << std::endl;
		return "";
	}
	return output_file;
}

// Callback function to capture HTML content into a string
size_t WriteToString(void* ptr, size_t size, size_t nmemb, std::string* data)
{
// 	data->append((char*)ptr, size * nmemb);
// 	return size * nmemb;
	FILE* fp = (FILE*)data;
	size_t written = fwrite(ptr, size, nmemb, fp);
	return written;
}

// 正确的 WriteToString 回调函数实现
size_t WriteToString1(void* ptr, size_t size, size_t nmemb, std::string* data) {
	size_t totalSize = size * nmemb;
	data->append((char*)ptr, totalSize);
	return totalSize;
}

size_t WriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


// Function to follow a short URL to its final destination
std::string DownloadVideo::followRedirect(const std::string& short_url) 
{
	CURL* curl;
	CURLcode res;
	char* effective_url = nullptr;
	std::string result_url;

	curl = curl_easy_init();
	if (curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, short_url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);

		res = curl_easy_perform(curl);
		if (res == CURLE_OK) 
		{
			curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
			if (effective_url) 
			{
				result_url = std::string(effective_url);
			}
		}
		else 
		{
			std::cerr << "Error following redirect: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	return result_url;
}

// Function to download a file using libcurl
bool DownloadVideo::downloadFile(const std::string& url, const std::string& output_path) 
{
	CURL* curl;
	FILE* fp = nullptr;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		fopen_s(&fp, output_path.c_str(), "wb");
		if (!fp) 
		{
			std::cerr << "Failed to open file: " << output_path << std::endl;
			curl_easy_cleanup(curl);
			return false;
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		// If the server uses HTTPS, ignore SSL verification (not recommended for production)
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) 
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		if (fp)
		{
			fclose(fp);
			fp = nullptr;
		}
		curl_easy_cleanup(curl);

		return (res == CURLE_OK);
	}

	return false;
}

// Function to get HTML content from a URL
std::string DownloadVideo::getHTMLContent(const std::string& url) 
{
	CURL* curl;
	CURLcode res;
	std::string html_content;

	curl = curl_easy_init();
	if (curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString1);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_content);

		// 如果目标网站使用 HTTPS，可以选择跳过 SSL 证书验证（不推荐用于生产环境）
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
	}

	return html_content;
}

// Function to extract the real video URL from the HTML content
std::string DownloadVideo::extractVideoURL(const std::string& html_content) 
{
	/*
	// 打开JSON文件
 	std::ifstream ifs("C:\\Users\\mei_s\\Desktop\\response_1723363211422.json");
 	if (!ifs.is_open()) {
 		std::cerr << "无法打开文件" << std::endl;
 		return "";
 	}
 
 	// 将文件内容读入字符串
 	std::string html_content{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
	*/
	// 使用Json::Reader解析字符串
	Json::Reader reader;
	Json::Value root;
	//if (!reader.parse(html_content, root)) {
	if (!reader.parse(html_content, root))
	{
		std::cerr << "解析失败" << std::endl;
		return "";
	}
	// 访问JSON数据
	//std::cout << "router: " << root["router"].asString() << std::endl;
	if (root.isMember("data") && root["data"].isMember("aweme_detail") && root["data"]["aweme_detail"].isMember("video"))
	{
		Json::Value video = root["data"]["aweme_detail"]["video"];
		if (video.isMember("play_addr_h264") && video["play_addr_h264"].isMember("url_list"))
		{
			if (video["play_addr_h264"]["url_list"].isArray() && video["play_addr_h264"]["url_list"].size() == 4)
			{
				std::string url = video["play_addr_h264"]["url_list"][3].asString();
				return url;
			}
		}
	}

	return "";
}

std::string DownloadVideo::getHTMLContentByUrl(const std::string& url)
{
	CURL* curl;
	CURLcode res;
	std::string response_string;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		struct curl_slist* headers = nullptr;

		// If an API token is required, set the Authorization header
		std::string bearer = "Authorization: Bearer 1o0mS21Am1oVlBIp0myVumsy1V8vsrb0nblzkTD9sH4GoQRSU8Zt9yG9Lg==";
		headers = curl_slist_append(headers, bearer.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


		// 使用 stringstream 拼接
		std::stringstream ss;
		const char* weburl = "https://api.tikhub.io/api/v1/douyin/app/v3/fetch_one_video_by_share_url?share_url=";
		ss << weburl << url.c_str();
		curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());

		//curl_easy_setopt(curl, CURLOPT_URL, "https://api.tikhub.io/api/v1/douyin/app/v3/fetch_one_video_by_share_url?share_url=https://v.douyin.com/e3x2fjE/");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString1);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

		// If the server uses HTTPS, ignore SSL verification (not recommended for production)
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
		if (headers) {
			curl_slist_free_all(headers); // Clean up header list
		}
	}
	curl_global_cleanup();

	return response_string;
}

std::string DownloadVideo::extractUrl(const std::string& text)
{
	std::regex urlregex(R"((http|https)://[^\s]+)"); 
	std::regex url_regex(R"((https?://[a-zA-Z0-9./?=&_%:-]+))");
	std::smatch urlMatch;

	if (std::regex_search(text, urlMatch, url_regex)) {
		return urlMatch.str();
	}
	return "";
}
