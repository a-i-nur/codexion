/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:20:54 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:16:30 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
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
typedef struct s_dongle
{
	int				id;
	int				available;
	long			cooldown_until;
	pthread_mutex_t	mutex;
}	t_dongle;
typedef struct s_coder		t_coder;
typedef struct s_request
{
	t_coder	*coder;
	long	arrival_order;
	long	deadline;
	int		considered;
	int		selected;
}	t_request;
typedef struct s_request_heap
{
	t_request	*requests;
	int			size;
	int			capacity;
	long		next_arrival_order;
}	t_request_heap;
typedef struct s_simulation	t_simulation;
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
	int				ready;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
	t_request_heap	request_heap;
	pthread_t		monitor_thread;
	int				started_threads;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;
}	t_simulation;

/* Returns a t_args structure filled with default values. */
t_args			new_args(void);
/* Parses command-line arguments and stores an error type on failure. */
void			parse_args(int argc, char **argv, t_args *args);
/* Converts a numeric string to int. Returns -1 on invalid input or overflow. */
int				ft_str_to_num(const char *num);
/* Returns the length of str. */
size_t			ft_strlen(const char *str);
/* Compares up to n characters. Returns 0 when strings are equal. */
int				ft_strncmp(const char *s1, const char *s2, size_t n);
/* Prints the message matching error_type. */
void			error_message(int error_type);

/* Returns the current time in milliseconds. */
long			get_time_ms(void);
/* Returns milliseconds elapsed since start_time. */
long			get_elapsed_ms(long start_time);
/* Sleeps for duration_ms milliseconds with short usleep chunks. */
void			precise_sleep(long duration_ms);
/* Sleeps for duration_ms or until the simulation stops. */
void			simulation_sleep(t_simulation *simulation,
					long duration_ms);

/* Prints a synchronized timestamped log message unless simulation stopped. */
void			log_message(t_simulation *simulation,
					int coder_id, char *message);
/* Prints a synchronized burnout message and stops further normal logs. */
void			log_burnout(t_simulation *simulation, int coder_id);

/* Initializes simulation-wide state. Returns 0 on success, 1 on error. */
int				init_simulation(t_simulation *simulation, t_args args);
/* Returns 1 if the simulation stop flag is set, otherwise 0. */
int				simulation_stopped(t_simulation *simulation);
/* Sets the stop flag and wakes scheduler waiters. */
void			stop_simulation(t_simulation *simulation);
/* Sets the common start time and wakes all waiting threads. */
void			start_simulation(t_simulation *simulation);
/* Waits for the start gate. Returns 1 if work may start, otherwise 0. */
int				wait_for_simulation_start(t_simulation *simulation);
/* Destroys simulation resources allocated during initialization. */
void			destroy_simulation(t_simulation *simulation);

/* Initializes coders and mutexes. Returns 0 on success, 1 on error. */
int				init_coders(t_simulation *simulation);
/* Destroys coder mutexes and frees the coder array. */
void			destroy_coders(t_simulation *simulation);

/* Initializes dongles and mutexes. Returns 0 on success, 1 on error. */
int				init_dongles(t_simulation *simulation);
/* Destroys dongle mutexes and frees the dongle array. */
void			destroy_dongles(t_simulation *simulation);
/* Waits for and takes two dongles. Returns 1 on success, 0 on stop/error. */
int				acquire_dongles(t_coder *coder);
/* Releases a coder's dongles and wakes scheduler waiters. */
void			release_dongles(t_coder *coder);
/* Takes both dongles if available and cooled down. Returns 1 on success. */
int				take_dongles_if_usable(t_coder *coder);

/* Updates coder's last compile start time. */
void			update_last_compile_start(t_coder *coder);
/* Increments coder's completed compile count. */
void			increment_compiles_done(t_coder *coder);
/* Returns coder's last compile start time. */
long			get_last_compile_start(t_coder *coder);
/* Returns coder's completed compile count. */
int				get_compiles_done(t_coder *coder);

/* Entry point for a coder thread. Returns NULL when the thread exits. */
void			*coder_routine(void *data);
/* Creates all coder threads. Returns 0 on success, 1 on error. */
int				start_coder_threads(t_simulation *simulation);
/* Joins all successfully started coder threads. */
void			join_coder_threads(t_simulation *simulation);

/* Initializes the scheduler request heap. Returns 0 on success, 1 on error. */
int				init_request_heap(t_simulation *simulation);
/* Frees the scheduler request heap. */
void			destroy_request_heap(t_simulation *simulation);
/* Inserts a coder request into the heap. Returns 0 on success, 1 on error. */
int				request_heap_insert(t_simulation *simulation,
					t_coder *coder);
/* Removes a coder's request from the heap if present. */
void			request_heap_remove_coder(t_simulation *simulation,
					t_coder *coder);
/* Returns 1 if coder is selected by the scheduler, otherwise 0. */
int				request_heap_coder_has_priority(
					t_simulation *simulation, t_coder *coder);
/* Returns 1 if first request has priority over second, otherwise 0. */
int				request_has_priority(t_simulation *simulation,
					t_request *first, t_request *second);

/* Entry point for the monitor thread. Returns NULL when the thread exits. */
void			*monitor_routine(void *data);
/* Creates the monitor thread. Returns 0 on success, 1 on error. */
int				start_monitor_thread(t_simulation *simulation);
/* Joins the monitor thread. */
void			join_monitor_thread(t_simulation *simulation);

#endif
