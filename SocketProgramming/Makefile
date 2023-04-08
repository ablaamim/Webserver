NAME = TcpClient
NAME1 = TcpServer

SRC = tcpClient.c \
	common.c

SRC1 = tcpServer.c \
	common.c

OBJ = $(SRC:.c=.o)

OBJ1 = $(SRC1:.c=.o)

CC = gcc

CFLAGS = #-Wall -Wextra -Werror -g

RM = rm -f

all: $(NAME) $(NAME1)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

$(NAME1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(NAME1) $(OBJ1)

clean:
	$(RM) $(OBJ) $(OBJ1)

fclean: clean
	$(RM) $(NAME) $(NAME1)

re: fclean all

.PHONY: all clean fclean re