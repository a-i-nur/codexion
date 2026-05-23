/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 22:35:49 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/23 22:36:18 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_args	new_args(void)
{
	t_args	args;

	args.num_of_coders = 0;
	args.time_to_burnout = 0;
	args.time_to_compile = 0;
	args.time_to_debug = 0;
	args.time_to_refactor = 0;
	args.num_of_compiles_req = 0;
	args.dongle_cooldown = 0;
	args.scheduler = NULL;
	args.error_type = 0;
	return (args);
}
