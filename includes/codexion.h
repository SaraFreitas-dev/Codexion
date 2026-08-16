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

typedef struct s_heap
{
	struct coder		**data;
	int					size;
}	t_heap;

typedef struct dongle
{
	int					dongle_id;
	bool				is_available;
	long				released_at_ms; // For the cooldown
	pthread_mutex_t		lock;
	t_heap				heap; // Waiting list of coders
	pthread_cond_t		cond;
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
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					number_of_compiles_required;
	int					dongle_cooldown;
	char				*scheduler;
	long				start_time_ms;
	bool				should_stop; // Shared flag
	t_dongle			*dongles;
	t_coder				*coders;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
}	t_simulation;

typedef enum event_type
{
	DONGLE_TAKEN,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNED_OUT,
	EVENT_COUNT
}	t_event;

//_____________________PARSER.C_____________________
char	**verify_args(int argc, char **argv);

//______________________INIT.C_______________________
int		init_simulation(t_simulation *simul, char **args);
void	cleanup_simulation(t_simulation *simul);

//______________________LOG.C_______________________
void	print_log(t_simulation *simul, t_coder *coder, t_event event);

//____________________SCHEDULER.C____________________
long	calculate_priority(t_simulation *simul, t_coder *coder);

//_____________________UTILS.C______________________
long	get_time_ms(void);
void	ft_swap(t_coder **a, t_coder **b);

//______________________HEAP.C______________________
void	sift_up(t_simulation *simul, t_heap *heap, int i);
void	sift_down(t_simulation *simul, t_heap *heap, int i);
int		min_heap_push(t_simulation *simul, t_heap *heap, t_coder *new_coder);
t_coder	*min_heap_pop(t_simulation *simul, t_heap *heap);

#endif