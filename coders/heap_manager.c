#include "codexion.h"


int		get_priority(t_request child, t_request parent, int scheduler)
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
	if (dongle->queue_size > 0)
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
	while (1)
	{
		if (check_for_stop(coder->table))
		{
			return (1);
		}
		pthread_mutex_lock(&coder->table->log_mutex);
		if (coder->id == coder->left_dongle->queue[0].coder_id
			&& coder->id == coder->right_dongle->queue[0].coder_id
			&& coder->left_dongle->is_available && coder->right_dongle->is_available
			&& get_time() - coder->left_dongle->released_at >= coder->table->args->dongle_cooldown
			&& get_time() - coder->right_dongle->released_at >= coder->table->args->dongle_cooldown)
		{
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
			coder->left_dongle->is_available = 0;
			coder->right_dongle->is_available = 0;
			coder->last_compile_start = get_time();
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

// int		take_dongle(t_dongle *dongle, t_coder *coder)
// {
// 	while (1) {
// 		pthread_mutex_lock(&coder->table->log_mutex);
// 		if (coder->table->stop || get_time() - dongle->released_at < coder->table->args->dongle_cooldown)
// 		{
// 			pthread_mutex_unlock(&coder->table->log_mutex);
// 			return (1);
// 		}
// 		if (coder->id == dongle->queue[0].coder_id) {
// 			printf("%ld %d is taken dongle\n", get_time() - coder->table->start_time, coder->id);
// 			dongle->is_available = 0;
// 			pthread_mutex_unlock(&coder->table->log_mutex);
// 			pop_and_bubble_down(coder, dongle, coder->table->args->scheduler);
// 			break;
// 		}
// 		pthread_mutex_unlock(&coder->table->log_mutex);
// 		usleep(300);
// 	}
// 	return (0);
// }

int    take_both_dongles(t_coder *coder)
{
    if (check_for_stop(coder->table)) {
        return (1); }

	if (take_dongle(coder))
	{
		return (1);
	}

    if (check_for_stop(coder->table)) {
    	return (1); }

	// pthread_mutex_lock(&coder->table->log_mutex);
	
	// pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}