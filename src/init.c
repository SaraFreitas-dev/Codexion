/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 23:35:45 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/06 23:35:45 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Inicialize the heap on the dongle struct
*/
static int	init_dongle_heap(t_dongle *dongle, int capacity)
{
	dongle->heap.data = malloc(sizeof(t_coder *) * capacity);
	if (!dongle->heap.data)
	{
		fprintf(stderr, "ERROR: Failed to initialize dongle heap.\n");
		return (1);
	}
	dongle->heap.size = 0;
	return (0);
}

/*
Fill each dongle's data. Keeping in mind the subject rule:
"There is one dongle between each pair of coders.
Therefore, if there are several coders, each coder has a dongle
on their left side and a dongle on their right side.
If there is only one coder, there should be only one dongle on the table."
*/
int	init_dongles(t_simulation *simul)
{
	int			id;
	t_dongle	*dongle;

	simul->dongles = malloc(sizeof(t_dongle) * simul->number_of_coders);
	if (!simul->dongles)
	{
		fprintf(stderr, "ERROR: Failed to initialize dongles.\n");
		return (1);
	}
	id = 0;
	while (id < (simul->number_of_coders))
	{
		dongle = &simul->dongles[id];
		dongle->dongle_id = id + 1;
		dongle->is_available = true;
		dongle->released_at_ms = 0;
		if (init_dongle_heap(dongle, simul->number_of_coders) == 1
			|| pthread_mutex_init(&dongle->lock, NULL) != 0
			|| pthread_cond_init(&dongle->cond, NULL) != 0)
			return (1);
		id++;
	}
	return (0);
}

/*
Fill each coder's data.
right_dongle wraps around using the % operator,
so the last coder's right dongle is the first coder's left one:
This closes the circle required by the subject:
(coder N sits next to coder 1).
*/
int	init_coders(t_simulation *simul)
{
	int		id;
	t_coder	*coder;

	simul->coders = malloc(sizeof(t_coder) * simul->number_of_coders);
	if (!simul->coders)
	{
		fprintf(stderr, "ERROR: Failed to initialize coders.\n");
		return (1);
	}
	id = 0;
	while (id < (simul->number_of_coders))
	{
		coder = &simul->coders[id];
		coder->coder_id = id + 1;
		coder->times_compiled = 0;
		coder->last_compile_start_ms = simul->start_time_ms;
		coder->simul = simul;
		coder->left_dongle = &simul->dongles[id];
		coder->right_dongle = (&simul->dongles[(id + 1)
				% simul->number_of_coders]);
		id++;
	}
	return (0);
}

/*
At this point, the arguments have been parsed and verified,
So they just need to be assigned on the t_simulation.
No malloc needed here: t_simulation is passed by pointer
to an already-existing struct in main
*/
int	init_simulation(t_simulation *simul, char **args)
{
	simul->number_of_coders = atoi(args[1]);
	simul->time_to_burnout = atoi(args[2]);
	simul->time_to_compile = atoi(args[3]);
	simul->time_to_debug = atoi(args[4]);
	simul->time_to_refactor = atoi(args[5]);
	simul->number_of_compiles_required = atoi(args[6]);
	simul->dongle_cooldown = atoi(args[7]);
	simul->scheduler = args[8];
	simul->start_time_ms = get_time_ms();
	simul->should_stop = false;
	if (init_dongles(simul) == 1)
		return (1);
	if (init_coders(simul) == 1)
		return (1);
	if (pthread_mutex_init(&simul->log_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&simul->stop_lock, NULL) != 0)
		return (1);
	return (0);
}

void	cleanup_simulation(t_simulation *simul)
{
	int			id;
	t_dongle	*dongle;

	id = 0;
	while (id < simul->number_of_coders)
	{
		dongle = &simul->dongles[id];
		free(dongle->heap.data);
		pthread_mutex_destroy(&dongle->lock);
		pthread_cond_destroy(&dongle->cond);
		id++;
	}
	pthread_mutex_destroy(&simul->log_lock);
	pthread_mutex_destroy(&simul->stop_lock);
	free(simul->dongles);
	free(simul->coders);
}
