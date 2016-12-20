//Problème: plusieurs caractères sont affichés aléatoirement (problème de buffer)

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
	int fd, openpid, openpidcomm, nread;
	char buf[BUF_SIZE];
	char buf_print[BUF_SIZE];
	struct linux_dirent *d;
	int bpos;
	char d_type, dname;
	
	//le PID est l'argument après l'ensemble des options
	char *p_id; 
	//p_id = argv[optind];
	
	//getOpt
	extern char *optarg;
	extern int optind,opterr,optopt;
	
	struct option longopts[] = {
		{"pid", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{0,0,0,0}
	};
	
	//Flags
	int hflag = 0, pflag = 0;
	
	//Switches pour le getOpt
	while ((c = getopt_long(argc, argv,"hp:",longopts,NULL)) != -1){
		switch(c){
		case 'h':
			hflag = 1;break;
		case 'p':
		    printf("%s", optarg);
			exit(0);
			p_id = optarg;
			if(p_id == NULL)
				abort();
			pflag = 1;break;
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
	
	if(hflag == 1){
		printf("\n Cette fonction liste l'ensemble des processus en cours\n");
		printf("-h ou --help affiche l'aide\n");
		printf("-p ou --pid affiche les informations relative au pid mis en option\n");
	}else{
		//Erreur si p_id pas donné et pflag demandé
		if((pflag == 1) && (p_id == NULL)){
			printf("Un PID est necessaire en paramètre pour utiliser l'option -p. \n");
			return EXIT_FAILURE;
		}
		
		
		if((fd = open("/proc",O_RDONLY|O_DIRECTORY)) == -1){
			printf("ErrorFD");
			return EXIT_FAILURE;
		}
		
		while(1){
			//Commande getdents
			if((nread = syscall(SYS_getdents, fd, buf, BUF_SIZE)) == -1)
			{
				printf("Error Getdents");
				return EXIT_FAILURE;
			}
			
			if(nread == 0)
				break;
			
			bpos = 0;
			
			while(bpos < nread){
				
				d = (struct linux_dirent *) (buf+bpos);
				d_type =  *(buf + bpos +d->d_reclen -1);
							
				//Dans proc, il y'a l'ensemble des PIDs et d'autres fichiers.
				//Les fichiers qui commencent par un nombre sont des PIDs
				dname = *d->d_name;
				if(isdigit(dname)){

					//Si le flag p est utilisé
					if(pflag == 1){
						
						//On compare le nom et le p_id fourni par l'utilisateur
						if(strcmp(d->d_name, p_id) == 0){
							//On ouvre le fichier PID qu'on est en train d'analyser
							openpid = openat(fd, d->d_name, O_RDONLY | O_DIRECTORY);
							
							//On ouvre le fichier "comm" qui contient le nom du processus
							openpidcomm = openat(openpid, "comm", O_RDONLY); 
						
							int n;
							//On place les caractères dans un buffer, qu'on affiche ensuite
							while((n = read(openpidcomm, buf_print, BUF_SIZE-1)) > 0){
								buf_print[n] = '\0';
								printf("%4s -- %s", d->d_name, buf_print);
							}
							close(openpidcomm);
							close(openpid);
							
							break;
						}
						
					}else{
						//On ouvre le fichier PID qu'on est en train d'analyser
						openpid = openat(fd, d->d_name, O_RDONLY | O_DIRECTORY);
						
						//On ouvre le fichier "comm" qui contient le nom du processus
						openpidcomm = openat(openpid, "comm", O_RDONLY); 
					
						int n;
						//On place les caractères dans un buffer, qu'on affiche ensuite
						while((n = read(openpidcomm, buf_print, BUF_SIZE-1)) > 0){
							buf_print[n] = '\0';
							printf("%4s -- %s", d->d_name, buf_print);
						}
						close(openpidcomm);
						close(openpid);
					}	
				}
				//Incremente bpos 
				bpos += d->d_reclen;
			}
		}		
	}
	
	
	close(fd);
	exit(EXIT_SUCCESS);
};
