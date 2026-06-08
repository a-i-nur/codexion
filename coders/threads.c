/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 12:29:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:50:44 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	while (!simulation_stopped(coder->simulation)
	&& get_compiles_done(coder)
	< coder->simulation->args.num_of_compiles_req)
	{
		if (!acquire_dongles(coder))
			return (NULL);
		update_last_compile_start(coder);
		log_message(coder->simulation, coder->id, "is compiling");
		precise_sleep(coder->simulation->args.time_to_compile);
		increment_compiles_done(coder);
		release_dongles(coder);
		if (simulation_stopped(coder->simulation))
			return (NULL);
		log_message(coder->simulation, coder->id, "is debugging");
		precise_sleep(coder->simulation->args.time_to_debug);
		if (simulation_stopped(coder->simulation))
			return (NULL);
		log_message(coder->simulation, coder->id, "is refactoring");
		precise_sleep(coder->simulation->args.time_to_refactor);
	}
	return (NULL);
}

int	start_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		if (pthread_create(&simulation->coders[i].thread, NULL,
			&coder_routine, &simulation->coders[i]) != 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

void	join_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
}

