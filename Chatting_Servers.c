#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <stdio.h>
#include <process.h>

unsigned WINAPI recvmess(void * arg);
void sendmess(char * message, int len);
int clntnum = 0;

SOCKET Clnt[10];
HANDLE thread, hMutex;
int main() {
	WSADATA wsadata;
	SOCKADDR_IN servAddr, clntAddr;
	SOCKET sock, Client;

	printf("Chating SerVer\n");
	int SERVER_PORT = 9190;
	char SERVER_IP = "127.0.0.1";

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != -1) {
		printf("WSA starting Success!\n");
	}
	hMutex = CreateMutex(NULL, FALSE, NULL);
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock != -1) {
		printf("Server Base setting Siccess!\n");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVER_PORT);

	if (bind(sock, (SOCKADDR *)&servAddr, sizeof(servAddr)) != SOCKET_ERROR) {
		printf("Server Ip Address setting Success!\n");
	}

	if (listen(sock, 5) != SOCKET_ERROR) {
		printf("Server Setting Done!\n");
	}

	printf("Server Port : %d \n", SERVER_PORT);

	while (1) {
		int ClntSz = sizeof(clntAddr);
		Client = accept(sock, (SOCKADDR *)&clntAddr, &ClntSz);

		Clnt[clntnum++] = Client;

		thread = (HANDLE)_beginthreadex(NULL, 0, recvmess, (void*)&Client, 0, NULL);
		printf("The Client has Connect | Client IP : %s \n", inet_ntoa(clntAddr.sin_addr));
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}

unsigned WINAPI recvmess(void * arg) {
	SOCKET Clnts = *((SOCKET *)arg);
	char message[100];
	int len;
	while ((len = recv(Clnts, message, strlen(message) , 0)) != 0)
		sendmess(message, len);

	for (int i = 0; i < clntnum; i++) {
		if (Clnts == Clnt[i]) {
			while (i++ < clntnum - 1) {
				Clnt[i] = Clnt[i + 1];
				break;
			}
		}
	}
	clntnum--;
	closesocket(Clnts);
	return 0;
}

void sendmess(char * message, int len) {
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntnum; i++) {
		send(Clnt[i], message, len , 0);
	}
	ReleaseMutex(hMutex);
}
