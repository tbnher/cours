#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>



int copier(int source, int destination, int taille){

    char tableau[1024];

    int res = 0;
    int test = 0;
    for(int i = 0; i < taille ; i++){
        if(read(source,tableau,taille)==-1){
            perror("source incorrecte");
            close(source);
            close(destination);
            return 1;
        }
        test = write(destination,tableau,taille);
        res += test;
        if(test==-1){
            perror("prob destination");
            close(source);
            close(destination);
            return 1;
        }
    }
   

    close(source);
    close(destination);

    return res;
}



int extrait_fichier(int fd_archive){

    uint32_t tableau[1024];
    if(read(fd_archive,tableau,1)==-1){
        perror("prob archive");
        return -1;
    }

    int sizeOfName = tableau[0];
    printf("%d\n",tableau[0]);
    printf("%d\n",sizeOfName);
    
    
    char name[1024];

    if(read(fd_archive,name,sizeOfName)==-1){
        perror("a");
        return -1;
    }

    //Creer tableau char taille 1024 et boucle for pour insérer les valeurs du buffer dans le tab
    
    printf("%s\n",name);
    
    uint64_t sizeOfFile[1];

    if(read(fd_archive,sizeOfFile,8)==-1){
        perror("prob taille fichier");
        return -1;
    }

    printf("toto:%ld\n",sizeOfFile[0]);

    int fd_dest = open(name,O_WRONLY | O_CREAT | O_TRUNC, 0666);
    printf("%d\n",fd_dest);
    if(fd_dest==-1){
        perror("prob creation fichier");
        return -1;
    }

    

    int sizeOfRead = read(fd_archive,tableau,sizeOfFile[0]);
    if(sizeOfRead==-1){
        perror("prob lecture contenu");
        return -1;
    }


    int res = write(fd_dest,tableau,sizeOfRead);
    if(res==-1){
        perror("prob ecriture");
        return -1;
    }
    return res;
    
}

int main(void){

    int fd = open("/home/infoetu/arthur.blanchet.etu/s3/c/test",O_RDONLY);
    if(fd==-1){
        perror("main error open");
        return -1;
    }

    int fd2 = open("/home/infoetu/arthur.blanchet.etu/s3/c/test1",O_WRONLY | O_CREAT | O_TRUNC, 0666);
    int fd3 = open("/home/infoetu/arthur.blanchet.etu/s3/c/fichiers_textes.arch",O_RDONLY);
    lseek(fd3,4,SEEK_SET);
    copier(fd,fd2,5*sizeof(char));
    extrait_fichier(fd3);
}