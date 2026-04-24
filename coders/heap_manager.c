#include "codexion.h"


int		get_priority(t_request child, t_request parent, int scheduler)
{
	if (scheduler)
		return (child.requested_at < parent.requested_at);
	return (child.deadline < parent.deadline);
}

void	push_request_to_heap(t_dongle *dongle, t_request request, int scheduler)
{
	t_request	temp;
	int			i;
	int			parent;

	dongle->queue[dongle->queue_size] = request;
	i = dongle->queue_size;
	dongle->queue_size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (get_priority(dongle->queue[i], dongle->queue[parent], scheduler))
		{
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
	//if (dongle->queue->coder_id == dongle->queue[0].coder_id)
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	dongle->queue_size--;
}

void	check_priority_node(t_request request, t_coder *coder)
{
	struct timespec	ts;

	push_request_to_heap(coder->left_dongle, request, coder->table->args->scheduler);
	push_request_to_heap(coder->right_dongle, request, coder->table->args->scheduler);
	while (
		coder->left_dongle->is_available && coder->right_dongle->is_available
		&& coder->left_dongle->queue[0].coder_id == coder->id
		&& coder->right_dongle->queue[0].coder_id == coder->id
		&& get_time() - coder->left_dongle->released_at >= coder->table->args->dongle_cooldown 
		&& get_time() - coder->right_dongle->released_at >= coder->table->args->dongle_cooldown 
	)
	{
		if (coder->table->stop) {
			pop_coder_from_heap(coder->left_dongle);
			pop_coder_from_heap(coder->right_dongle);
			return ;
		}
		ts = get_time_spec(coder->table->args->dongle_cooldown);
		pthread_cond_timedwait(&coder->table->dongles->cond, &coder->table->dongles->mutex, &ts);
	}
	pop_coder_from_heap(coder->left_dongle);
	pop_coder_from_heap(coder->right_dongle);
	coder->left_dongle->is_available = 0;
	coder->right_dongle->is_available = 0;
}

void	take_both_dongles(t_coder *coder)
{
	t_request	request;

	request.coder_id = coder->id;
	request.requested_at = get_time();
	request.deadline = coder->last_compile_start + coder->table->args->time_to_burnout;

	if (check_for_stop(coder->table))
		return ;
	pthread_mutex_lock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	check_priority_node(request, coder);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	if (check_for_stop(coder->table))
		return ;
	pthread_mutex_lock(&coder->table->log_mutex);
	printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
	coder->dongles_i_have++;
	pthread_mutex_unlock(&coder->table->log_mutex);
}
