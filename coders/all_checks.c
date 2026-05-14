#include "codexion.h"

int	check_for_stop(t_table *table)
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

int	check_for_compiles(t_coder *coder)
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
	int		compiles_required;

	pthread_mutex_lock(&coder->table->log_mutex);
	number_of_coders = coder->table->args->number_of_coders;
	compiles_required = coder->table->args->number_of_compiles_required;
	if (coder->table->done >= number_of_coders * compiles_required)
	{
		coder->table->stop = 1;
	}
	pthread_mutex_unlock(&coder->table->log_mutex);
}

int	check_can_take_dongle(t_coder *coder)
{
	long	left_released;
	long	right_released;
	long	left_cooldown;
	long	right_cooldown;

	left_released = coder->left_dongle->released_at;
	right_released = coder->right_dongle->released_at;
	left_cooldown = coder->table->args->dongle_cooldown;
	right_cooldown = coder->table->args->dongle_cooldown;
	if (coder->id == coder->left_dongle->queue[0].coder_id
		&& coder->id == coder->right_dongle->queue[0].coder_id
		&& coder->left_dongle->is_available && coder->right_dongle->is_available
		&& get_time() - left_released >= left_cooldown
		&& get_time() - right_released >= right_cooldown)
	{
		return (1);
	}
	return (0);
}
int	monitor_check(t_table *table, int i)
{
	pthread_mutex_lock(&table->log_mutex);
	if (i < table->args->number_of_coders
		&& !table->stop && table->start_simulation)
	{
		pthread_mutex_unlock(&table->log_mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->log_mutex);
	return (0);
}
