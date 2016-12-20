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
	
	//Input pour le "i"
	char input[1024];
	
	//Variable "Path"
	char *path;
	
	//getOpt
	extern char *optarg;
	extern int optind,opterr,optopt;
	
	struct option longopts[] = {
		{"directory", no_argument, NULL, 'd'},
		{"interactive", no_argument, NULL, 'i'},
		{0,0,0,0}
	};
	
	//Flags
	int dflag = 0, iflag = 0;
	
	//Switches pour le getOpt
	while ((c = getopt_long(argc, argv,"id",longopts,NULL)) != -1){
		switch(c){
		case 'i':
			iflag = 1;break;
		case 'd':
			dflag = 1;break;
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
	if(argc > optind)
	{
		//On définit le path actuel
		path = strdup(argv[optind]);
		
		//verification si c'est un fichier ou un dossier
		if(open(argv[optind], O_RDONLY | O_DIRECTORY) == -1){
			//On regarde si l'erreur est causée car ce n'est pas un DIR!
			if (errno == ENOTDIR)
			{
				//Si on attend une confirmation
				if(iflag == 1){
					while(1){
						printf("Voulez-vous vraiment supprimer ce fichier Y/N ? ");
						
						//On attend l'input
						if (!fgets(input, sizeof(input)-1, stdin)){
							printf("\n");
						}
						
						//On supprime le \n de fin pour ne pas avoir d'erreur
						if (strchr(input, '\n'))
							*strchr(input, '\n') = 0;
				
						//On attend une entrée "Y or N"
						if(!strcmp(input, "Y") || !strcmp(input, "y")){
							break;
						}else if(!strcmp(input, "N") || !strcmp(input, "n")){
							printf("Aucune suppression n'est réalisée.\n");
							return EXIT_FAILURE;
						}else{printf("Veuillez entrer Y ou N. \n");}
					}
				}

				//On fait le rm normal
				printf("suppression de '%s'.\n", path);
				unlink(path);
			}else{
				printf("L'element n'est pas trouvable.\n");
				return EXIT_FAILURE;
			}
		}else{
			
			//Si on attend une confirmation
			if(iflag == 1){
				while(1){
					printf("Voulez-vous vraiment supprimer ce fichier Y/N ? ");
					
					//On attend l'input
					if (!fgets(input, sizeof(input)-1, stdin)){
						printf("\n");
					}
					
					//On supprime le \n de fin pour ne pas avoir d'erreur
					if (strchr(input, '\n'))
						*strchr(input, '\n') = 0;
					
					//On attend une entrée "Y or N"
					if(!strcmp(input, "Y") || !strcmp(input, "y")){
						break;
					}else if(!strcmp(input, "N") || !strcmp(input, "n")){
						printf("Aucune suppression n'est réalisée.\n");
						return EXIT_FAILURE;
					}else{printf("Veuillez entrer Y ou N. \n");}
				}
			}
			
			if(dflag == 1){
				//Suppression autorisée
				printf("suppression du répertoire '%s'.\n", path);
				
				//-----------------------------------------------------------------------------------------------------
				//Cette option n'est pas implémentée: (par l'accord de Mr Combéfis)
				//En effet, pour supprimer un répertoire, il faut utiliser la commande (rmdir(path)) (syscall de niveau 2)
				//Mais pour ce faire, il faut préalablement vider le contenu du repertoire path, ce qui demande
				//une suppression récursive des éléments présents dans ce repertoire.
				//Ayant déjà réalisé une option récursive (dans le "myls -R"), Mr Combéfis nous a autorisée
				//a ne pas implémenter cette partie du code.
				//-----------------------------------------------------------------------------------------------------
				
				
			}else{
				printf("Impossible de supprimer un répertoire.\n");
				return EXIT_FAILURE;
			}
		}
		
	}else{
		// si pas d'argument, on a un probleme
		printf("Un paramètre doit être entré\n");
		return EXIT_FAILURE;
	}
};
