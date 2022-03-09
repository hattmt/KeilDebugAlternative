
#include "Keil_Comm.h"
#include "tcpipservr.h"
#include "gdb_server.h"
#include "string.h"

Gdb_Server gdb_server;

int main(int argc, char* argv[])
{
	int status = false;

	Keil_Comm keil;

	printf("Configurer les ports keil entre 5101 et 5110\n");

	if (argc != 3)
	{
		
		printf("Argument 1 = chemin projet keil\n");
		printf("Argument 2 = port serveur gbd\n");

	}
	//lancement et conection à Keil
	status = keil.Connect_Keil("Result.txt", "C:/Keil_v5/UV4/UV4.exe");
	
	if (status == false) {
		printf("Error: couldn't connect to keil\n");
		return -1;
	}
	//ouverture du projet
	status = keil.Set_View(2);
	status = keil.Open_Project(argv[1]);

	//afficher keil
	/*status = keil.Enter_Debug_Mode();

	keil.Get_Registers();

	string registers = keil.Registers_to_xml();*/

	 gdb_server;


	 while (1)
	 {
		 gdb_server.Start(argv[2], &keil);


		 while (gdb_server.Loop());
	 }





	//keil.Get_project_value(1);
	//creer le serveur gdb avec initialisation
	/*Gdb_Server gdb
	gdb.start();*/
	//uint8_t buff[500] = { 0 };
	//attendre la connexion
	//unhexify(buff, "72657365742068616c74", strlen("72657365742068616c74"));//reselt halt

	//unhexify(buff, "556e61626c6520746f206d61746368207265717565737465642073706565642031303030206b487a2c207573696e6720393530206b487a0a", strlen("556e61626c6520746f206d61746368207265717565737465642073706565642031303030206b487a2c207573696e6720393530206b487a0a"));

	//hexify( "O556e61626c6520746f206d61746368207265717565737465642073706565642031303030206b487a2c207573696e6720393530206b487a0a", buff, strlen("O556e61626c6520746f206d61746368207265717565737465642073706565642031303030206b487a2c207573696e6720393530206b487a0a"));



	//lancer le debugger dans keil


	/*status = create_server();



	//$qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;fork-events+;vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+#df
	add_offset("PacketSize=10000;qXfer:memory-map:read+;qXfer:features:read+;qXfer:threads:read+;QStartNoAckMode+;vContSupported+");//packet size
	//$vMustReplyEmpty#3a
	add_offset("");
	//$QStartNoAckMode#b0
	add_offset("OK");
	//$!#21
	add_offset("OK");//exteded remote debugging
	//$Hg0#df
	add_offset("OK");//All other future operations should apply to all threads
	//qXfer:features:read:target.xml:0,1000#0c
	add_offset(("l" + registers).c_str());
	//$qTStatus#49
	add_offset("");
	//$?#3f
	add_offset("S02");
	//$qXfer:threads:read::0,1000#92
	add_offset("l<?xml version=\"1.0\"?><threads></threads>");
	//$Hc-1#09
	add_offset("OK");
	//$qC#b4
	add_offset("QC0");
	//$qAttached#8f
	add_offset("1");
	//$qOffsets#4b
	add_offset("Text=0;Data=0;Bss=0");
	//$g#67
	add_offset("00000000c00000000000002066000000ec1f0020ffffffffffffffffffffffffffffffffffffffffffffffffffffffff00ed00e0e81f002067040008e203000800000001");
	            78000020780200207802002078020020140F0008180000200000000000000000FFFFFFFFFFFFFFFF140F0008140F0008FFFFFFFF78060020470100089C0E00080000000178060020FCFFFFFFFFFFFF00
				78000020780200207802002078020020140F0008180000200000000000000000FFFFFFFFFFFFFFFF140F0008140F0008FFFFFFFF78060020470100089C0E00080000000178060020FCFFFFFFFFFF00
	
	add_offset("l<?xml version=\"1.0\"?><threads></threads>");

	add_offset("l<memory - map><memory type=\"ram\" start=\"0x00000000\" length=\"0x08000000\"/><memory type=\"flash\" start=\"0x08000000\" length=\"0x10000\"><property name=\"blocksize\">0x400</property></memory><memory type=\"ram\" start=\"0x08010000\" length=\"0xf7ff0000\"/></memory-map>");
	//$qXfer:memory-map:read::0,1000#ab
	
	//$m8000f9c,2#95
	add_offset("fee7981c");

	//add_offset("O"+string(hexify()));*/



	/*char xml_register_file[500] = {0};

	char xml_register_header[200]= "l<?xml version=\"1.0\"?>< !DOCTYPE target SYSTEM \"gdb-target.dtd\" ><target version = \"1.0\"><architecture>arm< / architecture>< feature name = \"org.gnu.gdb.arm.m-profile\">";
	int header_size = strlen(xml_register_header);
	
	keil.Get_Registers();

	/*for (int i = 0; i < register_nb; i++)
	{ 
		snprintf(xml_register_file + offset, "<reg name=\"%s\" bitsize=\"%d\" regnum=\"%d\" save-restore=\"%s\" type=\"%s\" group=\"%s\"/>", reg_name[i], bit_size[i]);
	]
	*/


		//gdb / features / arm-m-profile.xml
		
	
	

}