#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100

// gcc -o prg -Wall -Wextra -Werror

// Copie un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination en copiant taille octets
// retourne le nombre d'octets copiés en cas de succès, -1 en cas d'erreur
int copierDesarchiver(int source, int destination, int taille){
    if (source == -1) {
        perror("problème source\n");
        return -1;
    }
    if (destination == -1) {
        perror("problème destination\n");
        return -1;
    }

    uint8_t *buffer[BUFFER_SIZE];
    uint32_t totalNumberWrite = 0;

    int bufferOffset=0;

    while (bufferOffset < taille){
        int neededBufferSize = BUFFER_SIZE;
        if (bufferOffset + BUFFER_SIZE > taille){
            neededBufferSize = taille - BUFFER_SIZE;
        }
        int resultRead = read(source, buffer, neededBufferSize);
        if (resultRead == -1) {
            perror("problème lecture\n");
            return -1;
        }
        int resultWrite = write(destination, buffer, neededBufferSize);
        if (resultWrite == -1) {
            perror("problème écriture\n");
            return -1;
        }

        bufferOffset += neededBufferSize;
        totalNumberWrite += resultWrite;
    }
    return totalNumberWrite;
}

// Extrait le prochain fichier contenu dans l'archive. Si un fichier
// portant le même nom que celui indiqué dans l'archive existe déjà il
// doit être écrasé.
//
// Retourne -1 en cas d'erreur et le nombre d'octets du fichier
// extrait en cas de succès

int extrait_fichier(int fd_archive){
    int totalwritten = 0;
    uint8_t fileNameSize = 0;
    char fileName[512];
    uint64_t fileSize = 0;

    if (read(fd_archive,&fileNameSize,sizeof(uint8_t))==-1){
        perror("problème taille du nom du fichier\n");
        return -1;
    }

    if (read(fd_archive,fileName,fileNameSize)==-1){
        perror("pbolème nom du fichier\n");
        return -1;
    }

    if (read(fd_archive,&fileSize,sizeof(uint64_t))){
        perror("problème taille du fichier\n");
        return -1;
    }
    int fileDestination = (open(fileName,O_WRONLY | O_CREAT | O_TRUNC, 0666));

    if (fileDestination == -1){
        perror("problème destination extract\n");
        return -1;
    }

    if (copierDesarchiver(fd_archive,fileDestination,fileSize)==-1){
        perror("problème de copie extrait\n");
        return -1;
    }

    if (fileDestination == -1){
        perror("problème close\n");
        return -1;
    }
    return total_written;
}

// Extrait l'archive dans le répertoire courant.
//
// Retourne -1 en cas d'erreur, le nombre de fichiers créés en cas de
// succès.
int extrait_archive(const char *archive){
    uint32_t nbFiles = 0;

    int archiveSource = open(archive,O_RDONLY,0666);
    if (archiveSource == -1){
        perror("problème archive source\n");
        return -1;
    }

    if (read(archiveSource, &nbFiles, sizeof(uint32_t))==-1){
        perror("problème nbr fichiers archives\n");
        return -1;
    }

    for (uint32_t fileNumber = 0;fileNumber < nbFiles;fileNumber+=1){
        if (extrait_fichier(archiveSource)==-1){
            perror("problème extraction fichier\n");
            return -1;
        }
    }

    close(archiveSource);

    return nbFiles;
}

// Retourne la taille d'un fichier désigné par le descripteur fd, à l'aide
// de la fonction lseek/
// Le curseur associé au fichier doit être replacé à sa place d'origine
// Retourn -1 en cas d'erreur, la taille du fichier en cas de succès
int taille_fichier(int fd){
    int size = lseek(fd, 0, SEEK_END);

    printf("%d\n",size);

    lseek(fd,0,SEEK_SET);
    return size;
}

