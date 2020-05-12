// LoginSever.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <fstream>
#include <thread>
#include <deque>
#include <mutex>
#include <map>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

const char* localSvrAddr = "127.0.0.1";
const int BUFF_SIZE = 1024;

SOCKET sockSvr;
SOCKADDR_IN addrSvr;
int addr_size = sizeof(SOCKADDR);

//char sendBuff[BUFF_SIZE];
char recvBuff[BUFF_SIZE];

struct user
{
	string userName;
	string password;
};

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//初始化有误
	{
		cout << "载入Socket库失败！" << endl;
		cin.get();
		return 0;
	}
	
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion)) {
		cout << "socket版本有误" << endl;
		cin.get();
		return 0;
	}
	sockSvr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//第三个参数
	addrSvr.sin_family = AF_INET;
	addrSvr.sin_addr.S_un.S_addr = inet_addr(localSvrAddr);
	addrSvr.sin_port = htons(80U);
	int bindRes = ::bind(sockSvr, (SOCKADDR*)&addrSvr, sizeof(SOCKADDR));
	if (bindRes != 0) {
		cout << "绑定出错！" << endl;
		WSACleanup();
		return -1;
	}
	FD_SET socketsSet;
	FD_ZERO(&socketsSet);
	FD_SET(sockSvr, &socketsSet);//数组最长64
	TIMEVAL tv;
	tv.tv_sec = 3L;
	tv.tv_usec = 0L;
	listen(sockSvr, SOMAXCONN);
	while (true)
	{
		FD_SET tempSockets=socketsSet;
		int sRes=select(0, &tempSockets, NULL, &tempSockets, &tv);//把tempSockets传入其中，检测有响应的socket，将tempSockets重新赋值成有响应的socket
		if (sRes == 0) {
			cout << "服务器正在运行......" << endl;
			continue;
		}
		else if (sRes > 0) {
			//有响应
			for (unsigned int i = 0; i < tempSockets.fd_count; ++i) {
				if (tempSockets.fd_array[i] == sockSvr) {
					SOCKET newSockCli = accept(sockSvr, NULL, NULL);
					if (newSockCli == INVALID_SOCKET) {
						continue;
					}
					FD_SET(newSockCli, &socketsSet);
					SOCKADDR_IN sockaddr;
					int length = sizeof(SOCKADDR_IN);
					getpeername(newSockCli, (SOCKADDR*)&sockaddr, &length);
					cout << sockaddr.sin_addr.s_addr << " 客户端连接成功！" << endl;
				}
				else {
					SOCKET tempSockCli = tempSockets.fd_array[i];
					memset(recvBuff, 0, sizeof(recvBuff));
					int recvRes = recv(tempSockCli, recvBuff, sizeof(recvBuff), 0);
					if (0 == recvRes) {
						//客户端下线
						//从socket数组中删除
						//释放资源
						SOCKADDR_IN sockaddr;
						int length = sizeof(SOCKADDR_IN);
						getpeername(tempSockCli, (SOCKADDR*)&sockaddr, &length);
						cout << sockaddr.sin_addr.s_addr << " 客户端下线" << endl;
						FD_CLR(tempSockCli, &socketsSet);
						closesocket(tempSockCli);
					}
					else if (recvRes > 0) {
						cout << recvBuff << endl;
						for (unsigned int j = 0; j < socketsSet.fd_count; ++j) {
							if (socketsSet.fd_array[j] != sockSvr) {
								int sendRes = send(socketsSet.fd_array[j], recvBuff, sizeof(recvBuff), 0);
							}
						}
					}
					else {
						int error = WSAGetLastError();
						cout << error << endl;
						//处理错误
					}
				}
			}
		}
		else {
			int error = WSAGetLastError();
			//处理错误
		}
	}
	for (unsigned int i = 0; i < socketsSet.fd_count; ++i) {
		closesocket(socketsSet.fd_array[i]);
	}
	WSACleanup();
	cin.get();
	return 0;
}