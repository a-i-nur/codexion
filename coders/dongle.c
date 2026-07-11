/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 14:33:26 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:12:16 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Destroys initialized dongle mutexes and frees the dongle array. */
static void	cleanup_dongles(t_simulation *simulation, int count)
{
	while (count > 0)
	{
		count--;
		pthread_mutex_destroy(&simulation->dongles[count].mutex);
	}
	free(simulation->dongles);
	simulation->dongles = NULL;
}

int	init_dongles(t_simulation *simulation)
{
	int	i;

	simulation->dongles = malloc(sizeof(t_dongle)
			* simulation->args.num_of_coders);
	if (simulation->dongles == NULL)
		return (1);
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		simulation->dongles[i].id = i + 1;
		simulation->dongles[i].available = 1;
		simulation->dongles[i].cooldown_until = 0;
		if (pthread_mutex_init(&simulation->dongles[i].mutex, NULL) != 0)
			return (cleanup_dongles(simulation, i), 1);
		i++;
	}
	return (0);
}

void	destroy_dongles(t_simulation *simulation)
{
	if (simulation->dongles == NULL)
		return ;
	cleanup_dongles(simulation, simulation->args.num_of_coders);
}
