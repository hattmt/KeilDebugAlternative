/** 
/*   Author: Henri Attimont
/****/
#include "Keil_Comm.h"
#include "gdb_server.h"

static bool Dbg_brkpt = false;

#define RETURN_RESULT(func) \
 if(func==UVSC_STATUS_SUCCESS) return true; \
 else return false;\
 
void cb_data(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA * data);
void log_cb_data(const char* msg, int msgLen);

extern Gdb_Server gdb_server;
Keil_Comm::Keil_Comm(){}


bool Keil_Comm::Connect_Keil(string log_file, string UV_path)
{
	UVSC_STATUS status;

	status = UVSC_Init(5101, 5110);

	if (status != UVSC_STATUS_SUCCESS)
		return false;

	//status = UVSC_OpenConnection( NULL, &keil_handle, &port, (char*)UV_path.c_str(), UVSC_RUNMODE_NORMAL, cb_data, NULL, (char*)log_file.c_str(), false, log_cb_data);
	status = UVSC_OpenConnection(NULL, &keil_handle, &port, NULL, UVSC_RUNMODE_NORMAL, cb_data, NULL, (char*)log_file.c_str(), false, log_cb_data);


	if (status != UVSC_STATUS_SUCCESS)
		return false;

	return true;
}


bool Keil_Comm::Open_Project(string uvprojx_path)
{
	PRJDATA* prjdata = NULL;
	int size_path = 0;

	size_path = uvprojx_path.length() + 1;
	prjdata = (PRJDATA*)calloc(1, sizeof(PRJDATA) + size_path);

	memcpy((char*)prjdata->szNames, uvprojx_path.c_str(), size_path);
	prjdata->nLen = size_path;

	this->UVPROJX_open(uvprojx_path);

	RETURN_RESULT(UVSC_PRJ_LOAD( keil_handle, prjdata, sizeof(PRJDATA) + size_path))
}


bool Keil_Comm::Set_View(bool visible)
{
	if ( visible == true )
		RETURN_RESULT(UVSC_GEN_SHOW(keil_handle))
	else
		RETURN_RESULT(UVSC_GEN_HIDE(keil_handle))
}

bool Keil_Comm::Get_project_value(bool visible)
{
	return 1;
}


bool Keil_Comm::Get_Registers(void)
{
	reg_values_length = 4096;
	reg_groups_nb = 10;
	registers_nb = 100;

	//groupes de registres
	UVSC_DBG_ENUM_REGISTER_GROUPS(keil_handle, reg_groups, &reg_groups_nb);

	//registres + valeurs
	UVSC_DBG_ENUM_REGISTERS(keil_handle, registers, &registers_nb);

	//valeurs uniquement (a la suite dans un tableau)
	UVSC_DBG_READ_REGISTERS(keil_handle, reg_values, &reg_values_length);

	return false;
}


bool Keil_Comm::Update_Registers(void)
{
	reg_values_length = 4096;

	//valeurs uniquement (a la suite dans un tableau)
	UVSC_DBG_READ_REGISTERS(keil_handle, reg_values, &reg_values_length);

	return false;
}


string Keil_Comm::Read_Exp( string expression )
{
	VSET *set = (VSET*)calloc(1, sizeof(VSET) + expression.size());

	memcpy( &set->str, expression.c_str(), expression.size());
	UVSC_DBG_CALC_EXPRESSION(this->keil_handle, set, sizeof(VSET) + expression.size());
	free(set);
	return "";
}



string Keil_Comm::Read_memory( uint32_t address , uint32_t size )
{
	AMEM* amem = (AMEM*)calloc(1, sizeof(AMEM) + size);
	string ret="";
	char val[3] = { 0 };

	amem->nAddr = address;
	amem->nBytes = size;

	UVSC_DBG_MEM_READ( this->keil_handle, amem, sizeof(AMEM) + size);

	char parameters[10] = "";
	
	for (int i = 0; i < size; i++)
	{
		sprintf(val, "%02hhX", amem->aBytes[i]);
		ret += val;
		memset(val, 0, 3);
	}
	
	free(amem);
	return ret;
}

