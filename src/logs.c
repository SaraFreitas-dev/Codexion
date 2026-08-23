/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 18:11:08 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/07 18:11:08 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Get the correct str event from the enum list (on the .h file)
const char	*get_event_str(t_event event)
{
	static const char	*event_strings[5] = {
		"has taken a dongle",
		"is compiling",
		"is debugging",
		"is refactoring",
		"burned out"};

	if (event < DONGLE_TAKEN || event >= EVENT_COUNT)
	{
		fprintf(stderr, "ERROR: Invalid event type.\n");
		return (NULL);
	}
	return (event_strings[event]);
}

// Get the current timestamp in milliseconds to use on the report
long	get_curr_timestamp(t_simulation *simul)
{
	long	elapsed_ms;
	long	now_ms;

	if (!simul)
	{
		fprintf(stderr, "ERROR: Cannot compute timestamp - simulation "
			"data is missing.\n");
		return (-1);
	}
	now_ms = get_time_ms();
	elapsed_ms = now_ms - simul->start_time_ms;
	return (elapsed_ms);
}

/*
Print a timestamped event while serializing access to stdout.
Normal events are ignored once the simulation has stopped.
*/
static void	print_timestamp(t_simulation *simul, t_event event,
	t_coder *coder, long timestamp_in_ms)
{
	const char	*event_type;

	event_type = get_event_str(event);
	if (!event_type)
		return ;
	pthread_mutex_lock(&simul->log_lock);
	if (event != BURNED_OUT && should_stop_now(simul))
	{
		pthread_mutex_unlock(&simul->log_lock);
		return ;
	}
	if (USE_COLOR && event == BURNED_OUT)
		printf("\033[31m%ld %d %s\033[0m\n",
			timestamp_in_ms, coder->coder_id, event_type);
	else
		printf("%ld %d %s\n", timestamp_in_ms, coder->coder_id, event_type);
	pthread_mutex_unlock(&simul->log_lock);
}

// Print by each event: timestamp_in_ms X has taken a dongle
// Lock/unlock the mutex to prevent threads from printing at the same time
void	print_log(t_simulation *simul, t_coder *coder, t_event event)
{
	long		timestamp_in_ms;

	if (!coder || !simul)
	{
		fprintf(stderr, "ERROR: Failed to print a log.\n");
		return ;
	}
	timestamp_in_ms = get_curr_timestamp(simul);
	if (timestamp_in_ms == -1)
	{
		fprintf(stderr, "ERROR: Failed to print a log.\n");
		return ;
	}
	print_timestamp(simul, event, coder, timestamp_in_ms);
}
