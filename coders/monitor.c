#include "codexion.h"

void	*monitor(void *arg)
{
	t_table		*table;
	int			i;

	table = (t_table *)arg;
	while (!check_for_stop(table))
	{
		i = 0;
		while (monitor_check(table, i))
		{
			pthread_mutex_lock(&table->mutex);
			if (check_for_burnout(table, i))
			{
				print_burnout(table, i);
				table->stop = 1;
				pthread_mutex_unlock(&table->mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&table->mutex);
			i++;
		}
		usleep(300);
	}
	return (NULL);
}
