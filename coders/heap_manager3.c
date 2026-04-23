#include "codexion.h"


int		get_priority(t_request child, t_request parent, int scheduler)
{
	if (scheduler)
		return (child.requested_at < parent.requested_at);
	return (child.deadline < parent.deadline);
}

void	push_request_to_heap(t_dongle *dongle, t_request request, scheduler)
{
	t_request	temp;
	int			i;
	int			parent;

	dongle->queue[queue_size] = request->coder_id;
	i = queue_size;
	queue_size++;

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

void	check_priority_node(t_dongle *dongle, t_request request, int scheduler)
{
	push_request_to_heap()
}

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request	request;

	request.coder_id = coder->id;
	request.requested_at = get_time();
	request.deadline = coder->last_compile_start + coder->table->arg->time_to_burnout;

	pthread_mutex_lock(dongle->mutex);
	check_priority_node(donge, request, coder->table->arg->scheduler);
	printf("%d %d is taken dongle\n", get_time() - coder->table->start_time, coder_id);
	pthread_mutex_unlock(dongle->mutex);
}
