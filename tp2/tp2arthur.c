#include <fcntl.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1000

// Copie un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination en copiant taille
// octets retourne le nombre d'octets copiés en cas de succès, -1 en cas
// d'erreur
int copier(int source, int destination, int taille);

// Extrait le prochain fichier contenu dans l'archive. Si un fichier
// portant le même nom que celui indiqué dans l'archive existe déjà il
// doit être écrasé.
//
// Retourne -1 en cas d'erreur et le nombre d'octets du fichier
// extrait en cas de succès

int extrait_fichier(int fd_archive);

// Extrait l'archive dans le répertoire courant.
//
// Retourne -1 en cas d'erreur, le nombre de fichiers créés en cas de
// succès.
int extrait_archive(const char *archive);

int copier(int source, int destination, int taille) {
  if (source == -1) {
    fprintf(stderr, "copy: source doesn't exist\n");
    return -1;
  }
  if (destination == -1) {
    fprintf(stderr, "copy: destination doesn't exist\n");
    return -1;
  }

  uint8_t buffer[BUFFER_SIZE];
  uint32_t total_written = 0;

  int buffer_offset = 0;
  int needed_buffer_size = BUFFER_SIZE;
  while (buffer_offset < taille) {
    if (taille - buffer_offset < BUFFER_SIZE) {
      needed_buffer_size = taille - buffer_offset;
    }

    int red = read(source, buffer, needed_buffer_size);
    if (red == -1) {
      perror("copy: can't read source");
      return -1;
    }

    int written = write(destination, buffer, red);
    if (written == -1) {
      perror("copy: can't write in destination");
      return -1;
    }

    buffer_offset += BUFFER_SIZE;
    total_written += written;
  }

  return total_written;
}

int extrait_fichier(int fd_archive) {
  int total_written = 0;

  uint8_t file_name_size = 0;
  char file_name[256];
  uint64_t file_size = 0;

  if (read(fd_archive, &file_name_size, sizeof(uint8_t)) == -1) {
    perror("extract file: can't read name size");
    return -1;
  };

  if (read(fd_archive, file_name, file_name_size) == -1) {
    perror("extract file: can't read name");
    return -1;
  };
  file_name[file_name_size] = '\0';

  if (read(fd_archive, &file_size, sizeof(uint64_t)) == -1) {
    perror("extract file: can't read file size");
    return -1;
  };

  int file_destination = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (file_destination == -1) {
    perror("extract file: can't open destination file");
    return -1;
  }

  if ((total_written = copier(fd_archive, file_destination, file_size)) == -1) {
    return -1;
  }

  if (close(file_destination) == -1) {
    perror("extract file: can't close destination file");
    return -1;
  }

  return total_written;
}

int extrait_archive(const char *archive) {
  uint32_t files_quantity = 0;

  int archive_source = open(archive, O_RDONLY, 0666);
  if (archive_source == -1) {
    perror("extract archive: can't open archive source");
    return -1;
  }

  if (read(archive_source, &files_quantity, sizeof(uint32_t)) == -1) {
    perror("extract archive: can't read files quantity");
    return -1;
  };

  uint32_t files_number = 0;
  for (; files_number < files_quantity; files_number += 1) {
    if (extrait_fichier(archive_source) == -1) {
      fprintf(stderr, "extract archive: can't extract file\n");
      return -1;
    };
  }

  if (close(archive_source) == -1) {
    perror("extract archive: can't close archive source");
    return -1;
  }

  return files_number;
}

int main(int argc, char **argv) {
  for(int i = 1; i < argc; i++){
    extrait_archive(argv[i]);

  }

  return 0;
}