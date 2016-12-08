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
	
	//getOpt
	extern char *optarg;
	extern int optind,opterr,optopt;
	
	struct option longopts[] = {
		{"command", no_argument, NULL, 'c'},
		{"r", no_argument, NULL, 'r'},
		{0,0,0,0}
	};
	
	//Flags
	int cflag = 0, rflag = 0, hflag = 0;
	
	//Switches pour le getOpt
	while ((c = getopt_long(argc, argv,"cr",longopts,NULL)) != -1){
		switch(c){
		case 'c':
			cflag = 1;break;
		case 'r':
			rflag = 1;break;
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
		
		printf("PID    Commande\n");
		
		while(bpos < nread){
			
			d = (struct linux_dirent *) (buf+bpos);
			d_type =  *(buf + bpos +d->d_reclen -1);
			
			//get the file descriptor of the file in question
            sfd = openat(fd, d->d_name, O_RDONLY | O_DIRECTORY);
			
			
			//Incremente bpos 
			bpos += d->d_reclen;
		}
	}

	
	
	exit(EXIT_SUCCESS);
};
