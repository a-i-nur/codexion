/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 14:44:45 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:47:09 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor_routine(void *data)
{
	t_simulation	*simulation;
	int				i;

	simulation = (t_simulation *)data;
	while (!simulation_stopped(simulation))
	{
		i = 0;
		while (i < simulation->args.num_of_coders)
		{
			if (get_compiles_done(&simulation->coders[i])
				< simulation->args.num_of_compiles_req
				&& get_time_ms() - get_last_compile_start(
					&simulation->coders[i])
				>= simulation->args.time_to_burnout)
			{
				stop_simulation(simulation);
				log_burnout(simulation, simulation->coders[i].id);
				return (NULL);
			}
			i++;
		}
		usleep(500);
	}
	return (NULL);
}

int	start_monitor_thread(t_simulation *simulation)
{
	return (pthread_create(&simulation->monitor_thread, NULL,
			monitor_routine, simulation));
}

void	join_monitor_thread(t_simulation *simulation)
{
	pthread_join(simulation->monitor_thread, NULL);
}
