/*********************************************************************
 * File: game.cpp
 * Description: Contains the implementaiton of the game class
 *  methods.
 *
 *********************************************************************/

#include "connection.h"

/*********************************************
* Constructors
*********************************************/
Connection::~Connection()
{
	// 5. Disconnect.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-client
	int resultCode;
	if ((resultCode = shutdown(connectionSocket, SD_SEND)) == SOCKET_ERROR) 
	{
	    printf("shutdown failed: %d\n", WSAGetLastError());
	    closesocket(connectionSocket);
	    WSACleanup();
	    return;
	}

	//Cleanup
	closesocket(connectionSocket);
	WSACleanup();
}

/*********************************************
* Public
*********************************************/
/*********************************************
* Misc
*********************************************/
/*********************************************************************
 * initialize()
 * Starts a connection with an entered IP.
 *********************************************************************/
void Connection::initialize() throw (const int)
{
	//CLIENT SETUP
	// 1. Initialize Winsock.
	//https://docs.microsoft.com/en-us/windows/desktop/winsock/creating-a-basic-winsock-application
    int resultCode;
	if ((resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << "WSAStartup failed: " << resultCode << endl;
		throw 1;
	}

	// 2. Create a socket.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
	struct addrinfo *servinfo = NULL;
	struct addrinfo hints;

	PCSTR port = DEFAULT_PORT; //PCSTR = const char* (Pointer Constant STRing)

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	string servIP;
	cout << "Please enter an IP: ";
	cin >> servIP;


	if ((resultCode = getaddrinfo(servIP.c_str(), port, &hints, &servinfo) != 0))
	{
		printf("getaddrinfo failed: %d\n", resultCode);
	    WSACleanup();
		throw 1;
	}

	connectionSocket = socket(servinfo->ai_family, 
								servinfo->ai_socktype, 
									servinfo->ai_protocol);

	if (connectionSocket == INVALID_SOCKET) 
	{
	    printf("Error at socket(): %ld\n", WSAGetLastError());
	    freeaddrinfo(servinfo);
	    WSACleanup();
	    throw 1;
	}

	// 3. Connect to the server.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/connecting-to-a-socket
	if ((resultCode = connect(connectionSocket, servinfo->ai_addr, (int)servinfo->ai_addrlen)) == SOCKET_ERROR)
	{
		closesocket(connectionSocket);
		connectionSocket = INVALID_SOCKET;
	}

	freeaddrinfo(servinfo);

	if (connectionSocket == INVALID_SOCKET)
	{
		cout << "Failed to connect.\n";
		WSACleanup();
		throw 1;
	}

	connectionLive = true;
}

void Connection::disconnect()
{
	// 5. Disconnect.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-client
	int resultCode;
	if ((resultCode = shutdown(connectionSocket, SD_SEND)) == SOCKET_ERROR) 
	{
	    printf("Disconnect failed: %d\n", WSAGetLastError());
	    closesocket(connectionSocket);
	    WSACleanup();
	    return;
	}

	connectionLive = false;
}


void Connection::sendData(string data) throw (const int)
{
	// 4. Send and receive data.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/sending-and-receiving-data-on-the-client
	int resultCode;
	if ((resultCode = send(connectionSocket, data.c_str(), data.size(), 0))
			== SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectionSocket);
		WSACleanup();
		throw 1;	
	}
}

string Connection::receiveData() throw (const int)
{
	char incomingData[MAX_BUF_LEN] = {};
	
	int resultCode = recv(connectionSocket, incomingData, MAX_BUF_LEN - 1, 0);

	string data(incomingData);

	if (resultCode > 0)
		cout << "Received " << resultCode << " bytes: " << data << endl;

	return data;
}