// Copie entierement un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination
// retourne le nombre d'octets copiés
int copierArchiver(int source, int destination){
    if (source == -1){
        fprintf(stderr,"pas de source\n");
        return -1;
    }
    if (destination == -1){
        fprintf(stderr,"pas de destination");
        return -1;
    }
    int sourceFileSize = 0;
    if (sourceFileSize == -1){
        return -1;
    }

    uint8_t *buffer[BUFFER_SIZE];
    uint32_t totalNumberWrite = 0;

    int bufferOffset = 0;
    int neededBufferSize = BUFFER_SIZE;
    while (bufferOffset < sourceFileSize){
        if (sourceFileSize - bufferOffset < BUFFER_SIZE){
            neededBufferSize = sourceFileSize - bufferOffset;
        }
        int numberRead = read(source, buffer, neededBufferSize);
        if (numberRead == -1) {
            perror("problème lecture source\n");
            return -1;
        }

        int numberWrite = write(destination, buffer, numberRead);
        if (numberWrite == -1) {
            perror("problème écriture dest\n");
            return -1;
        }

    bufferOffset += BUFFER_SIZE;
    totalNumberWrite += numberWrite;
  }

  return totalNumberWrite;
}
// Ajoute le fichier désigné par le chemin fichier dans l'archive
// désigné par le descripteur fd_archive
//
// Retourne -1 en cas d'erreur et le nombre d'octets écrits dans
// l'archive en cas de succès
int archive_un_fichier(int fd_archive, const char *fichier){
    uint8_t fileNameSize = strlen(fichier);
    uint64_t fileSize = 0;

    int sourceFile = open(fichier, O_RDONLY, 0666);
    if (sourceFile == -1) {
        perror("problème ouverture archive\n");
        return -1;
    }

    fileSize = taille_fichier(sourceFile);

    if (write(fd_archive, &fileNameSize, sizeof(uint8_t)) == -1) {
        perror("problème taille du nom du fichier archive\n");
        return -1;
    }

    if (write(fd_archive, fichier, fileNameSize) == -1) {
        perror("problème écriture du nom du fichier archive\n");
        return -1;
    }

    if (write(fd_archive, &fileSize, sizeof(uint64_t)) == -1) {
        perror("problème taille du fichier archive\n");
        return -1;
    }

    if (copierArchiver(sourceFile, fd_archive) == -1) {
        return -1;
    }

    if (close(sourceFile) == -1) {
        perror("problème fermture du fichier archive\n");
        return -1;
    }

    return fileSize;
}

// Créé un fichier archive à partir d'une liste de fichier source.
// Ecrase le fichier destination s'il existe déjà
//
// Retourne -1 en cas d'erreur et la taille de l'archive finale en
// nombre d'octets en cas de succès
int creer_archive(const char *fichier_archive, char **liste_fichiers, uint32_t nb_fichiers){
    int destinationArchive = open(fichier_archive, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (destinationArchive == -1) {
        perror("problème ouverture fichier destination\n");
        return -1;
    }

    if (write(destinationArchive, &nb_fichiers, sizeof(uint32_t)) == -1) {
        perror("problème écriture taille total des fichiers\n");
        return -1;
    }

    int res = 0;
    for (uint32_t fileNumber = 0; fileNumber < nb_fichiers; fileNumber += 1) {
        int tailleEcrite = archive_un_fichier(destinationArchive, liste_fichiers[fileNumber]);
        if (tailleEcrite == -1) {
            fprintf(stderr, "problème achivage d'un des fichiers\n");
            return -1;
        };
        res += tailleEcrite;
    }

    if (close(destinationArchive) == -1) {
        perror("problème fermeture destinationArchive\n");
        return -1;
    }

    return res;
}

int main(void)
{
    int filesourcedesarch1 = open("../test/fichiers_binaires.arch",O_RDONLY,0666);
    int filesourcedesarch2 = open("../test/fichiers_textes.arch",O_RDONLY,0666);
    
    extrait_fichier(filesourcedesarch1);
    extrait_fichier(filesourcedesarch2);

}

// int main(int argc, char **argv) {
//     if (argc < 3) {
//         fprintf(stderr, "pas assez d'arguments");
//         return -1;
//     }
//     char *files[argc - 2];
//     for (int i = 0; i < argc - 2; i++) {
//         files[i] = argv[i + 2];
//     }
//     creer_archive(argv[1], files, 2);

//     return 0;
// }