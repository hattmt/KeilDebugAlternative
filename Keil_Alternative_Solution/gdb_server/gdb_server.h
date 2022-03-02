#pragma once

#if !defined( _CRT_SECURE_NO_WARNINGS )
    #define _CRT_SECURE_NO_WARNINGS		// This test file is not intended to be secure.
#endif
#include "Keil_Comm.h"
#include <iostream>
#include "tcpipservr.h"


using namespace std;

class Gdb_Server
{

public:
    
    
    
    Gdb_Server();
   

    bool Start(string port, Keil_Comm* keil_comm);
    bool Write(string data);
    bool Loop();
    void Read_gdb_cmd();
    string format_keil_data_registers();


private:
    Tcp_Ip_server server;
    char recv_buf[1000] = { 0 };
    int buf_cursor = 0;

    string prev_command;

    string rx_command;
    Keil_Comm* keil = NULL;
};