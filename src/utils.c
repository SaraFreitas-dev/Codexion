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

	if (strcmp(simul->scheduler, "edf") == 0)
		priority_ms = coder->last_compile_start_ms + simul->time_to_burnout;
	else
		priority_ms = get_time_ms();
	return (priority_ms);
}

void	get_cooldown_deadline(t_dongle *dongle, int cooldown,
	struct timespec *ts)
{
	long	deadline_ms;

	deadline_ms = dongle->released_at_ms + cooldown;
	ts->tv_sec = deadline_ms / 1000;
	ts->tv_nsec = deadline_ms % 1000 * 1000000;
}

// To swap the data nodes in the heap struct
void	ft_swap(t_coder **a, t_coder **b)
{
	t_coder	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
