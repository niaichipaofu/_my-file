#pragma once


//----------------------------------------------------------------------------
//   Socket自用类                                                v1.0
//   作者 ： yk
//----------------------------------------------------------------------------
//
//   email:  iamabirdyk219@gmail.com
//
//
//----------------------------------------------------------------------------
//   说明：
//   这个socket类仅仅只是本人为使用方便而进行编写。由于知识有限，所编写的程序必定存在很大问
//   题。如发现问题请联系我，互相学习交流。在此谢过。本人将不断对其优化。
//   
//
//
//   v1.0 May 2019
//  
//----------------------------------------------------------------------------










//-----------------------------------------------------------------------------
//
// 为Windows和Linux提供系统头文件，用于客户端与服务端程序的构建。
// 
//-----------------------------------------------------------------------------
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sys/types.h>
#include<cstring>

#ifdef _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")

#elif __linux__
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<unistd.h>
#include<dirent.h>

//linux
#endif



//-----------------------------------------------------------------------------
// 基于TCP网络通信协议的客户端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用Start（ip，port）函数进行创建。
//
//-----------------------------------------------------------------------------
class Socket_Client_IPv4_TCP
{
public:
	void Soket_client_Init(std::string ip, unsigned int port)       //获取端口号及ip地址
	{
		this->Ip = ip;
		this->Port = port;

	}
	short Socket_client_eastablish()      							//创建Socket套接字，获取一个可用的id号，失败则返回-1
	{
		#ifdef _WIN32
		if (WSAStartup(this->SocketVersion, &this->wsaData) != 0)
		{
			return -1;
		}
		#endif
		this->Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		#ifdef _WIN32
		if (this->Client == INVALID_SOCKET)
		{
			return -1;
		}
		return 0;
		#elif __linux__
		if (this->Client == -1)
		{
			return -1;
		}
		return 0;
		#endif
		
	}

	void Socket_client_Setaddress()            						  //配置地址信息
	{
	
		this->Sockaddr_in.sin_family = AF_INET;
		this->Sockaddr_in.sin_port = htons(this->Port);
		#ifdef _WIN32
		this->Sockaddr_in.sin_addr.S_un.S_addr = inet_addr(this->Ip.c_str());
		#elif __linux__
		this->Sockaddr_in.sin_addr.s_addr = inet_addr(this->Ip.c_str());
		#endif
		
		
	}

	short Socket_client_connect()									//连接服务端程序
	{
		
		while(connect(this->Client, (sockaddr *)&this->Sockaddr_in, sizeof(this->Sockaddr_in)) == -1)
		{
			;
		}
		return 0;
	}
	void Socket_clinet_close()									  //关闭与服务端的连接，并释放id号
	{
		#ifdef _WIN32
		closesocket(this->Client);
		WSACleanup();
		#elif __linux__
		close(this->Client);
		#endif
	}

	short Socket_client_Start(std::string ip, unsigned int port)  //创建客户端程序
	{
		this->Soket_client_Init(ip, port);
		if (this->Socket_client_eastablish() == -1)
			return -1;
		this->Socket_client_Setaddress();
		//if (this->Socket_client_connect() == -1)
			//return -1;
		return 0;
	}
	int Socket_client_receverStr(char *buf)                      //获取一个字符串
	{	
		return recv(this->Client,buf, this->_Buflen,0);
	}

	short Socket_client_receverFile(std::string path)
	{
		char *buf;
		std::ofstream outfile(path.c_str(),std::ios_base::app);
		if (!outfile)
			return -1;
		else
		{
			while (recv(this->Client, buf, this->_Buflen, 0))
			{
				outfile << buf<<"\n";
			}
			outfile.close();
		}
		return 0;
	}
	 short Socket_client_sendStr(std::string _data)
	 {
		 if (send(this->Client, _data.c_str(), strlen(_data.c_str()), 0) < 0)
			 return -1;
	 }
	 short Socket_client_sendFile(std::string path)
	 {
		
		 std::string buf;
 		 std::ifstream infile(path.c_str());
		 std::ofstream outfile(path, std::ios_base::app);
		 if (!infile)
			 return -1;
		 else
		 {
			 while (std::getline(infile, buf))
			 {
				 std::cout << buf << std::endl;
				 send(this->Client, buf.c_str(), strlen(buf.c_str()), 0);
			 }
		 }
		 return 0;
	 }


	void print()
	{
		std::cout<<"asd"<< Port <<"aaa"<<Ip.c_str()<< std::endl;
	}
private:
	std::string Ip;
	unsigned int Port;
	long _Buflen=1024;

