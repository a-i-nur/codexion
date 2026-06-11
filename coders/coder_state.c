/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:11:30 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 15:11:34 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	update_last_compile_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
}

void	increment_compiles_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->state_mutex);
}

long	get_last_compile_start(t_coder *coder)
{
	long	last_compile_start;

	pthread_mutex_lock(&coder->state_mutex);
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	return (last_compile_start);
}

int	get_compiles_done(t_coder *coder)
{
	int	compiles_done;

	pthread_mutex_lock(&coder->state_mutex);
	compiles_done = coder->compiles_done;
	pthread_mutex_unlock(&coder->state_mutex);
	return (compiles_done);
}
