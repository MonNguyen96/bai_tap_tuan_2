// SimpleClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

DWORD WINAPI ReceiverThread(LPVOID lpParameter);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8000);

	system("PAUSE");
	int res = connect(client, (SOCKADDR *)&addr, sizeof(addr));
	if (res == SOCKET_ERROR)
	{
		res = WSAGetLastError();
		printf("Ma loi: %d", res);
		system("pause");
		return 1;
	}

	CreateThread(0, 0, ReceiverThread, &client, 0, 0);

	char sendBuf[256];
	while (1)
	{
		gets_s(sendBuf);
		send(client, sendBuf, strlen(sendBuf), 0);
		if (strcmp(sendBuf, "exit") == 0)
			break;
	}
	system("PAUSE");
	closesocket(client);
	WSACleanup();

	return 0;
}

DWORD WINAPI ReceiverThread(LPVOID lpParameter)
{
	int res;
	char buf[256];
	SOCKET client = *(SOCKET *)lpParameter;
	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == 0 || res == SOCKET_ERROR)
			break;
		buf[res] = 0;
		puts(buf);
	}

	return 0;
}
