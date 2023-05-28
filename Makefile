NAME = Webserv

SRC = ./MainInc/main.cpp \
	./tools/tools.cpp \
	./parsing/ConfigurationParser.cpp \
	./Request/Request.cpp \
	./parsing/debug.cpp \
	./parsing/libcpp.cpp \
	./Servers/Servers.cpp \
	./global_interface/Webserv.cpp \
	./response/Response.cpp \
	./response/utils.cpp \
	./response/checkRequest.cpp \
	./response/debug.cpp \
	./response/resource.cpp \
	./response/sendResponse.cpp \
	./response/methods/get.cpp \
	./response/methods/post.cpp \
	./response/methods/delete.cpp \
	

OBJ = $(SRC:.cpp=.o)

CC = clang++

FLAGS = -std=c++11 -g3 #-fsanitize=address #-Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re