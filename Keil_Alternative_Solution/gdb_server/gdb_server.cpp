/** 
/*   Author: Henri Attimont
/****/

#include "gdb_server.h"
#include "tcpipservr.h"


#define TAB_LENGTH 8



size_t unhexify(uint8_t* bin, const char* hex, size_t count);

string endian_swap(string data) {
	string tmp="";

	for ( int i = 0; i < data.length(); i+=2 )
	{
		tmp.insert(tmp.begin(), data[i + 1]);
		tmp.insert(tmp.begin(), data[i]);
	}

	return tmp;
}




string Gdb_Server::format_keil_data_registers( )
{
	string final_data = "";

	for (int i = 0; i < 17; i++)
	{
#ifdef DEBUG
		cout <<" "<< keil->registers[i].szReg << " " << keil->registers[i].szVal<<"\n";
#endif
		string tmp = string(keil->registers[i].szVal + 2);
		tmp = endian_swap(tmp);
		final_data += tmp;
	}

	for (int i = 24; i < 26; i++)
	{
#ifdef DEBUG
		cout << " " << keil->registers[i].szReg << " " << keil->registers[i].szVal << "\n";
#endif
		string tmp = string(keil->registers[i].szVal + 2);
		tmp = endian_swap(tmp);
		final_data += tmp;
	}

	final_data += "FF";//base pri
	final_data += "FFFF";//fault mask

	for (int i = 27; i < 28; i++)
	{
#ifdef DEBUG
		cout << " " << keil->registers[i].szReg << " " << keil->registers[i].szVal << "\n";
#endif
		string tmp = string(keil->registers[i].szVal + 2);
		tmp = endian_swap(tmp);
		final_data += tmp;
	}
#ifdef DEBUG
	cout << "final " << final_data;
#endif

	return final_data;
}

void Gdb_Server::Read_gdb_cmd()
{

	printf("receive from client %s\n", rx_command.c_str());


	if (rx_command == "c") //continue
	{
		keil->Dbg_run();
	}

	if (rx_command == "!" || rx_command == "QStartNoAckMode" || rx_command == "Hg0" || rx_command == "Hc0" || rx_command == "Hc-1"|| rx_command == "qSymbol::" )
	{
		this->Write("OK");
	}

	if (rx_command == "qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;fork-events+;vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+")
		this->Write("PacketSize=10000;qXfer:memory-map:read+;qXfer:features:read+;qXfer:threads:read+;QStartNoAckMode+;vContSupported+");

	if (rx_command == "vMustReplyEmpty" || rx_command == "qTStatus")
		this->Write("");

	if (rx_command == "?")
		this->Write("S02");

	if (rx_command == "qXfer:threads:read::0,1000")
		this->Write("l<?xml version=\"1.0\"?><threads></threads>");


	if (rx_command == "qC")
		this->Write("QC0");

	if (rx_command == "qAttached")
		this->Write("1");

	if (rx_command == "qOffsets")
		this->Write("Text=0;Data=0;Bss=0");
	
	
	if (rx_command == "qXfer:features:read:target.xml:0,1000")
	{
		this->Write("l" + keil->Registers_to_xml());
	}

	if (rx_command == "g")
	{
		keil->Get_Registers();

		//this->Write("00000000"+"c0000000"+"00000020"+"66000000"+"ec1f0020"+"ffffffff"+"ffffffff"+"ffffffff"+"ffffffff"+"ffffffff"+"ffffffff"+"ffffffff"+"00ed00e0"+"e81f0020"+"67040008"+"e2030008"+"00000001"+"ffffffff"+"ffffffff"+ "ffffffffffffffffffffffff");
		//this->Write("00000000c00000000000002066000000ec1f0020ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00ed00e0e81f002067040008e203000800000001ffffffffffffffffffffffff");
					 //2000007820000278200002782000027808000F14200000180000000000000000FFFFFFFFFFFFFFFF08000F1408000F14FFFFFFFF200006780800014708000E9C0100000020000678FFFFFFFCFFFF00
					 //ec1f0020f9ffffff00000000c0000000000000206600000000ed00e067040008e20300080000000102000000616466636673646400ffffff6c040020d30b0008
		this->Write(format_keil_data_registers());
		
		//this->Write("00000000c00000000000002066000000ec1f0020ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00ed00e0e81f002067040008e203000800000001");
		//this->Write(string(keil->reg_values));
	}

	if (rx_command == "qXfer:memory-map:read::0,1000")
	{
		this->Write("l<memory-map><memory type=\"ram\" start=\"0x00000000\" length=\"0x08000000\"/><memory type=\"flash\" start=\"0x08000000\" length=\"0x10000\"><property name=\"blocksize\">0x400</property></memory><memory type=\"ram\" start=\"0x08010000\" length=\"0xf7ff0000\"/></memory-map>");
	}


	if (rx_command == "vCont?")
	{
		this->Write("vCont;c;C sig;s;S;t;r start,end");
	}


	if (rx_command[0] == 'Z') // insertion de breakpoint
	{
		uint8_t type = 0;
		uint32_t addresse = 0;

		sscanf(rx_command.c_str(), "z%u,%X", &type, &addresse);

		switch (rx_command[1])
		{
		case '1':

			break;

		case '2':

			break;
		case '3':

			break;

		case '4':

			break;

		default:
			break;
		}
	}

	if (rx_command[0] == 'z') // suppression de breakpoint
	{
		uint8_t type = 0;
		uint32_t addresse = 0;

		sscanf(rx_command.c_str(), "z%u,%X",&type, &addresse);

		switch (rx_command[1])
		{
		case '1':

			break;

		case '2':

			break;
		case '3':

			break;

		case '4':

			break;

		default:
			break;
		}
			
		//sscanf(address_str.c_str(), "%X", &address);
	}


	if (rx_command[0] == 'm') // lecture memoire
	{
		uint32_t address = 0;
		uint32_t size = 0;

		size_t find_coma = rx_command.find(',');

		string address_str = rx_command.substr(1, find_coma - 1);
		string size_str = rx_command.substr(find_coma + 1, rx_command.length() - find_coma);

		sscanf( address_str.c_str(), "%X", &address);
		sscanf( size_str.c_str(), "%u", &size);

#ifdef DEBUG
		printf("address = %X , size = %u\n", address, size);
#endif

		this->Write( keil->Read_memory(address, size));
	}


	if (rx_command.find("qRcmd") != string::npos ) // lecture memoire
	{
		uint8_t buff[500] = { 0 };

		size_t find_coma = rx_command.find(',');

		string data_str = rx_command.substr(find_coma + 1, rx_command.length() - find_coma);

		unhexify(buff, data_str.c_str(), data_str.length());

		printf("received from client Unexified: \"%s\"\n", buff);


	}

}






