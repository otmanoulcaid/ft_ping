NAME = ft_ping

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinc

SRC_DIR = src
OBJ_DIR = obj

SRCS = main.c parser.c dns.c socket.c icmp.c receiver.c stats.c utils.c
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: clean
