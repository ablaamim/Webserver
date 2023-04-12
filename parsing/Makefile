NAME = Parsing

SRC = main.cpp \
	ConfigurationParser.cpp \
	debug.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

CC = clang++

CFLAGS = -fsanitize=address -g3 #-std=c++11 #-Wall -Wextra -Werror

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

