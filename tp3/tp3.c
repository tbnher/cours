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
// Affiche les informations sur le fichier dont le chemin est passé en
// paramètre sur la sortie standard
//
// Retourne 0 en cas de succès, -1 en cas d'échec
int afficher_infos(const char *chemin);


// Parcours le répertoire dont le chemin est passé en paramètre
// et affiche les informations sur les fichiers qu'il contient
// sur la sortie standard
//
// retour 0 en cas de succè, -1 en cas d'échec
int afficher_repertoire(const char *chemin);


// Lance un traitement long dans un processus et retourne le pid du
// processus nouvellement créé ou -1 en cas d'erreur
int lancer_traitement(const char *chemin);

int afficher_infos(const char *chemin){

    struct stat buf; 
    if(stat(chemin, &buf)==-1){
        perror("problème chemin");
        return -1;
    }
    if(S_ISDIR(buf.st_mode)){
        printf("d ");
    } else if(S_ISLNK(buf.st_mode)){
        printf("l ");
    } else if(S_ISREG(buf.st_mode)){
        printf("- ");
    } else {
        printf("? ");
    }
    
    int statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    printf("%o ", statchmod);

    printf("%d %d %ld %s\n",buf.st_uid,buf.st_gid, buf.st_size, chemin);

    return 0;
}

int afficher_repertoire(const char *chemin){

    char cheminFinal[4096];
    DIR *directory = opendir(chemin);
    if(directory==NULL){
        perror("erreur répertoire");
        return -1;
    }
    struct dirent *file;
    while((file = readdir(directory))!=NULL){
    
        strcpy(cheminFinal,chemin);
        struct stat buf; 
        if(stat(chemin, &buf)==-1){
            perror("problème chemin");
            return -1;
        }
        if(!S_ISDIR(buf.st_mode)){
        snprintf(cheminFinal,4096,"%s/%s",chemin,"/");
        }
        snprintf(cheminFinal,4096,"%s/%s",chemin,file->d_name);
        afficher_infos(cheminFinal);
    }    
      printf("\n");

    return 0;
}






void traiter(void) {
    // initialise un générateur aléatoire à partir du pid du processus
    srand(getpid());
    printf("Processus de pid %d, je vais faire un traitement très long!\n", getpid());
    // Attend un temps aléatoire en 0 et 10s
    sleep(rand() % 10);
    printf("Processus de pid %d, traitement terminé\n", getpid());
}


int lancer_traitement(const char *chemin){

    int ppid = fork();
    if(ppid == 0){
        afficher_repertoire(chemin);
        //traiter();
        exit(0);
    } 
    return ppid;

}


int main(int argc, char **argv) {
    int status;
    pid_t wpid;
    for (int i = 1; i < argc; i++) {
      lancer_traitement(argv[i]);
    }
    while((wpid = wait(&status))>0);
    printf("Fils terminé\n");
    
    return 0;
}