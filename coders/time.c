/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 14:34:25 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/01 16:00:03 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	// This function will return the current time in milliseconds.
	// struct timeval is the type used by gettimeofday().
	// It contains:
	// tv.tv_sec   seconds
	// tv.tv_usec  microseconds
	struct timeval	tv;

	// Fill the variable tv with the current real time.
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	get_elapsed_ms(long start_time)
{
	return (get_time_ms() - start_time);
}

void precise_sleep(long duration_ms)
{
	long	start_time;

	start_time = get_time_ms();
	while (get_elapsed_ms(start_time) < duration_ms)
		usleep(500);
}