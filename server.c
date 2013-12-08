
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define USER_FILE "keychain_users.txt"
#define SERVER_PORT 9000
#define BACKLOG 20
#define CHAR_SIZE 30

//##### PROTOTYPE #####

void manageService(int socketUser, char *ident);
void writeAfter(char* fileName, char *ident, char *pwd, char* service);
void initMenu(int *socketUser);
int getUserInput(int socketUser, char* msg, char* userInput);

//##### FONCTION GERANT CLIENT #####

void initMenu(int *socketUser){
	int action;
	int isActionValid;
	char tmpBuffer[50];
	char ident[CHAR_SIZE];
	char pwd[CHAR_SIZE];
	char firstMsg[] = "What do you want to do ? \n(O) Exit. \n(1) Create new user. \n(2) Connect to your session. \nYour answer (number):";

	printf("thread started.\n");
	do{
		isActionValid = 0;
		do{
			// attend une réponse correcte.
			if(getUserInput(*socketUser, firstMsg, tmpBuffer) == 0){
				printf("thread closed by remote side.\n");
				action = 0;
				isActionValid = 1;
			} else if(sscanf(tmpBuffer, "%d", &action) == 1){
					if(action>=0 && action<=2){
						// vérifie si la reponse correspond à une action.
						isActionValid = 1;
					}
				}

		}while(isActionValid == 0);
		printf("action = %d\n",action);

		switch(action){
			case 0:
				// exit.
				close(*socketUser);
				break;
			case 1:
				// create new user.
				sprintf(tmpBuffer, "Your new ID (max %d char without space) : ", CHAR_SIZE);
				getUserInput(*socketUser, tmpBuffer, ident);
				getUserInput(*socketUser, "Your new password : ", pwd);
				writeAfter(USER_FILE, ident, pwd, NULL);
				manageService(*socketUser, ident);
				break;
			case 2:
				// connect to session.
				sprintf(tmpBuffer, "Your ID (max %d char without space) : ", CHAR_SIZE);
				getUserInput(*socketUser, tmpBuffer, ident);
				getUserInput(*socketUser, "Your password : ", pwd);
				if(checkUser(ident, pwd) == 1){
					manageService(*socketUser, ident);
				}
				break;
		}
	}while(action != 0);
	printf("thread ended.\n");
}


int getUserInput(int socketUser, char* msg, char* userInput){
	int msgLength;

	send(socketUser, msg, strlen(msg)+1, 0);
	msgLength = recv(socketUser, userInput, CHAR_SIZE, 0);
	// FIXME purge
	printf("%s, %s:%s(%d)\n",__FILE__,__FUNCTION__, userInput, msgLength);
	return msgLength;
}


int checkUser(char *ident, char *pwd){
	FILE *hFile;
	char currentIdent[CHAR_SIZE*2+1];
	char *currentPwd;
	int userFound = 0;
	int isOK = 0;
	hFile = fopen(USER_FILE,"r");
	while(feof(hFile) == 0 && userFound == 0){
		fgets(currentIdent, CHAR_SIZE, hFile);
		if(strlen(currentIdent) != 0){
			currentPwd = strchr(currentIdent, ':');
			*(currentIdent+strlen(currentIdent)-1)='\0';
			*(currentPwd++)='\0';
			printf("user=%s, passwd=%s\n", currentIdent, currentPwd);
			if(strcmp(ident, currentIdent) == 0){
				userFound = 1;
				if(strcmp(pwd, currentPwd) == 0){
					isOK = 1;	
				}
			}
			printf("userFound=%d, isOK=%d\n", userFound, isOK);
		}
	}
	return isOK;
}

void manageService(int socketUser, char *ident){
	int isSocketDown;
	int isActionValid;
	char service[CHAR_SIZE];
	char serviceIdent[CHAR_SIZE];
	char servicePasswd[CHAR_SIZE];
	int action = -1;
	char tmpBuffer[50];
	char secondMsg[] = "What do you want to do ? \n(0) exit. \n(1) add a service. \n(2) modifiy a service. \n(3) delete a service. \nYour answer :";

	do{
		isActionValid = 0;
		do{
			// attend une réponse correcte.
			if(getUserInput(socketUser, secondMsg, tmpBuffer) == 0){
				printf("thread closed by remote side.\n");
				action = 0;
				isActionValid = 1;
			} else if(sscanf(tmpBuffer, "%d", &action) == 1){
					if(action>=0 && action<=4){
						// vérifie si la reponse correspond à une action.
						isActionValid = 1;
					}
				}

		}while(isActionValid == 0);

		if(isSocketDown != 0 && action != 0){
			switch(action){
				case 1:
					// ajouter un service.
					sprintf(tmpBuffer, "Your new service name (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, service);
					sprintf(tmpBuffer, "Your new service ID (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, serviceIdent);
					sprintf(tmpBuffer, "Your new service password (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, servicePasswd);
					sprintf(tmpBuffer, "%s_shadow.txt", ident);
					writeAfter(tmpBuffer, serviceIdent, servicePasswd, service);
					break;
					
				case 2:
					// modifier un service.
					sprintf(tmpBuffer, "Your service name (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, service);
					sprintf(tmpBuffer, "Your new service ID (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, serviceIdent);
					sprintf(tmpBuffer, "Your new service password (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, servicePasswd);
					break;
					
				case 3:
					// supprimer un service.
					sprintf(tmpBuffer, "Your service name to delete (max %d char without space) : ", CHAR_SIZE);
					getUserInput(socketUser, tmpBuffer, service);
					break;
			}
		}
	}while(action != 0);
	printf("Deconnexion...\n");
}

void writeAfter(char* fileName, char *ident, char *pwd, char* service){
}




//##### FONCTION PRINCIPALE #####

int main(){
	short int exit = 0;
	int yes = 1;
	unsigned int sin_size = sizeof(struct sockaddr_in);

	printf("Starting server...\n");
	// initialisation de sockaddr_in.
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERVER_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(my_addr.sin_zero), '\0', 8);

	// création du socket " d'invite ".
	int initSocketFd = socket(PF_INET, SOCK_STREAM, 0);
	if(initSocketFd == -1){
		perror("ERROR : impossible to create the socket.");
		return EXIT_FAILURE;
	}

	// permet connection de plusieurs socket au meme port.
	if(setsockopt(initSocketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
		perror("ERROR : impossible to set socket option.");
		return EXIT_FAILURE;
	}

	// connection du socket au port, si possible.
	if(bind(initSocketFd,(struct sockaddr *) &my_addr, sin_size) == -1){
		perror("ERROR : impossible to bind the socket.");
		return EXIT_FAILURE;
	}

	while(!exit){
		int socketUser;
		pthread_t newUser;
		pthread_attr_t attr;

		// mise en écoute.
		if(listen(initSocketFd, BACKLOG) == -1){
			perror("ERROR : impossible to listen.");
		}else{
			socketUser = accept(initSocketFd,(struct sockaddr *) &my_addr, &sin_size);
			if(socketUser == -1){
				// vérification connection.
				perror("ERROR : accepting connection.");
			}else if(pthread_attr_init(&attr) != 0){
				// initialisation de l'attribut de thread.
				perror("ERROR : thread attribut.");
			}else if(pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED) != 0){
				// set l'attribut de thread en mode détachable (évite fuites mémoire en fin de thread).
				perror("ERROR : setting thread detachable.");
			}else if(pthread_create(&newUser, NULL, (void *) initMenu, &socketUser) != 0){
				// création et execution de la thread paramètrée.
				perror("ERROR : creating thread.");
			}else{
				printf("connexion accepted...\n");
			}
		}
	}

	return 0;
};
