/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:37:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 14:27:19 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Initializes fixed fields for one coder. */
static void	init_coder_values(t_simulation *simulation, int i)
{
	simulation->coders[i].id = i + 1;
	simulation->coders[i].compiles_done = 0;
	simulation->coders[i].last_compile_start = simulation->start_time;
	simulation->coders[i].simulation = simulation;
	simulation->coders[i].left_dongle = &simulation->dongles[i];
	simulation->coders[i].right_dongle = &simulation->dongles[
		(i + 1) % simulation->args.num_of_coders];
}

/* Destroys initialized coder mutexes and frees the coder array. */
static void	cleanup_coders(t_simulation *simulation, int count)
{
	while (count > 0)
	{
		count--;
		pthread_mutex_destroy(&simulation->coders[count].state_mutex);
	}
	free(simulation->coders);
	simulation->coders = NULL;
}

int	init_coders(t_simulation *simulation)
{
	int	i;

	simulation->coders = (t_coder *)malloc(sizeof(t_coder)
			* simulation->args.num_of_coders);
	if (simulation->coders == NULL)
		return (1);
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		init_coder_values(simulation, i);
		if (pthread_mutex_init(&simulation->coders[i].state_mutex, NULL) != 0)
		{
			cleanup_coders(simulation, i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	destroy_coders(t_simulation *simulation)
{
	int	i;

	if (simulation->coders == NULL)
		return ;
	i = simulation->args.num_of_coders;
	cleanup_coders(simulation, i);
}
