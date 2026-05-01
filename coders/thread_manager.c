#include "codexion.h"


int        request_dongles(t_coder *coder)
{
	int		result;

	result = 0;
	while (1)
	{
		if (check_for_stop(coder->table))
			return (1);
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
		if (coder->left_dongle->is_available && coder->right_dongle->is_available
			&& get_time() - coder->left_dongle->released_at >= coder->table->args->dongle_cooldown
			&& get_time() - coder->right_dongle->released_at >= coder->table->args->dongle_cooldown)
		{
			result = take_both_dongles(coder);
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			if (result)
				return (1);
			break;
		}
		pthread_mutex_unlock(&coder->right_dongle->mutex);
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		usleep(300);
	}
    return (0);
}

int		coder_compiles(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->table->log_mutex);
	current_time = get_time() - coder->table->start_time;
	if (coder->table->stop) {
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1); }
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
	if (coder->table->stop) {
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1); }
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
	if (coder->table->stop) {
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1); }
	printf("%ld %d is refactoring\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	time_sleep(coder->table, coder->table->args->time_to_refactor);
	return (0);
}

void	*thread_manager(void *arg)
{
    t_coder		*coder;

    coder = (t_coder *)arg;
	if (check_for_stop(coder->table))
		return (NULL); 
    while (check_for_compiles(coder))
    {
        if (request_dongles(coder))
            return (NULL);
        if (coder_compiles(coder))
			return (NULL);
		if (release_dongle(coder))
			return (NULL);
        if (coder_debug(coder))
            return (NULL);
        if (coder_refacture(coder))
            return (NULL);
        if (check_for_stop(coder->table))
            return (NULL);
		pthread_mutex_lock(&coder->table->log_mutex);
        coder->compile_count++;
		pthread_mutex_unlock(&coder->table->log_mutex);
    }
	check_for_done_simulation(coder);
    return (NULL);
}
