/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:35:49 by acherifi          #+#    #+#             */
/*   Updated: 2026/06/08 14:34:43 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	free_all(t_table *table)
{
	int	i;

	if (!table)
		return (1);
	pthread_mutex_destroy(&table->mutex);
	i = 0;
	if (table->dongles)
	{
		while (i < table->args->number_of_coders)
			pthread_mutex_destroy(&table->dongles[i++].mutex);
		free(table->dongles);
	}
	i = 0;
	if (table->coders)
	{
		while (i < table->args->number_of_coders)
			pthread_mutex_destroy(&table->coders[i++].mutex);
		free(table->coders);
	}
	if (table->args)
	{
		free(table->args);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_table		table;

	if (parsing(&table, argc, argv))
		return (free_all(&table));
	table.dongles = create_dongles(&table);
	if (!table.dongles)
		return (free_all(&table));
	table.coders = create_coders_data(&table);
	if (!table.coders)
		return (free_all(&table));
	if (pthread_create(&table.monitor, NULL, monitor, &table))
		return (free_all(&table));
	if (create_threads(&table))
	{
		pthread_join(table.monitor, NULL);
		return (free_all(&table));
	}
	pthread_mutex_lock(&table.mutex);
	table.stop = 1;
	pthread_mutex_unlock(&table.mutex);
	pthread_join(table.monitor, NULL);
	free_all(&table);
	return (0);
}
