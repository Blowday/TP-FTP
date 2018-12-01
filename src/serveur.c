/**
 *    TP de Réseau : Programmation d'une application client/serveur
 *    ---------------------------------------------------------------
 * 
 *		Application : FTP client 
 *    fichier : client.c				     
 *					      
 *		Auteurs :  Damien Wykland, Baptiste Bouvier et Ancelin Serre						      
 *		Date : 21 Novembre 2018 
 *									      
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "rxutils.h" /* nouvelle boite à outils */

#define SERVICE_DEFAUT "0"

void serveur_appli(char *service); /* programme serveur */

void startServeur(int sock_id); /* boucle du serveur*/

/*---------- programme serveur ----------*/

/* Procedure principale */
int main(int argc, char *argv[])
{
  /* numero de service par defaut */
	char *service = SERVICE_DEFAUT; 

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
		case 1:
			printf("defaut service = %s\n", service);
			break;
		case 2:
			service = argv[1];
			break;

		default:
			printf("Usage:serveur service (nom ou port) \n");
			exit(1);
	}

	/* service est le service (ou numero de port) auquel sera affecte ce serveur*/
	serveur_appli(service);
}

/**
 * Procedure correspondant au traitement du serveur l'application 
 * @param char* le service (numéro de port) utilisé
 * @return void
 */
void serveur_appli(char *service)
{

	/* Création de la socket d'écoute */
	struct sockaddr_in *p_adr_cli;
	remp_socket(service, NULL, SOCK_STREAM, &p_adr_cli);
	/* Socket d'écoute */
	int sock_id = socket(AF_INET, SOCK_STREAM, 0);
	/* Instancie l'adresse locale dans le descripteur de socket */
  bind(sock_id, (struct sockaddr *)p_adr_cli, sizeof(struct sockaddr_in) );
	/* On met la socket en écoute */
  listen(sock_id, 5);

  int pid;

  while(1) 
  {
    /* On accepte une connexion */
    int lg = sizeof( struct sockaddr );
	  int sock_cli = accept(sock_id, (struct sockaddr *)p_adr_cli, (socklen_t *)&lg );

    pid = fork();

    if(pid == 0) 
    {
      /* Boucle de jeu */
      startServeur(sock_cli);
      /* Fermeture du socket */
      close(sock_cli);
    }
  }

  /* Fermeture du socket d'écoute */
  close(sock_id);
}


/**
 * Boucle de jeu coté serveur
 * @param int le descripteur d'une socket utilisée par le serveur
 * pour l'échange de données avec le processus client
 * @return void
 */
void startServeur(int sock_cli) 
{
  /* Variables */
  char* commande = malloc(128);
  char* resultat = malloc(1024);
  char* buffer_temp = malloc(1024);

  memset(commande, 0, 128);
  memset(resultat, 0, 1024);

  /* Lecture commande */
  reception(sock_cli, commande, 128);

  if(strcmp(commande,"ls") == 0) {
    FILE* output = popen("ls", "r");

    if (output == NULL)
    {
        printf("impossible d'appliquer la commande\n");
        exit(1);
    }

    fread(resultat, 1024, 1, output);
    pclose(output);
    envoi(sock_cli, resultat, 1024);

  }else if(strcmp(commande,"put") == 0) {
    printf("%s\n", commande);
  }else if(strcmp(commande,"get") == 0) {
    printf("%s\n", commande);
  }else if(strcmp(commande,"quit") == 0) {
    printf("%s\n", commande);
  }else {
  }

  fflush(stdout);
  
  free(commande);
}


