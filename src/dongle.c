/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 11:11:12 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/16 11:11:12 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Try to acquire a single dongle: waits (via cond_wait) until the
dongle is available, its cooldown has passed, AND this coder is
next in line (top of the waiting heap). Marks it as taken before
returning.
*/
static void	try_take_dongle(t_simulation *simul,
	t_dongle *dongle, t_coder *coder)
{
	long	remaining_cooldown_t;

	pthread_mutex_lock(&dongle->lock);
	remaining_cooldown_t = get_time_ms() - dongle->released_at_ms;
	min_heap_push(simul, &dongle->heap, coder);
	while (!dongle->is_available
		|| remaining_cooldown_t < simul->dongle_cooldown
		|| dongle->heap.data[0] != coder)
	{
		pthread_cond_wait(&dongle->cond, &dongle->lock);
		remaining_cooldown_t = get_time_ms() - dongle->released_at_ms;
	}
	dongle->is_available = false;
	min_heap_pop(simul, &dongle->heap);
	pthread_mutex_unlock(&dongle->lock);
}

/*
Attempt to use both dongles from a coder:
Left and right side. But first, choose the one to take
first based on their dongle_id.
*/
void	take_both_dongles(t_simulation *simul, t_coder *coder)
{
	t_dongle	*first_place;
	t_dongle	*second_place;

	if ((coder->left_dongle->dongle_id) < (coder->right_dongle->dongle_id))
	{
		first_place = coder->left_dongle;
		second_place = coder->right_dongle;
	}
	else
	{
		first_place = coder->right_dongle;
		second_place = coder->left_dongle;
	}
	try_take_dongle(simul, first_place, coder);
	try_take_dongle(simul, second_place, coder);
}

/*
Release a currently occupied dongle.
If the heap.size > 0 it means that there are still
coders waiting to use it - So the condition variable
needs to be sinalized to let it know its available.
*/
static void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = true;
	dongle->released_at_ms = get_time_ms();
	pthread_cond_signal(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

// Release both dongles from one coder
void	release_both_dongle(t_coder *coder)
{
	release_dongle(coder->left_dongle);
	release_dongle(coder->right_dongle);
}
