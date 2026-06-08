/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:36:04 by acherifi          #+#    #+#             */
/*   Updated: 2026/06/08 14:07:29 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	monitor_check(t_table *table, int i)
{
	pthread_mutex_lock(&table->mutex);
	if (i < table->args->number_of_coders
		&& !table->stop && table->start_simulation)
	{
		pthread_mutex_unlock(&table->mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->mutex);
	return (0);
}

void	*monitor(void *arg)
{
	t_table		*table;
	int			i;

	table = (t_table *)arg;
	while (!check_for_stop(table))
	{
		i = 0;
		while (monitor_check(table, i))
		{
			if (check_for_burnout(table, i))
			{
				pthread_mutex_lock(&table->mutex);
				printf("%ld %d burned out\n",
					get_time() - table->start_time, i + 1);
				table->stop = 1;
				pthread_mutex_unlock(&table->mutex);
				return (NULL);
			}
			i++;
		}
		usleep(300);
	}
	return (NULL);
}
