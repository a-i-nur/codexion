/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:17:34 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_data(t_simulation *simulation, t_args args)
{
	if (init_simulation(simulation, args) != 0)
		return (1);
	if (init_dongles(simulation) != 0
		|| init_request_heap(simulation) != 0
		|| init_coders(simulation) != 0)
	{
		destroy_simulation(simulation);
		return (1);
	}
	return (0);
}

static int	run_simulation(t_simulation *simulation)
{
	if (start_coder_threads(simulation) != 0)
		return (1);
	if (start_monitor_thread(simulation) != 0)
	{
		stop_simulation(simulation);
		join_coder_threads(simulation);
		return (1);
	}
	start_simulation(simulation);
	join_coder_threads(simulation);
	stop_simulation(simulation);
	join_monitor_thread(simulation);
	return (0);
}

int	main(int argc, char **argv)
{
	t_args			args;
	t_simulation	simulation;
	int				status;

	args = new_args();
	parse_args(argc, argv, &args);
	if (args.error_type != 0)
	{
		error_message(args.error_type);
		return (1);
	}
	if (init_data(&simulation, args) != 0)
		return (1);
	status = run_simulation(&simulation);
	destroy_simulation(&simulation);
	return (status);
}
