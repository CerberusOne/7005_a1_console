#include "../A1/client.h"
#include "../A1/server.h"
#include "../A1/socketwrappers.h"

#define PATH "./Client_files/"

using namespace std;

void RecvFile(int socket, char* filename);
void SendFile(int socket, char *filename);
bool isValidFile(char *cfilename);
bool isCommand(string strcommand, int &command);
char *ParseString(string str);


int main (int argc, char *argv[]) {
	//char serverIP[BUFLEN], serverPort[BUFLEN];	
	int command;
	Cmd cmd;
	string filename, strcommand, serverIP, serverPort;
	char *cfilename, path[BUFLEN];

	//get ip, port and transfer port 
	cout << "Enter server IP:" << endl;
	cin >> serverIP; 

	cout << "Port: 7005" << endl;

	//cout << "Enter listening port: \n" << endl;
	//cin >> listeningPort; 

	//Client *commandConnection = new Client("127.0.0.1", 7005);
	Client *commandConnection = new Client(ParseString(serverIP), 7005);
	Server *transferConnection = new Server(70005);

	do{
		fflush(stdin);
		//get user input for command
		do {
			cout << "Enter command: " << endl;
			cin >> strcommand; 
		} while(!isCommand(strcommand, command));

		//get user input for filename
		if(command != EXIT){
			//get filename
			do {
				cout << "Enter fileame: " << endl;
				cin >> filename; 
				cfilename = ParseString(filename);
				strcpy(path, PATH);
				strcat(path, cfilename);
			} while (command == SEND && !isValidFile(path));
		} else {
			cfilename = ParseString("exit");
		}

		//create a command
		cmd = CreateCmd(command, cfilename);
		//send the command
		commandConnection->SendCmd(cmd);
		
		//send or receive the file
		if(cmd.type == SEND) {
			printf("sending file\n");
			SendFile(transferConnection->GetSocket(), path);
		} else if(cmd.type == GET) {
			printf("getting file\n");
			RecvFile(transferConnection->GetSocket(), path);
		} else {
			printf("exiting\n");
		}
	} while (cmd.type != EXIT);

	return 0;
}

char *ParseString(string str) {
	char *cstr;

	cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());

	return cstr;
}

bool isCommand(string strcommand, int &command) {
	if(strcommand == "send") {
		command = SEND;
		return true;
	} else if(strcommand == "get") {
		command = GET;
		return true;
	} if(strcommand == "exit") {
		command = EXIT;
		exit(1);
		return true;
	} else {
		cout << strcommand << endl;
		return false;
	}
}

bool isValidFile(char *cfilename) {
	FILE *file;

	if((file = fopen(cfilename, "rb")) == NULL) {
		printf("file doesn't exist\n");
		return false;
	}

	fclose(file);
	return true;
}

void SendFile(int socket, char *filename) {
	FILE *file;
	//char path[BUFLEN];
	char buffer[BUFLEN];
	char end[BUFLEN] = "0";
	int bytesRead, bytesSent;	

	if((file = fopen(filename, "rb")) == NULL) {
		perror("file doesn't exist\n");
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