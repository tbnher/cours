#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100



int copier(int source, int destination, int taille) {
  if (source == -1) {
    perror("prob source\n");
    return -1;
  }
  if (destination == -1) {
    perror("prob dest\n");
    return -1;
  }

  uint8_t *buffer[BUFFER_SIZE];
  uint32_t totalNumberWrite = 0;

  int bufferOffset = 0;
  while (bufferOffset < taille) {
    int neededBufferSize = BUFFER_SIZE;
    if (bufferOffset + BUFFER_SIZE > taille) {
      neededBufferSize = taille - bufferOffset;
    }

    int numberRead = read(source, buffer, neededBufferSize);
    if (numberRead == -1) {
      perror("prob copie\n");
      return -1;
    }

    int numberWrite = write(destination, buffer, neededBufferSize);
    if (numberWrite == -1) {
      perror("prob destination\n");
      return -1;
    }

    bufferOffset += neededBufferSize;
    totalNumberWrite += numberWrite;
  }

  return totalNumberWrite;
}







int extrait_fichier(int fd_archive) {
  uint8_t fileNameSize = 0;
  char fileName[512];
  uint64_t fileSize = 0;

  if (read(fd_archive, &fileNameSize, sizeof(uint8_t)) == -1) {
    perror("prob taille nom fichier\n");
    return -1;
  };

  if (read(fd_archive, fileName, fileNameSize) == -1) {
    perror("prob nom fichier\n");
    return -1;
  };
  fileName[fileNameSize] = '\0';

  if (read(fd_archive, &fileSize, sizeof(uint64_t)) == -1) {
    perror("prob taille fichier\n");
    return -1;
  };
    printf("%s\n",fileName);
  int fileDestination = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fileDestination == -1) {
    perror("prob dest extrait\n");
    return -1;
  }

  if (copier(fd_archive, fileDestination, fileSize) == -1) {
    perror("prob copie extrait\n");
    return -1;
  }

  if (close(fileDestination) == -1) {
    perror("prob close");
    return -1;
  }

  return fileSize;
}

int extrait_archive(const char *archive) {
  uint32_t nbFiles = 0;

  int archiveSource = open(archive, O_RDONLY, 0666);
  if (archiveSource == -1) {
    perror("prob archive source\n");
    return -1;
  }

  if (read(archiveSource, &nbFiles, sizeof(uint32_t)) == -1) {
    perror("prob nb fichiers archives\n");
    return -1;
  };

  for (uint32_t fileNumber = 0; fileNumber < nbFiles; fileNumber += 1) {
    if (extrait_fichier(archiveSource) == -1) {
      perror("prob extrait fichier\n");
      return -1;
    };
  }

  close(archiveSource);

  return nbFiles;
}



int taille_fichier(int fd){
    int size = lseek(fd, 0, SEEK_END); 

    printf("%d\n", size);

    lseek(fd,0,SEEK_SET);
    return size;
}


int copierArchive(int source, int destination) {
  if (source == -1) {
    fprintf(stderr, "copy: source doesn't exist\n");
    return -1;
  }
  if (destination == -1) {
    fprintf(stderr, "copy: destination doesn't exist\n");
    return -1;
  }

  int sourceFileSize = taille_fichier(source);
  if (sourceFileSize == -1) {
    return -1;
  }

  uint8_t *buffer[BUFFER_SIZE];
  uint32_t totalNumberWrite = 0;

  int bufferOffset = 0;
  int neededBufferSize = BUFFER_SIZE;
  while (bufferOffset < sourceFileSize) {
    if (sourceFileSize - bufferOffset < BUFFER_SIZE) {
      neededBufferSize = sourceFileSize - bufferOffset;
    }

    int numberRead = read(source, buffer, neededBufferSize);
    if (numberRead == -1) {
      perror("copy: can't read source");
      return -1;
    }

    int numberWrite = write(destination, buffer, numberRead);
    if (numberWrite == -1) {
      perror("copy: can't write in destination");
      return -1;
    }

    bufferOffset += BUFFER_SIZE;
    totalNumberWrite += numberWrite;
  }

  return totalNumberWrite;
}

int archive_un_fichier(int fd_archive, const char *fichier) {
  uint8_t fileNameSize = strlen(fichier);
  uint64_t fileSize = 0;
  
  int sourceFile = open(fichier, O_RDONLY, 0666);
  if (sourceFile == -1) {
    perror("archive file: can't open file to archive");
    return -1;
  }

  fileSize = taille_fichier(sourceFile);

  if (write(fd_archive, &fileNameSize, sizeof(uint8_t)) == -1) {
    perror("archive file: can't write name size");
    return -1;
  };
  
  
  if (write(fd_archive, fichier, fileNameSize) == -1) {
    perror("archive file: can't write name");
    return -1;
  };

  if (write(fd_archive, &fileSize, sizeof(uint64_t)) == -1) {
    perror("archive file: can't write file size");
    return -1;
  };

  if (copierArchive(sourceFile, fd_archive) == -1) {
    return -1;
  }

  if (close(sourceFile) == -1) {
    perror("archive file: can't close file to archive");
    return -1;
  }

  return fileSize;
}

int creer_archive(const char *fichier_archive, char **liste_fichiers,
                  uint32_t nb_fichiers) {
  int destinationArchive =
      open(fichier_archive, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (destinationArchive == -1) {
    perror("prob open destination");
    return -1;
  }
  if (write(destinationArchive, &nb_fichiers, sizeof(uint32_t)) == -1) {
    perror("prob ecrire taille fichiers");
    return -1;
  };

  int res = 0;
  for (uint32_t fileNumber = 0; fileNumber < nb_fichiers; fileNumber += 1) {
    int tailleEcrite =
        archive_un_fichier(destinationArchive, liste_fichiers[fileNumber]);
    if (tailleEcrite == -1) {
      fprintf(stderr, "prob achive un fichier dans creer\n");
      return -1;
    };
    res += tailleEcrite;
  }

  if (close(destinationArchive) == -1) {
    perror("prob close destinationArchive");
    return -1;
  }

  return res;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "pas assez d'arguments");
    return -1;
  }
  char *files[argc - 2];
  for (int i = 0; i < argc - 2; i++) {
    files[i] = argv[i + 2];
  }
  creer_archive(argv[1], files, 2);

  return 0;
}