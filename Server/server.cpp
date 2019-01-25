#include <fstream>
#include "../A1/client.h"
#include "../A1/server.h"
#include "../A1/socketwrappers.h"


#define PATH "./Server_files/"

void SendFile(int socket, char *filename);
void RecvFile(int socket, char* filename);


using namespace std;

/*------------------------------------------------------------------------------------------------
-- SOURCE FILE: server.cpp
--
-- PROGRAM:     COMP7005 - Assignment 1 FTP
--
-- FUNCTIONS:   
--
-- DATE:        Oct 2, 2017
--
-- DESIGNER:    Aing Ragunathan
--
-- PROGRAMMER:  Aing Ragunathan
--
-- NOTES:       Server class methods for creating and using a client for the FTP program
--------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Server
--
-- DATE:       Oct 2, 2017
--
-- DESIGNER:   Aing Ragunathan
--
-- PROGRAMMER: Aing Ragunathan
--
-- INTERFACE:  main()
--
-- PARAMETER:  none
--
-- RETURNS:    void
--
-- NOTES:      Constructor for a Server
----------------------------------------------------------------------------------------------- */
int main (int argc, char *argv[]) {
	Cmd cmd;			//command structure 
	char path[BUFLEN];	//path of files according to the root directory of the executable

	Server *commandConnection = new Server(7005);					//Server object for commands		
	//Client *transferConnection = new Client("127.0.0.1", 70005);	//Client object for transfers
	Client *transferConnection = new Client(commandConnection->GetTransferIP(), 70005);	//Client object for transfers
	
	do{ 
		cmd = commandConnection->WaitCommand();					//Wait for the client 
		printf("Type: %d\n",cmd.type);
		printf("Filename: %s\n",cmd.filename);

		strcpy(path, PATH);
		strcat(path, cmd.filename);

		if(cmd.type == SEND) {
			RecvFile(transferConnection->GetSocket(), path);
		} else if (cmd.type == GET) {
			SendFile(transferConnection->GetSocket(), path);
		}

	} while (cmd.type != EXIT);

	return 0;
}

void SendFile(int socket, char *filename) {
	FILE *file;
	char buffer[BUFLEN];
	char end[BUFLEN] = "0";
	int bytesRead, bytesSent;

	if((file = fopen(filename, "rb")) == NULL) {
		perror("file doesn't exist");
		bytesSent = send(socket, end, sizeof(end), 0);
		return;
	}

	//bzero(buffer, sizeof(buffer));
	memset(buffer, '\0', BUFLEN);

	while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) != 0) {
		//bytesRead = fread(buffer, 1, sizeof(buffer), file);
		bytesSent = send(socket, buffer, sizeof(buffer), 0);
		printf("Bytes read = %d\n", bytesRead);
		printf("Bytes sent = %d\n", bytesSent);
		printf("%s\n", buffer);	
		
		//bzero(buffer, sizeof(buffer));
		memset(buffer, '\0', BUFLEN);
	}
	send(socket, end, sizeof(end), 0);
	fclose(file);
}

void RecvFile(int socket, char* filename) {
	FILE *file;
	char buffer[BUFLEN];
	int bytesRecv;

	if((file = fopen(filename, "ab")) == NULL) {
		printf("file failed to open: %s\n", filename);
		return;
	}	

	truncate(filename, 0);

	//working with jpeg
    while(1) {
        bzero(buffer, sizeof(buffer));
        //memset(buffer, '\0', BUFLEN);

        // read file data from socket
        if((bytesRecv = recv(socket, buffer, sizeof(buffer), 0)) < 0) {
        	return;
        }

        if(strcmp(buffer, "0") == 0) {
        	return;
        }

        if(fwrite(buffer, 1, bytesRecv, file) < 0)
        	return;
    }
}
