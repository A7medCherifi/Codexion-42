#include "codexion.h"


int main(int argc, char **argv)
{
	t_table table;
    int     i;

	if (parsing(&table.args, argc, argv))
		return (free_all(&table));
	pthread_mutex_init(&table.log_mutex, NULL);
    table.dongles = create_dongles(&table);
    if (!table.dongles) {
		return (free_all(&table));
    }
    pthread_create(&table.monitor, NULL, monitor, &table);
    table.coders = create_coders(&table);
	if (!table.coders) {
		return (free_all(&table)); }
	i = 0;
	while (i < table.args->number_of_coders) {
		pthread_join(table.coders[i].thread, NULL);
		i++;
	}
	pthread_mutex_lock(&table.log_mutex);
	table.stop = 1;
	pthread_mutex_unlock(&table.log_mutex);
	pthread_join(table.monitor, NULL);
	free_all(&table);
	return (0);
}
