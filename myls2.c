//ERREURS:
//index = 1 dans les boucles à la fin pausent un problème
//qsort pose un problème
//Liste les fichiers, triés par dossiers, séparé par une ligne vide.

//----------------------------------------------------------
//INCLUDES
//----------------------------------------------------------
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

//----------------------------------------------------------
//DEFINITIONS DE VARIABLES
//----------------------------------------------------------
#define BUF_SIZE 1024

static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


//----------------------------------------------------------
//STRUCTURES
//----------------------------------------------------------
/*Creation de la structure pour les directory*/
struct linux_dirent {
	long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	/*unsigned char  d_type;   /* File type */
	char d_name[];
};

//----------------------------------------------------------
//MAIN
//----------------------------------------------------------
int main(int argc, char * const argv[], const char *optstring){
	//---------------------
	//VARIABLES
	//---------------------
	
	//Boucles
	int i, index, c;
	
	//Paramètres nécessaire pour le getdents 
	int fd, fdwhile, nread;
	char buf[BUF_SIZE];
	struct linux_dirent *d;
	int bpos;
	char d_type;
	
	//Variable "Path"
	char *path;
	
	//Tableau: détiens le nom de mes fichiers/dossiers à afficher
	int arlen = 10;
	int arpos = 0;
	
	char **array = calloc(arlen, sizeof(char*));
	if (array == NULL) {
		//perror("recArray");
		return EXIT_FAILURE;
	}
	
	
	//Tableau: liste recursive des dossiers
	int recArlen = 10;
	int recArpos = 0;
	
	char **recArray = calloc(arlen, sizeof(char*));
	if (recArray == NULL) {
		//perror("recArray");
		return EXIT_FAILURE;
	}
	
	//Structure pour les statistiques
	struct stat filestat;
	struct stat modestat;
	
	//getOpt
	extern char *optarg;
	extern int optind,opterr,optopt;
	
	struct option longopts[] = {
		{"all", no_argument, NULL, 'a'},
		{"recursive", no_argument, NULL, 'R'},
		{"long", no_argument, NULL, 'l'},
		{0,0,0,0}
	};
	
	//Flags
	int lflag = 0, rflag = 0, aflag = 0;
	
	//Switches pour le getOpt
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

	//---------------------
	//CODE
	//---------------------
	//Definition du filedescriptor (fd) en fonction de la présence ou non de paramètre
	
	if(argc > optind)
	{
		//On définit le path actuel
		path = strdup(argv[optind]);
		if(fd= open(argv[optind], O_RDONLY | O_DIRECTORY) == -1){
			if (errno == ENOTDIR)
			{
				printf("%s\n", argv[optind]);
			}else{
				printf("error OPEN\n");
			}
			return EXIT_FAILURE;
		};
	}else{
		path = strdup(".");
		if(fd = open(".", O_RDONLY | O_DIRECTORY) == -1){
			printf("error OPEN\n");
			return EXIT_FAILURE;
		};
	}

	
	
	//On ajoute le path actuel au recArray
	recArray[recArpos++] = path;
	if (array == NULL)
		return EXIT_FAILURE;
		
	//Boucle principale (Récupère le nom des fichiers, les stock dans "array")
	//On fait ceci pour chaque element dans "recArray" pour gerer l'option "recursive -r"
	for(i=1; i<=recArpos; i++)
	{
		//le filedescriptor depend du dossier qu'on traite
		if((fdwhile = open(recArray[i-1], O_RDONLY | O_DIRECTORY)) == -1){
			if (errno == ENOTDIR)
			{
				printf("%s\n", recArray[i-1]);
			}else{
				printf("error OPEN in\n");
			}
		}
		while(1){
			//Syscall getdents
			if((nread = syscall(SYS_getdents, fdwhile, buf, BUF_SIZE)) == -1)
			{
				//printf("recarray[i-1]: %s\n",recArray[i-1]);
				if(errno == EBADF)
				{
					//printf("EBADF");
				}

			}
			//S'il n'y a plus rien, on fini la boucle
			if (nread ==0)
				break;
			
			bpos = 0;
			
			//On boucle pour récupérer tout les fichiers du répertoire actuellement utilisé
			while (bpos<nread){
				d = (struct linux_dirent *) (buf+bpos);
				d_type =  *(buf + bpos +d->d_reclen -1);
				
				//On verifie qu'on a assez de memoire allouée pour "array"
				if (arpos == arlen) {
					arlen *= 2;
					array = realloc(array, arlen * sizeof(char*));
						//ERREUR traitée
				}
				//On ajoute le nom du fichier/dossier dans l'array
				array[arpos] = strdup(d->d_name);
				arpos++;
				
				//Si la ligne actuellement traitée est un dossier, on l'ajoute à recArray
				//On le liste pas les "." et ".."
				if (d_type == DT_DIR && (strcmp(d->d_name, ".") != 0) && (strcmp(d->d_name, "..") != 0)) {
					//On verifie qu'on a assez de memoire allouée pour "recArray"
					if (recArpos == recArlen) {
						recArlen *= 2;
						recArray = realloc(recArray, recArlen * sizeof(char*));
						//ERREUR traitée
					}
					
					//On ajoute le path à recArray
					recArray[recArpos] = calloc(strlen(recArray[i-1])+strlen(d->d_name)+2, sizeof(char*));
					sprintf(recArray[recArpos], "%s/%s", recArray[i-1], d->d_name);	
					recArpos++;
				}
				//Incremente bpos 
				bpos += d->d_reclen;
			}
		}	
		
		arlen = arpos;
		if((array = realloc(array, arlen * sizeof(char*))) == NULL){
			printf("error");
			exit(EXIT_FAILURE);
		}
		
		
		//On trie la liste récupérée
		qsort(array, arlen, sizeof(char*), cmpstringp);
		//qsort(recArray, recArlen, sizeof(char*), cmpstringp);
		
		//On affiche le nom du sous-dossier
		printf("\n %s: \n", recArray[i-1]);
		
		//On affiche les informations
		if(lflag == 1)
		{
			if(aflag == 1)
			{
				//On affiche les fichiers cachés, avec des informations complémentaires
				for(index = 0; index < arlen; index++)
				{
					stat(array[index], &filestat);
					printf("size: %ld last:%s %s \n", filestat.st_size, ctime(&filestat.st_atime), array[index]);
				}
			}else{
				//On masque les fichiers cachés, avec des infomations complémentaires
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
				//On affiche les fichiers cachés
				for(index = 0; index < arlen; index++)
				{
					printf("%s\n", array[index]);
				}
			}else{
				
				//On masque les fichiers cachés
				for(index = 0; index < arlen; index++)
				{
					if(array[index][0] != '.'){
						printf("%s\n", array[index]);
					}
				}
			}
		}
		if(rflag == 1){
			//Si le rflag est à 1, on clean l'ensemble des éléments dans array
			//Et on laisse reboucler sur les autres dossiers de "recArray"
			//free(array);
			//printf("path: %s\n", recArray[i-1]);
	
			arpos = 0;
			//arlen = 10;
			//array = realloc(array, arlen * sizeof(char*));
			
		}else{
			//Si on accepte pas la récursivité, on break la boucle
			break;
		}
	}
	
	
	exit(EXIT_SUCCESS);
};
