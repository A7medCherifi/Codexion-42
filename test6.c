#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t		mutex;
pthread_cond_t		cond;

int					counter = 0;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
    pthread_cond_t	cond;
	int				id;
	int				is_available;
	int				released_at;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*dongles;
}	t_coder;

t_dongle 	dongless[2];

void	grap_dongle(t_dongle *dongle, int id)
{
	pthread_mutex_lock(&dongle->mutex);
	while (!dongle->is_available)
	{
		printf("Thread %d waiting for dongle %d ...\n", id, dongle->id);
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dongle->is_available = 0;
	printf("Thread %d grabbed resource %d\n", id, dongle->id);
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle, int id)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	printf("Thread %d released dongle %d\n", id, dongle->id);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	*func(void *arg)
{
	t_coder		*coder;
	t_dongle	*res;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		res = &dongless[0];
	else
		res = &dongless[1];
	grap_dongle(res, coder->id);
	sleep(2);
	release_dongle(res, coder->id);

	return (NULL);
}


int	main()
{
	t_coder		coders[5];
	int			i;

	dongless[0].id = 0;
	dongless[0].is_available = 1;
	dongless[0].released_at = 0;
	pthread_mutex_init(&dongless[0].mutex, NULL);
	pthread_cond_init(&dongless[0].cond, NULL);

	dongless[1].id = 1;
	dongless[1].is_available = 1;
	dongless[1].released_at = 0;
	pthread_mutex_init(&dongless[1].mutex, NULL);
	pthread_cond_init(&dongless[1].cond, NULL);

	i = 0;
	while (i < 4) {
		coders[i].id = i + 1;
		coders[i].dongles = dongless;
		pthread_create(&coders[i].thread, NULL, func, &coders[i]);
		i++;
	}
	i = 0;
	while (i < 4)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	pthread_mutex_destroy(&dongless[0].mutex);
    pthread_cond_destroy(&dongless[0].cond);
    pthread_mutex_destroy(&dongless[1].mutex);
    pthread_cond_destroy(&dongless[1].cond);
}
