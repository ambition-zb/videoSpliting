#include "Window.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#pragma comment(lib, "legacy_stdio_definitions")
#include <windows.h>
#include <shobjidl.h> 
#include <iostream>

#include "VideoManager.h"
#include "FFmpegHandler.h"
#include "Decode.h"
#include "Encode.h"
#include "Message.h"

#include <locale>
#include <codecvt>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


Window::Window()
{

}

Window::~Window()
{

}

void Window::Init()
{
	CVideoManager videoMng;

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return;

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1250, 600, "Smart Video Editing", nullptr, nullptr);
	if (window == nullptr)
		return;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImFontConfig fontCfg;
	fontCfg.OversampleH = 3; // 字体横向过采样，默认值是3
	fontCfg.OversampleV = 1; // 字体纵向过采样，默认值是1
	io.Fonts->AddFontFromFileTTF("msyhbd.ttc", 16.0f, &fontCfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL2_Init();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
			ImGui::Begin("windows!", NULL, window_flags);                          // Create a window called "Hello, world!" and append into it.

			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::SetWindowSize(ImVec2(1250, 600));

			ImGui::SetWindowFontScale(1.5f); // 设置字体缩放比例为1.5

			if (ImGui::TreeNode(u8"输入文件夹"))
			{
				CreateInput(u8"", m_strFolderPath);
				ImGui::SameLine();
				if (ImGui::Button(u8"选择文件夹")) {
					m_strFolderPath = SelectFolder();
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"开始处理", ImVec2(100.0f, 0.0f))) {
					if (!m_strFolderPath.empty()) {
						videoMng.setVideoFolder(m_strFolderPath);
					}
				}
				ImGui::Separator();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"输入链接"))
			{
				CreateInput(u8"", m_strPath_Url);
				ImGui::SameLine();
				if (ImGui::Button(u8"选择保存路径")) {
					m_strPath_Url = SelectFolder();
				}

				CreateInput(u8"文件夹名称", m_strFolder_Url);

				ImGui::Separator();
				if (ImGui::Button(u8"添加链接", ImVec2(100.0f, 0.0f))) {
					m_vUrls.push_back("");
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"开始处理", ImVec2(100.0f, 0.0f))) {
					if (!m_vUrls.empty() /*&& IsUrlsValid()*/)
					{
						std::string strPath = m_strPath_Url + "\\" + m_strFolder_Url;
						videoDownload.setFolder(strPath);
						for (std::string url : m_vUrls)
						{
							if(!url.empty())
								videoDownload.download(url);
						}
						videoMng.setVideoFolder(strPath);
					}
					else
					{
						std::cerr << "Not enough urls\n";
					}
				}
				ImGui::Separator();

				int bDeleteIndex = -1;
				for (int i = 0; i < m_vUrls.size(); i++)
				{
					// 创建 ImGui::InputText 控件
					char buffer[1024] = "";
					size_t bufferSize = sizeof(buffer);
					strncpy_s(buffer, bufferSize, m_vUrls[i].c_str(), bufferSize - 1);
					buffer[bufferSize - 1] = '\0';
					ImGui::InputText(("URL " + std::to_string(i + 1)).c_str(), buffer, IM_ARRAYSIZE(buffer));

					std::string ut = buffer;
					// 更新 m_vUrls 向量
					m_vUrls[i] = ut;

					ImGui::SameLine();
					ImGui::PushID(i);
					if (ImGui::Button("-", ImVec2(30.0f, 0.0f))) {
						bDeleteIndex = i;
					}
					ImGui::PopID();
				}

				// 删除与被点击的按钮对应的项目
				if (bDeleteIndex != -1) {
					m_vUrls.erase(m_vUrls.begin() + bDeleteIndex);
				}
				ImGui::Separator();
				ImGui::TreePop();
			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		//glUseProgram(last_program);

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::CreateInput(std::string strName, std::string& strValue)
{
	char buffer[1024] = "";
	size_t bufferSize = sizeof(buffer);
	strncpy_s(buffer, bufferSize, strValue.c_str(), bufferSize - 1);
	buffer[bufferSize - 1] = '\0';
	ImGui::InputText(strName.c_str(), buffer, IM_ARRAYSIZE(buffer));
}

std::string Window::SelectFolder()
{
	std::string strFolder = "";

	IFileDialog* pfd = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr)) {
		DWORD dwOptions;
		pfd->GetOptions(&dwOptions);
		pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);

		hr = pfd->Show(NULL);
		if (SUCCEEDED(hr)) {
			IShellItem* psi = nullptr;
			hr = pfd->GetResult(&psi);
			if (SUCCEEDED(hr)) {
				PWSTR pszFolderPath = nullptr;
				psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);

				char path[MAX_PATH];
				size_t convertedChars = 0;
				wcstombs_s(&convertedChars, path, pszFolderPath, MAX_PATH);

				strFolder = std::string(path);
				CoTaskMemFree(pszFolderPath);
				psi->Release();
			}
		}
		pfd->Release();
	}
	CoUninitialize();

	return strFolder;
}

bool Window::IsUrlsValid()
{
	int numNonEmptyItems = (int)std::count_if(m_vUrls.begin(), m_vUrls.end(), [](const std::string& item) {
		return !item.empty();
		});
	return numNonEmptyItems >= 4;
}

