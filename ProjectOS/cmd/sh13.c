#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>



#define BUF_SIZE 2048
char input[1024];



void commandline(){
	printf("> ");
}

void waitinput(){
	//On attend l'input
	if (!fgets(input, sizeof(input)-1, stdin)){
		printf("\n");
		exit(0);
	}
}

void execcmd(){
	pid_t pid;
	
	//On supprime le \n de fin pour ne pas avoir d'erreur
	if (strchr(input, '\n'))
		*strchr(input, '\n') = 0;
	
	//Si on met quit, on arrête.
	if(!strcmp(input, "quit")){
		exit(1);
	}
	if(!strcmp(input, "exit")){
		exit(1);
	}
	
	//Si on ne met rien, rien ne se passe.
	if(!strcmp(input, "")){
		return;
	}
	
	pid = fork();
	
	if (pid < 0) //Erreur
	{
		printf("Une erreur s'est produite.\n");
		return;
	}
	else if(pid == 0) //Dans le fils
	{
		//On récupère les 4 paramètres de "input"
		char *cmd = strtok(input," ");
		char *p1 = strtok(NULL," ");
		char *p2 = strtok(NULL," ");
		char *p3 = strtok(NULL," ");
		
		char path[100];
		
		//On definit le path (ici, mes fichiers sont au même endroit)
		sprintf(path, "./%s", cmd);
		
		
		execlp(path, cmd, p1, p2, p3, NULL);
		
		//Si l'execution n'est pas possible, erreur
		printf("impossible d'éxecuter la commande: %s", input);
		exit(1);
	}
	else{ //Dans le parent
		int info;
		if(waitpid(pid, &info, 0) < 0){
			printf("Erreur waitpid");
			return;
		}	
	}
}

int main(int argc, char *argv[])
{
	char buf[BUF_SIZE];
	
	//Mode BASH
	//Si on donne des paramètres en appellant le shell...
	if (argc != 1){
		int fd = open(argv[optind], O_RDONLY);
		
		//On met les caractères dans un buffer
		while((read(fd, buf, BUF_SIZE)) >0);
			
		//A chaque \n qu'on trouve, on coupe (détermination des différentes commandes dans le fichier)
		char *save, *token;
		
		token = strtok_r(buf, "\n", &save);
		
		//On execute chaque ligne
		while (token != NULL){
			printf("\n Execution: %s \n", token);
			
			//On met la commande dans input
			strcpy(input,token);
			
			execcmd();
			
			//On prend la ligne suivante dans le fichier
			token = strtok_r(NULL, "\n", &save);
		}
		close(fd);
	}
	else{
		//Mode interactif: shell
		while(1){
			commandline();
			waitinput();
			execcmd();
		}
		return 0;
	}
	
	
}
