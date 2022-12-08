#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
// Copie un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination en copiant taille octets
// retourne le nombre d'octets copiés en cas de succès, -1 en cas d'erreur
int copier(int source, int destination, int taille){

    char tableau[10];

    
    for(int i = 0; i < taille ; i++){
        if(read(source,tableau,1)==-1){
            perror("source incorrecte");
            close(source);
            close(destination);
            return 1;
        }
    }
    int res = write(destination,tableau,taille);
    if(res==-1){
        perror("prob destination");
        close(source);
        close(destination);
        return 1;
    }

close(source);
    close(destination);

    return res;
}


// Extrait le prochain fichier contenu dans l'archive. Si un fichier
// portant le même nom que celui indiqué dans l'archive existe déjà il
// doit être écrasé.
//
// Retourne -1 en cas d'erreur et le nombre d'octets du fichier
// extrait en cas de succès

int extrait_fichier(int fd_archive){

    char tableau[10];

    if(read(fd_archive,tableau,1)==-1){
        perror("problème d'archivage");
        return -1;
    }

    int sizeOfName = tableau[0];

    if(read(fd_archive,tableau,sizeOfName)==-1){
        perror("problème création du tableau");
        return -1;
    }

    //char name = tableau[1];

    if(read(fd_archive,tableau,6)==-1){
        perror("problème de taille de fichier");
        return -1;
    }

    //const char* fileName = "/home/infoetu/nathan.fontaine.etu/Bureau/S3/tp2/test";
    char path = "/home/infoetu/nathan.fontaine.etu/Bureau/S3/tp2/test";
    int fd_dest = open(path,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    printf("%d\n",fd_dest);
    if(fd_dest==-1){
        perror("problème de création de fichier");
        return -1;
    }

    int sizeOfFile = tableau[2];

    int sizeOfRead = read(fd_archive,tableau,sizeOfFile);
    if(sizeOfRead==-1){
        perror("problème de lecture du contenu");
        return -1;
    }

    int res = write(fd_dest,tableau,sizeOfRead);
    if(res==-1){
        perror("problème d'écriture");
        return -1;
    }
    return res;

}

// Extrait l'archive dans le répertoire courant.
//
// Retourne -1 en cas d'erreur, le nombre de fichiers créés en cas de
// succès.
int extrait_archive(const char *archive){
    return 0;
}