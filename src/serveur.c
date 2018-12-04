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
  bind(sock_id, (struct sockaddr *)p_adr_cli, sizeof(struct sockaddr_in));
  /* On met la socket en écoute */
  listen(sock_id, 5);

  int pid;

  while (1)
  {
    /* On accepte une connexion */
    int lg = sizeof(struct sockaddr);
    int sock_cli = accept(sock_id, (struct sockaddr *)p_adr_cli, (socklen_t *)&lg);

    pid = fork();

    if (pid == 0)
    {
      /* Boucle de jeu */
      printf("Nouvelle Connexion\n");
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
  char *commande = malloc(128);
  char *resultat = malloc(1024);
  char *filename = malloc(128);
  char *filesize = malloc(128);
  char *buffer = malloc(1024);

  int keepGoing = 1;

  while (keepGoing)
  {
    memset(commande, 0, 128);
    memset(resultat, 0, 1024);
    memset(filename, 0, 128);
    memset(filesize, 0, 128);

    /* Lecture commande */
    reception(sock_cli, commande, 128);

    if (strcmp(commande, "ls") == 0)
    {
      FILE *output = popen("ls", "r");

      if (output == NULL)
      {
        printf("impossible d'appliquer la commande\n");
        exit(1);
      }

      fread(resultat, 1024, 1, output);
      pclose(output);
      envoi(sock_cli, resultat, 1024);
    }
    else if (strcmp(commande, "put") == 0)
    {
      /* Lecture nom du fichier à recevoir */
      reception(sock_cli, filename, 128);
      /* Creation du fichier */
      FILE *f = fopen("test1.txt", "w+");

      /* Lecture de la taille du fichier à recevoir */
      reception(sock_cli, filesize, 128);
      long int fz = atoi(filesize);

      long int recus = 0;

      printf("Reception du fichier %s de taille %ld\n", filename, fz);

      /* Reception du fichier */
      while (recus < fz)
      {
        recus += reception(sock_cli, buffer, 1024);
        if (recus > fz)
        {
          fwrite(buffer, (fz - recus + 1024), 1, f);
        }
        else
        {
          fwrite(buffer, 1024, 1, f);
        }
      }

      printf("Fichier %s reçu.\n", filename);

      fclose(f);
    }
    else if (strcmp(commande, "get") == 0)
    {
      /* Lecture nom du fichier à envoyer */
      reception(sock_cli, filename, 128);
      printf("%s\n", filename);

      //Ouverture du fichier à envoyer
      FILE *f = fopen(filename, "r");
      if (f == NULL)
      {
        printf("Erreur ouverture du fichier.\n");
        envoi(sock_cli, "NO", 2);
      }
      else
      {
        envoi(sock_cli, "OK", 2);

        fseek(f, 0L, SEEK_END);
        long int fz = ftell(f);
        rewind(f);
        sprintf(filesize, "%li", fz);

        //envoi de la taille du fichier au serveur
        if (envoi(sock_cli, filesize, 128) < 0)
        {
          printf("Echec de l'envoi de la taille du fichier\n");
          exit(1);
        }

        printf("Envoi du fichier %s de taille %ld\n", filename, fz);

        long int envoyes = 0;

        /* Envoi du fichier */
        while (envoyes < fz)
        {
          fread(buffer, 1024, 1, f);
          envoyes += envoi(sock_cli, buffer, 1024);
        }

        printf("Fin de l'envoi\n");
        fclose(f);
      }
    }
    else if (strcmp(commande, "quit") == 0)
    {
      printf("Fin de la connexion.\n");
      keepGoing = 0;
    }
    else
    {
    }

    fflush(stdout);
  }

  free(commande);
  free(filename);
  free(filesize);
  free(resultat);
  free(buffer);
}
