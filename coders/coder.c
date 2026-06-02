/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:37:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/02 16:21:19 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		simulation->coders[i].id = i + 1;
		simulation->coders[i].compiles_done = 0;
		simulation->coders[i].last_compile_start = simulation->start_time;
		simulation->coders[i].simulation = simulation;
		pthread_mutex_init(&simulation->coders[i].state_mutex, NULL);
		i++;
	}
	return (0);
}

void	destroy_coders(t_simulation *simulation)
{
	int	i;

	if (simulation->coders == NULL)
		return ;
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_mutex_destroy(&simulation->coders[i].state_mutex);
		i++;
	}
	free(simulation->coders);
	simulation->coders = NULL;
}
