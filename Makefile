NAME = Webserv

SRC = ./MainInc/main.cpp \
	./tools/tools.cpp \
	./parsing/ConfigurationParser.cpp \
	./Request/Request.cpp \
	./parsing/debug.cpp \
	./parsing/libcpp.cpp \
	./Servers/Servers.cpp \
	./global_interface/Webserv.cpp \
	./Response/Response.cpp \
	./Response/utils.cpp \
	./Response/cgi.cpp \
	./Response/methods/get.cpp \
	./Response/methods/post.cpp \
	./Response/methods/delete.cpp

OBJ = $(SRC:.cpp=.o)

CC = clang++

FLAGS = -std=c++98 -g3 -fsanitize=address -Wall -Wextra -Werror

all: $(NAME)

$(NAME): 
	$(CC) $(FLAGS) $(SRC) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re