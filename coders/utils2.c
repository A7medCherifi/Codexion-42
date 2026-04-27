#include "codexion.h"

long get_time() {
    struct	timeval		tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


struct timespec get_time_spec(long	time)
{
	struct	timespec	ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += time * 1000000;
	while (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	return (ts);
}
int		check_for_stop(t_table *table)
{
	pthread_mutex_lock(&table->log_mutex);
	if (table->stop) {
		pthread_mutex_unlock(&table->log_mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->log_mutex);
	return (0);
}

void	time_sleep(t_table *table, int	time)
{
	long	start;

	start = get_time();
	while (get_time() - start < time) 
	{
		if (check_for_stop(table)) {
			return ;
		}
		usleep(500);
	}
}
