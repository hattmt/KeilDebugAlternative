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

typedef enum
{
    bkpt_set,
    bkpt_unset,
    bkpt_unset_all,
}bkpt_action;



class Keil_Comm
{

public:
    Keil_Comm();
    
    //Gestion socket TCP IP debug/build/flash
    bool Connect_Keil( string log_file, string UV_path);
    bool Open_Project( string uvprojx_path );
    bool Set_View(uint8_t visible);
    bool Get_Registers(void);
    bool Update_Registers(void);
    bool Enter_Debug_Mode(void);
    string Registers_to_xml(void);
    bool Get_project_value(bool visible);
    string Read_Exp(string expression);
    string Read_memory(uint32_t address, uint32_t size);
    bool Dbg_run ( void );
    bool Dbg_Reset(void);
    bool Dbg_Stop(void);
    bool Dbg_Step_Into(void);
    bool Dbg_is_breakpoint();
    bool Dbg_change_bp(uint32_t address, BKTYPE bkpt_type, uint32_t size, bkpt_action set);
    bool Dbg_write_memory(uint32_t address,char* data, uint32_t size);
    bool Dbg_Write_Reg( uint32_t reg_nb, uint32_t data);
    
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