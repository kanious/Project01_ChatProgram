#include "GUIManager.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ClientSendManager.h"
#include "ChannelInfo.h"

using namespace ImGui;
using namespace std;
GUIManager* GUIManager::m_pInstance = nullptr;
GUIManager::GUIManager()
{
	ZeroMemory(m_chText, sizeof(m_chText));
	m_currentChannel = "";
	m_listAllChannels.clear();
	m_mapJoinedChannels.clear();
	m_tabChange = false;
}

GUIManager::~GUIManager()
{
}

GUIManager* GUIManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new GUIManager();
	return m_pInstance;
}

void GUIManager::DestroyInstance()
{
	m_listAllChannels.clear();
	m_mapJoinedChannels.clear();
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void GUIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();


	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(ImVec2(800.f, 600.f));
	if (Begin("Pumpkins Talk", (bool*)0, ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_NoCollapse |
									 ImGuiWindowFlags_NoMove |
									 ImGuiWindowFlags_NoTitleBar |
									 ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		ImVec2 screenSize = GetWindowSize();

		UserList(screenSize.x, screenSize.y);
		ContentArea(screenSize.x, screenSize.y);
		TextInputArea(screenSize.x);

		End();
	}


	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

int GUIManager::Ready(GLFWwindow* window)
{
	//initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//platform / render bindings
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 460"))
	{
		return 3;
	}

	//imgui style (dark mode for the win)
	ImGui::StyleColorsDark();

	return 0;
}

void GUIManager::Close()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUIManager::AddAllChannelList(string name)
{
	list<string>::iterator iter;
	for (iter = m_listAllChannels.begin(); iter != m_listAllChannels.end(); ++iter)
	{
		if (*iter == name)
			return;
	}
	m_listAllChannels.push_back(name);
}

void GUIManager::UpdateJoinedChannelMap(ChannelInfo& info)
{
	ChannelMap::iterator iter = m_mapJoinedChannels.find(info.name);
	if (iter == m_mapJoinedChannels.end())
	{
		ChannelInfo newChannel;
		newChannel.masterId = info.masterId;
		newChannel.name = info.name;
		list<string>::iterator iterList;
		for (iterList = info.listClient.begin(); iterList != info.listClient.end(); ++iterList)
			newChannel.listClient.push_back(*iterList);
		m_mapJoinedChannels.insert(ChannelMap::value_type(newChannel.name, newChannel));
	}
	else
	{
		iter->second.masterId = info.masterId;
		iter->second.name = info.name;
		iter->second.listClient.clear();
		list<string>::iterator iterList;
		for (iterList = info.listClient.begin(); iterList != info.listClient.end(); ++iterList)
			iter->second.listClient.push_back(*iterList);
	}
}

void GUIManager::UpdateSystemChannel(std::string name)
{
	ChannelMap::iterator iter = m_mapJoinedChannels.find("System");
	if (iter != m_mapJoinedChannels.end())
	{
		list<string>::iterator iterList = iter->second.listClient.begin();
		*iterList = name;
	}
}

void GUIManager::AddConvo(unsigned int clientId, string channelName, string clientName, std::string message)
{
	ChannelMap::iterator iter = m_mapJoinedChannels.find(channelName);
	if (iter != m_mapJoinedChannels.end())
	{
		string str(clientName);
		if (clientId == iter->second.masterId)
			str.append("(master)");
		str.append(": ");
		str.append(message);
		iter->second.vecChat.push_back(string(str));
	}
}

void GUIManager::RemoveFromAllChannel(string channelName)
{
	list<string>::iterator iterAll;
	for (iterAll = m_listAllChannels.begin(); iterAll != m_listAllChannels.end(); ++iterAll)
	{
		if (*iterAll == channelName)
		{
			m_listAllChannels.erase(iterAll);
			break;
		}
	}

	ChannelMap::iterator iter = m_mapJoinedChannels.find(channelName);
	if (iter != m_mapJoinedChannels.end())
		m_mapJoinedChannels.erase(iter);
}

void GUIManager::RemoveJoinedChannel(std::string channelName)
{
	ChannelMap::iterator iter = m_mapJoinedChannels.find(channelName);
	if (iter != m_mapJoinedChannels.end())
	{
		m_mapJoinedChannels.erase(iter);
	}
}

void GUIManager::InitAllInformation()
{
	m_listAllChannels.clear();
	m_mapJoinedChannels.clear();
	m_currentChannel = "";
}
bool pressed = false;
void GUIManager::PressSendButton()
{
	pressed = true;
}

void GUIManager::UserList(float screenX, float screenY)
{
	if (BeginChild("##lists", ImVec2(screenX * 0.98f, screenY * 0.14f)))
	{
		//PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0, 0.3, 0.0, 1));
		if (BeginChild("##player_lists", ImVec2(screenX * 0.48f, screenY * 0.14f)))
		{
			ChannelMap::iterator iter = m_mapJoinedChannels.find(m_currentChannel);
			if (iter == m_mapJoinedChannels.end())
				Text("");
			else
			{
				int count = 0;
				list<string>::iterator iterClient;
				for (iterClient = iter->second.listClient.begin(); iterClient != iter->second.listClient.end(); ++iterClient)
				{
					Text(iterClient->c_str());
					if (count % 2 == 0)
						SameLine(screenX * 0.25f);
					count++;
				}
			}
			EndChild();
		}
		//PopStyleColor();

		SameLine(screenX * 0.48f + 7.f);
		PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0, 0.2, 0.0, 1));
		if (BeginChild("##channel_lists", ImVec2(screenX * 0.49f, screenY * 0.14f)))
		{
			if (m_listAllChannels.size() <= 0)
				Text("");
			else
			{
				int count = 0;
				list<string>::iterator iter;
				for (iter = m_listAllChannels.begin(); iter != m_listAllChannels.end(); ++iter)
				{
					Text(iter->c_str());
					if (count % 2 == 0)
						SameLine(screenX * 0.25f);
					count++;
				}
			}
			EndChild();
		}
		PopStyleColor();

		EndChild();
	}
	else
	{
		EndChild();
	}
}

