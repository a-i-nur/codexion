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
	coders/simulation.c

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

test_errors: $(NAME)
	-./$(NAME)
	-./$(NAME) 4 800 200 200 200 3 10
	-./$(NAME) 4 800 200 200 200 3 10 fifo extra
	-./$(NAME) 4 800 200 200 200 3 10 bad
	-./$(NAME) 4 800 200 200 200 3 10 FIFO
	-./$(NAME) 4 800 200 200 200 3 10 edf2
	-./$(NAME) 4 -800 200 200 200 3 10 fifo
	-./$(NAME) -4 800 200 200 200 3 10 fifo
	-./$(NAME) 4 800x 200 200 200 3 10 fifo
	-./$(NAME) 4 800 20.5 200 200 3 10 fifo
	-./$(NAME) 4 800 +200 200 200 3 10 fifo
	-./$(NAME) 4 800 "" 200 200 3 10 fifo
	-./$(NAME) 0 800 200 200 200 3 10 fifo
	-./$(NAME) 4 0 200 200 200 3 10 fifo
	-./$(NAME) 4 800 0 200 200 3 10 fifo
	-./$(NAME) 4 800 200 0 200 3 10 fifo
	-./$(NAME) 4 800 200 200 0 3 10 fifo
	-./$(NAME) 4 800 200 200 200 0 10 fifo
	-./$(NAME) 4 800 200 200 200 3 -10 fifo
	-./$(NAME) 2147483648 800 200 200 200 3 10 fifo

test_edge: $(NAME)
	./$(NAME) 1 200 100 100 100 1 0 fifo
	./$(NAME) 2 200 200 200 200 2 0 fifo
	./$(NAME) 2 800 200 200 200 2 0 fifo
	./$(NAME) 4 800 200 200 200 3 10 fifo
	./$(NAME) 4 800 200 200 200 3 10 edf
	./$(NAME) 10 350 100 100 100 5 10 edf
	./$(NAME) 200 800 60 60 60 1 0 fifo

leaks: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) 4 800 200 200 200 3 10 fifo

.PHONY: all clean fclean re run run_fifo run_edf run_burnout test_errors test_edge leaks
