#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

#include "../include/lib_app_tftp.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

/* General functions section */
void show_sintaxis_error(void) {

	printf("Invalide option. use: app -s | app -c filename IP_Server\n");

}

void format_filename(char * fullpath, char * filename){
    // char short_filename[30] = "";

    int i = 0, index = 0, size_filename = 0;

    // printf("\nFull path: %s", fullpath);

    size_filename = strlen(fullpath);

    for(i = 0; i < size_filename; i++) {
        if(fullpath[i] == '\\') {
            index = i + 1;
        }        
    }
        
    strncpy(filename,fullpath + index, size_filename-index);

}

int obtaing_mode_app(int argc, char *argv[]) {

	int mode_to_app = 0;
	
	printf("\nInit mode detection");
	if(argc == 1) {									/* preventing zero arguments */
		show_sintaxis_error();
		mode_to_app = ERROR_MODE;
	} else if(strcmp(argv[1], "-s") == 0) {
		printf("\nmode server invoked");
		if(argc == 2) { 							/* If is 2 the only possible mode is server */
			/* Setting server mode */
        	mode_to_app = SERVER;
    	} else {
    		show_sintaxis_error();
    		mode_to_app = ERROR_MODE;
    	}
    } else if(strcmp(argv[1], "-c") == 0) { 
    	printf("\nmode client invoked");
    	if(argc == 4) {								/* If is 4 the only possible mode is client */
    		/* Setting client mode */
        	mode_to_app = CLIENT;
    	} else {
    		show_sintaxis_error();
    		mode_to_app = ERROR_MODE;
    	}
    } else {
    	printf("\nno mode invoked");
        show_sintaxis_error();
        mode_to_app = ERROR_MODE;
    }

    return mode_to_app;
}

void format_HS_block(int OPCODE, char * filename, char * transfer_mode, RRQ_block * block_to_send) {
	
	int i = 0, j = 0;
	int size_block_to_send = 0;
	int size_transfer_mode = 0;
	// RRQ_block block_to_send;

	uint8_t buffer_to_send[BUFLEN] = {0x00};

	// Getting limits
	size_block_to_send = strlen(filename);
	size_transfer_mode = strlen(transfer_mode);

	// setting opcode in block
	block_to_send->data[0] = 0x00;
	block_to_send->data[1] = (uint8_t) OPCODE;			

	// setting data block into block to send
	for(i = 0; i < size_block_to_send; i++) {
		block_to_send->data[i+2] = filename[i];
	}
	i += 2;
	block_to_send->data[i] = 0x00;		// limit 1

	// setting transfer mode into block to send

	for(j = 0, i++; j < size_transfer_mode ; j++, i++) {
		block_to_send->data[i] = transfer_mode[j];
	}
	block_to_send->data[i] = 0x00;		// limit 2

	block_to_send->size = i + 1;	

}

void format_ACK_block(ACK_block * block, int num_block){
	
	block->opcode[0] = 0;
	block->opcode[1] = ACK;

	block->blocknum[0] = num_block >> 8;
	block->blocknum[1] = (uint8_t)num_block;

}

int extract_ACK_block(ACK_block * block) {
	int numblock = 0;

	numblock = (int)block->blocknum[0] << 8;
	numblock = numblock + block->blocknum[1];

	return numblock;
}

void extract_HS_block(uint8_t * block_to_extract, char * filename, char * mode) {
	int i = 0, j= 0;
	int first_zero = 0, end_of_block = 0, sizeofname = 0;
	int opcode = 0;



	for(i = 2; end_of_block == 0; i++){				//finding zeros
		if (first_zero == 0) {
			if(block_to_extract[i] == 0) {
				first_zero = i;
				// printf("\nFirst zero at: %d", first_zero);
			}			
		}	
		else {
			if(block_to_extract[i] == 0) {
				end_of_block = i;
				// printf("\nLast zero at: %d", end_of_block);
			}
		}
	}

	memcpy(filename,&block_to_extract[2],first_zero-2);
	filename[first_zero-2] = '\0';

	memcpy(mode,&block_to_extract[first_zero+1],end_of_block-first_zero-1);
	mode[end_of_block-first_zero] = '\0';

}

void obtaing_local_ip(char * IP) {
	
    WSADATA wsaData;
    int iResult;
    char hostname[256];
    struct addrinfo hints, *res = NULL;
    struct sockaddr_in *addr;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);        
    }

    // Get the hostname
    iResult = gethostname(hostname, sizeof(hostname));
    if (iResult != 0) {
        printf("gethostname failed: %d\n", WSAGetLastError());
        WSACleanup();        
    }

    // printf("Hostname: %s\n", hostname);

    // Get the address info
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(hostname, NULL, &hints, &res);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();        
    }

    addr = (struct sockaddr_in *) res->ai_addr;
    strcpy(IP,inet_ntoa(addr->sin_addr));

    // printf("IPv4 Address: %s\n", IP);

    freeaddrinfo(res);
    WSACleanup();
    
}

