#pragma once
#include "Common.h"

class Message
{
public:
	// 禁止拷贝构造和赋值操作
	Message(const Message&) = delete;
	Message& operator=(const Message&) = delete;

	// 提供一个静态方法来获取单例实例
	static std::shared_ptr<Message> getInstance() {
		std::call_once(initFlag, []() {
			instance.reset(new Message());
			});
		return instance;
	}

	void setMessageInfo(std::string strInputFolder, std::string strOutputFolder, std::string strTempFolder);
	std::string getOutputFolder();
	std::string getTempFolder();
	int getSceneIndex();


	bool createDirectory(const std::string& path);
private:
	// 私有构造函数，禁止从外部创建实例
	Message() {
	}

	static std::shared_ptr<Message> instance;
	static std::once_flag initFlag;


	std::string m_strInputFolder;
	std::string m_strTempFolder;
	std::string m_strOutputFolder;

	int m_nSceneIndex = 0;
};

