
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MYPORT 9000
#define BACKLOG 20



//##### STRUCTURES #####
struct sockaddr{
	unsigned short sa_family;	// famille d'adresse.
	char sa_data[14];		// 14 bytes d'adresse protocole.
	}

struct in_addr{
	unsigned long s_addr;		//adresse IPv4.
	}

struct sockaddr_in{
	short int sin_family;		// famille d'adresse.
	unsigned short int sin_port;	// numéro du port.
	struct in_addr sin_addr;	// contient l'adresse IP.
	unsigned char sin_zero[8];	// taille du struct sockaddr.
}
