/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:36:14 by acherifi          #+#    #+#             */
/*   Updated: 2026/06/08 14:10:08 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	coder_compiles(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_time();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
	pthread_mutex_lock(&coder->table->mutex);
	current_time = get_time() - coder->table->start_time;
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->mutex);
		return (1);
	}
	printf("%ld %d is compiling\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->mutex);
	time_sleep(coder->table, coder->table->args->time_to_compile);
	return (0);
}

int	coder_debug(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->table->mutex);
	current_time = get_time() - coder->table->start_time;
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->mutex);
		return (1);
	}
	printf("%ld %d is debugging\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->mutex);
	time_sleep(coder->table, coder->table->args->time_to_debug);
	return (0);
}

int	coder_refacture(t_coder *coder)
{
	long	current_time;

	if (check_for_stop(coder->table))
		return (1);
	pthread_mutex_lock(&coder->table->mutex);
	current_time = get_time() - coder->table->start_time;
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->mutex);
		return (1);
	}
	printf("%ld %d is refactoring\n", current_time, coder->id);
	pthread_mutex_unlock(&coder->table->mutex);
	time_sleep(coder->table, coder->table->args->time_to_refactor);
	return (0);
}

int	threads_processing(t_coder *coder)
{
	if (request_dongles(coder))
		return (1);
	if (coder_compiles(coder))
		return (1);
	if (release_dongle(coder))
		return (1);
	if (coder_debug(coder))
		return (1);
	if (coder_refacture(coder))
		return (1);
	if (check_for_stop(coder->table))
		return (1);
	check_for_done_simulation(coder);
	return (0);
}

void	*thread_manager(void *arg)
{
	t_coder		*coder;
	int			time_to_compile;
	int			dongle_cooldown;

	coder = (t_coder *)arg;
	while (1)
	{
		if (check_if_start_simulation(coder))
			break ;
		usleep(100);
	}
	if (check_for_stop(coder->table))
		return (NULL);
	time_to_compile = coder->table->args->time_to_compile;
	dongle_cooldown = coder->table->args->dongle_cooldown;
	if (coder->id % 2 == 0)
		time_sleep(coder->table, (time_to_compile + dongle_cooldown) / 2);
	while (!check_for_stop(coder->table))
	{
		if (threads_processing(coder))
			return (NULL);
	}
	return (NULL);
}
