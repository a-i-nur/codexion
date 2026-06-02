/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:53:37 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/01 17:04:37 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_simulation	new_simulation(t_args args)
{
	t_simulation	simulation;

	simulation.args = args;
	simulation.start_time = get_time_ms();
	return (simulation);
}