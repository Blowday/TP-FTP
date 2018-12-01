
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>

/* Remplit la structure avec le port et l'adresse IP */
void remp_socket ( char *service, char *nom, int typesock,
		  struct sockaddr_in **p_adr_serv);

/* Fonction d'envoi du buffer de taille 'taille' à la socket spécifiée */
int envoi(int socket, char *buffer, int taille);

/* Fonction de réception du buffer de taille 'taille' à partir de la socket spécifiée */
int reception( int socket, char *buffer, int taille);