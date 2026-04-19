#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t		mutex;
pthread_cond_t		cond;

int			job = 1;

void *func()
{
	int		i;

	i = 1;
	while (i < 6) {
		usleep(200000);
		if (job == i) {
			printf("no job received, waiting...\n");
		}
		else {
			printf("worker: got job %d\n", i);
			i++;
		}
	}
	printf("Done\n");
	return (NULL);
}

int	main()
{
	pthread_t	thread;
	pthread_t	monitor;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_create(&thread, NULL, func, NULL);
	usleep(300000);
	job++;
	usleep(300000);
	job++;
	usleep(300000);
	job++;
	usleep(300000);
	job++;
	usleep(300000);
	job++;
	pthread_join(thread, NULL);
	
}
