
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>

void remp_socket ( char *service, char *nom, int typesock,
		  struct sockaddr_in **p_adr_serv);

int envoi(int socket, char *buffer, int taille);

int reception( int socket, char *buffer, int taille);