#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include "ligne_commande.c"
// Affiche un prompt à l'utilisateur
void affiche_prompt();

// Lit une ligne depuis l'entrée standard et la traite de façon à
// produire une liste de commandes à exécuter.
//
// Une liste de commande est un tableau de commande.  Une commande et
// une table de chaîne de caractères dans un formation compatible avec
// le tableau argv d'un main.
//
// l'entier pointé par flag sera mis à jour avec une des valeur suivantes:
// - -1 : une erreur s'est produite
// - 0 : la ligne a été analysée correctement et elle ne se termine pas par le caractère &
// - 1 : la ligne a été analysée correctement et elle se termine par le caractère &
//
// l'entier pointé par nb sera mlis à jour avec le nombre de
// commandes retournées, c'est à dire la taille du tableau de commandes
//
// En cas d'erreur, la fonction retourne NULL



// Execute les commandes données par le tableau commandes dont la taille est nb.
// Si arriere_plan vaut 1, les commandes doivent être lancées en arrière plan
void execute_ligne_commande(char ***commandes, int nb, int arriere_plan);


void affiche_prompt()
    {
   char *invite = NULL;
   char const *user = getenv("USERNAME");

   if (user == NULL)
   {
      user = getenv("USER");

      if (user == NULL)
      {
         user = "";
      }

   }
      
      char host[1024];
      host[1023] = '\0';
      gethostname(host,1023);

      if (host == NULL)
      {
         perror("prob hostname");
      }


      char aux[1024];
      aux[1023] = '\0';
      getcwd(aux,1023);

      if (aux == NULL)
      {
         perror("prob chemin");
      }
      
      

      invite = malloc(strlen(user) + strlen(host) + strlen(aux) + 6);

      if (invite != NULL)
      {
         strcat(invite, user);
         strcat(invite, "@");
         strcat(invite, host);
         strcat(invite, ":");
         strcat(invite, aux);
         strcat(invite, "$");
         strcat(invite, " ");
      }

         
   printf("%s",invite);   
   fflush(NULL);

}


void execute_ligne_commande(char ***commandes, int nb, int arriere_plan){
   for(int i = 0; i< nb; i++){
      int pid = fork();
      if(pid==0){

            if(execvp(commandes[i][0],commandes[i]) == -1){
               perror(commandes[i][0]);
               exit(1);
               return;
            }
         } else {
            if(arriere_plan==0){
               int status;
               waitpid(pid,&status,0);
            }
            
         }      
      }
   libere(commandes);
   return;
}


int main(void) {
   char ***prompt;
   do{
      
      int status = -1;
      int ppid;
      affiche_prompt();
      int flag;
      int nb;
      prompt = ligne_commande(&flag,&nb);
      if(nb!=0){

         
         if(strcmp(prompt[0][0],"exit")==0){
            return 0;
         }
         if(flag == -1){
            perror("probleme ligne commande");
            return -1;
         } else if (flag == 0){
            execute_ligne_commande(prompt,nb,0);

         } else{
            execute_ligne_commande(prompt,nb,1);

         } 
      } else {
         printf("\n");      
         while((ppid =waitpid(-1,&status,WNOHANG)) > 0);

      }
   } while(1);
    return 0;
}