#include "codexion.h"


int		get_priority(t_request child, t_request parent, int scheduler)
{
	if (scheduler)
		return (child.requested_at < parent.requested_at);
	return (child.deadline < parent.deadline);
}

void	push_and_bubble_up(t_dongle *dongle, t_request request, int scheduler)
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
			// swap_nodes(&dongle->queue[parent], &dongle->queue[i]);
			temp = dongle->queue[parent];
			dongle->queue[parent] = dongle->queue[i];
			dongle->queue[i] = temp;
			i = parent;
		}
		else {
			break;
		}
	}
}

void	pop_and_bubble_down(t_dongle *dongle, int scheduler)
{
	int		small;
	int		right;
	int		left;
	int		i;

	if (dongle->queue_size == 0)
		return ;
	dongle->queue[0] = dongle->queue[dongle->queue_size - 1];
	dongle->queue_size--;
	i = 0;
	while (1)
	{
		left = (i * 2) + 1;
		right = (i * 2) + 2;
		small = i;
		if (left < dongle->queue_size
			&& get_priority(dongle->queue[left], dongle->queue[small], scheduler))
			small = left;
		if (right < dongle->queue_size
			&& get_priority(dongle->queue[right], dongle->queue[small], scheduler))
			small = right;
		if (small == i)
			break;
		swap_nodes(&dongle->queue[i], &dongle->queue[small]);
		i = small;
	}
}

int		take_dongle(t_coder *coder)
{
	// if (get_time() >= request.deadline)
	// {
	// 	return (1);
	// }
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	while (1)
	{
		pthread_mutex_lock(&coder->table->log_mutex);
		if (coder->table->stop) {
			pthread_mutex_unlock(&coder->table->log_mutex);
			return (1);
		}
		if (coder->id == left->queue[0].coder_id && left->is_available
			 && coder->id == right->queue[0].coder_id && right->is_available)
		{
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			pop_and_bubble_down(coder->left_dongle, coder->table->args->scheduler);
			pop_and_bubble_down(coder->right_dongle, coder->table->args->scheduler);
			coder->left_dongle->is_available = 0;
			coder->right_dongle->is_available = 0;
			pthread_mutex_unlock(&coder->table->log_mutex);
			break;
		}
		else {
			pthread_mutex_unlock(&coder->table->log_mutex);
			usleep(300);
		}
	}
	return (0);
}

int    take_both_dongles(t_coder *coder)
{
    // t_request    request;

    // request.coder_id = coder->id;
    // request.requested_at = get_time();
	// pthread_mutex_lock(&coder->table->log_mutex);
    // request.deadline = coder->last_compile_start + coder->table->args->time_to_burnout;
	// pthread_mutex_unlock(&coder->table->log_mutex);

    if (check_for_stop(coder->table)) {
        return (1); }

	if (take_dongle(coder)) {
		return (1);
	}

    if (check_for_stop(coder->table)) {
    	return (1); }

	pthread_mutex_lock(&coder->table->log_mutex);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}
