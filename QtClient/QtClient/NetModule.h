#pragma once
#include<WinSock2.h>
#include<qdebug.h>
#include<thread>
#include<queue>
#include<condition_variable>
#include<mutex>
#include<string>
#include<qqueue.h>
#pragma comment(lib,"ws2_32.lib")

class NetModule
{
public:
	void init();
	void netConnect();
	void sendMsg(std::string& msg);
	std::string recvMsg();
	static NetModule* Instance();
private:
	NetModule();
	NetModule(const NetModule& other){}
	void operator=(const NetModule& other) {}
	~NetModule();
	enum { BUFF_SIZE = 1024 };
	bool isClosed;
	bool hasConnected;
	SOCKADDR_IN sockSvrAddr;
	std::thread sendThread;
	std::thread recvThread;
	std::queue<std::string> sendMsgQue;
	std::queue<std::string> recvMsgQue;
	std::condition_variable send_ConVar;
	std::condition_variable recv_ConVar;
	char readBuff[BUFF_SIZE];
	std::mutex m_sendMutex;
	std::mutex m_recvMutex;
	SOCKET sockSvr;
};
