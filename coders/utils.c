/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acherifi <acherifi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 12:05:03 by acherifi          #+#    #+#             */
/*   Updated: 2026/04/19 12:05:03 by acherifi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int free_all(t_arguments **arg, t_table **tbl, t_coder **cdr) {
	free(*arg);
	free(*tbl);
	free(*cdr);
	*arg = NULL;
	*tbl = NULL;
	*cdr = NULL;
	return (1);
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
