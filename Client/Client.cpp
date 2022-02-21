#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

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
	char fileName[256] = "test.txt";
	unsigned short timeOut = 500;
	cout << "The following settings are used to connect to the server:\n\tIP: " << ip << "\n\tportTCP: " << portTCP << "\n\tportUDP: " << portUDP << endl;
	cout << "Other settings:\n\tFile name: " << fileName << "\n\tConfirmation waiting time: " << timeOut << "ms" << endl;

	// адрес сервера
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(portTCP);
	addr.sin_family = AF_INET;
	//---

	// создание TCP сокета
	SOCKET TCPconn = socket(AF_INET, SOCK_STREAM, NULL);
	// соединение с сервером
	if (connect(TCPconn, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: failed connect to server.\n";
		return 1;
	}
	else {
		// обмен данными
		cout << "Connected!\n";
		char msg[256];
		recv(TCPconn, msg, sizeof(msg), NULL);
		cout << "Server: " << msg << endl;

		// загрузка файла в память


		// отправка имени и размера файла, а также порта для UDP сокета
		send(TCPconn, fileName, sizeof(msg), NULL);
		// send file size		
		send(TCPconn, itoa(portUDP, new char, 10), sizeof(msg), NULL);

		// создание UDP сокета



	}
	closesocket(TCPconn);
	system("pause");
}