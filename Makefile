NAME = Webserv

SRC = ./MainInc/main.cpp \
	./parsing/ConfigurationParser.cpp \
	./parsing/debug.cpp \
	./parsing/libcpp.cpp \
	./Servers/Servers.cpp \
	./global_interface/Webserv.cpp \

OBJ = $(SRC:.cpp=.o)

CC = clang++

all: $(NAME)

CFLAGS =  -std=c++98 -Wall -Wextra -Werror #-fsanitize=address -g3

$(NAME): 
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all