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
    uint8_t fileNameSize = 0;
    char fileName[512];
    uint64_t fileSize = 0;

    if (read(fd_archive, &fileNameSize, sizeof(uint8_t)) == -1) {
        perror("problème de taille de nom du fichier\n");
        return -1;
    };

    if (read(fd_archive, fileName, fileNameSize) == -1) {
        perror("problème de nom de fichier\n");
        return -1;
    };
    fileName[fileNameSize]='\0';

    if (read(fd_archive, &fileSize, sizeof(uint64_t)) == -1) {
        perror("problème de taille du fichier\n");
        return -1;
    };

    printf("%s\n",fileName);

    int fileDestination = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fileDestination == -1) {
        perror("problème de destination extrait\n");
        return -1;
    }


    if (copier(fd_archive, fileDestination, fileSize) == -1) {
        perror("problème de copie extrait\n");
        return -1;
    }

    if (close(fileDestination) == -1) {
        perror("problème close");
        return -1;
    }

  return fileSize;
}



// Extrait l'archive dans le répertoire courant.
//
// Retourne -1 en cas d'erreur, le nombre de fichiers créés en cas de
// succès.
int extrait_archive(const char *archive){
    uint32_t nbFiles = 0;

    int archiveSource = open(archive, O_RDONLY, 0666);
    if (archiveSource == -1){
        perror("pas de fichier source");
        return -1;
    }

    if (read(archiveSource, &nbFiles, sizeof(uint32_t)) == -1){
        perror("pas de fichier archive");
        return -1;
    }

    for (uint32_t fileNumber = 0; fileNumber < nbFiles; fileNumber += 1) {
        if (extrait_fichier(archiveSource)==-1){
            perror("problème d'extraction du fichier");
            return -1;
        };
    }

    close(archiveSource);
    
    return nbFiles;
}
