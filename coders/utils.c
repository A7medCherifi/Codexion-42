/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:36:16 by acherifi          #+#    #+#             */
/*   Updated: 2026/05/19 14:36:16 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	start_coders_simulation(t_table *table)
{
	long	current_time;
	int		i;

	i = 0;
	current_time = get_time();
	while (i < table->args->number_of_coders)
	{
		pthread_mutex_lock(&table->coders[i].mutex);
		table->coders[i].last_compile_start = current_time;
		pthread_mutex_unlock(&table->coders[i].mutex);
		i++;
	}
	pthread_mutex_lock(&table->mutex);
	table->start_time = current_time;
	table->start_simulation = 1;
	pthread_mutex_unlock(&table->mutex);
}

int	my_isdigit(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-')
	{
		if (ft_atoi(str) == 0)
			return (1);
	}
	if (str[i] == '+' && strlen(str) > 1)
		i++;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
			i++;
		else
			return (0);
	}
	return (1);
}

t_dongle	*create_dongles(t_table *table)
{
	int	i;

	table->dongles = malloc(sizeof(t_dongle) * (table->args->number_of_coders));
	if (!table->dongles)
		return (NULL);
	i = 0;
	while (i < table->args->number_of_coders)
	{
		if (pthread_mutex_init(&table->dongles[i].mutex, NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&table->dongles[i].mutex);
			return (NULL);
		}
		table->dongles[i].id = i;
		table->dongles[i].queue_size = 0;
		table->dongles[i].is_available = 1;
		table->dongles[i].released_at = 0;
		i++;
	}
	return (table->dongles);
}

t_coder	*create_coders_data(t_table *table)
{
	int		i;

	i = 0;
	table->coders = malloc(sizeof(t_coder) * (table->args->number_of_coders));
	if (!table->coders)
		return (NULL);
	pthread_mutex_lock(&table->mutex);
	table->stop = 0;
	table->start_simulation = 0;
	pthread_mutex_unlock(&table->mutex);
	while (i < table->args->number_of_coders)
	{
		if (pthread_mutex_init(&table->coders[i].mutex, NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&table->coders[i].mutex);
			return (NULL);
		}
		fill_coders(table, i);
		i++;
	}
	return (table->coders);
}

int	create_coders(t_table *table)
{
	int		i;

	i = 0;
	while (i < table->args->number_of_coders)
	{
		if (pthread_create(&table->coders[i].thread, NULL,
				thread_manager, &table->coders[i]))
		{
			pthread_mutex_lock(&table->mutex);
			table->stop = 1;
			pthread_mutex_unlock(&table->mutex);
			while (--i >= 0)
				pthread_join(table->coders[i].thread, NULL);
			return (1);
		}
		i++;
	}
	start_coders_simulation(table);
	return (0);
}
