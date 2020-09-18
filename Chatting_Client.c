#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdio.h>
#include <process.h>

SOCKET sock;

unsigned WINAPI sendmsgs(void * arg);
unsigned WINAPI recvmsgs(void * arg);


int main() {
	WSADATA wsadata;
	SOCKADDR_IN servAddr, clntAddr;
	HANDLE sendmsg, recvmsg;

	int SERVER_PORT = 9190;

	printf("welcome to chating Server!\n");


	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != -1) {
		printf("WSA starting Success!\n");
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock != -1) {
		printf("Server Base setting Success!\n");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(SERVER_PORT);

	if (connect(sock, (SOCKADDR *)&servAddr, sizeof(servAddr)) != SOCKET_ERROR) {
		printf("You are connected to the server.\n");
	}
	else {
		printf("server is Offline\n");
		system("pause");
		exit(1);
	}
	printf("if you want exit then put q!\n");

	sendmsg = (HANDLE)_beginthreadex(NULL, 0, sendmsgs, (void *)&sock, 0, NULL);

	recvmsg = (HANDLE)_beginthreadex(NULL, 0, recvmsgs, (void *)&sock, 0, NULL);

	WaitForSingleObject(sendmsg, INFINITE);
	WaitForSingleObject(recvmsg, INFINITE);

	closesocket(sock);
	WSACleanup();
	return 0;
}

unsigned WINAPI sendmsgs(void * arg) {
	SOCKET Clnt = *((SOCKET *)arg);
	char message[120];

	while (1) {
		fgets(message, 120, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
			closesocket(Clnt);
			exit(0);
		}
		send(Clnt, message, strlen(message), 0);
	}
	return 0;
}

unsigned WINAPI recvmsgs(void * arg) {
	int Clnt = *((SOCKET *)arg);
	char message[120];
	int len;
	while (1) {
		len = recv(Clnt, message, 120, 0);
		if (len == -1) {
			return -1;
		}
		message[len] = 0;
		printf("Message : %s \n" , message);
	}
	return 0;
}
