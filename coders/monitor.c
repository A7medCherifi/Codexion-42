#include "codexion.h"

void	broadcast(t_table *table)
{
	int i;
	
	i = 0;
	while (i < table->args->number_of_coders)
	{
		pthread_mutex_lock(&table->dongles[i].mutex);
		pthread_cond_broadcast(&table->dongles[i].cond);
		pthread_mutex_unlock(&table->dongles[i].mutex);
		i++;
	}

}

void	*monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			i;

	table = (t_table *)arg;
	while (!check_for_stop(table) && table->done < (table->args->number_of_coders * table->args->number_of_compiles_required)) {
		usleep(300);
		i = 0;
		while (i < table->args->number_of_coders) {
			pthread_mutex_lock(&table->log_mutex);
			burnout_time = get_time() - table->coders[i].last_compile_start;
			pthread_mutex_unlock(&table->log_mutex);
			if (burnout_time >= table->args->time_to_burnout && !check_for_stop(table)) {
				pthread_mutex_lock(&table->log_mutex);
				printf("%ld %d burned out\n", get_time() - table->start_time, i + 1);
				table->stop = 1;
				pthread_mutex_unlock(&table->log_mutex);
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}
