/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:54 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/02 16:17:23 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>

# define ERROR_INVALID_ARGS -1
# define ERROR_EMPTY_ARG -2
# define ERROR_INVALID_NUMBER -3
# define ERROR_NUM_OF_CODERS -4
# define ERROR_DONGLE_COOLDOWN -5
# define ERROR_INVALID_SCHEDULER -6

typedef struct s_args
{
	int		num_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		num_of_compiles_req;
	int		dongle_cooldown;
	char	*scheduler;
	int		error_type;
}	t_args;
typedef struct s_simulation	t_simulation;
typedef struct s_coder
{
	int				id;
	int				compiles_done;
	long			last_compile_start;
	t_simulation	*simulation;
	pthread_mutex_t	state_mutex;
}	t_coder;
typedef struct s_simulation
{
	t_args			args;
	long			start_time;
	int				stop;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	t_coder			*coders;
}	t_simulation;

t_args			new_args(void);
void			parse_args(int argc, char **argv, t_args *args);
int				ft_str_to_num(const char *num);
size_t			ft_strlen(const char *str);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
void			error_message(int error_type);

long			get_time_ms(void);
long			get_elapsed_ms(long start_time);
void			precise_sleep(long duration_ms);

void			log_message(t_simulation *simulation,
					int coder_id, char *message);

t_simulation	new_simulation(t_args args);
int				simulation_stopped(t_simulation *simulation);
void			stop_simulation(t_simulation *simulation);
void			destroy_simulation(t_simulation *simulation);

int				init_coders(t_simulation *simulation);
void			destroy_coders(t_simulation *simulation);

#endif
