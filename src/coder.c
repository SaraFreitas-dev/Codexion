/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 21:02:34 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/28 22:49:36 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Lock the request_counter to increase its value
When a coder requests a dongle ("priority wqiting list")
*/
void	assign_request_order(t_simulation *simul, t_coder *coder)
{
	pthread_mutex_lock(&simul->request_lock);
	simul->request_counter++;
	coder->request_order = simul->request_counter;
	pthread_mutex_unlock(&simul->request_lock);
}

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
static bool	compile_phase(t_simulation *simul, t_coder *coder)
{
	if (!take_both_dongles(simul, coder))
		return (false);
	pthread_mutex_lock(&coder->time_lock);
	coder->last_compile_start_ms = get_time_ms();
	pthread_mutex_unlock(&coder->time_lock);
	print_log(simul, coder, COMPILING);
	usleep(simul->time_to_compile * 1000);
	pthread_mutex_lock(&coder->time_lock);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->time_lock);
	release_both_dongle(coder);
	return (true);
}

/*
Each coder loops through compile -> debug -> refactor until the
simulation is stopped. should_stop_now() safely reads the shared
stop flag using stop_lock before starting the next phase.
*/
void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*simul;

	coder = (t_coder *)arg;
	simul = coder->simul;
	while (!should_stop_now(simul))
	{
		if (!compile_phase(simul, coder))
			break ;
		if (should_stop_now(simul))
			break ;
		sleep_phase(simul, coder, DEBUGGING, simul->time_to_debug);
		if (should_stop_now(simul))
			break ;
		sleep_phase(simul, coder, REFACTORING, simul->time_to_refactor);
	}
	return (NULL);
}
