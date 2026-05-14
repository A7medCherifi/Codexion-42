#include "codexion.h"

int		coder_compiles(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->table->log_mutex);
	coder->last_compile_start = get_time();
	current_time = get_time() - coder->table->start_time;
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1);
	}
	printf("%ld %d is compiling\n", current_time, coder->id);
	coder->table->done++;
	pthread_mutex_unlock(&coder->table->log_mutex);
	time_sleep(coder->table, coder->table->args->time_to_compile);
	return (0);
}

int		coder_debug(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	current_time = get_time() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->log_mutex);
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1);
	}
	printf("%ld %d is debugging\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	time_sleep(coder->table, coder->table->args->time_to_debug);
	return (0);
}
int		coder_refacture(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	current_time = get_time() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->log_mutex);
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1);
	}
	printf("%ld %d is refactoring\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	time_sleep(coder->table, coder->table->args->time_to_refactor);
	return (0);
}

int		threads_processing(t_coder *coder)
{
	if (request_dongles(coder))
        return (1);
	if (coder_compiles(coder))
		return (1);
	if (release_dongle(coder))
		return (1);
	if (coder_debug(coder))
		return (1);
	if (coder_refacture(coder))
		return (1);
	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->table->log_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}

void	*thread_manager(void *arg)
{
    t_coder		*coder;

    coder = (t_coder *)arg;
	while (1)
	{
		if (check_if_start_simulation(coder))
			break;
		usleep(100);
	}
	if (check_for_stop(coder->table))
		return (NULL); 
	if (coder->id % 2 == 0)
	{
		time_sleep(coder->table, coder->table->args->time_to_compile + coder->table->args->dongle_cooldown / 2);
	}
    while (check_for_compiles(coder))
	{
		if (threads_processing(coder))
			return (NULL);
	}
	check_for_done_simulation(coder);
	return (NULL);
}
