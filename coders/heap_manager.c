/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_manager.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:36:02 by acherifi          #+#    #+#             */
/*   Updated: 2026/05/20 10:33:20 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	get_priority(t_request child, t_request parent, int scheduler)
{
	if (scheduler)
		return (child.requested_at < parent.requested_at);
	else
	{
		if (child.deadline < parent.deadline)
			return (1);
		if (child.deadline > parent.deadline)
			return (0);
		if (child.compiles < parent.compiles)
			return (1);
		if (child.compiles > parent.compiles)
			return (0);
		if (child.coder_id < parent.coder_id)
			return (1);
	}
	return (0);
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
			return ;
	}
}

void	pop_and_bubble_down(t_dongle *dongle, int scheduler)
{
	int		smal;
	int		left;
	int		i;

	if (pop_from_heap(dongle))
		return ;
	i = 0;
	while (1)
	{
		left = (i * 2) + 1;
		smal = i;
		if (left < dongle->queue_size
			&& get_priority(dongle->queue[left], dongle->queue[smal],
				scheduler))
			smal = left;
		if ((left + 1) < dongle->queue_size
			&& get_priority(dongle->queue[left + 1], dongle->queue[smal],
				scheduler))
			smal = left + 1;
		if (smal == i)
			break ;
		swap_nodes(&dongle->queue[i], &dongle->queue[smal]);
		i = smal;
	}
}

int	take_both_dongles(t_coder *coder)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		pthread_mutex_lock(&coder->left_dongle->mutex);
	}
	if (check_can_take_dongle(coder))
	{
		take_and_pop(coder);
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		pthread_mutex_unlock(&coder->right_dongle->mutex);
		print_and_pop_dongles(coder);
		return (1);
	}
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	return (0);
}

int	take_dongles(t_coder *coder)
{
	while (1)
	{
		if (check_for_stop(coder->table))
			return (1);
		if (take_both_dongles(coder))
			break ;
		usleep(300);
	}
	if (check_for_stop(coder->table))
	{
		return (1);
	}
	return (0);
}