Gdb_Server::Gdb_Server()
{
}

bool Gdb_Server::Start(string port, Keil_Comm* keil_comm)
{
	this->keil = keil_comm;
	keil->Enter_Debug_Mode();
	return server.Create_server(port);
}


bool Gdb_Server::Write(string data)
{
	prev_command = data;
	
	bool status = false;

    int size = data.length();
    char* tab = (char*)calloc(size + 6, 1);

    unsigned char hash = 0;

	if (tab == NULL)
		return false;

    for (int i = 0; i < size; i++)
        hash += data[i] % 256;

    sprintf(tab, "$%s#%02hhX", data.c_str(), hash);


    printf("write to client: %s\n", tab);

	
	status = server.Write_Server(tab, size + 4);

	free(tab);

	return status;
}





bool Gdb_Server::Loop()
{
	TCP_RX_Status status = server.Loop();
	unsigned char hash = 0;


	
	if ( status == BYTE_Received )
	{
		recv_buf[buf_cursor] = server.Read_byte();
		buf_cursor++;

		if (recv_buf[0] == '+')
		{
#ifdef DEBUG
			printf("Client ACKN\n\n");
#endif
			buf_cursor = 0;
			return true;
		}

		if (recv_buf[0] == '-')
		{
			printf("Client WRONG ACKN!!\n\n");
			buf_cursor = 0;
			this->Write(prev_command);
			return true;
		}

		char* end = strchr(recv_buf, '#');

		if (end != NULL)
		{
			if (strlen(end + 1) == 2)//implementer la verif du hash
			{
				/*for (int i = 0; i < end - recv_buf - 1; i++)
					hash += recv_buf[i + 1] % 256;
					*/
				//if ( std::to_string(hash)== string (end))
				{
					//traiter
					server.Write_Server((char*)"+", 1);
					rx_command = string(recv_buf + 1);
					rx_command.resize(rx_command.size() - 3);
					this->Read_gdb_cmd();
					memset(recv_buf, 0, sizeof(recv_buf));
					buf_cursor = 0;
					return true;
				}
			/*	else
				{
					server.Write_Server((char*)"-", 1);
					memset(recv_buf, 0, sizeof(recv_buf));
					buf_cursor = 0;
				}*/
			}
		}


	}

	if (status == ERROR_Received)
	{
		return false;
	}


	return true;
}









size_t unhexify(uint8_t* bin, const char* hex, size_t count)
{
	size_t i;
	char tmp;

	if (!bin || !hex)
		return 0;

	memset(bin, 0, count);

	for (i = 0; i < 2 * count; i++) {
		if (hex[i] >= 'a' && hex[i] <= 'f')
			tmp = hex[i] - 'a' + 10;
		else if (hex[i] >= 'A' && hex[i] <= 'F')
			tmp = hex[i] - 'A' + 10;
		else if (hex[i] >= '0' && hex[i] <= '9')
			tmp = hex[i] - '0';
		else
			return i / 2;

		bin[i / 2] |= tmp << (4 * ((i + 1) % 2));
	}

	return i / 2;
}

static const char hex_digits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f'
};

size_t hexify(char* hex, const uint8_t* bin, size_t count, size_t length)
{
	size_t i;
	uint8_t tmp;

	if (!length)
		return 0;

	for (i = 0; i < length - 1 && i < 2 * count; i++) {
		tmp = (bin[i / 2] >> (4 * ((i + 1) % 2))) & 0x0f;
		hex[i] = hex_digits[tmp];
	}

	hex[i] = 0;

	return i;
}

