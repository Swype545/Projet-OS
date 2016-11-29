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

static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


/*Creation de la structure pour les directory*/
struct linux_dirent {
	long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[];
};

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
	
	//On stock le path
	char *path;
	
	//On definit l'array contenant l'ensemble de mes dossiers/fichiers
	//Permettra de trier mes valeurs par après
	int arlen = 10;
	int arpos = 0;
	
	char **array = calloc(arlen, sizeof(char*));
	if (array == NULL) {
		perror("array");
		return EXIT_FAILURE;
	}
	
	//La liste pour le recursive des dossiers
	int recArlen = 10;
	int recArpos = 0;
	char **recArray = calloc(arlen, sizeof(char*));
	if (recArray == NULL) {
		perror("recArray");
		return EXIT_FAILURE;
	}
	
	//La structure pour les stats
	struct stat filestat;
	
	
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
	
	//Permet de definir le filedescriptor si on a des paramètres ou non
	if(argc > optind)
	{
		path = strdup(argv[optind]);
		
		fd= open(argv[optind], O_RDONLY | O_DIRECTORY);
	}else{
		path = strdup(".");
		
		fd = open(".", O_RDONLY | O_DIRECTORY);
	}
	
	//On ajoute le path actuel au recArray
	recArray[recArpos++] = path;
	
	
	//Gestion d'erreur
	//if (array == NULL){
		assert(array);
		assert(arlen);
		//exit(EXIT_FAILURE);
	//}

	//On boucle pour recuperer l'ensemble des fichier dans le directory
	while(1){
		
		//Syscall getdents avec paramètres adéquats
		nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
		if (nread ==0)
			//S'il n'y a plus rien, on fini la boucle
			break;
		
		//On definit la position à 0
		bpos = 0;
		
		//On boucle pour récuperer tout les fichiers dans le répertoire
		while (bpos<nread){
			//Recupere la ligne du fichier correspondant
			d= (struct linux_dirent *) (buf+bpos);
			//char d_name = &(d->d_name);
			//On affiche les données
			
			/*if(d->d_type = DT_DIR){
				
				if (recArpos == recArlen) {
					recArlen *= 2;
					recArray == realloc(recArray, recArlen * sizeof(char*));
						//ERREUR traitée
				}
				
				//On recupère les path de tout les dossier là où je demande un ls
				recArray[recArpos++] = calloc(strlen(path)+strlen(d->d_name)+2, sizeof(char*));
				sprintf(recArray[recArpos++], "%s/%s", path, d->d_name);
			}*/
			
			if (arpos == arlen) {
				arlen *= 2;
				array == realloc(array, arlen * sizeof(char*));
					//ERREUR traitée
			}
			
			//On ajoute dans l'array
			array[arpos] = strdup(d->d_name);
			arpos++;
			
			//Incremente bpos 
			bpos += d->d_reclen;
		}
	}
	
	/*arlen = arpos;
	array = realloc(array, arlen * sizeof(char*));
	perror("realloc");*/
	
	if(lflag == 1)
	{
		if(aflag == 1)
		{
			//On doit afficher les fichiers cachés
			for(index = 0; index < arlen; index++)
			{
				stat(array[index], &filestat);
				printf("size: %ld last:%s %s \n", filestat.st_size, ctime(&filestat.st_atime), array[index]);
			}
		}else{
			
			//On cache les fichiers cacher
			for(index = 0; index < arlen; index++)
			{
				if(array[index][0] != '.'){
					stat(array[index], &filestat);
					printf("size: %ld last:%s %s\n", filestat.st_size, ctime(&filestat.st_atime), array[index]);
				}
			}
		}
	}else{
		if(aflag == 1)
		{
			//On doit afficher les fichiers cachés
			for(index = 0; index < arlen; index++)
			{
				printf("%s\n", array[index]);
			}
		}else{
			
			//On cache les fichiers cacher
			for(index = 0; index < arlen; index++)
			{
				if(array[indexindex][0] != '.'){
					printf("%s\n", array[index]);
				}
			}
		}
	}
	
	//Arrête le programme 
	exit(EXIT_SUCCESS);
};
