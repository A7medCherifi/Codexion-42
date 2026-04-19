#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t		mutex;
pthread_cond_t		cond;

int					counter = 0;

void *func()
{
	for (int i = 0; i < 100; i++) {
		pthread_mutex_lock(&mutex);
		counter++;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}

void *monitor_func()
{
	int		reached = 0;

	pthread_mutex_lock(&mutex);
	while (counter < 400) {
		while (counter < reached + 50) {
			pthread_cond_wait(&cond, &mutex);
		}
		reached += 50;
		printf("counter reached %d\n", reached);
	}
	pthread_mutex_unlock(&mutex);
	printf("Done\n");
	return (NULL);
}

int	main()
{
	pthread_t	thread[4];
	pthread_t	monitor;
	int			i;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	i = 0;
	while (i < 4) {
		pthread_create(&thread[i], NULL, func, NULL);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_func, NULL);
	i = 0;
	while (i < 4)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}
