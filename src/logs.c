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

// Print by each event: timestamp_in_ms X has taken a dongle
// Lock/unlock the mutex to prevent threads from printing at the same time
void	print_log(t_simulation *simul, t_coder *coder, t_event event)
{
	long		timestamp_in_ms;
	int			coder_id;
	const char	*event_type;

	if (!coder || !simul)
	{
		fprintf(stderr, "ERROR: Failed to print a log.\n");
		return ;
	}
	timestamp_in_ms = get_curr_timestamp(simul);
	coder_id = coder->coder_id;
	event_type = get_event_str(event);
	if ((timestamp_in_ms == -1) || event_type == NULL)
	{
		fprintf(stderr, "ERROR: Failed to print a log.\n");
		return ;
	}
	pthread_mutex_lock(&simul->log_lock);
	printf("%ld %d %s\n", timestamp_in_ms, coder_id, event_type);
	pthread_mutex_unlock(&simul->log_lock);
}
