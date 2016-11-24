#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char * const argv[], const char *optstring){
	extern char *optarg;
	extern int optind,opterr,optopt;

	int mflag = 0, hflag = 0, nflag = 0,vflag = 0;
	char *nvalue = NULL;

	int index, c;
	struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"name", required_argument, NULL, 'n'},
		{"verbose",no_argument, NULL, 'v'},
		{0,0,0,0}
	};


	while ((c = getopt_long(argc, argv,"Mhn:v",longopts,NULL)) != -1)
		switch(c){
		case 'M':
			mflag = 1;break;
		case 'h':
			hflag = 1;break;
		case 'n':
			nvalue = optarg;
			nflag = 1;break;
		case 'v':
			vflag = 1;break;
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

	//si le parametre h est donné
	if(hflag == 1)
	{
		printf("-M ecrit en majuscule \n -h ou --help affiche le manuel \n -n ou --name dit hello au nom affiché en parametre \n -v ou --verbose donne la meteo en plus d'afficher Hello World\n");
	}
	//si le parametre m est donné
	if(mflag == 1)
	{
		if(nflag == 0)
		{
			printf("HELLO WORLD \n");
		}
		if(nflag == 1)
		{
			printf("HELLO %s \n", nvalue);
		}
	}else{
		//cas de base
		if(nflag == 0)
		{
			printf("Hello World \n");
		}
		if(nflag == 1)
		{
			printf("Hello %s \n", nvalue);
		}
	}
	//Si on a le parametre verbose
	if(vflag == 1)
	{
		printf("Il fait beau \n Il fait 15 degres \n yolo \n");
	}
};
