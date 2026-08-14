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

/*
- sift_up: moves a too-small node up, swapping with its parent,
until the min-heap property holds again.
- sift_down: moves a too-large node down, swapping with its
SMALLER child, until the min-heap property holds again.
*/

/*
min_heap_push / min_heap_pop functions:
Add or remove elements from the heap.
push calls sift_up (new node climbs to its correct spot).
pop calls sift_down (root replacement sinks to its correct spot).
*/