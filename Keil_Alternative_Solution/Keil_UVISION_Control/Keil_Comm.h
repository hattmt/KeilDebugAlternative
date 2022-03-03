/** 
/*   Author: Henri Attimont
/****/

#pragma once

#if !defined( _CRT_SECURE_NO_WARNINGS )
#define _CRT_SECURE_NO_WARNINGS		// This test file is not intended to be secure.
#endif

#if !defined( WIN32_LEAN_AND_MEAN )
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>

#include <windows.h>
#include <crtdbg.h>
#include "UVSC_C.h"
#include "tinyxml2.h"



using namespace std;
using namespace tinyxml2;

class Keil_Comm
{

public:
    Keil_Comm();
    
    //Gestion socket TCP IP debug/build/flash
    bool Connect_Keil( string log_file, string UV_path);
    bool Open_Project( string uvprojx_path );
    bool Set_View( bool visible);
    bool Get_Registers(void);
    bool Update_Registers(void);
    bool Enter_Debug_Mode(void);
    string Registers_to_xml(void);
    bool Get_project_value(bool visible);
    string Read_Exp(string expression);
    string Read_memory(uint32_t address, uint32_t size);
    bool Dbg_run ( void );
    bool Dbg_is_breakpoint();
    
    char reg_values[4096] = { 0 }; int reg_values_length = 4096;
    
    SSTR reg_groups[10] = { 0 }; int reg_groups_nb = 10;//jusqu'a 10 groupes de registres;
    REGENUM registers[100] = { 0 }; int registers_nb = 100;

private:
    string project_path;
    
    //XML project opened
    XMLDocument doc;
    XMLElement* Target = NULL;
    XMLElement* Target_Common_Options = NULL;
    
    
    
    //intialisation communication
    int keil_handle = 1;
    int port = UVSC_PORT_AUTO;

    //registers
    
   
    

    //methods
     //Gestion du fichier projet uvproj
    bool UVPROJX_open(string path);
    bool UVPROJX_close_save();
    string UVPROJX_get_cpu_data(int element);
};
