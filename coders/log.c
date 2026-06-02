/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:14:24 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/01 16:44:56 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_message(long start_time, int coder_id, char *message)
{
	// current time - simulation start time
	long	timestamp;

	// How many milliseconds passed since simulation started?
	timestamp = get_elapsed_ms(start_time);
	printf("%ld %d %s\n", timestamp, coder_id, message);
}