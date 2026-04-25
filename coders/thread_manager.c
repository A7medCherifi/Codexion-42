#include "codexion.h"


int        request_dongles(t_coder *coder)
{
    if (check_for_stop(coder->table))
        return (1);
    if (coder->id % 2 == 1) {
        take_dongle(coder->right_dongle, coder);
        take_dongle(coder->left_dongle, coder);
    } else {
        take_dongle(coder->right_dongle, coder);
        take_dongle(coder->left_dongle, coder);
    }
    return (0);
}

int		coder_compiles(t_coder *coder)
{
	int		current_time;

	if (check_for_stop(coder->table))
		return (1);
	current_time = get_time() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->log_mutex);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->table->log_mutex);
	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%d %d is compiling\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	usleep(coder->table->args->time_to_compile * 1000);
	return (0);
}

int		coder_debug(t_coder *coder)
{
	int		current_time;

	if (check_for_stop(coder->table))
		return (1);
	current_time = get_time() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%d %d is debugging\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	usleep(coder->table->args->time_to_debug * 1000);
	return (0);
}
int		coder_refacture(t_coder *coder)
{
	int		current_time;

	if (check_for_stop(coder->table))
		return (1);
	current_time = get_time() - coder->table->start_time;
	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%d %d is refactoring\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
	usleep(coder->table->args->time_to_refactor * 1000);
	return (0);
}

void    *thread_manager(void *arg)
{
    t_coder     *coder;
    int            number_of_compiles;

    coder = (t_coder *)arg;
    number_of_compiles = coder->table->args->number_of_compiles_required;
    coder->compile_count = 1;
    while (coder->compile_count <= number_of_compiles)
    {
        if (request_dongles(coder)) {
            return (NULL);
        }
        if (coder_compiles(coder)) {
			return (NULL);
        }
		release_dongle(coder->right_dongle);
		release_dongle(coder->left_dongle);
        if (coder_debug(coder)) {
            return (NULL);
        }
        if (check_for_stop(coder->table)) {
            return (NULL);
        }
        if (coder_refacture(coder)) {
            return (NULL);
        }
        if (check_for_stop(coder->table)) {
            return (NULL);
        }
        coder->compile_count++;
    }
    return (NULL);
}
