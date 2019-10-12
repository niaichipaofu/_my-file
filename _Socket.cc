
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
//   v1.1 JUN 2019
//  
//----------------------------------------------------------------------------




#include "_Socket.h"

//-----------------------------------------------------------------------------
// 基于IPV4_TCP网络通信协议的客户端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用函数Client（ip，port）
// 函数进行创建。
//-----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
//函数声明：bool Client_eastablish();	
//函数参数：   无
//函数功能：
//		创建socket获取ID号
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_eastablish()
{

#ifdef _WIN32
	if (WSAStartup(this->SocketVersion, &this->wsaData) != 0)
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#endif
	this->Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef _WIN32
	if (this->Client == INVALID_SOCKET)
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#elif __linux__
	if (this->Client == -1)
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}
#endif
	printf("\n[Client Eastablish Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：void Client_Setaddress();	
//函数参数：   无
//函数功能：
//		配置客户端端口地址及所用协议
//----------------------------------------------------------------------------------------------
void Socket_Client_IPv4_TCP::Client_Setaddress()
{
	this->Sockaddr_in.sin_family = AF_INET;
	this->Sockaddr_in.sin_port = htons(this->_Port);
#ifdef _WIN32
	this->Sockaddr_in.sin_addr.S_un.S_addr = inet_addr(this->_IP.c_str());
#elif __linux__
	this->Sockaddr_in.sin_addr.s_addr = inet_addr(this->_IP.c_str());
#endif
	printf("\n[Client Setaddress Successfully]\n");
	return;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Client_connect();	
//函数参数：   无
//函数功能：
//		连接指定IP地址上的服务端
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_connect()
{
	static unsigned short number = 500;
	while (connect(this->Client, (sockaddr*)& this->Sockaddr_in, sizeof(this->Sockaddr_in)) == -1)
	{
		number--;
		if (!number)	//尝试连接500次
		{
#ifdef _WIN32
			printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
			this->connectstatus = false;
			return false;
#elif __linux__
			this->error_message = strerror(errno);
			printf("\n%s\n", this->error_message);
			this->connectstatus = false;
			return false;

#endif

		}
	}
	printf("\n[Client Connect Successfully][Server_ip=%s  Server_port=%d]\n", this->_IP.c_str(), this->_Port);

	this->connectstatus = true;
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Client_sendStr();	
//函数参数：   _str =[输入的字符串，默认Hello World！]
//函数功能：
//		发送给服务端一个字符串
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_sendStr(std::string _str)
{
	if (send(this->Client, _str.c_str(), strlen(_str.c_str()) + 1, 0) < 0)
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Client SendString Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Client_receverStr();	
//函数参数：   *buf = [储存字符串]
//函数功能：
//		接收一个字符串
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_receverStr(char* buf)
{
	if (!recv(this->Client, buf, this->_Buflen, 0))
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Client ReceverString Successfully]\n");
	return true;
}



//----------------------------------------------------------------------------------------------
//函数声明：bool Client_sendFile();	
//函数参数：  std::string _File_path =[ 一个文件的路径]
//函数功能：
//		发送一个指定的文件
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_sendFile(std::string _File_path)
{
	int readLen = 0;

	std::ifstream infile;
	infile.open(_File_path.c_str(), std::ios::binary);
	if (!infile)
	{
		printf("\n[ERROR OPENING FILE!!!]\n");
		return false;
	}
	while (!infile.eof())				//文件结束标志位的判断
	{
		infile.read(this->_Buf, this->_Buflen);
		readLen = infile.gcount();		//获取流中的长度
		send(this->Client, this->_Buf, readLen, 0);
	}
	infile.close();
	printf("\n[Client SendFile Successfully]\n");
	return true;
}



//----------------------------------------------------------------------------------------------
//函数声明：bool Client_receverFile();	
//函数参数：  std::string _File_path =[ 一个文件的路径]
//函数功能：
//		接收一个指定的文件
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Client_receverFile(std::string _File_path)
{

	int readLen = 0;

	std::ofstream outfile;
	outfile.open(_File_path.c_str(), std::ios::binary);
	if (!outfile)
	{
		printf("\n[ERROR OPENING FILE!!!]\n");
		return false;
	}
	do
	{
		readLen = recv(this->Client, this->_Buf, this->_Buflen, 0);
		if (readLen == 0)
		{
			break;
		}
		else
		{
			outfile.write(this->_Buf, readLen);
		}
	} while (true);
	outfile.close();
	printf("\n[Client ReceverFile Successfully]\n");
	return true;
}



//----------------------------------------------------------------------------------------------
//函数声明：bool Clinet_close();	
//函数参数：   无
//函数功能：
//		关闭socket ，释放占用ID
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::Clinet_close()
{
#ifdef _WIN32
	if (closesocket(this->Client))
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
	if (WSACleanup())
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#elif __linux__
	if (close(this->Client) == -1)
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}
#endif
	printf("\n[Client Closed Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool _Client();	
//函数参数：   无
//函数功能：
//		创建并配置好客户端(简单用例)
//----------------------------------------------------------------------------------------------
bool Socket_Client_IPv4_TCP::_Client()
{
	if (!(this->Client_eastablish()))
	{
		printf("\n[Client Error eastablish !!!]\n");
		return false;
	}
	this->Client_Setaddress();

	return true;
}



//-----------------------------------------------------------------------------
// 基于IPV4_TCP网络通信协议的客户端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用函数Client（ip，port）
// 函数进行创建。
//-----------------------------------------------------------------------------

//*********************************************************************************************************

//-----------------------------------------------------------------------------
// 基于IPV4_TCP网络通信协议的服务端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用函数Sever（ip，port）
// 函数进行创建。
//-----------------------------------------------------------------------------




//----------------------------------------------------------------------------------------------
//函数声明：bool Server_eastablish();	
//函数参数：   无
//函数功能：
//		初始化服务端并获取一个服务端ID
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_eastablish()
{
#ifdef _WIN32
	if (WSAStartup(this->SocketVersion, &this->wsaData) != 0)
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#endif
	this->Sever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef _WIN32
	if (this->Sever == INVALID_SOCKET)
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#elif __linux__
	if (this->Sever == -1)
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}
#endif
	printf("\n[Server Eastablish Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：void Server_Setaddress();	
//函数参数：   无
//函数功能：
//		配置服务端地址信息
//----------------------------------------------------------------------------------------------
void Socket_Server_IPv4_TCP::Server_Setaddress()
{
	this->Sockaddr_in.sin_family = AF_INET;
	this->Sockaddr_in.sin_port = htons(this->_Port);
#ifdef _WIN32
	this->Sockaddr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif __linux__
	this->Sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	printf("\n[Server Setaddress Successfully]\n");
	return;

}

//----------------------------------------------------------------------------------------------
//函数声明：bool Server_Bindaddress();	
//函数参数：   无
//函数功能：
//		将地址信息与服务端ID进行绑定
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_Bindaddress()
{
	if (bind(this->Sever, (struct sockaddr*) & this->Sockaddr_in, sizeof(this->Sockaddr_in)) == -1)
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Server Bindaddress Successfully]\n");
	return true;
}

//----------------------------------------------------------------------------------------------
//函数声明：bool Server_Listening();	
//函数参数：   无
//函数功能：
//		开启监听模式，监听指定ip及端口
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_Listening()
{
	if (listen(this->Sever, this->_waitlen) == -1)
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Server Starts Listening]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Server_accept();	
//函数参数：   无
//函数功能：
//		等待客户端连接
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_accept()
{
#ifdef _WIN32
	if ((this->Client = accept(this->Sever, (SOCKADDR*)& this->_Client_address, &this->client_addresslen)) == INVALID_SOCKET)
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#elif __linux__
	if ((this->Client = accept(this->Sever, (struct sockaddr*) & this->_Client_address, &this->client_addresslen)) == -1)
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}
#endif
	printf("\n[Successful Access with Client][Client_ip=%s  Client_port=%d]\n", inet_ntoa(this->_Client_address.sin_addr), ntohs(this->_Client_address.sin_port));
	accessDevice Device;
	time_t now = time(0);
	tm* ltm = localtime(&now);
	Device.ip = inet_ntoa(this->_Client_address.sin_addr);
	Device.port = ntohs(this->_Client_address.sin_port);
	Device.tm_year = ltm->tm_year;
	Device.tm_mon = ltm->tm_mon;
	Device.tm_mday = ltm->tm_mday;
	Device.tm_hour = ltm->tm_hour;
	Device.tm_min = ltm->tm_min;
	Device.tm_sec = ltm->tm_sec;

	this->clientDevice.push_back(Device);
	return true;
}

//----------------------------------------------------------------------------------------------
//函数声明：bool Server_sendStr();	
//函数参数：   std::string _str=[发送一个字符串指定[Hello World!]]
//函数功能：
//		向客户端发送一个字符串
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_sendStr(std::string _str)
{

	if (send(this->Client, _str.c_str(), strlen(_str.c_str()) + 1, 0) < 0)
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Server SendString Successfully]\n");
	return true;
}

//----------------------------------------------------------------------------------------------
//函数声明：bool Server_receverStr();	
//函数参数：  char* buf -接收字符串
//函数功能：
//		从客户端接收一个字符串
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_receverStr(char* buf)
{
	if (!recv(this->Client, buf, this->_Buflen, 0))
	{
#ifdef _WIN32
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
#elif __linux__
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;

#endif
	}
	printf("\n[Server ReceverString Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Server_sendFile();	
//函数参数： std::string _File_path - [文件路径默认in.txt]
//函数功能：
//		向客户端发送一个文件
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_sendFile(std::string _File_path)
{
	int readLen = 0;

	std::ifstream infile;
	infile.open(_File_path.c_str(), std::ios::binary);
	if (!infile)
	{
		printf("\n[ERROR OPENING FILE!!!]\n");
		return false;
	}
	while (!infile.eof())				//文件结束标志位的判断
	{
		infile.read(this->_Buf, this->_Buflen);
		readLen = infile.gcount();		//获取流中的长度
		send(this->Client, this->_Buf, readLen, 0);
	}
	infile.close();
	printf("\n[Server SendFile Successfully]\n");
	return true;
}


//----------------------------------------------------------------------------------------------
//函数声明：bool Server_receverFile();	
//函数参数： std::string _File_path - [文件路径默认out.txt]
//函数功能：
//		从客户端接收一个文件
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_receverFile(std::string _File_path)
{
	int readLen = 0;

	std::ofstream outfile;
	outfile.open(_File_path.c_str(), std::ios::binary);
	if (!outfile)
	{
		printf("\n[ERROR OPENING FILE!!!]\n");
		return false;
	}
	do
	{
		readLen = recv(this->Client, this->_Buf, this->_Buflen, 0);
		if (readLen == 0)
		{
			break;
		}
		else
		{
			outfile.write(this->_Buf, readLen);
		}
	} while (true);
	outfile.close();
	printf("\n[Server ReceverFile Successfully]\n");
	return true;
}

//----------------------------------------------------------------------------------------------
//函数声明：bool Server_close();	
//函数参数： 无
//函数功能：
//		关闭服务端
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::Server_close()
{
#ifdef _WIN32
	if (!closesocket(this->Client))
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
	if (!closesocket(this->Sever))
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
	if (!WSACleanup())
	{
		printf("\n[ERROR OCCURRING!!!]=[ERROR CODE=%d]\n", WSAGetLastError());
		return false;
	}
#elif __linux__
	if (!close(this->Client))
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}
	if (!close(this->Sever))
	{
		this->error_message = strerror(errno);
		printf("\n%s\n", this->error_message);
		return false;
	}

#endif
	printf("\n[Server close Successfully]\n");
	return true;
}

//----------------------------------------------------------------------------------------------
//函数声明：bool _Server();	
//函数参数：   无
//函数功能：
//		创建并配置好服务端并开启监听模式(简单用例)
//----------------------------------------------------------------------------------------------
bool Socket_Server_IPv4_TCP::_Server()
{
	if (!(this->Server_eastablish()))
	{
		printf("\n[Server Error Eastablish !!!]\n");
		return false;
	}
	this->Server_Setaddress();
	if (!(this->Server_Bindaddress()))
	{
		printf("\n[Server Error Bindaddress !!!]\n");
		return false;
	}if (!(this->Server_Listening()))
	{
		printf("\n[Server Error Listening !!!]\n");
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// 基于IPV4_TCP网络通信协议的服务端类。
//
// 开放系统函数，使用者可以自行进行创建，也可直接使用函数Sever（ip，port）
// 函数进行创建。
//-----------------------------------------------------------------------------





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
//   v1.1 JUN 2019
//  
//----------------------------------------------------------------------------
