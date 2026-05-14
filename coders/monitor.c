#include "codexion.h"

void	*monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			i;

	table = (t_table *)arg;
	while (!check_for_stop(table))
	{
		i = 0;
		while (monitor_check(table, i))
		{
			pthread_mutex_lock(&table->log_mutex);
			burnout_time = get_time() - table->coders[i].last_compile_start;
			if (burnout_time >= table->args->time_to_burnout)
			{
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
