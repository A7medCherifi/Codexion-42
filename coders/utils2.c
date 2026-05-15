#include "codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	time_sleep(t_table *table, int time)
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

void	print_and_pop_dongles(t_coder *coder)
{
	long	start_time;

	start_time = coder->table->start_time;
	printf("%ld %d is taken dongle\n", get_time() - start_time, coder->id);
	printf("%ld %d is taken dongle\n", get_time() - start_time, coder->id);
	pop_and_bubble_down(coder->left_dongle, coder->table->args->scheduler);
	pop_and_bubble_down(coder->right_dongle, coder->table->args->scheduler);
}

int	check_for_burnout(t_table *table, int i)
{
	long	burnout_time;

	burnout_time = get_time() - table->coders[i].last_compile_start;
	if (burnout_time >= table->args->time_to_burnout)
	{
		return (1);
	}
	return (0);
}

int	release_dongle(t_coder *coder)
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
