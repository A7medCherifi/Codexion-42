#include "codexion.h"

void	swap_nodes(t_request *parent, t_request *child)
{
	t_request	temp;

	temp = *parent;
	*parent = *child;
	*child = temp;
	printf("hahahaha\n\n");
}

int check_numbers(char *argument, int min)
{
	int		number;

	if (strlen(argument) == 0)
		return (-1);
	if (my_isdigit(argument))
    {
		number = ft_atoi(argument);
		if (number >= min)
			return (number);
	}
	return (-1);
}


void check_add_agrs(int *arg, char *str, int *isvalid, int min)
{
	int		value;

	if (!*isvalid)
		return ;
	value = check_numbers(str, min);
	if (value < 0) {
		*isvalid = 0;
		return ;
	}
	*arg = value;
}

void check_sheduler(int *arg, char *str, int *isvalid)
{
	if (!*isvalid)
		return ;
	if (strcmp(str, "fifo") == 0) {
		*arg = 1;
	} else if (strcmp(str, "edf") == 0) {
		*arg = 0;
	} else {
		*isvalid = 0;
		return ;
	}
}

int parsing(t_table *table, int argc, char **argv)
{
	int			isvalid;

	memset(table, 0, sizeof(t_table));
	if (argc != 9) {
		return (1);
	}
	table->args = malloc(sizeof(t_arguments));
	if (!table->args)
		return (1);
	isvalid = 1;
	check_add_agrs(&table->args->number_of_coders, argv[1], &isvalid, 1);
	check_add_agrs(&table->args->time_to_burnout, argv[2], &isvalid, 1);
	check_add_agrs(&table->args->time_to_compile, argv[3], &isvalid, 0);
	check_add_agrs(&table->args->time_to_debug, argv[4], &isvalid, 0);
	check_add_agrs(&table->args->time_to_refactor, argv[5], &isvalid, 0);
	check_add_agrs(&table->args->number_of_compiles_required, argv[6], &isvalid, 1);
	check_add_agrs(&table->args->dongle_cooldown, argv[7], &isvalid, 0);
	check_sheduler(&table->args->scheduler, argv[8], &isvalid);
	if (!isvalid) {
		return (1);
	}
	return (0);
}
