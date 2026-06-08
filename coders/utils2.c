/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:36:32 by acherifi          #+#    #+#             */
/*   Updated: 2026/06/08 14:45:14 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	time_sleep(t_table *table, int time)
{
	long	start;

	start = get_time();
	if (time == 0)
	{
		usleep(time);
		return ;
	}
	while ((get_time() - start) < time)
	{
		if (check_for_stop(table))
		{
			return ;
		}
		usleep(500);
	}
}

void	print_and_pop_dongles(t_coder *coder)
{
	long	start_time;

	start_time = coder->table->start_time;
	pthread_mutex_lock(&coder->table->mutex);
	if (coder->table->stop)
	{
		pthread_mutex_unlock(&coder->table->mutex);
		return ;
	}
	printf("%ld %d is taken dongle\n", get_time() - start_time, coder->id);
	printf("%ld %d is taken dongle\n", get_time() - start_time, coder->id);
	pthread_mutex_unlock(&coder->table->mutex);
}

int	check_for_burnout(t_table *table, int i)
{
	long	burnout_time;

	pthread_mutex_lock(&table->coders[i].mutex);
	burnout_time = get_time() - table->coders[i].last_compile_start;
	if (burnout_time >= table->args->time_to_burnout)
	{
		pthread_mutex_unlock(&table->coders[i].mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->coders[i].mutex);
	return (0);
}

int	release_dongle(t_coder *coder)
{
	if (check_for_stop(coder->table))
	{
		return (1);
	}
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->is_available = 1;
	coder->left_dongle->released_at = get_time();
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->is_available = 1;
	coder->right_dongle->released_at = get_time();
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	return (0);
}
