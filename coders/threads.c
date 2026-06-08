/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 12:29:15 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/03 14:34:00 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	(void)coder;
	return (NULL);
}

int	start_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		if (pthread_create(&simulation->coders[i].thread, NULL,
			&coder_routine, &simulation->coders[i]) != 0)
		return (1);
		i++;
	}
	return (0);
}

void	join_coder_threads(t_simulation *simulation)
{
	int	i;

	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
}