/* Protocol functions section */

int init_HS_server(char * filename_to_receive,char * IP_client) {
		
		int opcodeBlockreceived = 0;
		int sizeofUDPBlock = 0;
		uint8_t blockUDPReceived[BUFLEN];
		char transfer_mode[10] = "";
		// char IP_client[31];
		ACK_block response_ACK = {0};		

		opcodeBlockreceived =wait_Block_UDP(blockUDPReceived,&sizeofUDPBlock,IP_client,69);

		extract_HS_block((void *)&blockUDPReceived, filename_to_receive, transfer_mode);

		printf("\nOpcode: %d", opcodeBlockreceived);

		if(opcodeBlockreceived == RRQ) {
			printf("\nRequest received from %s ", IP_client);
			printf("\nsending ACK message to %s ", IP_client);
		}
		else {
			printf("\nHandshaking fail !!");
			return 1;
		}

		printf("\nFilename request: %s from: %s", filename_to_receive, IP_client);

		format_ACK_block(&response_ACK,0);

		send_block_UDP((void *)&response_ACK, 4, IP_client, 5069);

	return 0;
}

int init_HS_client(RRQ_block * block, char * IP_server, int PORT_server ) {
	int opcode_ACK = 0;
	int size_of_response = 0;
	int block_number_response = 0xFFFFFFFF;

	ACK_block response_server = {0};

	/* sending request to transmition to server */
	send_block_UDP(block->data,block->size,IP_server,PORT_server);

	/* waiting ACK message from server */
	printf("\nwaiting ACK message from %s", IP_server);

	opcode_ACK = wait_Block_UDP((void *)&response_server, &size_of_response, IP_server,PORT_server+5000);

	block_number_response = extract_ACK_block(&response_server);

	printf("\nNum block in ACK message %d", block_number_response);

	if(opcode_ACK == ACK) {
		printf("\nACK messsage from %s is received.", IP_server);
		if(block_number_response == 0) {
			printf(" is correct.");
		}
		else {
			printf("\n Error in handshaking !!!");
			return 1;
		}
	}
	return 0;
}

int send_block_UDP(void * blockUDP, int size_block_UDP, char * IP_server, int PORT) {
	WSADATA wsa;
	struct sockaddr_in si_other;
	int s,slen;

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;		
	}

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		return 1;
	}

	//setup address structure
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr(IP_server);
	slen = sizeof(si_other);

	if (sendto(s, blockUDP, size_block_UDP, 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return 1;
	}	

	closesocket(s);
	WSACleanup();
	return 0;
}

int wait_Block_UDP(uint8_t * blockUDP, int * sizeBlockUDP, char * IP_client, int PORT) {

    WSADATA wsaData;
    SOCKET sockfd;    
    struct sockaddr_in serverAddr, clientAddr;
    int addr_size, recvBytes;
	
	//Initialise winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
	// Set IP of server
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket with server address	
    bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
	// Set size of client address 
    addr_size = sizeof(clientAddr);  
    
	//Receive datablocks from client
	*sizeBlockUDP = recvfrom(sockfd, blockUDP, BUFLEN, 0, (struct sockaddr *) &clientAddr, &addr_size);
	
    strcpy(IP_client,inet_ntoa(clientAddr.sin_addr));

	// printf("\nRequest from: %s", IP_client);

	// closing socket
    closesocket(sockfd);

    // cleaning Winsock
    WSACleanup();

    return blockUDP[1];

}

int init_reception_in_server(char * filename, char * IP_client) {
	int i = 0;
	int total_of_blocks = 0;
	int current_block = 0;
	int expected_block = 1;
	int final_block = 0;
	int bytestowrite = SIZE_DATA;
	int opcode_obteined = 0;
	int sizereceived = 0;
	int size_total_file = 0;

	char fullpath[256] = "./download/";

	FILE *file_from_client;					// pointer no inited for file to receive

	ACK_block ACK_response = {0};			//ACK block
	DATA_block block_received = {0x00};		//DATA block

	
	//setting full download path

	strcat(fullpath,filename);

	// checking if file is accesible

	file_from_client = fopen(fullpath,"wb");

	if(file_from_client == NULL) {			// if error in access to file
		printf("\nError to open this file: %s, please check", filename);
	}
	else {
		printf("\nStart reception...\n");
	}

	/* init reception  */

	while(final_block == 0) {

		opcode_obteined = wait_Block_UDP(( void * )&block_received, &sizereceived, IP_client,6000);
		printf("\rOPCODE: %u is OK", block_received.opcode);
		printf("\t#Block: %6.u", block_received.blocknum);
		printf("\tSize of last block received: %8.d", sizereceived-4);

		if(expected_block == (int) block_received.blocknum) {  	 		// Check if expected received is corrrect
			printf("\tExpected block is: OK");

			if(sizereceived < BUFLEN) {
				final_block = 1;
				bytestowrite = SIZE_DATA - (BUFLEN-sizereceived);
			}

			size_total_file += bytestowrite;
			printf("\tByte to write in file: %d", size_total_file);

			fwrite(block_received.data,1,bytestowrite,file_from_client);
		
			format_ACK_block(&ACK_response, (int)block_received.blocknum);
			Sleep(001);
			send_block_UDP((void *)&ACK_response, 4, IP_client, 6001);
	
		}
		else {
			printf("\nError in expected block");
		}
		expected_block++;
	}

	printf("\nEnd of reception.");

	/*   Final of reception */

	
	fclose(file_from_client);
}