void GUIManager::ContentArea(float screenX, float screenY)
{
	if (BeginChild("##content", ImVec2(screenX * 0.983f, screenY * 0.785f)))
	{
		if (m_mapJoinedChannels.size() <= 0)
		{
			EndChild();
			return;
		}
		
		if (BeginTabBar("Channel List"))
		{
			ChannelMap::iterator iter;
			for (iter = m_mapJoinedChannels.begin(); iter != m_mapJoinedChannels.end(); ++iter)
			{
				ImGuiTabItemFlags_ flag = ImGuiTabItemFlags_None;
				if (m_tabChange)
				{
					if (m_currentChannel == iter->second.name)
					{
						flag = ImGuiTabItemFlags_SetSelected;
						m_tabChange = false;
					}
				}
				if (BeginTabItem(iter->second.name.c_str(), (bool*)0, flag))
				{
					if (!m_tabChange)
						m_currentChannel = iter->second.name;
					string str("##text_area_");
					str.append(iter->second.name);
					if (BeginChild(str.c_str()))
					{
						vector<string>& vecChat = iter->second.vecChat;
						vector<string>::iterator iterConvo;
						for (iterConvo = vecChat.begin(); iterConvo != vecChat.end(); ++iterConvo)
							Text(iterConvo->c_str());

						EndChild();
					}
					EndTabItem();
				}
			}
			EndTabBar();
		}

		EndChild();
	}
	else
	{
		EndChild();
	}
}

void GUIManager::TextInputArea(float screenX)
{
	int comboSizeX = screenX * 0.15f;
	int textSizeX = screenX * 0.7f;
	SetNextItemWidth(comboSizeX);
	if (BeginCombo("##channel_list", m_currentChannel.c_str(), ImGuiComboFlags_None))
	{
		ChannelMap::iterator iter;
		for (iter = m_mapJoinedChannels.begin(); iter != m_mapJoinedChannels.end(); ++iter)
		{
			bool is_selected = (m_currentChannel == iter->second.name);
			if (Selectable(iter->second.name.c_str(), is_selected))
			{
				m_currentChannel = iter->second.name;
				m_tabChange = true;
			}
		}
		EndCombo();
	}

	SameLine(comboSizeX + 15.f);
	SetNextItemWidth(textSizeX);
	InputText("##text", m_chText, sizeof(m_chText));
	if (pressed)
		SetKeyboardFocusHere(-1);
	SameLine(comboSizeX + 25.f + textSizeX);
	if (Button("Send", ImVec2(screenX - comboSizeX - 35.f - textSizeX, 0.f)) || pressed)
	{
		string str(m_chText);
		if (str.size() > 0)
		{
			ClientSendManager::GetInstance()->MessageParcing(m_currentChannel, str);
			memset(m_chText, 0, sizeof(m_chText));
		}
		pressed = false;
	}
}
