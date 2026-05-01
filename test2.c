#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t	mutex;
pthread_cond_t	cond;

int				go = 0;

void *func(void *arg)
{
	int		id;

	id = *(int *)arg;
	pthread_mutex_lock(&mutex);
	while (!go)
		pthread_cond_wait(&cond, &mutex);
	printf("Thread %d passed through\n", id);
	pthread_mutex_unlock(&mutex);
	return (NULL);
}

void *monitor_func()
{
	pthread_mutex_lock(&mutex);
	printf("Main: gate is opening...\n");
	go = 1;
	sleep(2);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	return (NULL);
}

int	main()
{
	int			i;
	int			id[5];

	pthread_t	threads[5];
	pthread_t	monitor;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	i = 0;
	while (i < 5) {
		id[i] = i;
		pthread_create(&threads[i], NULL, func, &id[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_func, NULL);
	i = 0;
	while (i < 5) {
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}