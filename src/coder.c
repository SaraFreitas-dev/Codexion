/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 21:02:34 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/20 21:02:34 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Prints the log from the current event (	DEBUGGING or REFACTORING).
Pauses the calling thread for the requested
milliseconds, then lets it continue. 
*/
static void	sleep_phase(t_simulation *simul, t_coder *coder,
	t_event event, long duration_ms)
{
	print_log(simul, coder, event);
	usleep(duration_ms * 1000);
}

/*
Print and manage the COMPILING event / phase.
*/
static void	compile_phase(t_simulation *simul, t_coder *coder)
{
	take_both_dongles(simul, coder);
	coder->last_compile_start_ms = get_time_ms();
	print_log(simul, coder, COMPILING);
	usleep(simul->time_to_compile * 1000);
	coder->times_compiled++;
	release_both_dongle(coder);
}

/*
Each coder loops compile->debug->refactor until told to stop.
should_stop is shared, so it's read into a local copy while
holding stop_lock — the lock is released immediately after,
before running the (long) phases, so other coders aren't blocked.
*/
void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*simul;
	bool			to_stop;

	coder = (t_coder *) arg;
	simul = coder->simul;
	while (1)
	{
		pthread_mutex_lock(&simul->stop_lock);
		to_stop = simul->should_stop;
		pthread_mutex_unlock(&simul->stop_lock);
		if (to_stop)
			break ;
		compile_phase(simul, coder);
		sleep_phase(simul, coder, DEBUGGING, simul->time_to_debug);
		sleep_phase(simul, coder, REFACTORING, simul->time_to_refactor);
	}
	return (NULL);
}
