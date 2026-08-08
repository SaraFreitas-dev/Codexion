/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 22:31:26 by sarfreit          #+#    #+#             */
/*   Updated: 2026/08/03 22:31:26 by sarfreit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdbool.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct dongle
{
	int					dongle_id;
	bool				is_available;
	long				released_at_ms; // For the cooldown
	pthread_mutex_t		lock;
}	t_dongle;

typedef struct coder
{
	int					coder_id;
	int					times_compiled;
	long				last_compile_start_ms; // For burnout and deadline (EDF)
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	pthread_t			thread; // To join the thread
	struct simulation	*simul;
}	t_coder;

typedef struct simulation
{
	int					number_of_coders;
	int					time_to_compile;
	int					time_to_burnout;
	int					dongle_cooldown;
	int					time_to_debug;
	int					time_to_refactor;
	int					number_of_compiles_required;
	char				*scheduler;
	t_coder				*coders;
	t_dongle			*dongles;
	long				start_time_ms;
	bool				should_stop; // Shared flag
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
}	t_simulation;

typedef enum event_type
{
	DONGLE_TAKEN,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNED_OUT
}	t_event;

char	**verify_args(int argc, char **argv);

#endif