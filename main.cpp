// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "time.h"
#include"_Socket.h"

int main()
{
	

	Socket_Client_IPv4_TCP client("127.0.0.1", 9514);//client("193.112.91.66",9514);
	client._Client();
	char send_buf[]="[杨康][ 我是客户端]";
	char recevie_buf[100];
	clock_t start, finish;
	double  duration;
	if (client.Client_connect())
	{
		client.Client_sendStr(send_buf);
		//client.Client_sendFile("1.jpg");
		client.Client_receverStr(recevie_buf);
		printf("\n%s\n",recevie_buf);

	}

	client.Clinet_close();
	
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
