NAME = Webserv

SRC = main.cpp \
	parsing.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

CC = clang++

CFLAGS = #-Wall -Wextra -Werror

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

