#include "codexion.h"

int	main(int argc, char **argv)
{
	t_table		table;

	if (pthread_mutex_init(&table.log_mutex, NULL))
		return (1);
	if (parsing(&table, argc, argv))
		return (free_all(&table));
	table.dongles = create_dongles(&table);
	if (!table.dongles)
		return (free_all(&table));
	table.coders = create_coders_data(&table);
	if (!table.coders)
		return (free_all(&table));
	pthread_create(&table.monitor, NULL, monitor, &table);
	if (create_threads(&table))
	{
		pthread_join(table.monitor, NULL);
		return (free_all(&table));
	}
	pthread_mutex_lock(&table.log_mutex);
	table.stop = 1;
	pthread_mutex_unlock(&table.log_mutex);
	pthread_join(table.monitor, NULL);
	free_all(&table);
	return (0);
}
