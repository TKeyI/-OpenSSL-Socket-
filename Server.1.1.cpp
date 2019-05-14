/*
服务端server
(1)加载套接字库，创建套接字（WSAStartup()/socket()）;
(2)绑定套接字到一个IP地址和一个端口上（bind()）;
(3)将套接字设置为监听模式等待连接请求（listen()）;
(4)请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字（accept()）;
(5)用返回的套接字和客户端进行通信（send()/recv()）;
(6)返回，等待另一个连接请求;
(7)关闭套接字，关闭加载的套接字库（closesocket()/WSACleanup()）
*/

#include<winsock2.h>
#include<stdio.h>
#include<iostream>
#include<cstring>
using namespace std;
#pragma comment(lib, "ws2_32.lib")//告知编译器链接Ws2_32.lib

int main(int argc, char* argv[]) {
	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);//指2.2版本的socket
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
		return 0;
	}

	//创建套接字  
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockServer == INVALID_SOCKET) {
		printf("Socket Error!");
		return 0;
	}

	//绑定IP和端口  
	sockaddr_in addrClients;
	addrClients.sin_family = AF_INET;
	addrClients.sin_port = htons(8888);
	addrClients.sin_addr.S_un.S_addr = INADDR_ANY;
	/*
	socket INADDR_ANY 监听0.0.0.0地址, 
	socket只绑定端口, 只要是往这个端口发送的所有ip都能连上.
	*/
	if (bind(sockServer, (LPSOCKADDR)& addrClients, sizeof(addrClients)) == SOCKET_ERROR) {
		printf("Bind Error!");
	}

	//开始监听  
	if (listen(sockServer, 5) == SOCKET_ERROR) {
		/*
		int listen(int sockfd, int backlog);
		• 第一个参数, 为要监听的socket描述字,
		• 第二个参数, 为相应socket可以排队的最大连接个数.
		
		socket()函数创建的socket默认是主动类型的, 
		listen函数将socket变为被动类型的, 等待客户的连接请求.
		*/
		printf("Listen Error!");
		return 0;
	}

	//循环接收数据  
	SOCKET sockClient;
	sockaddr_in clientAddr;
	int nAddrlen = sizeof(clientAddr);
	char revData[255];

	while (true) {
		printf("等待连接...\n");
		sockClient = accept(sockServer, (SOCKADDR*)& clientAddr, &nAddrlen);
		/*
		int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		• 第一个参数, 为服务器的socket描述字, 
		• 第二个参数, 为指向struct sockaddr *的指针, 用于返回客户端的协议地址
		• 第三个参数, 为协议地址的长度.
		
		如果accpet成功, 那么返回由内核自动生成的一个新描述字, 代表与返回客户的TCP连接.
		*/
		if (sockClient == INVALID_SOCKET) {
			printf("Accept Error!");
			continue;
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(clientAddr.sin_addr));
		//inet_ntoa(), 将十进制网络字节序转换为点分十进制IP格式的字符串

		//接收数据  
		int ret = recv(sockClient, revData, 255, 0);
		if (ret > 0) {
			revData[ret] = 0x00;
			printf(revData);
			cout << endl;
		}

		//发送数据  
		const char* dataToClient = "你好，TCP客户端！\n";
		send(sockClient, dataToClient, strlen(dataToClient), 0);
		closesocket(sockClient);
	}

	closesocket(sockServer);
	WSACleanup();
	return 0;
}
