#include "codexion.h"

int		has_priority(t_request a, t_request b, int scheduler) {
	if (scheduler)
		return (a.requested_at < b.requested_at);
	return (a.deadline < b.deadline);
}

void	push_request_heap(t_dongle *dongle, t_request request, int scheduler)
{
	int			i;
	int			parent;
	t_request	temp;

	dongle->queue[dongle->queue_size] = request;
	i = dongle->queue_size;
	dongle->queue_size++;

	while (i > 0) {
		parent = (i - 1) / 2;
		if (has_priority(dongle->queue[i], dongle->queue[parent], scheduler)) {
			temp = dongle->queue[parent];
			dongle->queue[parent] = dongle->queue[i];
			dongle->queue[i] = temp;
			i = parent;
		}
		else
			break;
	}
}

void	pop_coder_heap(t_dongle *dongle)
{
	if (dongle->queue_size == 0)
		return ;
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	dongle->queue_size--;
}

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	struct	timespec	ts;
	t_request			request;
	long				cooldown_end;

	request.coder_id = coder->id;
	request.requested_at = get_time();
	request.deadline = coder->last_compile_start + coder->table->args->time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	push_request_heap(dongle, request, coder->table->args->scheduler);

	while (dongle->queue[0].coder_id != coder->id
		|| !dongle->is_available
		|| get_time() - dongle->released_at < coder->table->args->dongle_cooldown)
	{
		if (coder->table->stop) {
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		if (dongle->queue[0].coder_id == coder->id && dongle->is_available) {
			cooldown_end = dongle->released_at + coder->table->args->dongle_cooldown;
			ts.tv_sec = cooldown_end / 1000;
			ts.tv_nsec = (cooldown_end % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pop_coder_heap(dongle);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);

	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%ld %d has taken a dongle\n", get_time() - coder->table->start_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->released_at = get_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	*func(void *arg)
{
	t_coder *coder;
	int current_time;

	coder = (t_coder *)arg;
	coder->compile_count = 1;
	while (coder->compile_count <= coder->table->args->number_of_compiles_required && !coder->table->stop)
	{
		if (coder->table->stop)
			return (NULL);

		if (coder->id % 2 == 1) {
			take_dongle(coder->right_dongle, coder);
			take_dongle(coder->left_dongle, coder);
		} else {
			take_dongle(coder->left_dongle, coder);
			take_dongle(coder->right_dongle, coder);
		}

		pthread_mutex_lock(&coder->table->log_mutex);
		coder->last_compile_start = get_time();
		pthread_mutex_unlock(&coder->table->log_mutex);
		current_time = get_time() - coder->table->start_time;
		pthread_mutex_lock(&coder->table->log_mutex);
		printf("%d %d is compiling\n", current_time, coder->id);
		pthread_mutex_unlock(&coder->table->log_mutex);
		usleep(coder->table->args->time_to_compile * 1000);
		if (coder->table->stop)
			return (NULL);

		release_dongle(coder->right_dongle);
		release_dongle(coder->left_dongle);

		current_time = get_time() - coder->table->start_time;
		pthread_mutex_lock(&coder->table->log_mutex);
		printf("%d %d is debugging\n", current_time, coder->id);
		pthread_mutex_unlock(&coder->table->log_mutex);
		usleep(coder->table->args->time_to_debug * 1000);
		if (coder->table->stop)
			return (NULL);

		current_time = get_time() - coder->table->start_time;
		pthread_mutex_lock(&coder->table->log_mutex);
		printf("%d %d is refactoring\n", current_time, coder->id);
		pthread_mutex_unlock(&coder->table->log_mutex);
		usleep(coder->table->args->time_to_refactor * 1000);
		if (coder->table->stop)
			return (NULL);
		coder->compile_count++;
	}

	return (NULL);
}

void *burnout_monitor(void *arg)
{
	t_table		*table;
	int			burnout_time;
	int			i;

	table = (t_table *)arg;
	while (!table->stop)
	{
		usleep(1000);
		i = 1;
		while (i <= table->args->number_of_coders)
		{
			pthread_mutex_lock(&table->log_mutex);
			burnout_time = get_time() - table->coders[i].last_compile_start;
			pthread_mutex_unlock(&table->log_mutex);
			if (burnout_time >= table->args->time_to_burnout)
			{
				pthread_mutex_lock(&table->log_mutex);
				printf("%ld %d burned out\n", get_time() - table->start_time, i);
				pthread_mutex_lock(&table->stop_mutex);
				table->stop = 1;
				pthread_mutex_unlock(&table->stop_mutex);
				pthread_mutex_unlock(&table->log_mutex);
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}

int main(int argc, char **argv)
{
	t_table table;
	int i;

	if (parsing(&table.args, argc, argv))
		return (1);

	pthread_mutex_init(&table.log_mutex, NULL);
	table.coders = malloc(sizeof(t_coder) * (table.args->number_of_coders + 1));
	if (!table.coders)
		return (1);

	table.dongles = malloc(sizeof(t_dongle) * (table.args->number_of_coders + 1));
	if (!table.dongles)
		return (1);

	i = 0;
	while (i < table.args->number_of_coders)
	{
		table.dongles[i].queue_size = 0;
		table.dongles[i].id = i;
		table.dongles[i].is_available = 1;
		table.dongles[i].released_at = 0;
		pthread_mutex_init(&table.dongles[i].mutex, NULL);
		pthread_cond_init(&table.dongles[i].cond, NULL);
		i++;
	}

	pthread_create(&table.monitor, NULL, burnout_monitor, &table);

	i = 1;
	table.stop = 0;
	table.start_time = get_time();
	while (i <= table.args->number_of_coders)
	{
		if (table.stop)
		{
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
	if (table.stop)
		return (1);
	i = 1;
	while (i <= table.args->number_of_coders)
	{
		pthread_join(table.coders[i].thread, NULL);
		i++;
	}
	table.stop = 1;
	pthread_join(table.monitor, NULL);
	return (0);
}
