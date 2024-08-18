#include "Message.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void Message::setMessageInfo(std::string strInputFolder, std::string strOutputFolder, std::string strTempFolder)
{
	m_strInputFolder = strInputFolder;
	m_strOutputFolder = strOutputFolder;
	m_strTempFolder = strTempFolder;
	createDirectory(m_strOutputFolder);
	createDirectory(m_strTempFolder);
}

std::string Message::getOutputFolder()
{
	return m_strOutputFolder;
}

std::string Message::getTempFolder()
{
	return m_strTempFolder;
}

int Message::getSceneIndex()
{
	m_nSceneIndex++;
	return m_nSceneIndex;
}

bool Message::createDirectory(const std::string& path)
{
	// ���·���Ƿ����
	if (!fs::exists(path)) {
		try {
			// ����·��
			fs::create_directories(path);
			std::cout << "Path created: " << path << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Error creating path: " << e.what() << std::endl;
			return false;
		}
	}
	else {
		std::cout << "Path already exists: " << path << std::endl;
	}
	return true;
}

// ���徲̬��Ա����
std::shared_ptr<Message> Message::instance = nullptr;
std::once_flag Message::initFlag;

