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

// Get the correct str event from the enum list
static const char	*get_event_str(t_event event)
{
	static const char	*event_strings[5] = {
		"has taken a dongle",
		"is compiling",
		"is debugging",
		"is refactoring",
		"burned out"};

	return (event_strings[event]);
}
