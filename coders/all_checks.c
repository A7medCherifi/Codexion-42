/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   all_checks.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:35:45 by acherifi          #+#    #+#             */
/*   Updated: 2026/06/08 15:03:14 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_for_stop(t_table *table)
{
	pthread_mutex_lock(&table->mutex);
	if (table->stop)
	{
		pthread_mutex_unlock(&table->mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->mutex);
	return (0);
}

void	check_for_done_simulation(t_coder *coder)
{
	int		number_of_coders;
	int		compiles_required;
	int		all_done;
	int		i;

	i = 0;
	all_done = 1;
	number_of_coders = coder->table->args->number_of_coders;
	compiles_required = coder->table->args->number_of_compiles_required;
	while (i < number_of_coders)
	{
		pthread_mutex_lock(&coder->table->coders[i].mutex);
		if (coder->table->coders[i].compile_count <= compiles_required)
			all_done = 0;
		pthread_mutex_unlock(&coder->table->coders[i].mutex);
		if (!all_done)
			break ;
		i++;
	}
	if (all_done)
	{
		pthread_mutex_lock(&coder->table->mutex);
		coder->table->stop = 1;
		pthread_mutex_unlock(&coder->table->mutex);
	}
}

void	take_and_pop(t_coder *coder)
{
	coder->left_dongle->is_available = 0;
	pop_and_bubble_down(coder->left_dongle, coder->table->args->scheduler);
	coder->right_dongle->is_available = 0;
	pop_and_bubble_down(coder->right_dongle, coder->table->args->scheduler);
}

int	check_can_take_dongle(t_coder *coder)
{
	long	left_released;
	long	right_released;
	long	left_cooldown;
	long	right_cooldown;

	left_released = coder->left_dongle->released_at;
	right_released = coder->right_dongle->released_at;
	left_cooldown = coder->table->args->dongle_cooldown;
	right_cooldown = coder->table->args->dongle_cooldown;
	if (coder->id == coder->left_dongle->queue[0].coder_id
		&& coder->id == coder->right_dongle->queue[0].coder_id
		&& coder->left_dongle->is_available && coder->right_dongle->is_available
		&& get_time() - left_released >= left_cooldown
		&& get_time() - right_released >= right_cooldown)
	{
		return (1);
	}
	return (0);
}

void	fill_coders(t_table *table, int i)
{
	int		right_index;

	table->coders[i].id = i + 1;
	table->coders[i].table = table;
	table->coders[i].compile_count = 1;
	table->coders[i].left_dongle = &table->dongles[i];
	right_index = (i + 1) % table->args->number_of_coders;
	if (right_index == i)
		table->coders[i].right_dongle = NULL;
	else
		table->coders[i].right_dongle = &table->dongles[right_index];
}
