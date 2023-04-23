NAME = Webserver

CC = g++

CFLAGS = #-Wall -Wextra -Werror -std=c++98

SRC = ./Networking/Sockets/binding_socket.cpp \
	Networking/Sockets/simple_socket.cpp \
	Networking/Sockets/main.cpp \
	Networking/Sockets/listening_socket.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
