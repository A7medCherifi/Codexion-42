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
	long			requested_at;
	long			deadline;
	int				coder_id;
}	t_request;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	t_request		queue[2];
	long			released_at;
	int				queue_size;
	int				id;
	int				is_available;
}	t_dongle;

typedef struct s_coder
{
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_table			*table;
	long			last_compile_start;
	int				id;
	int				compile_count;
}	t_coder;

typedef struct s_table
{
	pthread_mutex_t	log_mutex;
	pthread_t		monitor;
	t_arguments		*args;
	t_coder			*coders;
	t_dongle		*dongles;
	long			start_time;
	int				stop;
	int				done;
}	t_table;


// FUNCTIONS
struct timespec	get_time_spec(long	time);
t_coder     	*create_coders(t_table *table);
t_dongle    	*create_dongles(t_table *table);
void 			*monitor(void *arg);
void			*thread_manager(void *arg);
long			get_time();
int				ft_atoi(char *nptr);
int				my_isdigit(char *str);
int				free_all(t_table *table);
int				parsing(t_arguments	**args, int argc, char **argv);
int				release_dongle(t_coder *coder);
int				check_for_stop(t_table *table);
void			broadcast(t_table *table);
int				take_both_dongles(t_coder *coder);
void			time_sleep(t_table *table, int	time);
int				check_for_compiles(t_coder *coder);
void			check_for_done_simulation(t_coder *coder);

#endif