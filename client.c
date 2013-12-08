#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SERVER_PORT 9000
#define CHAR_SIZE 30


//##### FONCTIONS #####

int usersAction(int my_socket){
	int  msgLength;
	char msg[150];
	char response[CHAR_SIZE];

	do{
		msgLength = recv(my_socket, &msg, sizeof(msg), 0);
		if(msgLength != 0){
			printf(msg);
			fflush(stdout);
			scanf("%30s", response);
			// FIXME purge.
			send(my_socket, response, strlen(response)+1, 0);
		}
	}while(msgLength != 0);
	close(my_socket);
	// fermer socket
}


//##### FONCTION PRINCIPALE #####

int main(int argc, char** argv){

	// initialisation de sockaddr_in.
	struct sockaddr_in their_addr;
	struct hostent *server;

	if( argc != 1)
		server = gethostbyname(argv[1]);
	else
		server = gethostbyname("localhost");

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(SERVER_PORT);
	their_addr.sin_addr = *((struct in_addr*) server->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);

	// création du socket client.
	int my_socket = socket(PF_INET, SOCK_STREAM, 0);
	printf("Socket created\n");
	if(my_socket == -1){
		perror("ERROR : creating socket.");
	}else if(connect(my_socket, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) == -1){
		// connection si possible.
		perror("ERROR : connecting.");
		return EXIT_FAILURE;
	}else{
		printf("connected...\n");
		usersAction(my_socket);
	}
	return 0;
};
