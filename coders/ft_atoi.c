#include "codexion.h"

static int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

static int	my_strlen(char *str)
{
	int	len;
	int	i;

	len = 0;
	i = 0;
	while (str[i] == '0')
		i++;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			i++;
			len++;
		}
		else
			return (len);
	}
	return (len);
}

static char	*skip_get_sign(char *nptr, int *sign)
{
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
		{
			*sign = -1;
		}
		nptr++;
	}
	return (nptr);
}

static int	overflow_check(int sign, int len, long nb)
{
	if ((len == 10 && sign == 1) && (nb < 0 || nb > 2147483647))
		return (-1);
	else if ((len == 10 && sign == -1) && (nb < 0 || nb > 2147483648))
		return (-1);
	return (nb * sign);
}

int	ft_atoi(char *nptr)
{
	long	nb;
	int		sign;
	int		len;
	int		i;

	i = 0;
	nb = 0;
	sign = 1;
	nptr = skip_get_sign(nptr, &sign);
	len = my_strlen(nptr);
	while (ft_isdigit(nptr[i]))
	{
		nb = nb * 10 + nptr[i] - '0';
		i++;
	}
	if (nb >= 0 && nb <= 2147483647 && len <= 10)
		return (overflow_check(sign, len, nb));
	else
	{
		if (sign == 1 && len > 10)
			return (-1);
		else if (sign == -1 && len > 10)
			return (-1);
	}
	return (overflow_check(sign, len, nb));
}
