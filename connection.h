/*********************************************************************
 * File: game.h
 * Description: Defines the game class for Asteroids
 *
 *********************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

//Console
#include <iostream>
using namespace std;

//Networking
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "6789"
#define MAX_BUF_LEN 1024

class Connection
{
private:
	SOCKET connectionSocket;
	WSADATA wsaData;
	bool connectionLive;

public:
	Connection() : connectionLive(false)
	{
		int resultCode;
		if ((resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
		{
			cout << "WSAStartup failed: " << resultCode << endl;
		}
	}

	~Connection();

	bool conntected() const { return connectionLive; }

	void initialize() throw (const int);
	void disconnect();

	void sendData(string data) throw (const int);
	string receiveData() throw (const int);	
};

#endif