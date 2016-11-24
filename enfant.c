#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>


int main()
{
	int shmid;
	int *shm;

	//Dans le fils, il faut rajouter les autorisation d'accès
	if((shmid = shmget(1111, 0, 0666)) < 0)
	{
		printf("Impossible de trouver la mémoire partagée.\n");
		exit(1);
	}

	if((shm=shmat(shmid, NULL,0)) == (int*) -1)
	{
		printf("Impossible d'attacher la mémoire partagée.\n");
		exit(1);
	}

	int value = 0;
	int i;
	for(i = 0; i < 5; i++)
	{
		value += shm[i];
	}

	printf("La somme vaut %d", value);


	return 0;
}
