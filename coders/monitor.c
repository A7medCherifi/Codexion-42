#include "codexion.h"

// void	broadcast(t_table *table)
// {
// 	int i;
	
// 	i = 0;
// 	while (i < table->args->number_of_coders)
// 	{
// 		pthread_mutex_lock(&table->dongles[i].mutex);
// 		pthread_cond_broadcast(&table->dongles[i].cond);
// 		pthread_mutex_unlock(&table->dongles[i].mutex);
// 		i++;
// 	}

// }

void	*monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			compile_valid;
	int			i;

	table = (t_table *)arg;
	while (!check_for_stop(table)) {
		i = 0;
		while (i < table->args->number_of_coders) {
			compile_valid = check_for_compiles(&table->coders[i]);
			pthread_mutex_lock(&table->log_mutex);
			burnout_time = get_time() - table->coders[i].last_compile_start;
			if (burnout_time >= table->args->time_to_burnout && compile_valid) {
				printf("%ld %d burned out\n", get_time() - table->start_time, i + 1);
				table->stop = 1;
				pthread_mutex_unlock(&table->log_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&table->log_mutex);
			i++;
		}
		usleep(300);
	}
	return (NULL);
}
