#include "codexion.h"

long get_time()
{
    struct	timeval		tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// struct timespec get_time_spec(long	time)
// {
// 	struct	timespec	ts;

// 	clock_gettime(CLOCK_REALTIME, &ts);
// 	ts.tv_nsec += time * 1000000;
// 	while (ts.tv_nsec >= 1000000000)
// 	{
// 		ts.tv_sec += 1;
// 		ts.tv_nsec -= 1000000000;
// 	}
// 	return (ts);
// }

int		check_for_stop(t_table *table)
{
	pthread_mutex_lock(&table->log_mutex);
	if (table->stop)
	{
		pthread_mutex_unlock(&table->log_mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->log_mutex);
	return (0);
}

int		check_for_compiles(t_coder *coder)
{
	int		number_of_compiles;

	pthread_mutex_lock(&coder->table->log_mutex);
	number_of_compiles = coder->table->args->number_of_compiles_required;
	if (coder->compile_count <= number_of_compiles && !coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}

void	check_for_done_simulation(t_coder *coder)
{
	int		number_of_coders;
	int		number_of_compiles_required;

	pthread_mutex_lock(&coder->table->log_mutex);
	number_of_coders = coder->table->args->number_of_coders;
	number_of_compiles_required = coder->table->args->number_of_compiles_required;
	if (coder->table->done >= number_of_coders * number_of_compiles_required)
	{
		coder->table->stop = 1;
	}
	pthread_mutex_unlock(&coder->table->log_mutex);
}

void	time_sleep(t_table *table, int	time)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time) 
	{
		if (check_for_stop(table))
		{
			return ;
		}
		usleep(500);
	}
}
