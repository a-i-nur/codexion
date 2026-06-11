/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 12:29:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 14:20:43 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	compile_code(t_coder *coder)
{
	if (!acquire_dongles(coder))
		return (0);
	update_last_compile_start(coder);
	log_message(coder->simulation, coder->id, "is compiling");
	simulation_sleep(coder->simulation,
		coder->simulation->args.time_to_compile);
	if (simulation_stopped(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	increment_compiles_done(coder);
	release_dongles(coder);
	return (!simulation_stopped(coder->simulation));
}

static int	debug_and_refactor(t_coder *coder)
{
	log_message(coder->simulation, coder->id, "is debugging");
	simulation_sleep(coder->simulation,
		coder->simulation->args.time_to_debug);
	if (simulation_stopped(coder->simulation))
		return (0);
	log_message(coder->simulation, coder->id, "is refactoring");
	simulation_sleep(coder->simulation,
		coder->simulation->args.time_to_refactor);
	return (!simulation_stopped(coder->simulation));
}

void	*coder_routine(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	if (!wait_for_simulation_start(coder->simulation))
		return (NULL);
	while (!simulation_stopped(coder->simulation)
		&& get_compiles_done(coder)
		< coder->simulation->args.num_of_compiles_req)
	{
		if (!compile_code(coder) || !debug_and_refactor(coder))
			return (NULL);
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
				coder_routine, &simulation->coders[i]) != 0)
		{
			stop_simulation(simulation);
			join_coder_threads(simulation);
			return (1);
		}
		simulation->started_threads++;
		i++;
	}
	return (0);
}

void	join_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->started_threads)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
	simulation->started_threads = 0;
}
