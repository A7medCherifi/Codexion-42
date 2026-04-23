#include "codexion.h"

void *monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			i;

	table = (t_table *)arg;
	while (!table->stop) {
		usleep(1000);
		i = 0;
		while (i < table->args->number_of_coders) {
			pthread_mutex_lock(&table->log_mutex);
			burnout_time = get_time() - table->coders[i].last_compile_start;
			pthread_mutex_unlock(&table->log_mutex);
			if (burnout_time >= table->args->time_to_burnout) {
				pthread_mutex_lock(&table->log_mutex);
				pthread_mutex_lock(&table->dongles->mutex);
				printf("%ld %d burned out\n", get_time() - table->start_time, i + 1);
				table->stop = 1;
				pthread_cond_broadcast(&table->coders->left_dongle->cond);
				pthread_cond_broadcast(&table->coders->right_dongle->cond);
				pthread_mutex_unlock(&table->dongles->mutex);
				pthread_mutex_unlock(&table->log_mutex);
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}
