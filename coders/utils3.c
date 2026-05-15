#include "codexion.h"

int	initialize_mutexes(t_table *table)
{
	if (pthread_mutex_init(&table->mutex, NULL))
		return (1);
	return (0);
}

void	print_burnout(t_table *table, int i)
{
	printf("%ld %d burned out\n",
		get_time() - table->start_time, i + 1);
}
