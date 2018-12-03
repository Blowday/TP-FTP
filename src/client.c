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
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>

#include "rxutils.h" /* nouvelle boite à outils */

#define SERVICE_DEFAUT "16010"
#define SERVEUR_DEFAUT "200.0.0.1"

void client_appli(char *serveur, char *service);

void startClient(int sock_id);

char lireChar();

/*---------- programme client ----------*/

/* Procedure principale */
int main(int argc, char *argv[])
{

  char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
  char *service = SERVICE_DEFAUT; /* numero de service par defaut (num de port) */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc)
  {
  case 1: /* arguments par defaut */
    printf("serveur par defaut: %s\n", serveur);
    printf("service par defaut: %s\n", service);
    break;
  case 2: /* serveur renseigne  */
    serveur = argv[1];
    printf("service par defaut: %s\n", service);
    break;
  case 3: /* serveur, service renseignes */
    serveur = argv[1];
    service = argv[2];
    break;
  default:
    printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
    exit(1);
  }

  /* 
    serveur est le nom (ou l'adresse IP) auquel le client va acceder 
    service le numero de port sur le serveur correspondant au  
    service desire par le client 
  */
  client_appli(serveur, service);
}

/**
 * Procedure correspondant au traitement du client de l'application
 * @param char* l'adresse du serveur
 * @param char* le service (numéro de port) utilisé 
 * @return void
 */
void client_appli(char *serveur, char *service)
{

  /* On créé un socket distant */
  struct sockaddr_in *p_adr_serv;

  /* On rempli les champs de la sock distante */
  remp_socket(service, serveur, SOCK_STREAM, &p_adr_serv);

  /* On crée la socket client et on stock son id */
  int sock_id = socket(AF_INET, SOCK_STREAM, 0);

  /* On lance la connexion au serveur avec en param l'id du socket et les infos du serveur */
  connect(sock_id, (struct sockaddr *)p_adr_serv, sizeof(struct sockaddr));
	
  /* Lancement de la boucle de jeu côté client */
  startClient(sock_id);

  /* On close la socket */
  close(sock_id);
}

/**
 * côté client
 * @param int le descripteur d'une socket utilisée par le client
 * pour l'échange de données avec le processus serveur
 * @return void
 */
void startClient(int sock_id)
{
  /* Variables */
  char *commande = malloc(128);
  char *resultat = malloc(1024);
  char *filename = malloc(128);
  char *filesize = malloc(128);
  char *buffer = malloc(1024);
 
  memset(commande, 0, 128);
  memset(resultat, 0, 1024);
  memset(filename, 0, 128);
  memset(filesize, 0, 128);
  memset(buffer, 0, 1024);

  /* Demande et envoi de la difficulté (nombre de vie) */
  printf("\nBonjour, veuillez saisir votre commande : ");
  scanf("%s", commande);   //ici, seul le premier mot est lu, donc "ls" ou "put" ou "get" ou "quit"


  if(strcmp(commande,"ls") == 0) {

    //envoi de la commande
    if (envoi(sock_id, commande, 128) < 0)
    {
        printf("Echec de l'envoi de la commande \n"); exit(1);
    }
    reception(sock_id, resultat, 1024);
    printf("%s\n", resultat);
    fflush(stdout);

  }else if(strcmp(commande,"put") == 0) {
    scanf("%s", filename); // filename = nom du fichier à récupérer depuis le serveur
    printf("%s\n", filename);

    //Ouverture du fichier à envoyer
    FILE* f = fopen(filename, "r");
    if(f == NULL){ printf("Erreur ouverture du fichier.\n"); exit(1);}

    //envoi de la commande au serveur
    if (envoi(sock_id, commande, 128) < 0)
    {
        printf("Echec de l'envoi de la commande \n"); exit(1);
    }

    //envoi du nom du fichier au serveur
    if (envoi(sock_id, filename, 128) < 0)
    {
        printf("Echec de l'envoi du nom du fichier \n"); exit(1);
    }
    fseek(f, 0L, SEEK_END);
    long int fz = ftell(f);
    printf("pls marche %ld\n", fz);
    rewind(f);
    sprintf(filesize, "%li", fz);

    //envoi de la taille du fichier au serveur
    if (envoi(sock_id, filesize, 128) < 0)
    {
        printf("Echec de l'envoi de la taille du fichier\n"); exit(1);
    }

    long int envoyes = 0;

    while(envoyes < fz) {
      fread(buffer, 1024, 1, f);
      envoyes += envoi(sock_id, buffer, 1024);
    }

    printf("Fin de l'envoi\n");

    fclose(f);

  }else if(strcmp(commande,"get") == 0) {
    printf("%s\n", commande);
  }else if(strcmp(commande,"quit") == 0) {
    printf("%s\n", commande);
  }else {
    printf("Erreur, tapez ls ou put ou get ou quit.\n"); exit(1);
  }

  free(commande);
}