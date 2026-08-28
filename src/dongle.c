/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 11:11:12 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/29 00:25:08 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	get_cooldown_deadline(t_dongle *dongle, int cooldown,
	struct timespec *ts)
{
	long	deadline_ms;

	deadline_ms = dongle->released_at_ms + cooldown;
	ts->tv_sec = deadline_ms / 1000;
	ts->tv_nsec = deadline_ms % 1000 * 1000000;
}

/*
Try to acquire a single dongle: waits (via cond_wait) until the
dongle is available, its cooldown has passed, AND this coder is
next in line (top of the waiting heap). Marks it as taken before
returning.
*/
static void	try_take_dongle(t_simulation *simul,
	t_dongle *dongle, t_coder *coder)
{
	long			remaining_cooldown_t;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->lock);
	coder->priority_ms = calculate_priority(simul, coder);
	assign_request_order(simul, coder);
	remaining_cooldown_t = get_time_ms() - dongle->released_at_ms;
	min_heap_push(simul, &dongle->heap, coder);
	while ((!dongle->is_available
			|| remaining_cooldown_t < simul->dongle_cooldown
			|| dongle->heap.data[0] != coder) && !should_stop_now(simul))
	{
		get_cooldown_deadline(dongle, simul->dongle_cooldown, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		remaining_cooldown_t = get_time_ms() - dongle->released_at_ms;
	}
	if (should_stop_now(simul))
	{
		pthread_mutex_unlock(&dongle->lock);
		return ;
	}
	dongle->is_available = false;
	print_log(simul, coder, DONGLE_TAKEN);
	min_heap_pop(&dongle->heap, simul);
	pthread_mutex_unlock(&dongle->lock);
}

/*
Attempt to use both dongles from a coder:
Left and right side. But first, choose the one to take
first based on their dongle_id.
*/
bool	take_both_dongles(t_simulation *simul, t_coder *coder)
{
	t_dongle	*first_place;
	t_dongle	*second_place;

	if (coder->left_dongle == coder->right_dongle)
	{
		try_take_dongle(simul, coder->left_dongle, coder);
		while (!should_stop_now(simul))
			usleep(1000);
		return (false);
	}
	if (coder->left_dongle->dongle_id < coder->right_dongle->dongle_id)
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
	if (should_stop_now(simul))
		return (false);
	try_take_dongle(simul, second_place, coder);
	return (!should_stop_now(simul));
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
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

// Release both dongles from one coder
void	release_both_dongle(t_coder *coder)
{
	release_dongle(coder->left_dongle);
	if (coder->left_dongle != coder->right_dongle)
		release_dongle(coder->right_dongle);
}
