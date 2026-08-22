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
	pthread_t		monitor_thread;
	int				i;

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
	i = 0;
	while (i < simul.number_of_coders)
	{
		pthread_create(&simul.coders[i].thread, NULL,
			coder_routine, &simul.coders[i]);
		i++;
	}
	pthread_create(&monitor_thread, NULL, monitor_routine, &simul);
	i = 0;
	while (i < simul.number_of_coders)
	{
		pthread_join(simul.coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
	cleanup_simulation(&simul);
	return (0);
}
