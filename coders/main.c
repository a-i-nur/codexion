/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/01 17:09:35 by aakhmeto         ###   ########.fr       */
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
	(void)simulation;
	return (0);
}
