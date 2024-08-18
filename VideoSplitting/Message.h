#pragma once
#include "Common.h"

class Message
{
public:
	// ��ֹ��������͸�ֵ����
	Message(const Message&) = delete;
	Message& operator=(const Message&) = delete;

	// �ṩһ����̬��������ȡ����ʵ��
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
	// ˽�й��캯������ֹ���ⲿ����ʵ��
	Message() {
	}

	static std::shared_ptr<Message> instance;
	static std::once_flag initFlag;


	std::string m_strInputFolder;
	std::string m_strTempFolder;
	std::string m_strOutputFolder;

	int m_nSceneIndex = 0;
};

