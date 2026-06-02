/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 22:35:46 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/27 14:24:09 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	error_message(int error_type)
{
	if (error_type == ERROR_INVALID_ARGS)
		printf("Error: Invalid number of arguments.\n");
	else if (error_type == ERROR_EMPTY_ARG)
		printf("Error: Empty argument detected.\n");
	else if (error_type == ERROR_INVALID_NUMBER)
		printf("Error: Time values and number of "
			"compiles required (must be int > 0).\n");
	else if (error_type == ERROR_NUM_OF_CODERS)
		printf("Error: Number of coders (must be int > 0).\n");
	else if (error_type == ERROR_DONGLE_COOLDOWN)
		printf("Error: Dongle cooldown (must be int >= 0).\n");
	else if (error_type == ERROR_INVALID_SCHEDULER)
		printf("Error: Invalid scheduler specified (must be fifo or edf).\n");
	else
		printf("Error: Unknown error type.\n");
	printf("Usage: ./codexion [num_of_coders int>0] [burnout_ms int>0] "
		"[compile_ms int>0] [debug_ms int>0] [refactor_ms int>0] "
		"[required_compiles int>0] [cooldown_ms int>=0] "
		"[scheduler fifo|edf]\n");
}
