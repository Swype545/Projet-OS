#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utmpx.h>

//Main
int main(int argc, char *argv[]) {
	
	
	//flags s q b H 
	int fcount = 0;
	int fboot = 0;
	int fhead = 0;
	int fsmall = 0;

	//boot time ==2 or user mode ==7
	int mode=7;
	
	// nombre d'utilisateurs
	int nbusers = 0;

	//tester 100 ou plus si ca marche pas
	int ar_len = 100;
	char time[ar_len];
	//structure time type tm		
	struct tm strutime;

	//Structure utmpx
	struct utmpx *strutmpx;

	//For error on cmd 
	char *prog_name;
	prog_name = argv[0];

	//flags bqsH
	int c;
	
	struct option options[] =
	{
			{"boot", no_argument, NULL, 'b'},
			{"count", no_argument, NULL, 'q'},
			{"name", no_argument, NULL, 's'},
			{"heading", no_argument, NULL, 'H'},
			{0,0,0,0}
	};
	while ((c = getopt_long(argc, argv, "bqsH",options, NULL)) != -1){
		switch(c){
				case 'b':
					fboot = 1;
					break;
				case 'q':
					fcount =  1;
					break;
				case 's':
					fsmall=  1;
					break;
				case 'H':
					fhead =  1;
					break;
				case '?':
				default :
					abort();
		}
	}
	//fonction Who
	if (fboot)
	{
		mode=2;
		//on change le type d'user
	}

	setutxent();
	//on va aller lire dans un struct on remet le curseur a zéro dans utmpx

	while ((strutmpx = getutxent()) != NULL) {
	//Pint Normal process
		if (strutmpx->ut_type == 7) {
			//who -H
			if (fhead == 1 && nbusers == 0 && fboot !=1 )
				{
					printf("NAME\tLINE\tTIME\t \n");	
					}
				//who 
				if (fcount != 1) {
			
					//afficher nom des user
					printf("%s\t", strutmpx->ut_user);
					//afficher line user
					printf("%s\t", strutmpx->ut_line);
					//getTime
					if (localtime_r((time_t *)&strutmpx->ut_tv.tv_sec, &strutime) == NULL) {
						perror(prog_name);
						return EXIT_FAILURE;
					}
					//afficher le temps 
					if (strftime(time,ar_len, "%Y-%m-%d %H:%M",&strutime)<0){
						perror(prog_name);
						return EXIT_FAILURE;
					}
					printf("%s\t", time);
					//printf("(%s)\t", strutmpx->ut_host);
					printf("\n");
				}
				//on va incrémenter le nbuser a chaque ittération, quaque fois que user process est de type normal
				nbusers++;
		}
	}

   	if(fboot)
    {
    	if (strutmpx->ut_type == mode) 
    	{
    		if (strftime(time,ar_len, "%Y-%m-%d %H:%M",&strutime)<0)
    		{
				perror(prog_name);
				return EXIT_FAILURE;
			}
		}

    	printf("time since reboot: %s\t", time);
    }
	//on compte le nombre d'utilisateurs
	if (fcount)
	{
		if (nbusers == -1) 
		{
        	perror(prog_name);
        	exit(EXIT_FAILURE);
    	}
        printf("\n#users=%d\n", nbusers);
	}

	endutxent();
	return EXIT_SUCCESS;	
}