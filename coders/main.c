/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:08:41 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_args			args;
	t_simulation	simulation;

	args = new_args();
	parse_args(argc, argv, &args);
	if (args.error_type != 0)
	{
		error_message(args.error_type);
		return (1);
	}
	simulation = new_simulation(args);
	if (init_dongles(&simulation) != 0)
	{
		destroy_simulation(&simulation);
		return (1);
	}
	if (init_request_heap(&simulation) != 0)
	{
		destroy_simulation(&simulation);
		return (1);
	}
	if (init_coders(&simulation) != 0)
	{
		destroy_simulation(&simulation);
		return (1);
	}
	if (start_coder_threads(&simulation) != 0)
	{
		destroy_simulation(&simulation);
		return (1);
	}
	join_coder_threads(&simulation);
	destroy_simulation(&simulation);
	return (0);
}
