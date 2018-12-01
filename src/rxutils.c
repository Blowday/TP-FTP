
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rxutils.h"

void remp_socket(char *service, char *nom, int typesock,
                 struct sockaddr_in **p_adr_serv)
{
  struct addrinfo hints; /* info à passer a getaddrinfo */
  struct addrinfo *res;  /* permet de recuperer les adresses a l aide de getaddrinfo */
  int s;

  /* RENSEIGNEMENT ADRESSES SOCKET  ------------------------------*/

  /* Mise a zero de la structure d'adresse socket */
  memset(&hints, 0, sizeof(struct addrinfo));

  /* Definition du domaine ( famille ) */
  hints.ai_family = AF_INET;    /* AF_INET pour IP */
  hints.ai_socktype = typesock; /* SOCK_STREAM pour TCP */
  hints.ai_protocol = 0;

  /* ------ RENSEIGNE @IP -----------------------------------------*/
  if (nom == NULL) /* Cas d'un serveur */
    hints.ai_flags = AI_PASSIVE;
  else
    hints.ai_flags = 0;

  s = getaddrinfo(nom, service, &hints, &res);
  if (s != 0)
  {
    printf("getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }
  *p_adr_serv = (struct sockaddr_in *)res->ai_addr;
}

int envoi(int socket, char *buffer, int taille)
{
  /* Nombre restant et nombre écrit d'octets*/
  int nb_restant, nb_ecrits;

  /* On commence avec la taille du buffer à envoyer */
  nb_restant = taille;

  /* On boucle tant qu'il reste des octets à envoyer */
  while (nb_restant > 0)
  {
    /* On tente d'envoyer des octets, et on stock le nombre EFFECTIVEMENT envoyé */
    nb_ecrits = write(socket, buffer, nb_restant);

    /* Si la réponse est négative, une erreur est survenue */
    if (nb_ecrits < 0)
    {
      printf("\nErreur : ecriture socket %d impossible\n", socket);
      return (nb_ecrits);
    }

    /* On modifie les variables par rapport au nombre d'octets effectivement envoyés*/
    nb_restant = nb_restant - nb_ecrits;
    buffer = buffer + nb_ecrits;
  }
  /* On renvoi le nombre d'octes envoyés */
  return (taille - nb_restant);
}


int reception( int socket, char *buffer, int taille)
{
  /* nombre restant et lus d'octets */
	int nb_restant, nb_lus;

  /* Au départ, 'taille' octets restant à lire*/ 
	nb_restant=taille ;

  /* tant qu'il reste des octets à lire */
	while ( nb_restant > 0 )
	{
    /* on stock le nombre d'octets effectivement lus */
		nb_lus = read(socket, buffer, nb_restant);

    /* Si la réponse est négative, il y a une erreur */
		if ( nb_lus<0 )
		{
	  		printf( "\nErreur : lecture socket %d impossible\n", socket);
			return (nb_lus);
		}

		nb_restant = nb_restant - nb_lus;

		/* Fin du transfert si rien n'a été lu */
		if (nb_lus==0)
		{
			return (taille-nb_restant);
		}
		buffer = buffer + nb_lus;

	}
  /* On renvoi le nombre d'octets effectivement lus*/
	return (taille-nb_restant);
}