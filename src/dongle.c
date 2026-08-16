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
int	try_take_dongle(t_simulation *simul, t_dongle *dongle, t_coder *coder)
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
	return (0);
}

/*
release_dongle — pseudocódigo

Vamos pensar nos passos, tal como fizemos com try_take_dongle.

função release_dongle(simul, dongle):
    lock(dongle->lock)

    dongle->is_available = true
    dongle->released_at_ms = tempo atual

    se dongle->heap.size > 0:
        sinaliza a condition variable (acorda quem estiver à espera)

    unlock(dongle->lock)
*/