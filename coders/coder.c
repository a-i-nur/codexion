/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:37:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 13:58:18 by aakhmeto         ###   ########.fr       */
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
		simulation->coders[i].left_dongle = &simulation->dongles[i];
		simulation->coders[i].right_dongle = &simulation->dongles[
			(i + 1) % simulation->args.num_of_coders];
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

void	update_last_compile_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
}

void	increment_compiles_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->state_mutex);
}

long	get_last_compile_start(t_coder *coder)
{
	long	last_compile_start;

	pthread_mutex_lock(&coder->state_mutex);
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	return (last_compile_start);
}

int	get_compiles_done(t_coder *coder)
{
	int	compiles_done;

	pthread_mutex_lock(&coder->state_mutex);
	compiles_done = coder->compiles_done;
	pthread_mutex_unlock(&coder->state_mutex);
	return (compiles_done);
}
