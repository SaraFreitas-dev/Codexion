/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:14:36 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/29 00:28:53 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Calculate the time in milliseconds
long	get_time_ms(void)
{
	long			now_ms;
	struct timeval	tv;	

	gettimeofday(&tv, NULL);
	now_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (now_ms);
}

/*
Compute the priority key based on the active scheduler (fifo or edf)
- fifo: the current time. Whoever calls this first gets the smallest
  value, so first-come is served first.
- edf: the coder's burnout deadline. Whoever is closest to burning out
  gets the smallest value, so the most urgent coder is served first.
The smaller value always wins priority in both modes.
*/
long	calculate_priority(t_simulation *simul, t_coder *coder)
{
	long	priority_ms;
	long	last_compile;

	if (strcmp(simul->scheduler, "edf") == 0)
	{
		pthread_mutex_lock(&coder->time_lock);
		last_compile = coder->last_compile_start_ms;
		pthread_mutex_unlock(&coder->time_lock);
		priority_ms = last_compile + simul->time_to_burnout;
	}
	else
		priority_ms = get_time_ms();
	return (priority_ms);
}

/*
Resolve equal priorities between two coders.
EDF: the coder with the lowest coder_id wins.
FIFO: the coder with the lowest request_order wins.
*/
long	priority_tie_breaker(t_simulation *simul, t_coder *coder)
{
	if (strcmp(simul->scheduler, "edf") == 0)
		return (coder->coder_id);
	return (coder->request_order);
}

/*
To use on the heap file
Compares two coders and decides which one has priority.
If priority_ms ties, falls back to priority_tie_breaker.
Returns true if "a" has priority over "b"
*/
bool	is_higher_priority(t_simulation *simul, t_coder *a, t_coder *b)
{
	if (a->priority_ms < b->priority_ms)
		return (true);
	if (a->priority_ms == b->priority_ms)
		return (priority_tie_breaker(simul, a)
			< priority_tie_breaker(simul, b));
	return (false);
}
