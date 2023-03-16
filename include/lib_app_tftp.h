#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdint.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SIZE_DATA 1420
#define BUFLEN SIZE_DATA + 4
#define LENGTH_MAX_FILENAME 256

enum app_mode {
	SERVER = 1,
	CLIENT = 2,
	ERROR_MODE = 0
};

enum opcode {
    RRQ = 0x01,
    WRQ = 0x02,
    DATA = 0x03,
    ACK = 0x04,
    OP_ERROR = 0x05
};

typedef struct {
    uint8_t opcode[2];    // 2 bytes for opcode
    uint8_t blocknum[2];  // 2 bytes for number of block    
} ACK_block;

typedef struct {
    uint16_t opcode;    // 2 bytes for opcode
    uint16_t blocknum;  // 2 bytes for number of block    
    uint8_t data[SIZE_DATA]; // 512 byte for data RFC 1350
} DATA_block;

typedef struct {
    int size;
    uint8_t data[SIZE_DATA];
} RRQ_block;



 /*******************************************************************************************
 * Inputs:  																				*
 * 	argc : Is the number of elements in array argv. 										* 		
 * 	argv: Is a array with a all elements used to call the app in console.					*
 * 																							*
 * OutPut:																					*		
 * 	return the mode of operation for this ejecution:										*
 * 		- Server:																			*
 * 			Works on a listen mode to get a file from client.								*
 * 																							*
 * 		- Client:																			*
 *			Works sending a request to conection for transmit a file to server. 			* 
 * 																							*
 * Description:																				*
 * 	This function should only be callled once per app ejecution. This function set the 		*
 * 	mode of operation of the app.															*
 *******************************************************************************************/
int obtaing_mode_app(int argc, char *argv[]);

 /*******************************************************************************************
 * Inputs: none                                                                             *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 * Description:                                                                             *
 *  This function show a sintasis error at app calling                                      *
 *******************************************************************************************/
void show_sintaxis_error(void);

 /*******************************************************************************************
 * Inputs:                                                                                  *
 *      fullpath: Is the full path given to app                                             *
 * OutPut: none                                                                             *       
 *      filename: Is only name of file to send with folders of path                         *
 * Description:                                                                             *
 *  This function format the filename to sent to server                                     *
 *******************************************************************************************/
void format_filename(char * fullpath, char * filename);

 /*******************************************************************************************
 * Inputs: none                                                                             *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function set in        *
 *  listening mode of operation. waiting the client's conection request                     *
 *******************************************************************************************/
int init_server_mode(void);

 /*******************************************************************************************
 * Inputs: Pointer to string variable to save current IP of system                          *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function obtain the    * 
 * current local IP to show in console                                                      *
 *******************************************************************************************/
void obtaing_local_ip(char * IP);

 /*******************************************************************************************
 * Inputs: Pointer to string variable to save the filename to receive                       *
 *                                                                                          *
 * OutPut: none                                                                             *
 *                                                                                          *
 * Return:                                                                                  *       
 *      0 -> If not error in runtime                                                        *
 *      1 -> If a error found in ejecution                                                  *
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function obtaing       *
 *  the filenamen from the (HS) handshaking block to filename to receive                    *
 *******************************************************************************************/
int init_HS_server(char * filename_to_receive, char * IP_client);

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      blockUDP: Is the variable to save a block from client                               *
 *      *sizeBlockUDP: pointer to save the size of block received                           *
 *                                                                                          *
 * OutPut:                                                                                  *
 *      opcode: Integer represent the kind of message received                              *
 *                                                                                          *
 * Return:                                                                                  *       
 *      opcode                                                                              *
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function obtaing       *
 *  the block, size of it from client also the OPCODE from client                           *
 *******************************************************************************************/
int wait_Block_UDP(uint8_t * blockUDP, int * sizeBlockUDP, char * IP_client, int PORT);

 /*******************************************************************************************
 * Inputs:                                                                                  *
 *      filename: Is the filename to send to server                                         *
 *      IP_server: Address of server                                                        *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function set in        *
 *  listening mode of operation. sending the conection requests to server                   *
 *******************************************************************************************/
int init_client_mode(char * filename, char * IP_server); 

 /*******************************************************************************************
 * Inputs:                                                                                  *
 *      OPCODE: Type of message                                                             *
 *      filename: Is the filename to send to server                                         *
 *      transfer_mode: Mode of transmition typically "octal"                                *
 *      IP_server: Address of server                                                        *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *      block_to_send: variable where we save the formated block                            *
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function set in        *
 *  a RRQ block all variables to send to send requests to server                            *
 *******************************************************************************************/
