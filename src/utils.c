/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 00:26:25 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/28 23:14:49 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
