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
	char buff[1024];

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
		// обмен данными...
		char HfileName[256];
		char HtempPort[256];
		//char* ptr_temp = new char[256]; // host var
		string fileName;
		unsigned short portUDP = 0;

		cout << "Client Connected!\n";

		// принятие имени и порта для UDP сокета
		recv(TCPconn, HfileName, sizeof(HfileName), NULL); // faleName
		fileName = (string)HfileName;
		// accept file size
		recv(TCPconn, HtempPort, sizeof(HtempPort), NULL);
		portUDP = atoi(HtempPort);
		cout << "Client: \n\tUDPport: " << portUDP << "\n\tFile name: " << fileName << endl;
		
		// создание UDP сокета
		SOCKET UDPconn;
		UDPconn = socket(AF_INET, SOCK_DGRAM, 0);
		if (UDPconn == INVALID_SOCKET) {
			cout << "UDP socket error" << endl;
			return -1;
		}

		// связывание сокета с локальным адресом 
		SOCKADDR_IN local_addr;
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = INADDR_ANY;
		local_addr.sin_port = htons(portUDP);

		if (bind(UDPconn, (sockaddr*)&local_addr, sizeof(local_addr))) {
			cout << "UDP bind error" << endl;
			closesocket(UDPconn);
			return -1;
		}

		// обработка пакетов, присланных клиентами
		while (1)
		{
			sockaddr_in client_addr;
			int client_addr_size = sizeof(client_addr);
			int bsize = recvfrom(UDPconn, &buff[0],
				sizeof(buff) - 1, 0,
				(sockaddr*)&client_addr, &client_addr_size);
			if (bsize == SOCKET_ERROR)
				cout << "recvfrom() error" << endl;
			// добавление завершающего нуля
			buff[bsize] = 0;
			// Вывод на экран 
			printf("C=>S:%s\n", &buff[0]);
			// ВЫХОД ИЗ ЦИКЛА
			if (!strcmp(buff, "quit")) break;

			// принятие и сборка файла...
			
		}
		
	}

	closesocket(TCPconn);
	closesocket(sListen);
	system("pause");
}
