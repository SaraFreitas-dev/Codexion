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

// Calculate the time in milliseconds (logs.c)
long	get_time_ms(void)
{
	long			now_ms;
	struct timeval	tv;	

	gettimeofday(&tv, NULL);
	now_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (now_ms);
}


// To swap the data nodes in the heap struct (heap.c)
void	ft_swap(t_entry *a, t_entry *b)
{
	t_entry	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}