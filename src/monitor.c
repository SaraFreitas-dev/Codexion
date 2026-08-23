/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 20:55:37 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/21 20:55:37 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Check if the coder has reached burnout
static bool	coder_is_in_burnout(t_simulation *simul, t_coder *coder)
{
	long	time_since_last_compile;
	long	last_compile;

	pthread_mutex_lock(&coder->time_lock);
	last_compile = coder->last_compile_start_ms;
	pthread_mutex_unlock(&coder->time_lock);
	time_since_last_compile = get_time_ms() - last_compile;
	return (time_since_last_compile >= simul->time_to_burnout);
}

// Check if the coder has reached the number of compiles required
static bool	coder_compilations_are_finished(t_simulation *simul, t_coder *coder)
{
	int	compiled;

	pthread_mutex_lock(&coder->time_lock);
	compiled = coder->times_compiled;
	pthread_mutex_unlock(&coder->time_lock);
	return (compiled >= simul->number_of_compiles_required);
}

/*
Loop through every coder once: log and stop early if anyone is
in burnout, otherwise track whether all of them have reached the
required number of compiles.
*/
static bool	check_all_coders(t_simulation *simul)
{
	t_coder	*coder;
	int		id;
	bool	all_finished;

	id = 0;
	all_finished = true;
	while (id < simul->number_of_coders)
	{
		coder = &simul->coders[id];
		if (coder_is_in_burnout(simul, coder))
		{
			print_log(simul, coder, BURNED_OUT);
			return (true);
		}
		if (!coder_compilations_are_finished(simul, coder))
			all_finished = false;
		id++;
	}
	return (all_finished);
}

/*
Monitor thread: repeatedly checks all coders until either a
burnout is detected or everyone has finished compiling — then
sets should_stop and exits.
*/
void	*monitor_routine(void *arg)
{
	t_simulation	*simul;

	simul = (t_simulation *)arg;
	while (1)
	{
		if (check_all_coders(simul))
		{
			pthread_mutex_lock(&simul->stop_lock);
			simul->should_stop = true;
			pthread_mutex_unlock(&simul->stop_lock);
			return (NULL);
		}
		usleep(2000);
	}
}
