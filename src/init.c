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
	if (init_mutex(simul) == 1)
		return (1);
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
		if (pthread_mutex_init(&dongle->lock, NULL) != 0)
		{
			fprintf(stderr, "ERROR: Failed to initialize dongle mutex.\n");
			return (1);
		}
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

int	init_mutex(t_simulation *simul)
{

}
/*

typedef struct dongle
{
	int					dongle_id;
	bool				is_available;
	long				released_at_ms; // For the cooldown
	pthread_mutex_t		lock;
}	t_dongle;

pthread_mutex_init → log_lock + stop_lock
init_dongles       → malloc + loop
init_coders        → malloc + loop
cleanup_simulation → free + destroy mutexes
*/