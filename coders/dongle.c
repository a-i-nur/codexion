/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 14:33:26 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 13:51:43 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_simulation *simulation)
{
	int i;

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
		{
			while (i-- > 0)
				pthread_mutex_destroy(&simulation->dongles[i].mutex);
			free(simulation->dongles);
			simulation->dongles = NULL;
			return (1);
		}
		i++;
	}
	return (0);
}

void	destroy_dongles(t_simulation *simulation)
{
	int	i;

	if (simulation->dongles == NULL)
		return ;
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		i++;
	}
	free(simulation->dongles);
	simulation->dongles = NULL;
}
