#include "NetModule.h"
NetModule* NetModule::Instance()
{
	static NetModule instance;
	return &instance;
}
void NetModule::sendMsg(std::string& msg)
{
	sendMsgQue.push(msg);
	send_ConVar.notify_one();
}
std::string NetModule::recvMsg()
{
	std::unique_lock<std::mutex> locker(m_recvMutex);
	recv_ConVar.wait(locker, [&]() {return isClosed || !recvMsgQue.empty(); });
	if (isClosed) return "";
	std::string res = std::move(recvMsgQue.front());
	recvMsgQue.pop();
	return res;
}
void NetModule::init()
{
	isClosed = false;
	hasConnected = false;
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//qDebug() << tr("网络库加载失败") << endl;
		qDebug() << "Load socket dll fail!";
		return;
	}
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))
	{
		qDebug() << "Socket version error!";
		return;
	}
	sockSvrAddr.sin_family = AF_INET;
	sockSvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockSvrAddr.sin_port = htons(80U);
	sockSvr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void NetModule::netConnect()
{
	if (sockSvr == INVALID_SOCKET) return;
	int conRes = ::connect(sockSvr, (SOCKADDR*)&sockSvrAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == conRes)
	{
		qDebug() << "Connect error!";
		return;
	}
	hasConnected = true;
	sendThread = std::thread([&]() {
		while (true) {
			//连接成功
			std::unique_lock<std::mutex> locker(m_sendMutex);
			send_ConVar.wait(locker, [&]() {return !sendMsgQue.empty()||isClosed; });
			if (isClosed) break;
			int sendRes=send(sockSvr, sendMsgQue.front().c_str(), sendMsgQue.front().length(), 0);
			if (sendRes < 0) {
				int error = WSAGetLastError();
			}
			sendMsgQue.pop();
			locker.unlock();
		}
	});
	recvThread = std::thread([&]() {
		while (true) {
			memset(readBuff, 0, sizeof(readBuff));
			int recRes = recv(sockSvr, readBuff, sizeof(readBuff), 0);
			if (recRes > 0) {
				recvMsgQue.push(std::string(readBuff));
				recv_ConVar.notify_one();
			}
			else if (recRes < 0) {
				int error = WSAGetLastError();
				break;
			}
		}
	});
}
NetModule::NetModule()
{
}
NetModule::~NetModule()
{
	if (hasConnected)
	{
		//强制关闭会让服务端手道10054的错误，不推荐
		//使用shutdown优雅关闭
		isClosed = true;
		send_ConVar.notify_one();
		sendThread.join();
		shutdown(sockSvr, SD_BOTH);
		//closesocket(sockSvr);
		recvThread.join();
	}
	WSACleanup();
}