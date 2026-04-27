#include "codexion.h"


int free_all(t_table *table)
{
	int		i;

	if (!table)
		return (1);
	if (table->args)
	{
		pthread_mutex_destroy(&table->log_mutex);
		if (table->dongles)
		{
			i = 0;
			while (i < table->args->number_of_coders) {
				pthread_mutex_destroy(&table->dongles[i].mutex);
				i++;
			}
		}
		free(table->args);
	}
    if (table->dongles) {
        free(table->dongles);
    }
    if (table->coders) {
        free(table->coders);
    }
	return (1);
}

int	my_isdigit(char *str)
{
	int	i;

	i = 0;
	while(str[i] == '+' && strlen(str) > 1) {
		i++;
	}
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
		pthread_mutex_init(&table->dongles[i].mutex, NULL);
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
	pthread_mutex_lock(&table->log_mutex);
	table->stop = 0;
	table->done = 1;
	table->start_time = get_time();
	pthread_mutex_unlock(&table->log_mutex);
	while (i < table->args->number_of_coders) {
		// if (table->stop) {
		// 	return (NULL);
		// }
		table->coders[i].id = i + 1;
		table->coders[i].table = table;
		table->coders[i].compile_count = 1;
		table->coders[i].last_compile_start = table->start_time;
		table->coders[i].left_dongle = &table->dongles[i];
		table->coders[i].right_dongle = &table->dongles[(i + 1) % table->args->number_of_coders];
		pthread_create(&table->coders[i].thread, NULL, thread_manager, &table->coders[i]);
		i++;
	}
    // if (table->stop)
	// {
	// 	return (NULL);
	// }
    return (table->coders);
}

void    release_dongle(t_coder *coder)
{
    pthread_mutex_lock(&coder->left_dongle->mutex);
    pthread_mutex_lock(&coder->right_dongle->mutex);
    coder->left_dongle->is_available = 1;
    coder->right_dongle->is_available = 1;
    coder->left_dongle->released_at = get_time();
    coder->right_dongle->released_at = get_time();
    pthread_mutex_unlock(&coder->left_dongle->mutex);
    pthread_mutex_unlock(&coder->right_dongle->mutex);
}
