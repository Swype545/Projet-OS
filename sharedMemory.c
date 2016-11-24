#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
	int shmid;
	int *shm;

	//On crée une mémoire partagée de longueur = 5 ints
	if ((shmid = shmget (1111, 5 * sizeof(int),IPC_CREAT | 0666)) < 0)
	{
		printf("Impossible de créer la mémoire partagée. \n");
		exit(1);
	}

	//L'adresse de cette mémoire partagée est mise dans le pointeur shm
	if ((shm = shmat (shmid, NULL, 0)) == (int*) -1)
	{
		printf("Impossible d'attacher la mémoire partagée.\n");
		exit(1);
	}

	shm[0]=5;
	shm[1]=6;
	shm[2]=4;
	shm[3]=7;
	shm[4]=3;

	pid_t pid = fork();
	if (pid < 0) //Erreur
	{
		printf("Une erreur s'est produite.\n");
	}
	else if(pid == 0) //Dans le fils
	{
		execlp("/home/student/enfant","enfant", NULL);
	}
	else //Dans le parent
	{
		printf("Je suis dans le parent. \n");
		wait(NULL);

		printf("Mon fils a fini. \n");
		if ((shmctl (shmid, IPC_RMID, 0)) == -1)
		{
			printf("Impossible de libérer la mémoire partagée.\n");
			exit(1);
		}
	}

	return 0;
}
