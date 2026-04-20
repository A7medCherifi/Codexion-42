/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 12:05:03 by acherifi          #+#    #+#             */
/*   Updated: 2026/04/20 18:58:59 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


int free_all(t_table *table)
{

    if (table->dongles) {
        free(table->dongles);
    }

    if (table->coders) {
        free(table->coders);
    }

    if (table->args) {
        free(table->args);
    }

	return (1);
}

int	my_isdigit(char *str)
{
	int	i;

	i = 0;
	while (str[i]) {
		if (str[i] >= '0' && str[i] <= '9')
			i++;
		else
			return (0);
	}
	return (1);
}

t_dongle    *create_dongles(t_table *table)
{
    int     i;

    i = 0;
    table->dongles = malloc(sizeof(t_dongle) * (table->args->number_of_coders));
	if (!table->dongles)
		return (NULL);

	while (i < table->args->number_of_coders)
	{
		table->dongles[i].queue_size = 0;
		table->dongles[i].id = i;
		table->dongles[i].is_available = 1;
		table->dongles[i].released_at = 0;
		pthread_mutex_init(&table->dongles[i].mutex, NULL);
		pthread_cond_init(&table->dongles[i].cond, NULL);
		i++;
	}
    return (table->dongles);
}

t_coder     *create_coders(t_table *table)
{
    int     i;

    i = 0;
    table->coders = malloc(sizeof(t_coder) * (table->args->number_of_coders));
	if (!table->coders)
		return (NULL);
	table->stop = 0;
	table->start_time = get_time();
	while (i < table->args->number_of_coders) {
		if (table->stop)
			return (NULL);
		table->coders[i].id = i + 1;
		table->coders[i].table = table;
		table->coders[i].last_compile_start = table->start_time;
		table->coders[i].left_dongle = &table->dongles[i];
		table->coders[i].right_dongle = &table->dongles[(i + 1) % table->args->number_of_coders];
		pthread_create(&table->coders[i].thread, NULL, thread_manager, &table->coders[i]);
		i++;
	}
    if (table->stop)
		return (NULL);
	
    return (table->coders);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->released_at = get_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
