/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_thread.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:03:53 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:14:43 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_monitor_thread(t_simulation *simulation)
{
	return (pthread_create(&simulation->monitor_thread, NULL,
			monitor_routine, simulation));
}

void	join_monitor_thread(t_simulation *simulation)
{
	pthread_join(simulation->monitor_thread, NULL);
}
