/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:14:24 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/02 13:31:21 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_message(t_simulation *simulation, int coder_id, char *message)
{
	long	timestamp;
	// current time - simulation start time
	// How many milliseconds passed since simulation started?
	timestamp = get_elapsed_ms(simulation->start_time);
	pthread_mutex_lock(&simulation->log_mutex);
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&simulation->log_mutex);
}
