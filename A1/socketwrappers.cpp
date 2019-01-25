#include "socketwrappers.h"

using namespace std;

int Socket(int family, int type, int protocol) {
	int n;
	
	if((n = socket(family, type, protocol)) < 0) {
		printf("socket error\n");
		return -1;
	}
	
	return n;
}

//useful method for correcting binding error: "Address Already in Use"
//NOTE: call before bind()
void SetReuse(int socket) {
	int yes;
	if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
	}	
}

//server
int Bind(int socket, struct sockaddr_in *addr) {
	int n;
	
	if((n = bind(socket, (struct sockaddr*)addr, sizeof(struct sockaddr_in))) < 0) {
		perror("binding error");
		close(socket);
		return -1;
	}

	return n;
}

//server
int Listen(int socket, int size) {
	int n;

	if((n = listen(socket, size)) < 0){
		printf("listening error\n");
		close(socket);	
		return -1;
	} 

	return 0;
}

//server
int Accept(int socket, struct sockaddr *addr, socklen_t *addrlen) {
	int n;

	printf("waiting for connection\n");
	if((n = accept(socket, addr, addrlen)) < 0) {
		printf("accept error\n");
		close(socket);
		return -1;
	}	

	printf("Accepted connection\n");
	return n;
}

/* Overload one function, rename it ConfigAddr
	-setup server: ConfigAddr(struct sockaddr_in *addrInfo, int port)
	-setup client: ConfigAddr(struct sockaddr_in *addrInfo, ip, port)
*/
void ConfigServerSocket(struct sockaddr_in *servaddr, int port) {
	bzero((char*)servaddr, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr->sin_port = htons (port);
}

void ConfigClientSocket(struct sockaddr_in *servaddr, const char* ip, int port) {
	hostent* hp;
	
	if((hp = gethostbyname("127.0.0.1")) == NULL) {
		printf("Unknown server address %s\n", ip);
	}

	bzero((char*)servaddr, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons (port);
	bcopy(hp->h_addr, (char*) &servaddr->sin_addr, hp->h_length);
}

bool Connect(int sockfd, struct sockaddr_in sockaddr) {
	if(connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
    	{
		printf("Client: Can't connec to server\n");
        	return false;
    	}

	printf("Client: Connected!\n");

	return true;
}

bool SendMsg(int sockfd, char* buffer) {
	if(send(sockfd, buffer, BUFLEN, 0) == -1) {
		perror("SendMsg Failed\n");
		return false;
	}

	return true;
}

int RecvMsg(int sockfd, char* buffer) {
	int bytesRecv;

	if((bytesRecv = recv(sockfd, buffer, BUFLEN, 0)) == -1) {
		perror("RecvMsg Failed");
		return -1;
	}

	return bytesRecv;
}

Cmd RecvCmd(int sockfd) {
	Cmd cmd;
	int bytesRecv;

	if((bytesRecv = recv(sockfd, &cmd, sizeof(Cmd), 0)) == -1) {
		perror("RecvCmd Failed");
	}

    printf("RecvCmd: %d %s\n", cmd.type, cmd.filename);

	return cmd;
}

Cmd CreateCmd(int type, char *filename) {
	Cmd cmd;

	cmd.type = type;
    strcpy(cmd.filename, filename);

	return cmd;	
}



