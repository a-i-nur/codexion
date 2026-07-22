NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRCS = coders/main.c \
	coders/args.c \
	coders/errors.c \
	coders/parse.c \
	coders/parse_utils.c \
	coders/time.c \
	coders/log.c \
	coders/simulation.c \
	coders/simulation_start.c \
	coders/simulation_stop.c \
	coders/coder.c \
	coders/coder_state.c \
	coders/threads.c \
	coders/dongle.c \
	coders/dongle_state.c \
	coders/dongle_acquire.c \
	coders/scheduler.c \
	coders/scheduler_remove.c \
	coders/scheduler_priority.c \
	coders/monitor.c \
	coders/monitor_thread.c

OBJS = $(SRCS:.c=.o)

HEADER = coders/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME) 2 800 200 200 200 2 0 fifo

run_fifo: $(NAME)
	./$(NAME) 4 800 200 200 200 3 10 fifo

run_edf: $(NAME)
	./$(NAME) 4 800 200 200 200 3 10 edf

run_burnout: $(NAME)
	./$(NAME) 2 200 200 200 200 2 0 fifo

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) 5 800 200 200 200 3 10 fifo

helgrind: $(NAME)
	valgrind --tool=helgrind ./$(NAME) 5 800 200 200 200 3 10 fifo

#stress: $(NAME)
#	python3 tests/stress.py

.PHONY: all clean fclean re run run_fifo run_edf run_burnout leaks helgrind stress