string Keil_Comm::Registers_to_xml(void)
{
	XMLDocument xml_register;

	
	string cpu_type = this->UVPROJX_get_cpu_data(0);

#ifdef DEBUG
	cout << "CPU Type:" << this->UVPROJX_get_cpu_data(0)<<endl;
#endif
	
	//Cortex M: on prend org.gnu.gdb.arm.m-profile et org.gnu.gdb.arm.m-system https://sourceware.org/gdb/onlinedocs/gdb/ARM-Features.html
	//if( cpu_type.find("Cortex-M") != string::npos )
		xml_register.LoadFile("arm_m.xml");

	XMLPrinter printer;
	xml_register.Accept(&printer);
	//cout << printer.CStr();

	return  printer.CStr();


	
	string xml_file = "";
	ostringstream stream;



	stream << "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target version=\"1.0\"><architecture>arm</architecture>";;

	//xml_file += "< feature name = \"org.gnu.gdb.arm.m-profile\">";
	/*xml_file += "< feature name = \"org.gnu.gdb.arm.m-system\">";*/

	int k = 0;
	stream << "<feature name=\"org.gnu.gdb.arm.m-profile\">";

	for (int j = 0; j < reg_groups_nb; j++)
	{


		for (int i = 0; i < 17; i++)
		{
			if (registers[i].nGi == j)
			{


				if (registers[i].isPC == 1)
					stream << "<reg name=\"pc";
				else
					stream << "<reg name=\"" << registers[i].szReg;


				stream << "\" bitsize=\"" << 32;
				stream << "\" regnum=\"" << k;
				stream << "\" save-restore=\"" << "yes";


				if (registers[i].isPC == 1)
					stream << "\" type=\"" << "code_ptr";
				else
					stream << "\" type=\"" << "int";
				//stream << "\" group=\"" << reg_groups[j].szStr;
				stream << "\" group=\"" << "general";
				stream << "\"/>";
				k++;
			}
		}


	}
	stream << "</feature>";
	stream << "</target>";

	xml_file += stream.str();


	ofstream myfile;
	myfile.open("test.xml");
	myfile << xml_file;
	myfile.close();

	/* char* data = (char*)xml_file.c_str();

	for (int i = 0; i < xml_file.length(); i++)
	{
		if (data[i] = '"')
			data[i] = 34;
	}*/


	return xml_file;
}

bool Keil_Comm::Enter_Debug_Mode(void)
{
	
	UVSC_STATUS status;
	status=  UVSC_DBG_ENTER(keil_handle) ;
	return 1;
}



/*   UVPROJX file parsing  */
bool Keil_Comm::UVPROJX_open( string path )
{
	//open keil project
	doc.LoadFile(path.c_str());
	
	//get data
	Target = doc.FirstChildElement("Project")->FirstChildElement("Targets")->FirstChildElement("Target");
	Target_Common_Options = Target->FirstChildElement("TargetOption")->FirstChildElement("TargetCommonOption");

	return 1;
}


/*
%*[^(]   read and discard everything up to opening paren
(        read and discard the opening paren
%[^)]    read and store up up to (but not including) the closing paren
%*[^\n]  read and discard up to (but not including) the newline
*/

string Keil_Comm::UVPROJX_get_cpu_data(int element)
{
	char RAM[30] = { 0 };
	char ROM[30] = { 0 };
	char CLOCK[30] = { 0 };
	char CPU[30] = { 0 };
	
	const char* title = Target_Common_Options->FirstChildElement("Cpu")->GetText();
	
	sscanf(title, "%*[^(](%[^)]%*[^(](%[^)])%*[^(](%[^)]%*[^(](\"%[^\"])", RAM, ROM, CLOCK,CPU);

	
	switch (element)
	{
		case 0:
			return string(CPU);
		break;

		case 1:
			return string(RAM);
		break;

		case 2:
			return string(ROM);
		break;

		case 3:
			return string(CLOCK);
		break;

	default:
		return string();
		break;
	}

}

bool breakpoint;

bool Keil_Comm::Dbg_run(void)
{

	UVSC_STATUS status;
	status = UVSC_DBG_START_EXECUTION(this->keil_handle);
	return 1;
}

bool Keil_Comm::Dbg_is_breakpoint(void)
{
	if (Dbg_brkpt == true)
	{
		Dbg_brkpt = false;
		return true;
	}

	return false;
}

bool Keil_Comm::UVPROJX_close_save()
{
	doc.SaveFile("D:/programming/debugkeil/debugkeil/MDK-ARM/debugkeil.uvprojx");
	this->Open_Project("D:/programming/debugkeil/debugkeil/MDK-ARM/debugkeil.uvprojx");
	return 1;
}


/* TCP IP UVSOCK communication */
void cb_data(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA* data)
{
	UVSOCK_CMD_RESPONSE cmd_rep;
		if (data ->msg.m_eCmd == UV_ASYNC_MSG)
		{
			if (data->msg.data.cmdRsp.cmd == UV_DBG_STOP_EXECUTION )
			{
				gdb_server.Write("T05");
			}
		}
}


void log_cb_data(const char* msg, int msgLen)
{
#ifdef DEBUG
	printf("%s\n", msg);
#endif // DEBUG

	
}
