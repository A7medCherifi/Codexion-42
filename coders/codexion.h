#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

// Forward declarations
typedef struct s_table t_table;
typedef struct s_coder t_coder;
typedef struct s_dongle t_dongle;

typedef struct s_arguments
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	int		scheduler;
}	t_arguments;

typedef struct s_request
{
	int				coder_id;
	long			requested_at;
	long			deadline;
}	t_request;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
    pthread_cond_t	cond;
	t_request		queue[2];
	int				queue_size;
	int				id;
	int				is_available;
	long			released_at;
}	t_dongle;

typedef struct s_coder
{
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_table			*table;
	int				id;
	int				compile_count;
	long			last_compile_start;
	int				dongles_i_have;
}	t_coder;

typedef struct s_table
{
	pthread_t		monitor;
	t_arguments		*args;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	pthread_cond_t	stop_cond;
	long			start_time;
	int				stop;
}	t_table;


// FUNCTIONS
struct timespec	get_time_spec(int	time);
t_coder     	*create_coders(t_table *table);
t_dongle    	*create_dongles(t_table *table);
void 			*monitor(void *arg);
void			*thread_manager(void *arg);
long			get_time();
int				ft_atoi(char *nptr);
int				my_isdigit(char *str);
int				free_all(t_table *table);
int				parsing(t_arguments	**args, int argc, char **argv);
void			take_both_dongles(t_coder *coder);
void			release_dongle(t_coder *coder);
int				check_for_stop(t_table *table);

#endif