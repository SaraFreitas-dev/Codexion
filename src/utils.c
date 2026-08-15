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

// To swap the data nodes in the heap struct
void	ft_swap(t_coder **a, t_coder **b)
{
	t_coder	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

