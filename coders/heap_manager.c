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

void    take_dongle(t_dongle *dongle, t_request request, t_coder *coder)
{
	if (get_time() >= request.deadline) {
		return ;
	}
    push_request_to_heap(dongle, request, coder->table->args->scheduler);
    pthread_mutex_lock(&coder->table->log_mutex);
    printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
    pthread_mutex_unlock(&coder->table->log_mutex);
	pop_coder_from_heap(dongle);

    dongle->is_available = 0;
}

int    take_both_dongles(t_coder *coder)
{
    t_request    request;

    request.coder_id = coder->id;
    request.requested_at = get_time();
    request.deadline = coder->last_compile_start + coder->table->args->time_to_burnout;

    if (check_for_stop(coder->table)) {
        return (1); }

	take_dongle(coder->left_dongle, request, coder);
	take_dongle(coder->right_dongle, request, coder);

    if (check_for_stop(coder->table)) {
    	return (1); }
	return (0);
}
