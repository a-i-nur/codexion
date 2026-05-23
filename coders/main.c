/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/23 23:44:59 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_args	args;

	args = new_args();
	parse_args(argc, argv, &args);
	if (args.error_type != 0)
	{
		error_message(args.error_type);
		return (1);
	}
	return (0);
}
