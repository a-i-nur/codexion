/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 14:44:45 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:14:34 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_coders_finished(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		if (get_compiles_done(&simulation->coders[i])
			< simulation->args.num_of_compiles_req)
			return (0);
		i++;
	}
	return (1);
}

static int	find_burned_out_coder(t_simulation *simulation)
{
	int		i;
	long	elapsed;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		elapsed = get_time_ms()
			- get_last_compile_start(&simulation->coders[i]);
		if (get_compiles_done(&simulation->coders[i])
			< simulation->args.num_of_compiles_req
			&& elapsed >= simulation->args.time_to_burnout)
			return (i);
		i++;
	}
	return (-1);
}

static void	wake_scheduler(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->scheduler_mutex);
	pthread_cond_broadcast(&simulation->scheduler_cond);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
}

void	*monitor_routine(void *data)
{
	t_simulation	*simulation;
	int				burned_out;

	simulation = (t_simulation *)data;
	if (!wait_for_simulation_start(simulation))
		return (NULL);
	while (!simulation_stopped(simulation))
	{
		if (all_coders_finished(simulation))
		{
			stop_simulation(simulation);
			return (NULL);
		}
		burned_out = find_burned_out_coder(simulation);
		if (burned_out >= 0)
		{
			stop_simulation(simulation);
			log_burnout(simulation, simulation->coders[burned_out].id);
			return (NULL);
		}
		wake_scheduler(simulation);
		usleep(500);
	}
	return (NULL);
}
