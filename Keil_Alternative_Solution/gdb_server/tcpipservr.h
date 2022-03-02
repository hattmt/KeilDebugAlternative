/** 
/*   Author: Henri Attimont
/***

#pragma once

#if !defined( _CRT_SECURE_NO_WARNINGS )
    #define _CRT_SECURE_NO_WARNINGS		// This test file is not intended to be secure.
#endif

#if !defined( WIN32_LEAN_AND_MEAN )
    #define WIN32_LEAN_AND_MEAN
#endif

#if !defined( NO_STDIO_REDIRECT )
    #define NO_STDIO_REDIRECT
#endif


#include <string> 
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h> 

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")



using namespace std;
/*int create_server(void);

int connection_write(const char* data);

int server_loop(void);

int close_server();

int add_offset(const char* cmd);*/


typedef enum {
    BYTE_Received,
    NO_BYTE_Received,
    ERROR_Received
}TCP_RX_Status;

class Tcp_Ip_server
{

 public:
     Tcp_Ip_server();

    //Gestion socket TCP IP debug/build/flash
    bool Create_server(string port );

    bool Write_Server(char* data, int size);

    bool Close_Server();

    TCP_RX_Status Loop();

    unsigned char Read_byte();


private:
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    string default_port = "50000";
    int status;
    char rx_byte = 0;


};
