#include "codexion.h"


int        request_dongles(t_coder *coder)
{
	int		is_valid;

	is_valid = 0;
	if (coder->id % 2 == 0)
		usleep(((coder->table->args->time_to_compile + coder->table->args->dongle_cooldown) / 3) * 100);
	while (1) {
		if (check_for_stop(coder->table)) {
			return (1); }
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
		if (coder->left_dongle->is_available && coder->right_dongle->is_available
			&& get_time() - coder->left_dongle->released_at >= coder->table->args->dongle_cooldown
			&& get_time() - coder->right_dongle->released_at >= coder->table->args->dongle_cooldown) {
			is_valid = take_both_dongles(coder);
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			if (is_valid) {
				return (1);
			}
			break;
		}
		else {
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			usleep(300);
		}
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
	coder->table->done++;
	time_sleep(coder->table, coder->table->args->time_to_compile);
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
	time_sleep(coder->table, coder->table->args->time_to_debug);
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
	time_sleep(coder->table, coder->table->args->time_to_refactor);
	return (0);
}

void	*thread_manager(void *arg)
{
    t_coder		*coder;
    int			number_of_compiles;

    coder = (t_coder *)arg;
	if (check_for_stop(coder->table)) {
		return (NULL);
	} 
    number_of_compiles = coder->table->args->number_of_compiles_required;
	pthread_mutex_lock(&coder->table->log_mutex);
    while (coder->compile_count <= number_of_compiles)
    {
		pthread_mutex_unlock(&coder->table->log_mutex);
        if (request_dongles(coder)) {
            return (NULL); }
        if (coder_compiles(coder)) {
			return (NULL); }
		if (check_for_stop(coder->table)) {
			return (NULL); }
		release_dongle(coder);
        if (coder_debug(coder)) {
            return (NULL); }
        if (coder_refacture(coder)) {
            return (NULL); }
        if (check_for_stop(coder->table)) {
            return (NULL); }
		pthread_mutex_lock(&coder->table->log_mutex);
        coder->compile_count++;
		pthread_mutex_unlock(&coder->table->log_mutex);
    }
	pthread_mutex_unlock(&coder->table->log_mutex);
	pthread_mutex_lock(&coder->table->log_mutex);
	coder->compile_count++;
	if (coder->table->done >= coder->table->args->number_of_coders * coder->table->args->number_of_compiles_required) {
		coder->table->stop = 1;
		// printf("l3zz dkhold\n");
	}
	pthread_mutex_unlock(&coder->table->log_mutex);
    return (NULL);
}
