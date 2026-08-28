/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 00:26:25 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/09 00:26:25 by sarfreit         ###   ########.fr       */
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
Safely checks whether the simulation has been told to stop.
Uses stop_lock so the shared should_stop flag can be read without
racing with the monitor thread that may update it.
*/
bool	should_stop_now(t_simulation *simul)
{
	bool	result;

	pthread_mutex_lock(&simul->stop_lock);
	result = simul->should_stop;
	pthread_mutex_unlock(&simul->stop_lock);
	return (result);
}

// To swap the data nodes in the heap struct
void	ft_swap(t_coder **a, t_coder **b)
{
	t_coder	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
