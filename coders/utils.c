#include "codexion.h"

int	free_all(t_table *table)
{
	if (!table)
		return (1);
	pthread_mutex_destroy(&table->log_mutex);
	if (table->args)
	{
		free(table->args);
	}
	if (table->dongles)
	{
		free(table->dongles);
	}
	if (table->coders)
	{
		free(table->coders);
	}
	return (1);
}

int	my_isdigit(char *str)
{
	int	i;

	i = 0;
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
	int		right_index;

	i = 0;
	table->coders = malloc(sizeof(t_coder) * (table->args->number_of_coders));
	if (!table->coders)
		return (NULL);
	pthread_mutex_lock(&table->log_mutex);
	table->stop = 0;
	table->done = 0;
	table->start_simulation = 0;
	pthread_mutex_unlock(&table->log_mutex);
	while (i < table->args->number_of_coders)
	{
		table->coders[i].id = i + 1;
		table->coders[i].table = table;
		table->coders[i].compile_count = 1;
		table->coders[i].left_dongle = &table->dongles[i];
		right_index = (i + 1) % table->args->number_of_coders;
		if (right_index == i)
			table->coders[i++].right_dongle = NULL;
		else
			table->coders[i++].right_dongle = &table->dongles[right_index];
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
			pthread_mutex_lock(&table->log_mutex);
			printf("ERROR: pthread create failed!\n");
			table->stop = 1;
			pthread_mutex_unlock(&table->log_mutex);
			while (--i >= 0)
				pthread_join(table->coders[i].thread, NULL);
			return (1);
		}
		i++;
	}
	pthread_mutex_lock(&table->log_mutex);
	table->start_time = get_time();
	while (--i >= 0)
		table->coders[i].last_compile_start = get_time();
	table->start_simulation = 1;
	pthread_mutex_unlock(&table->log_mutex);
	return (0);
}
