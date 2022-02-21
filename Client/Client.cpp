#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

#pragma warning(disable: 4996)
#define N 256
using namespace std;

int main() {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	const char* ip = "127.0.0.1";
	unsigned short portUDP = 6000, portTCP = 5555;
	char fileName[N] = "test.txt";
	unsigned short timeOut = 500;
	cout << "The following settings are used to connect to the server:"
		"\n\tIP: " << ip << "\n\tportTCP: " << portTCP << "\n\tportUDP: " << portUDP << endl;
	cout << "Other settings:\n\tFile name: " << fileName << "\n\tConfirmation waiting time: " << timeOut << "ms" << endl;

	// адрес сервера TCP
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(portTCP);
	addr.sin_family = AF_INET;
	//---
	// адрес сервера UDP
	SOCKADDR_IN dest_addr;
	int sizeof_dest_addr = sizeof(dest_addr);
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	dest_addr.sin_port = htons(portUDP);
	dest_addr.sin_family = AF_INET;
	//---

	// создание TCP сокета
	SOCKET TCPconn = socket(AF_INET, SOCK_STREAM, NULL);
	if (TCPconn < 0) {
		cout << "UDP socket error" << endl;
		return -1;
	}
	// соединение с сервером
	if (connect(TCPconn, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: failed connect to server.\n";
		return 1;
	}
	else {
		// обмен данными...
		cout << "Connected!" << endl;

		// загрузка файла в память


		// отправка имени файла и порта для UDP сокета
		send(TCPconn, fileName, sizeof(fileName), NULL);
		send(TCPconn, itoa(portUDP, new char, 10), sizeof(itoa(portUDP, new char, 10)), NULL);

		// создание UDP сокета
		SOCKET UDPconn = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
		if (UDPconn < 0) {
			cout << "UDP socket error" << endl;
			return -1;
		}

		while (1) {
			char msg[N];
			cout << "Enter message for send to server: ";
			cin.getline(msg, N); // ввод сообщения 
			// Передача сообщений на сервер
			sendto(UDPconn, msg, strlen(msg), 0,
				(sockaddr*)&dest_addr, sizeof(dest_addr));
			// ВЫХОД ИЗ ЦИКЛА
			if (!strcmp(msg, "quit")) break;

			// отправка файла...
		}

		closesocket(UDPconn);
	}
	closesocket(TCPconn);
	system("pause");
}