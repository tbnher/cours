#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1000
// Copie un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination en copiant taille octets
// retourne le nombre d'octets copiés en cas de succès, -1 en cas d'erreur
int copier(int source, int destination, int taille){

    if (source == -1) {
        fprintf(stderr, "copie: La destination n'existe pas\n");
        return -1;
    }
    if (destination == -1) {
        fprintf(stderr, "copie: La destination n'existe pas\n");
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
      perror("copie: impossible de lire la source");
      return -1;
    }

    int written = write(destination, buffer, red);
    if (written == -1) {
      perror("copie: impossible de lire la source");
      return -1;
    }

    buffer_offset += BUFFER_SIZE;
    total_written += written;
  }

  return total_written;
}

// Extrait le prochain fichier contenu dans l'archive. Si un fichier
// portant le même nom que celui indiqué dans l'archive existe déjà il
// doit être écrasé.
//
// Retourne -1 en cas d'erreur et le nombre d'octets du fichier
// extrait en cas de succès

int extrait_fichier(int fd_archive){
    int compteur = 0;
    uint8_t tab;
    int result_l_taille_titre = read(fd_archive, &tab, 1);
    if(result_l_taille_titre == -1){
        return -1;
    }else{
        compteur += 1;
    } 

    char nom[tab+1];
    int result_l_titre = read(fd_archive, &nom, tab);
    if(result_l_titre == -1){
        return -1;
    }else{
        compteur += result_l_titre;
        nom[tab] = '\0';
    } 

    uint64_t taille_fichier;
    int result_l_taille_f = read(fd_archive, &taille_fichier, 8);
    if(result_l_taille_f == -1){
        return -1;
    }else{
        compteur += 8;
    } 

    char donnees[taille_fichier+1];
    int result_l_fichier = read(fd_archive, &donnees, taille_fichier);
    if(result_l_fichier == -1){
        return -1;
    }else{
        compteur += result_l_fichier;
        donnees[taille_fichier] = '\0';
    }

    int fd;
    fd = open(nom, O_WRONLY | O_CREAT, 0666);
    if(fd == -1){
        return -1;
    }

    int result_r = write(fd, donnees, compteur);
    if(result_r == -1){
        return -1;
    }

    int result_c = close(fd);
    if(result_c == -1){
        return -1;
    }

    return compteur;
}

// Extrait l'archive dans le répertoire courant.
//
// Retourne -1 en cas d'erreur, le nombre de fichiers créés en cas de
// succès.
int extrait_archive(const char *archive){

    int fd_archive = open(archive, O_RDONLY);
    if(fd_archive == -1){
        return -1;
    }

    uint32_t nb_fichier = 0;
    int result_l_nb_fichier = read(fd_archive, &nb_fichier, 4);
    if(result_l_nb_fichier == -1){
        return -1;
    }

    for (uint32_t i = 0; i < nb_fichier; i++){
        int result = extrait_fichier(fd_archive);
        if(result == -1){
            return -1;
        }
    }

    int result_c = close(fd_archive);
    if(result_c == -1){
        return -1;
    }

    return nb_fichier;
}


int main(void)
{
    char chaine[] = "fichiers_binaires.arch";
    int result = extrait_archive(chaine);
    if (result == -1){
        return -1;
    }

    return result;
    
}

