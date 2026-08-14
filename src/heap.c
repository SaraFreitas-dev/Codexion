/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 22:06:24 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/13 22:06:24 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Compute the priority key based on the active scheduler (fifo or edf)
long	calculate_priority(t_simulation *simul, t_coder *coder)
{
	long	priority_ms;

	if (strcmp(simul->scheduler, "edf") == 0)
		priority_ms = coder->last_compile_start_ms + simul->time_to_burnout;
	else
		priority_ms = get_time_ms();
	return (priority_ms);
}

