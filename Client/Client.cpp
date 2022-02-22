#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <string>

#pragma warning(disable: 4996
const int N = 256;
const int MAXSIZE = 1024;
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
		fstream Sfile;
		Sfile.open(fileName, ios::in);

		// отправка имени файла и порта для UDP сокета
		send(TCPconn, fileName, sizeof(fileName), NULL);
		send(TCPconn, itoa(portUDP, new char, 10), sizeof(itoa(portUDP, new char, 10)), NULL);

		// создание UDP сокета
		SOCKET UDPconn = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (UDPconn < 0) {
			cout << "UDP socket error" << endl;
			return -1;
		}

		// отправка файла...
		string buffer;
		if (!Sfile.is_open())
			cout << "Unable to find file named: " << fileName;
		else {
			// построчное отправление
			while (getline(Sfile, buffer)) {
				cout << "Sending data of file...\n";
				int s_index = 0, sid = 0; // индекс для строки, идентификатор для датаграмм
				bool flag = false; // флаг для повторной отправки данных
				// если строку можно отправить целиком, то так и отправляем
				if (buffer.length() < MAXSIZE) {
					do {
						char conf[256];
						int sending_data_size = sendto(UDPconn, buffer.c_str(), strlen(buffer.c_str()), 0,
							(sockaddr*)&dest_addr, sizeof(dest_addr));
						// для подтверждения принимаем данные
						recv(TCPconn, conf, sizeof(conf), NULL);
						if (strcmp(conf, "OK"))
							flag = true;
					} while (flag);
				}
				// иначе разбиваем на части
				else {
					int count_iter;
					(buffer.length() % MAXSIZE) != 0 ? 
						count_iter = (buffer.length() / MAXSIZE) + 1 : 
						count_iter = (buffer.length() / MAXSIZE);
					for (int i = 0; i < count_iter; i++) {
						string new_buff;
						if (i == count_iter - 1) {
							new_buff = buffer.substr(i * (MAXSIZE - 1));
							do {
								char conf[256];
								sendto(UDPconn, new_buff.c_str(), strlen(new_buff.c_str()), 0,
									(sockaddr*)&dest_addr, sizeof(dest_addr));
								// для подтверждения принимаем данные
								recv(TCPconn, conf, sizeof(conf), NULL);
								if (strcmp(conf, "OK"))
									flag = true;
							} while (flag);
						}
						else {
							new_buff = buffer.substr(i * (MAXSIZE - 1), (MAXSIZE - 1));
							do {
								char conf[256];
								sendto(UDPconn, new_buff.c_str(), strlen(new_buff.c_str()), 0,
									(sockaddr*)&dest_addr, sizeof(dest_addr));
								// для подтверждения принимаем данные
								recv(TCPconn, conf, sizeof(conf), NULL);
								if (strcmp(conf, "OK"))
									flag = true;
							} while (flag);
						}
					}
				}
				// после каждого цикла сообщаем серверу о новой строчке
				sendto(UDPconn, "\n", strlen("\n"), 0,
					(sockaddr*)&dest_addr, sizeof(dest_addr));
			}
			// сообщение о закрытии файла
			sendto(UDPconn, "quit", strlen("quit"), 0,
				(sockaddr*)&dest_addr, sizeof(dest_addr));
		}
		Sfile.close();
		closesocket(UDPconn);
	}
	closesocket(TCPconn);
	system("pause");
}