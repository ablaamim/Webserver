NAME = Parsing

SRC = main.cpp \
	ConfigurationParser.cpp \
	debug.cpp \
	libcpp.cpp \
	../Servers/Servers.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

CC = clang++

CFLAGS = -std=c++17 -fsanitize=address -g3  #-Wall -Wextra -Werror

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

