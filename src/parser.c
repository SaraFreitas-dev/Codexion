/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 17:56:10 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/06 17:56:10 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// To prevent overflow while using atoi
int	is_within_int_range(char *str)
{
	long	value;
	int		i;

	value = 0;
	i = 0;
	while (str[i])
	{
		value = value * 10 + (str[i] - '0');
		if (value > 2147483647)
		{
			fprintf(stderr, "ARGUMENT ERROR: A value exceeds "
				"the INT_MAX limits.\n");
			return (0);
		}
		i++;
	}
	return (1);
}

int	has_valid_numbers(int argc, char **argv)
{
	int	i;
	int	j;

	i = 1;
	while (i < (argc - 1))
	{
		j = 0;
		while (argv[i][j])
		{
			if ((argv[i][j] >= '0') && argv[i][j] <= '9')
				j++;
			else
				return (0);
		}
		if (!is_within_int_range(argv[i]))
			return (0);
		if (atoi(argv[i]) == 0)
		{
			fprintf(stderr, "ARGUMENT ERROR: Values need to be "
				"superior to zero.\n");
			return (0);
		}
		i++;
	}
	return (1);
}

int	has_valid_scheduler(char *str)
{
	if ((strcmp(str, "fifo") == 0) || (strcmp(str, "edf") == 0))
		return (1);
	fprintf(stderr, "ARGUMENT ERROR: Invalid scheduler.\n");
	return (0);
}

char	**verify_args(int argc, char **argv)
{
	if (argc != 9)
		return (NULL);
	if (!has_valid_numbers(argc, argv) || !has_valid_scheduler(argv[8]))
		return (NULL);
	return (argv);
}
