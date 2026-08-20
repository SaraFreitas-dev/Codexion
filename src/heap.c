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
- sift_up: moves a too-small node up, swapping with its parent,
until the min-heap property holds again.
When it's used: right after a new element is added at the very
bottom (end) of the array — it needs to travel up to its correct spot.
*/
static void	sift_up(t_simulation *simul, t_heap *heap, int i)
{
	int				parent_i;
	long			parent_priority;
	long			curr_priority;

	while (i > 0)
	{
		parent_i = (i - 1) / 2;
		curr_priority = calculate_priority(simul, heap->data[i]);
		parent_priority = calculate_priority(simul, heap->data[parent_i]);
		if (curr_priority >= parent_priority)
			return ;
		else
		{
			ft_swap(&heap->data[i], &heap->data[parent_i]);
			i = parent_i;
		}
	}
}

/*
- sift_down: moves a too-large node down, swapping with its
SMALLER child, until the min-heap property holds again.
When it's used: right after removing the root (the min) and
moving the last leaf into its place — that displaced element
needs to sink down to where it actually belongs.
- get_smaller_child: assistant to the sift_down function
*/
static int	get_smaller_child(t_simulation *simul, t_heap *heap,
	int i)
{
	int		smaller_i;
	int		left_i;
	int		right_i;
	long	smaller_priority;

	left_i = 2 * i + 1;
	right_i = 2 * i + 2;
	smaller_i = i;
	smaller_priority = calculate_priority(simul, heap->data[smaller_i]);
	if (left_i < heap->size
		&& calculate_priority(simul, heap->data[left_i]) < smaller_priority)
	{
		smaller_i = left_i;
		smaller_priority = calculate_priority(simul, heap->data[left_i]);
	}
	if (right_i < heap->size
		&& calculate_priority(simul, heap->data[right_i]) < smaller_priority)
		smaller_i = right_i;
	return (smaller_i);
}

static void	sift_down(t_simulation *simul, t_heap *heap, int i)
{
	int				smaller_i;

	while (1)
	{
		smaller_i = get_smaller_child(simul, heap, i);
		if (smaller_i == i)
			return ;
		ft_swap(&heap->data[i], &heap->data[smaller_i]);
		i = smaller_i;
	}
}

/*
Add an elements from the heap.
push calls sift_up (new node climbs to its correct spot).
*/
int	min_heap_push(t_simulation *simul, t_heap *heap, t_coder *new_coder)
{
	if (heap->size >= simul->number_of_coders)
	{
		fprintf(stderr, "ERROR: Heap is full.\n");
		return (1);
	}
	heap->data[heap->size] = new_coder;
	heap->size++;
	sift_up(simul, heap, heap->size - 1);
	return (0);
}

/*
Remove an elements from the heap and return it.
The one to return is always the first one in the array since
its already sorted.
pop calls sift_down (root replacement sinks to its correct spot).
*/
t_coder	*min_heap_pop(t_simulation *simul, t_heap *heap)
{
	t_coder	*last_element;
	t_coder	*first_element;

	if (heap->size <= 0)
	{
		fprintf(stderr, "ERROR: Heap is already empty.\n");
		return (NULL);
	}
	last_element = heap->data[heap->size - 1];
	first_element = heap->data[0];
	heap->data[0] = last_element;
	heap->size--;
	sift_down(simul, heap, 0);
	return (first_element);
}
