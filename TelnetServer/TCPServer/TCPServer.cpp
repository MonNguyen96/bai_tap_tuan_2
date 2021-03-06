

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "process.h"

#define bufSize 1024
#define _CRT_SECURE_NO_WARNINGS

FILE* fp;
char buf[bufSize];
char notif[] = "Please enter your user and password";

typedef struct client {
	SOCKET sockclient=0;
	char* id;
}client;

client session[30];
char* checkId(char* mess) {
	if (strstr(mess, "client_id: ") == mess) {
		return mess + sizeof("client_id: ");
	}
	else
		return NULL;
}

int checkAccount(char *acc) {
	if ((fp = fopen("E:\\demo.txt", "r")) == NULL)
	{ /* Open source file. */
		perror("fopen source-file");
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
		if (strcmp(buf, acc) == 0) {
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

unsigned __stdcall echoThread(void *param) {
	int ret;
	char mess[1024];
	int position;
	SOCKET sockclient = (SOCKET)param;
	for (int i = 0; i < 30; i++) {
		if (session[i].sockclient == 0) {
			session[i].sockclient = sockclient;
			position = i;
			break;
		}

	}
	//receive message from client
	while (true) {
		ret = send(sockclient, notif, strlen(notif), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error! Cannot send mesage.");
			return 0;
		}
		ret = recv(sockclient, mess, sizeof(mess), 0);
		mess[ret] = '\0';
		if (ret == SOCKET_ERROR) {
			printf("\nClient disconnected");
			break;
		} else {
			if (checkAccount(mess) == 0)
				continue;
			else {
				printf("Correct!!!");
				break;
			}		
		}
	}
	closesocket(sockclient);
	return 0;
}


int main(int argc, CHAR* argv[])
{
	//Check argument
	if (argc != 2) {
		printf("Usage: %s <portnumber>\n", argv[0]);
		_getch();
		return 0;
	}
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot bind this address.");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		_getch();
		return 0;
	}

	printf("Server started!");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	SOCKET sockclient;
	while (1) {
		
		//accept request
		sockclient = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		_beginthreadex(0, 0, echoThread, (void *)sockclient, 0, 0); //start thread
		
	} //end accepting

	  //Step 5: Close socket
	closesocket(listenSock);

	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}


