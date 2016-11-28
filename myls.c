#include <getopt.h>

#include <dirent.h>
#include <unistd.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define BUF_SIZE 1024

/*Creation de la structure pour les directory*/
struct linux_dirent {
	//Nombre qui defini où est le fichier
	long d_ino;
	//Lien vers le prochain fichier
	off_t d_off;
	//La longueur de la structure
	unsigned short d_reclen;
	//Nom du directory
	char d_name[];
};

char* copyString(char *str){
	int size = sizeof(str)+1;
	char *copy = (char*) malloc (size * sizeof(char));
	int i;
	for (i = 0; i < size; i++)
	{
		copy[i] = str[i];
	}
	return copy;
}

//argv = les paramètres plus la commande
//argc = taille de ce tableau d'arguments
int main(int argc, char * const argv[], const char *optstring){
	//Definition des flag à 0
	int lflag = 0, rflag = 0, aflag = 0;
	char *nvalue = NULL;

	int index, c;
	
	int fd, nread;
	char buf[BUF_SIZE];
	struct linux_dirent *d;
	int bpos;
	char d_type;
	
	
	//Definition des attributs pour le getopt
	extern char *optarg;
	extern int optind,opterr,optopt;
	
	//Defini la structure
	struct option longopts[] = {
		{"all", no_argument, NULL, 'a'},
		{"recursive", no_argument, NULL, 'R'},
		{"long", no_argument, NULL, 'l'},
		{0,0,0,0}
	};


	while ((c = getopt_long(argc, argv,"Ral",longopts,NULL)) != -1){
		switch(c){
		case 'R':
			rflag = 1;break;
		case 'a':
			aflag = 1;break;
		case 'l':
			lflag = 1;break;
		case 0:
			break;
		case ':':
			fprintf(stderr, "%s: option '-%c' requiert un argument\n", argv[0], optopt);
			break;
		case '?':
		default:
			fprintf(stderr, "%s: option '-%c' est invalide: ignoree\n", argv[0], optopt);
			break;
		}
	}
	
	if(lflag == 0 && aflag == 0 && rflag == 0)
	{
		//fd = filedescriptor
		//Si taille tableau argc plus grand que 1, on defini fd comme étant le "premier paramètre" 
		//Sinon, fd = "." (current file directory)
		
		//Si pas de paramètre, alors argv[1] est potentiellement un path
		fd= open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
		
		//printf("noparam\n");
	} else {
		fd= open(argc > 2 ? argv[2] : ".", O_RDONLY | O_DIRECTORY);
	}
	
	//si le parametre a est donné
	if(aflag == 1)
	{
		printf("paramètre a\n");
	}
	//Si le parametre l est donné 
	if(lflag == 1)
	{
		printf("paramètre l\n");
	}
	
	//si le parametre R est donné
	if(rflag == 1)
	{
		printf("paramètre r\n");
	}

	//On boucle pour recuperer l'ensemble des fichier dans le directory
	while(1){
		
		//Syscall getdents avec paramètres adéquats
		nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
		if (nread ==0)
			//S'il n'y a plus rien, on fini la boucle
			break;

		//On crée un array de pointeur qui va contenir tout mes strings
		char *array[nread];
		
		//On definit la position à 0
		bpos = 0;
		
		//On boucle pour récuperer tout les fichiers dans le répertoire
		while (bpos<nread){
			//Recupere la ligne du fichier correspondant
			d= (struct linux_dirent *) (buf+bpos);
			char d_name = &(d->d_name);
			//On affiche les données
			if(aflag == 1)
			{
				//array[bpos] = copyString(d->d_name);
				//Si le paramètre "all" est activé
				printf("%s\n", d->d_name);
			}else{
				//array[bpos] = copyString(d->d_name);
				if(&d->d_name[0] == "."){}
				else{
					//printf("%c", d_name);
					printf("%4d %s\n", d->d_reclen, d->d_name);
				}
				
			}
			//printf("%s", array[0]);
			
			//Incremente bpos 
			bpos += d->d_reclen;
		}
	}

	//Arrête le programme 
	exit(EXIT_SUCCESS);
};
