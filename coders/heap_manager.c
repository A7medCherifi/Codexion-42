#include "codexion.h"

int		has_priority(t_request a, t_request b, int scheduler)
{
	if (scheduler)
		return (a.requested_at < b.requested_at);
	return (a.deadline < b.deadline);
}

void	push_request_to_heap(t_dongle *dongle, t_request request, int scheduler)
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

void	pop_coder_from_heap(t_dongle *dongle)
{
	if (dongle->queue_size == 0)
		return ;
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	dongle->queue_size--;
}


void	check_priority_node(t_dongle *dongle, t_coder *coder, t_request request)
{
	struct	timespec	ts;
	long				cooldown_end;

	push_request_to_heap(dongle, request, coder->table->args->scheduler);
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
	pop_coder_from_heap(dongle);
	dongle->is_available = 0;
}

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request			request;
	
	request.coder_id = coder->id;
	request.requested_at = get_time();
	request.deadline = coder->last_compile_start + coder->table->args->time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	check_priority_node(dongle, coder, request);
	pthread_mutex_unlock(&dongle->mutex);
	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%ld %d has taken a dongle\n", get_time() - coder->table->start_time, coder->id);
	pthread_mutex_unlock(&coder->table->log_mutex);
}
