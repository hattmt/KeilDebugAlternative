#include "tcpipservr.h"


Tcp_Ip_server::Tcp_Ip_server()
{
   
}

bool Tcp_Ip_server::Create_server(string port)
{
    WSADATA wsaData;

    struct addrinfo* result = NULL;
    struct addrinfo hints;


    default_port = port;

    // Initialize Winsock
    status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != 0) {
        printf("WSAStartup failed with error: %d\n", status);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    status = getaddrinfo(NULL, default_port.c_str(), &hints, &result);
    if (status != 0) {
        printf("getaddrinfo failed with error: %d\n", status);
        WSACleanup();
        return false;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    status = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (status == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);
    
    printf("Server created and ready on %s\n", port.c_str());

    status = listen(ListenSocket, SOMAXCONN);
    if (status == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    return true;
}

bool Tcp_Ip_server::Write_Server( char * data, int size )
{
    status = send(ClientSocket, (const char*)data, size, 0);

    if (status == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return false;
    }

    return true;
}

bool Tcp_Ip_server::Close_Server()
{
    // shutdown the connection since we're done
    status = shutdown(ClientSocket, SD_SEND);

    if (status == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return false;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return true;
}


unsigned char Tcp_Ip_server::Read_byte()
{
    return rx_byte;
}


TCP_RX_Status Tcp_Ip_server::Loop()
{   
    status = recv(ClientSocket, &rx_byte, 1, 0);

    if ( status > 0 ) {
        return BYTE_Received;
    }
    else if (status == 0) {
        printf("Connection closing...\n");
        Close_Server();
        return ERROR_Received;
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        Close_Server();
        return ERROR_Received;
    }

    return NO_BYTE_Received;
}
