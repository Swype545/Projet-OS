
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utmpx.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	//flags s q H 
	int fhead = 0;
	int fsmall = 0;
	int fcount = 0;
	
	//tester 100 ou plus si ca marche pas
	int arlen = 100;
	char strTime[arlen];

	//structure time type tm		
	struct tm strutime;

	//Structure utmpx
	//on va utiliser cette structure pour récuperer toutes les informations à chaque ittération pour un utilisateur.
	struct utmpx *strutmpx;

	// nombre d'utilisateurs pour le -q
	int nbusers = 0;


	//Gestion d'erreurs on renvoit le nom du programme
	char *prog_name;
	prog_name = argv[0];

	//on va aller lire dans un struct on remet le curseur a zéro dans utmpx
	setutxent();

	//détermination des flags
	int c;

	struct option options[] =
	{
			{"count", no_argument, NULL, 'q'},
			{"name", no_argument, NULL, 's'},
			{"heading", no_argument, NULL, 'H'},
			{0,0,0,0}
	};
	while ((c = getopt_long(argc, argv, "qsH",options, NULL)) != -1){
		switch(c){
				
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
	
	//on va inscrire dans la structure utmpx, tant qu'il y aura des user le get ne sera pas nul on aura donc les info via utmpx pour chaque iutilisateur
	while ((strutmpx = getutxent()) != NULL) {
			
			//on va filtrer les valeur d'utmpxqui auront le label 7 (pour normal process)
        	if (strutmpx->ut_type == 7) {

				//on va vérifier si le flag h est coché et qu'on est a la première ittération on va afficher un header
				if (fhead == 1 && nbusers == 0 )
					{
                   		printf("NAME\tLINE\tTIME\t  \n");	
                    	}
					// si on ne doit pas compter le nbre d'users
            		if (fcount != 1) {
                		//afficher nom des user
                		printf("%s\t", strutmpx->ut_user);
						//affiche le nom du device
                		printf("%s\t", strutmpx->ut_line);
						//getTime
                		if (localtime_r((time_t *)&strutmpx->ut_tv.tv_sec, &strutime) == NULL) {
                    		perror(prog_name);
                    		return EXIT_FAILURE;
                		}
						//afficher le temps 
						if (strftime(strTime,arlen, "%Y-%m-%d %H:%M",&strutime)<0){
							perror(prog_name);
							return EXIT_FAILURE;
						}
                		printf("%s\t", strTime);
                		printf("(%s)\t", strutmpx->ut_host);
                		printf("\n");
            		}
            		//on va incrémenter le nbuser a chaque ittération si on a le flag de comptage
            		if (fcount)
            			nbusers++;
        	}
    	}
	//on compte le nombre d'utilisateurs
	if (fcount)
	{
		if (nbusers == -1) 
		{
        	perror(prog_name);
        	exit(EXIT_FAILURE);
    	}
    	//on l'affiche en tenant compte du nombre d'utilisateurs
    	if (nbusers >1)
    	{
    		printf("\n #user=%d \n", nbusers);
    	}
        printf("\n #users=%d \n", nbusers);
	}

	endutxent();
	return EXIT_SUCCESS;	
}