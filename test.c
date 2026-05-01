#include "codexion.h"


void	*func(void *arg)
{
	t_coder			*coder;
	int				current_time;

	coder = (t_coder *)arg;
	coder->compile_count = 1;
	while (coder->compile_count <= coder->table->args->number_of_compiles_required && !coder->table->stop)
	{
		if (coder->table->stop)
			return (NULL);
		printf("hhh\n");
		coder->last_compile_start = get_time();
		current_time = get_time() - coder->table->start_time;
		if (coder->id % 2 == 0) {
			coder->left_dongle->is_available = 1;
			pthread_mutex_lock(&coder->left_dongle->mutex);
			printf("%d %d has taken a dongle\n", current_time, coder->id);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			coder->left_dongle->released_at = current_time + coder->table->args->dongle_cooldown;
		}
		else if (coder->id % 2 == 1) {
			coder->right_dongle->is_available = 1;
			pthread_mutex_lock(&coder->right_dongle->mutex);
			printf("%d %d has taken a dongle\n", current_time, coder->id);
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			coder->right_dongle->released_at = current_time + coder->table->args->dongle_cooldown;
		}
		if (coder->left_dongle && coder->right_dongle) {
			current_time = get_time() - coder->table->start_time;
			pthread_mutex_lock(&coder->table->log_mutex);
			printf("%d %d is compiling\n", current_time, coder->id);
			pthread_mutex_unlock(&coder->table->log_mutex);
			usleep(coder->table->args->time_to_compile * 1000);
			if (coder->table->stop) return (NULL);

			current_time = get_time() - coder->table->start_time;
			pthread_mutex_lock(&coder->table->log_mutex);
			printf("%d %d is debugging\n", current_time, coder->id);
			pthread_mutex_unlock(&coder->table->log_mutex);
			usleep(coder->table->args->time_to_debug * 1000);
			if (coder->table->stop) return (NULL);

			current_time = get_time() - coder->table->start_time;
			pthread_mutex_lock(&coder->table->log_mutex);
			printf("%d %d is refactoring\n", current_time, coder->id);
			pthread_mutex_unlock(&coder->table->log_mutex);
			usleep(coder->table->args->time_to_refactor * 1000);
			if (coder->table->stop) return (NULL);

			coder->compile_count++;
		}
		else
			printf("still waiting...\n");
	}
	
	return (NULL);
}

void	*burnout_monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			compile_times;
	int			i;
	int			j;

	table = (t_table *)arg;
	compile_times = table->args->number_of_compiles_required;
	j = 0;
	while (j < compile_times)
	{
		usleep(1000);
		i = 1;
		while (i <= table->args->number_of_coders)
		{
			burnout_time = get_time() - table->coders[i].last_compile_start;
			if (burnout_time >= table->args->time_to_burnout)
			{
				pthread_mutex_lock(&table->log_mutex);
				printf("%d %d burned out\n", get_time() - table->start_time, i);
				table->stop = 1;
				pthread_mutex_unlock(&table->log_mutex);
				return (NULL);
			}
			i++;
		}
		j++;
	}

	return (NULL);
}

int main(int argc, char **argv)
{
	t_table		table;
	int			i;
	
	if (parsing(&table.args, argc, argv))
		return (1);
	pthread_mutex_init(&table.log_mutex, NULL);
	table.coders = malloc(sizeof(t_coder) * (table.args->number_of_coders + 1));
	if (!table.coders)
		return (1);

	table.dongles = malloc(sizeof(t_coder) * (table.args->number_of_coders));
	if (!table.dongles)
		return (1);
	table.stop = 0;
	pthread_create(&table.monitor, NULL, burnout_monitor, &table);
	table.start_time = get_time();
	i = 0;
	while (i < table.args->number_of_coders)
	{
		table.dongles[i].id = i;
		table.dongles[i].is_available = 0;
		table.dongles[i].released_at = 0;
		pthread_mutex_init(&table.dongles->mutex, NULL);
		pthread_cond_init(&table.dongles->cond, NULL);
		i++;
	}
	i = 1;
	while (i <= table.args->number_of_coders)
	{
		if (table.stop) {
			return (1);
		}
		table.coders[i].id = i;
		table.coders[i].table = &table;
		table.coders[i].last_compile_start = table.start_time;
		table.coders[i].left_dongle = &table.dongles[i - 1];
		table.coders[i].right_dongle = &table.dongles[i % table.args->number_of_coders];
		pthread_create(&table.coders[i].thread, NULL, func, &table.coders[i]);
		i++;
	}
	i = 1;
	if (table.stop) {
		return (1);
	}
	while (i <= table.args->number_of_coders)
	{
		pthread_join(table.coders[i].thread, NULL);
		i++;
	}
	pthread_join(table.monitor, NULL);
}
