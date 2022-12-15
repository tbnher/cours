#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

// Copie un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination en copiant taille octets
// retourne le nombre d'octets copiés en cas de succès, -1 en cas d'erreur
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

// Retourne la taille d'un fichier désigné par le descripteur fd, à l'aide
// de la fonction lseek/
// Le curseur associé au fichier doit être replacé à sa place d'origine
// Retourn -1 en cas d'erreur, la taille du fichier en cas de succès
int taille_fichier(int fd);


// Copie entierement un fichier désigné par le descripteur source
// dans un fichier désigné par le descripteur destination
// retourne le nombre d'octets copiés
int copier(int source, int destination);

// Ajoute le fichier désigné par le chemin fichier dans l'archive
// désigné par le descripteur fd_archive
//
// Retourne -1 en cas d'erreur et le nombre d'octets écrits dans
// l'archive en cas de succès
int archive_un_fichier(int fd_archive, const char *fichier);


// Créé un fichier archive à partir d'une liste de fichier source.
// Ecrase le fichier destination s'il existe déjà
//
// Retourne -1 en cas d'erreur et la taille de l'archive finale en
// nombre d'octets en cas de succès
int creer_archive(const char *fichier_archive, char **liste_fichiers, uint32_t nb_fichiers);