void format_HS_block(int OPCODE, char * filename, char * transfer_mode, RRQ_block * block_to_send);


 /*******************************************************************************************
 * Inputs:                                                                                  *
 *      num_block: number of block to be setting in block to send                           *
 *                                                                                          *
 * OutPut:                                                                                  *       
 *      block: ACK block with block number ready to send                                    *
 *                                                                                          *
 * Description:                                                                             *
 *  This function put number of block into ACK block to send.                               *
 *******************************************************************************************/
void format_ACK_block(ACK_block * block, int num_block);

 /*******************************************************************************************
 * Inputs:                                                                                  *
 *      block: ACK block received from transmition                                          *
 *                                                                                          *
 * OutPut: returns                                                                          *       
 *      num_block: number of block contained in block received                              *
 *                                                                                          *
 * Description:                                                                             *
 *  This function extract a number of block from a ACK block received.                      *
 *******************************************************************************************/
int extract_ACK_block(ACK_block * block);

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      block: Package of data to send.                                                     *        
 *      IP_server: Net Address of server                                                    *
 *      PORT_server                                                                         *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 *   return:                                                                                *
 *      0 = > Its (HS)handshaking done and ok to and from server                            *
 *      1 = > There is a error in handshaking                                               *
 *                                                                                          *
 * Description:                                                                             *
 *  This function should only be called once per app ejecution. This function send a RRQ    *
 *  request to server, and wait from server a ACK answer.                                   *
 *******************************************************************************************/
int init_HS_client(RRQ_block * block, char * IP_server, int PORT_server );

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      blockUDP: Packages of byte to send by UDP                                           *
 *      size_block_UDP: Package of data to send.                                            *        
 *      IP_server: Net Address of server                                                    *
 *      PORT_server: Port to be use to send request                                         *
 *                                                                                          *
 * OutPut: none                                                                             *       
 *                                                                                          *
 *   return:                                                                                *
 *      0 = > Everything is OK                                                              *
 *      1 = > There is a error in transmition                                               *
 *                                                                                          *
 * Description:                                                                             *
 *  This function send a package of byte to the address and port specific                   *
 *******************************************************************************************/
int send_block_UDP(void * blockUDP, int size_block_UDP, char * IP_server, int PORT);

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      block_to_extract: Packages of byte to received by UDP                               *
 *                                                                                          *
 * OutPut:                                                                                  *
 *      filename: name of file to receive.                                                  *
 *      mode: TFTP transfer mode                                                            *
 *   return:                                                                                *
 *      none                                                                                *
 *                                                                                          *
 * Description:                                                                             *
 *  This function extract filename and mode of transfer from block of bytes given.          *
 *******************************************************************************************/
void extract_HS_block(uint8_t * block_to_extract, char * filename, char * mode);


/*******************************************************************************************
 * Inputs:                                                                                  *
 *      filename: Name of file to receive                                                   *
 *      IP_client: Net address of client                                                    *
 * OutPut:                                                                                  *
 *      none                                                                                *
 *   return:                                                                                *
 *      0: Error free                                                                       *
 *      1: Error in process                                                                 *
 * Description:                                                                             *
 *  This function manage the reception of file in server                                    *
 *******************************************************************************************/
int init_reception_in_server(char * filename, char * IP_client);

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      filename: Name of file to transmit                                                  *
 *      IP_server: Net address of server                                                    *
 * OutPut:                                                                                  *
 *      none                                                                                *
 *   return:                                                                                *
 *      0: Error free                                                                       *
 *      1: Error in process                                                                 *
 * Description:                                                                             *
 *  This function manage the transmition of file in client                                  *
 *******************************************************************************************/
int init_transmision_in_client(char * filename, char * IP_server);

/*******************************************************************************************
 * Inputs:                                                                                  *
 *      current: number of the current element                                              *
 *      totalelements: total of elements                                                    *
 *      size_of_bar: size of bar in amount of characters to display                         *
 * OutPut:                                                                                  *
 *      none                                                                                *
 *   return:                                                                                *
 *      node                                                                                *
 * Description:                                                                             *
 *  This function show a progress bar of process running                                    *
 *******************************************************************************************/
void view_progress_bar(int current,int totalelements, int size_of_bar);