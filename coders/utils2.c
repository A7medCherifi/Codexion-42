#include "codexion.h"

long get_time()
{
    struct	timeval		tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	time_sleep(t_table *table, int	time)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time) 
	{
		if (check_for_stop(table))
		{
			return ;
		}
		usleep(500);
	}
}
int		release_dongle(t_coder *coder)
{
	if (check_for_stop(coder->table))
	{
		return (1);
	}
    pthread_mutex_lock(&coder->table->log_mutex);
    coder->left_dongle->is_available = 1;
    coder->right_dongle->is_available = 1;
    coder->left_dongle->released_at = get_time();
    coder->right_dongle->released_at = get_time();
    pthread_mutex_unlock(&coder->table->log_mutex);
	return (0);
}
