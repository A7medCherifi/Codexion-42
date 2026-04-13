#include "codexion.h"

void free_all(arguments_t **args) {
	free(*args);
	return ;
}

int	my_isdigit(char *str)
{
	int	i;

	i = 0;
	while (str[i]) {
		if (str[i] >= '0' && str[i] <= '9')
			i++;
		else
			return (0);
	}
	return (1);
}