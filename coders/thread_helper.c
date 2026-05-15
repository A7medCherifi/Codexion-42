#include "codexion.h"

int	check_if_start_simulation(t_coder *coder)
{
	pthread_mutex_lock(&coder->table->log_mutex);
	if (coder->table->start_simulation)
	{
		pthread_mutex_unlock(&coder->table->log_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}

int	pop_from_heap(t_dongle *dongle)
{
	if (dongle->queue_size == 0)
		return (1);
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	if (dongle->queue_size > 0)
		dongle->queue_size--;
	return (0);
}

int	send_request_to_queue(t_coder *coder, t_dongle *dongle)
{
	t_request	request;
	int			time_to_burnout;

	if (check_for_stop(coder->table))
		return (1);
	if (!dongle)
	{
		time_sleep(coder->table, coder->table->args->time_to_burnout * 1000);
		return (1);
	}
	time_to_burnout = coder->table->args->time_to_burnout;
	request.coder_id = coder->id;
	request.requested_at = get_time();
	pthread_mutex_lock(&coder->table->log_mutex);
	request.compiles = coder->compile_count;
	request.deadline = coder->last_compile_start + time_to_burnout;
	push_and_bubble_up(coder, dongle, request);
	pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}

int	request_dongles(t_coder *coder)
{
	if (send_request_to_queue(coder, coder->left_dongle)
		|| send_request_to_queue(coder, coder->right_dongle))
	{
		return (1);
	}
	if (check_for_stop(coder->table))
	{
		return (1);
	}
	return (take_dongles(coder));
}

int	create_threads(t_table *table)
{
	int		i;

	if (create_coders(table))
	{
		pthread_mutex_lock(&table->log_mutex);
		table->stop = 1;
		pthread_mutex_unlock(&table->log_mutex);
		return (1);
	}
	i = 0;
	while (i < table->args->number_of_coders)
	{
		pthread_join(table->coders[i].thread, NULL);
		i++;
	}
	return (0);
}