	struct sockaddr_in Sockaddr_in;
    
	#ifdef _WIN32                                                         //客户端相关信息
	
	WSADATA wsaData;
	SOCKET Client;
	WORD SocketVersion = MAKEWORD(2, 2);
    
	#elif __linux__
	
	int Client;
	
	#endif
};




//-----------------------------------------------------------------------------
// 基于TCP网络通信协议的服务端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用Start（ip，port）函数进行创建。
//
//-----------------------------------------------------------------------------
class Socket_Sever_IPv4_TCP
{
public:
	void Soket_sever_Init(unsigned int port)                                   //获取端口号
	{
		this->Port = port;
	}
	short Socket_sever_eastablish()                                            //初始化服务端
	{
		#ifdef _WIN32
		if (WSAStartup(SocketVersion, &wsaData) != 0)
		{
			return -1;
		}
		#endif
		this->Sever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		#ifdef _WIN32
		if (this->Sever == INVALID_SOCKET)
		{
			return -1;
		}
		#elif __linux__
		if (this->Sever == -1)
		{
			return -1;
		}
		#endif
		return 0;
	}
	void Socket_sever_Setaddress()												//配置服务端地址信息
	{
		this->Sockaddr_in.sin_family = AF_INET;
		this->Sockaddr_in.sin_port = htons(this->Port);

		#ifdef _WIN32
		this->Sockaddr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		
		#elif __linux__

		this->Sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
		#endif
	}
	short Socket_sever_Bindaddress()                                              //绑定配置信息到相关id号上
	{
		if (bind(Sever, (struct sockaddr*)&this->Sockaddr_in, sizeof(this->Sockaddr_in)) == -1)
			return -1;
		return 0;
	}
	short Socket_sever_Listening()													//监听指定端口
	{
		return listen(this->Sever, this->_waitlen);
		
	}

	short Socket_sever_Accept()														//等待连接
	{
		#ifdef _WIN32
		if ((this->Client = accept(this->Sever, (SOCKADDR*)&this->_Client_address, &this->client_addresslen)) == INVALID_SOCKET)
			return -1;
		#elif __linux__
		if ((this->Client = accept(this->Sever, (struct sockaddr*)&this->_Client_address, &this->client_addresslen)) == -1)
			return -1;
		#endif
		return 0;
	}

	short Socket_sever_Start(unsigned int port)										//创建一个服务端程序并开启监听模式
	{
		this->Soket_sever_Init(port);
		if (this->Socket_sever_eastablish()==-1)
			return -1;
		this->Socket_sever_Setaddress();
		if (this->Socket_sever_Bindaddress() == -1)
			return -1;
		if (this->Socket_sever_Listening() == -1)
			return -1;
		return 0;
		

	}

	int Socket_sever_receverStr(char *buf)
	{
		return recv(this->Client, buf, this->_Buflen, 0);
	}

	short Sket_sever_receverFile(std::string path)
	{
		char *buf;
		std::ofstream outfile(path.c_str(), std::ios_base::app);
		if (!outfile)
			return -1;
		else
		{
			while (recv(this->Client, buf, this->_Buflen, 0))
			{
				outfile << buf << "\n";
			}
			outfile.close();
		}
		return 0;
	}
	short Socket_sever_sendStr(std::string _data)
	{
		if (send(this->Client, _data.c_str(), (int)strlen(_data.c_str()), 0) < 0)
			return -1;
		return 0;
	}
	short Socket_sever_sendFile(std::string path)
	{

		std::string buf;
		std::ifstream infile(path.c_str());
		std::ofstream outfile(path, std::ios_base::app);
		if (!infile)
			return -1;
		else
		{
			while (std::getline(infile, buf))
			{
				std::cout << buf << std::endl;
				send(this->Client, buf.c_str(), strlen(buf.c_str()), 0);
			}
		}
		return 0;
	}




private:
	unsigned int Port;
	long _Buflen = 1024;
	int _waitlen = 25;
	struct sockaddr_in _Client_address;
	struct sockaddr_in Sockaddr_in;
	#ifdef _WIN32
	WSADATA wsaData;
	SOCKET Sever;
	SOCKET Client;
	int client_addresslen = sizeof(this->_Client_address);
	WORD SocketVersion = MAKEWORD(2, 2);
	#elif __linux__
	int Sever;
	int Client;
	socklen_t client_addresslen = sizeof(this->_Client_address);
	#endif
	
};

#ifdef _WIN32

#elif _WIN64
//win64

#elif __linux__
//linux

#elif __unix__
//unix

#endif 