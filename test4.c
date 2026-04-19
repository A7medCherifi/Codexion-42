#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t		mutex;
pthread_cond_t		cond;

int					order = 1;

void *func(void *arg)
{
	int		id;

	id = *(int *)arg;
	for (int round = 1; round <= 3; round++) {
		pthread_mutex_lock(&mutex);
		while (id != order)
			pthread_cond_wait(&cond, &mutex);
		printf("Thread %d - round %d\n", id, round);
		if (id == 3)
			order = 1;
		else
			order++;
		// pthread_cond_signal(&cond);
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}

int	main()
{
	pthread_t	thread[3];
	pthread_t	monitor;
	int			i;
	int			id[3];

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	i = 0;
	while (i < 3) {
		id[i] = i + 1;
		pthread_create(&thread[i], NULL, func, &id[i]);
		i++;
	}
	i = 0;
	while (i < 3)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
}