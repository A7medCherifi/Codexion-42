#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
typedef struct table_s table_t;
typedef struct coder_s coder_t;
typedef struct dongle_s dongle_t;

typedef struct arguments_s
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	int		scheduler;
}	arguments_t;

typedef struct dongle_s
{
	pthread_mutex_t	mutex;
    pthread_cond_t	cond;
	int				id;
	int				is_available;
	int				released_at;
}	dongle_t;

typedef struct coder_s
{
	pthread_t		thread;
	dongle_t		*left_dongle;
	dongle_t		*right_dongle;
	table_t			*table;
	int				id;
	int				compile_count;
	int				last_compile_start;
}	coder_t;

typedef struct table_s
{
	arguments_t		*args;
	coder_t			*coders;
	dongle_t		*dongles;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	int				start_time;
	int				stop_time;
}	table_t;


// FUNCTIONS
int	ft_atoi(char *nptr);
int	my_isdigit(char *str);
void free_all(arguments_t **args);
int parsing(arguments_t	**args, int argc, char **argv);

#endif