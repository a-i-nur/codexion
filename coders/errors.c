/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 22:35:46 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/05/23 23:52:48 by aakhmeto         ###   ########.fr       */
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
	printf("Usage: ./codexion [num1] [num2] [num3] "
		"[num4] [num5] [num6] [num7] [scheduler]\n");
}
