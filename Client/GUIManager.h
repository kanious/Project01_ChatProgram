#ifndef _GUIMAANGER_H_
#define _GUIMAANGER_H_

#include "Define.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class ChannelInfo;
class GUIManager
{
private:
	static GUIManager*				m_pInstance;
	char							m_chText[128];
	std::string						m_currentChannel;
	std::list<std::string>			m_listAllChannels;
	typedef std::unordered_map<std::string, ChannelInfo> ChannelMap;
	ChannelMap						m_mapJoinedChannels;

	bool							m_tabChange;

public:
	explicit GUIManager();
	~GUIManager();
	static GUIManager* GetInstance();
	void DestroyInstance();
public:
	void RenderUI();
	int Ready(GLFWwindow*);
	void Close();

public:
	void AddAllChannelList(std::string name);
	void UpdateJoinedChannelMap(ChannelInfo& info);
	void UpdateSystemChannel(std::string name);
	void AddConvo(unsigned int clientId, std::string channelName, std::string clientName, std::string message);
	void RemoveFromAllChannel(std::string channelName);
	void RemoveJoinedChannel(std::string channelName);
	void InitAllInformation();
	void PressSendButton();

private:
	void UserList(float screenX, float screenY);
	void ContentArea(float screenX, float screenY);
	void TextInputArea(float screenX);
};


#endif //_GUIMAANGER_H_