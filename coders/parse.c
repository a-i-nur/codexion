/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 12:52:22 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/23 23:04:11 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	parse_first_4_nums(t_args *args, char **argv)
{
	args->num_of_coders = ft_str_to_num(argv[1]);
	if (args->num_of_coders <= 0)
	{
		args->error_type = ERROR_NUM_OF_CODERS;
		return ;
	}
	args->time_to_burnout = ft_str_to_num(argv[2]);
	if (args->time_to_burnout < 1)
	{
		args->error_type = ERROR_INVALID_NUMBER;
		return ;
	}
	args->time_to_compile = ft_str_to_num(argv[3]);
	if (args->time_to_compile < 1)
	{
		args->error_type = ERROR_INVALID_NUMBER;
		return ;
	}
	args->time_to_debug = ft_str_to_num(argv[4]);
	if (args->time_to_debug < 1)
	{
		args->error_type = ERROR_INVALID_NUMBER;
		return ;
	}
}

void	parse_last_3_nums(t_args *args, char **argv)
{
	args->time_to_refactor = ft_str_to_num(argv[5]);
	if (args->time_to_refactor < 1)
	{
		args->error_type = ERROR_INVALID_NUMBER;
		return ;
	}
	args->num_of_compiles_req = ft_str_to_num(argv[6]);
	if (args->num_of_compiles_req < 1)
	{
		args->error_type = ERROR_INVALID_NUMBER;
		return ;
	}
	args->dongle_cooldown = ft_str_to_num(argv[7]);
	if (args->dongle_cooldown < 0)
	{
		args->error_type = ERROR_DONGLE_COOLDOWN;
		return ;
	}
}

void	parse_scheduler(t_args *args, char **argv)
{
	if (ft_strncmp(argv[8], "fifo", 4) == 0 && ft_strlen(argv[8]) == 4)
		args->scheduler = "fifo";
	else if (ft_strncmp(argv[8], "edf", 3) == 0 && ft_strlen(argv[8]) == 3)
		args->scheduler = "edf";
	else
	{
		args->error_type = ERROR_INVALID_SCHEDULER;
		return ;
	}
}

void	parse_args(int argc, char **argv, t_args *args)
{
	int		i;

	i = 1;
	if (argc != 9)
	{
		args->error_type = ERROR_INVALID_ARGS;
		return ;
	}
	while (i <= 8)
	{
		if (argv[i] == NULL || argv[i][0] == '\0')
		{
			args->error_type = ERROR_EMPTY_ARG;
			return ;
		}
		i++;
	}
	parse_first_4_nums(args, argv);
	if (args->error_type != 0)
		return ;
	parse_last_3_nums(args, argv);
	if (args->error_type != 0)
		return ;
	parse_scheduler(args, argv);
}
