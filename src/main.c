/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 18:12:38 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/06 18:12:38 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	char			**parsed_args;
	t_simulation	simul;

	parsed_args = verify_args(argc, argv);
	if (parsed_args == NULL)
	{
		fprintf(stderr, "ERROR: Invalid arguments.\n");
		return (1);
	}
	if (init_simulation(&simul, parsed_args) == 1)
	{
		fprintf(stderr, "ERROR: Simulation initialization failed.\n");
		return (1);
	}
	return (0);
}

/*
	if (verify_args(argc, argv) == NULL)
		printf("ERROR: Invalid arguments.\n");
	else
		printf("Success.\n");
*/