int init_transmision_in_client(char * filename, char * IP_server) {
	int i = 0;
	int NumBlockSending = 0;
	int residueofblock = 0;
	int current_block = 1;
	int opcode_obtained = 0;
	int num_block_obtained = 0;
	int size_response = 0;
	int sizetosend = BUFLEN;

	FILE *file_to_send;

	ACK_block response_received = {0x00};
	DATA_block block_to_send = {0x00};

	file_to_send = fopen(filename, "rb");

	if(file_to_send == NULL) {
		printf("\nError to open this file: %s, please check", filename);
		return 1;
	}

	//obtaining size of file

	fseek(file_to_send, 0L, SEEK_END); 		
	NumBlockSending = ftell(file_to_send);	
	rewind(file_to_send); 			

	// calculing numbers of blocks
	residueofblock = NumBlockSending % SIZE_DATA;
	NumBlockSending = ( NumBlockSending / SIZE_DATA ) + 1;	

	printf("\nNumber of blocks to send: %d and a residue of: %d\n\n", NumBlockSending - 1, residueofblock);

	// Sending file

	// sending all entire blocks
	while(current_block <= NumBlockSending) {
		if(current_block != NumBlockSending){			
			fread(&block_to_send.data,1,SIZE_DATA, file_to_send);			
		}
		else {
			fread(&block_to_send.data,1,residueofblock, file_to_send);
			sizetosend = residueofblock+4;
		}

		// setting headers of block
		block_to_send.opcode = (uint16_t)DATA;
		block_to_send.blocknum = (uint16_t)current_block;

	// printf("\nSending block #: %d with size %d", current_block, sizeof(block_to_send));

		send_block_UDP((void *)&block_to_send, sizetosend, IP_server, 6000);

	// printf("\nWaiting ACK of BLOCK: %d", current_block);
		// wait_Block_UDP((void *)&response_server, &size_of_response, IP_server,PORT_server+5000);
		opcode_obtained = wait_Block_UDP((void *)&response_received, &size_response, IP_server, 6001);

		num_block_obtained = extract_ACK_block(&response_received);

		if(opcode_obtained == ACK) {
	//printf("\nACK received for block: %d", num_block_obtained);
		}
		else {
			printf("\nError in response of server, please check.");
			return 1;
		}

		view_progress_bar(current_block,NumBlockSending, 50);
		current_block++;
	}

	fclose(file_to_send);

	return 0;
}


/*  Mode Function section */
int init_server_mode(void) {
	int error = 0;
	

	uint8_t buffer[BUFLEN];
	// data_block_RWQ HS_Block;

	char IP_server[30];
	char IP_client[30];
	char filename_to_receive[LENGTH_MAX_FILENAME];


	obtaing_local_ip(IP_server);

	printf("\nLocal IP: %s", IP_server);

	printf("\nWaiting for request of conection");

	init_HS_server(filename_to_receive,IP_client);

	printf("\nwaiting for transmition of file");

	init_reception_in_server(filename_to_receive, IP_client);

	return error;
}

int init_client_mode(char * filename, char * IP_server) {
	
	int size = 0;
	int i = 0;
	char transfer_mode[] = "octal";
	int HS_sucess = 1;
	RRQ_block buffer_to_send = {0x00};	
	char short_filename[30] = "";
	
	// formating all variables into RRQ block to send

	format_filename(filename,short_filename);

	format_HS_block(RRQ, short_filename, transfer_mode, &buffer_to_send);

	HS_sucess = init_HS_client(&buffer_to_send, IP_server,69);

	if (HS_sucess == 0) {
		printf("\nHandshaking sucesss !! \nSending file.");
	}
	else {
		return 1;
	}

	// Init transmition of file 

	init_transmision_in_client(filename, IP_server);

}

void view_progress_bar(int current,int totalelements, int size_of_bar){
    int i = 0, j = 0;
    int full = 0;
    int empty = size_of_bar;
	int percentage = 0;
	
	percentage = (int) (current*100/totalelements);

    full = (int) (( ( percentage * size_of_bar) / 100) + 0.5) ;
    empty = size_of_bar - full;
    


    printf("\rProgress: %3.d%% ", percentage);

    for(j = 1; j <= full; j++){
        printf("%c", 219);
    }
    for(j = 1; j <= empty; j++) {
        printf("%c", 177);
    }

    printf(" block:%d of %d ", current, totalelements);     

    if(percentage == 100) {
        printf("\n");
    }
    fflush(stdout); // Flushing buffer


}