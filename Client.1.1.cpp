/*
客户端client
(1)加载套接字库，创建套接字（WSAStartup()/socket()）;
(2)向服务器发出连接请求（connect()）;
(3)和服务器进行通信（send()/recv()）;
(4)关闭套接字，关闭加载的套接字库（closesocket()/WSACleanup()）;
*/

/*
Windows与Linux socket程序的不同

(1)头文件
windows: winsock.h|winsock2.h
linux: netinet/in.h（大部分都在这儿）, unistd.h（close函数在这儿）, sys/socket.h（在in.h里已经包含了,可以省了）

(2)初始化
windows: 需要用WSAStartup启动Ws2_32.lib, 并且要用#pragma comment(lib,"Ws2_32")来告知编译器链接该lib
linux: 不需要

(3)关闭socket
windows: closesocket(...)
linux: close(...)

(4)send函数最后一个参数
windows: 一般设置为0
linux: 最好设置为MSG_NOSIGNAL, 如果不设置, 在发送出错后有可能会导致程序退出
*/

#include<winsock2.h>
#include<stdio.h>
#include<iostream>
#include<cstring>
using namespace std;
#pragma comment(lib, "ws2_32.lib")//告知编译器链接Ws2_32.lib

SOCKET sockClient;
sockaddr_in addrServer;

//加载套接字库
void loadSocket() {
	WORD sockVersion = MAKEWORD(2, 2);//指2.2版本的socket
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
		/*
		int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
		• 第一个参数, 指明程序请求使用的Socket版本, 高位字节指明副版本, 低位字节指明主版本
		• 第二个参数, 操作系统利用它, 返回请求的Socket的版本信息

		Windows下, 调用WSAStartup()函数, 绑定对应版本的socket.
		以后程序就可以调用所请求的Socket库中的函数了.
		*/
		return;
	}
}

//创建套接字
void createSocket() {
	sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*
	int socket(int domain, int type, int protocol);
	• domain, 协议族. 协议族决定了socket的地址类型, 在通信中必须采用对应的地址,
		AF_INET, 决定了要用32位的ipv4地址与16位的端口号的组合;
	• type, 指定socket类型;
	• protocol, 指定协议, IPPROTO_TCP对应TCP传输协议.

	socket()用于创建一个socket描述符, 它唯一标识一个socket.
	返回的socket描述字存在于协议族（address family，AF_XXX）空间中, 但没有一个具体的地址.
	若要赋一个地址, 就必须调用bind()函数, 否则就当调用connect()、listen()时系统会自动随机分配一个端口.
	*/
	if (sockClient == INVALID_SOCKET) {
		printf("Invalid Socket!");
		return;
	}
}

//向服务器发出连接请求
void connectServer(sockaddr_in addr, int host, const char* addrC) {
	//地址结构根据地址创建socket时的地址协议族的不同而不同
	addr.sin_family = AF_INET;
	addr.sin_port = htons(host);
	addr.sin_addr.S_un.S_addr = inet_addr(addrC);

	//向服务器发出连接请求
	if (connect(sockClient, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR) {
		/*
		int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
		• 第一个参数, 为客户端的socket描述字,
		• 第二个参数, 为服务器的socket地址,
		• 第三个参数, 为socket地址的长度.

		客户端通过调用connect函数来建立与TCP服务器的连接.
		*/
		printf("Connect Error!");
		closesocket(sockClient);
		return;
	}
}

int main(){
	//加载套接字库
	loadSocket();

	while (true) {
		//创建套接字
		createSocket();

		//向服务器发出连接请求
		connectServer(addrServer, 8888, "127.0.0.2");

		string data;
		cin >> data;
		const char* dataToServer;
		dataToServer = data.c_str();//string转const char*

		//和服务器进行通信
		send(sockClient, dataToServer, strlen(dataToServer), 0);
		/*
		ssize_t send(int sockfd, const void *buf, size_t len, int flags);
		• 第一个参数, 为建立好连接的socket,
		• 第二个参数, 指明存放要发送数据的缓冲区,
		• 第三个参数, 为指向数据的长度,
		• 第四个参数, flags一般设0.

		send()用来将数据由指定的socket传给对方主机,
		成功则返回实际传送出去的字符数, 失败返回-1, 错误原因存于error
		*/

		char recData[255];
		int ret = recv(sockClient, recData, 255, 0);
		/*
		int recv( SOCKET s,char* buf,int len,int flags);
		• 第一个参数, 指定接收端套接字描述符,
		• 第二个参数, 指明用来存放接收到数据的缓冲区,
		• 第三个参数, 指明buf的长度,
		• 第四个参数, flags一般设0.
		*/
		if (ret > 0) {
			//recv()返回值: <0, 出错; =0, 连接关闭; >0, 接收到数据大小.
			recData[ret] = 0x00;
			printf(recData);
		}
		//关闭套接字
		closesocket(sockClient);
	}

	//关闭加载的套接字库
	WSACleanup();
	/*
	Windows下, 程序在完成对请求的Socket库的使用后, 要调用WSACleanup()函数, 
	来解除与Socket库的绑定,并且释放Socket库所占用的系统资源.
	*/
	return 0;
}
