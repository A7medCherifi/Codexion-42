#include "codexion.h"


int	get_priority(t_request child, t_request parent, int scheduler)
{
	if (scheduler)
		return (child.requested_at < parent.requested_at);
	return (child.deadline < parent.deadline);
}

void	push_and_bubble_up(t_coder *coder, t_dongle *dongle, t_request request)
{
	t_request	temp;
	int			i;
	int			parent;
	int			scheduler;

	dongle->queue[dongle->queue_size] = request;
	i = dongle->queue_size;
	dongle->queue_size++;
	scheduler = coder->table->args->scheduler;
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

void	pop_and_bubble_down(t_dongle *dongle, int scheduler)
{
	int		smal;
	int		right;
	int		left;
	int		i;

	if (pop_from_heap(dongle))
		return ;
	i = 0;
	while (1)
	{
		left = (i * 2) + 1;
		right = (i * 2) + 2;
		smal = i;
		if (left < dongle->queue_size
			&& get_priority(dongle->queue[left], dongle->queue[smal], scheduler))
			smal = left;
		if (right < dongle->queue_size
			&& get_priority(dongle->queue[right], dongle->queue[smal], scheduler))
			smal = right;
		if (smal == i)
			break;
		swap_nodes(&dongle->queue[i], &dongle->queue[smal]);
		i = smal;
	}
}

int	take_dongle(t_coder *coder)
{
	while (1)
	{
		if (check_for_stop(coder->table))
			return (1);
		pthread_mutex_lock(&coder->table->log_mutex);
		if (check_can_take_dongle(coder))
		{
			if (coder->table->stop)
			{
				pthread_mutex_unlock(&coder->table->log_mutex);
				return (1);
			}
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			coder->left_dongle->is_available = 0;
			coder->right_dongle->is_available = 0;
			pop_and_bubble_down(coder->left_dongle, coder->table->args->scheduler);
			pop_and_bubble_down(coder->right_dongle, coder->table->args->scheduler);
			pthread_mutex_unlock(&coder->table->log_mutex);
			break;
		}
		pthread_mutex_unlock(&coder->table->log_mutex);
		usleep(300);
	}
	return (0);
}

int	take_both_dongles(t_coder *coder)
{
    if (check_for_stop(coder->table)) 
	{
        return (1);
	}
	if (take_dongle(coder)) {
		return (1); }
    if (check_for_stop(coder->table))
	{
    	return (1);
	}
	return (0);
}
