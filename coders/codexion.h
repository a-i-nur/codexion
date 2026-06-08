/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:54 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:44:17 by aakhmeto         ###   ########.fr       */
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
typedef struct s_coder		t_coder;
typedef struct s_dongle
{
	int				id;
	int				available;
	long			cooldown_until;
	pthread_mutex_t	mutex;
}   t_dongle;
typedef struct s_request
{
	t_coder	*coder;
	long	arrival_order;
	long	deadline;
}	t_request;
typedef struct s_request_heap
{
	t_request	*requests;
	int			size;
	int			capacity;
	long		next_arrival_order;
}	t_request_heap;
typedef struct s_coder
{
	int				id;
	int				compiles_done;
	long			last_compile_start;
	t_simulation	*simulation;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
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
	t_dongle		*dongles;
	t_request_heap	request_heap;
	pthread_t		monitor_thread;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;
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
void			log_burnout(t_simulation *simulation, int coder_id);

t_simulation	new_simulation(t_args args);
int				simulation_stopped(t_simulation *simulation);
void			stop_simulation(t_simulation *simulation);
void			destroy_simulation(t_simulation *simulation);

int				init_coders(t_simulation *simulation);
void			destroy_coders(t_simulation *simulation);

int				init_dongles(t_simulation *simulation);
void			destroy_dongles(t_simulation *simulation);
int				acquire_dongles(t_coder *coder);
void			release_dongles(t_coder *coder);

void			update_last_compile_start(t_coder *coder);
void			increment_compiles_done(t_coder *coder);
long			get_last_compile_start(t_coder *coder);
int				get_compiles_done(t_coder *coder);

void			*coder_routine(void *data);
int				start_coder_threads(t_simulation *simulation);
void			join_coder_threads(t_simulation *simulation);

int				init_request_heap(t_simulation *simulation);
void			destroy_request_heap(t_simulation *simulation);
int				request_heap_insert(t_simulation *simulation,
					t_coder *coder);
t_coder			*request_heap_remove(t_simulation *simulation);
t_coder			*request_heap_peek(t_simulation *simulation);

void			*monitor_routine(void *data);
int				start_monitor_thread(t_simulation *simulation);
void			join_monitor_thread(t_simulation *simulation);

#endif
