#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

using namespace std;

int main() {
	//WSAStartup
	// загрузка необходимой библиотеки
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}
	//---

	const char* ip = "127.0.0.1";
	unsigned short portTCP = 5555;
	char download_dir[256] = "temp";

	cout << "The server is ready to go, your default settings:\n\tIP: " << ip << "\n\tport: " << portTCP << "\n\tDownload directory: " << download_dir << endl;

	// адресс
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = INADDR_ANY; //inet_addr(ip);
	addr.sin_port = htons(portTCP);// если задано 0, то ОС сама выделит свободный номер порта для сокета
	addr.sin_family = AF_INET; // семейство протоколов, всегда равно AF_INET или AF_INET6 для IPv6
	//---

	// создание TCP сокета
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // создает безымянный сокет, определяющий домен (1й арг), тип (2й арг) и протокол (3й арг) подключения
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // связываем сокет с адресом хоста и портом 
	listen(sListen, SOMAXCONN); // прослушивание (SOMAXCONN содержит длину очереди входящих запросов на установление соединения (кол подключаемых устройств))

	cout << "Waiting for a connection..." << endl;
	SOCKET TCPconn;
	TCPconn = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr); // извлечение запросов на подключение из прослушивания
	//---

	if (TCPconn == 0) {
		cout << "Error #2\n";
	}
	else {
		char fileName[256];
		char portUDP[256];

		// передача данных...
		cout << "Client Connected!\n";
		char msg[256] = "The server is ready to accept the file!";
		send(TCPconn, msg, sizeof(msg), NULL);

		// принятие имени и размера файла, а также порта для UDP сокета
		recv(TCPconn, fileName, sizeof(msg), NULL);
		// accept file size
		recv(TCPconn, portUDP, sizeof(msg), NULL);
		cout << "Client: \n\tUDPport: " << portUDP << "\n\tFile name: " << fileName << endl;
	}

	closesocket(TCPconn);
	closesocket(sListen);
	system("pause");
}
