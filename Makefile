NAME = Webserv

SRC = ./MainInc/main.cpp \
	./tools/tools.cpp \
	./parsing/ConfigurationParser.cpp \
	./Request/Request.cpp \
	./parsing/libcpp.cpp \
	./Servers/Servers.cpp \
	./global_interface/Webserv.cpp \
	./global_interface/matchLocation.cpp \
	./response/Response.cpp \
	./response/utils.cpp \
	./response/init.cpp \
	./response/directory.cpp \
	./response/debug.cpp \
	./response/error.cpp \
	./response/serve.cpp \
	./response/redirection.cpp \
	./response/mimeTypes.cpp \
	./response/methods/get.cpp \
	./response/methods/post.cpp \
	./response/methods/delete.cpp \
	./response/cgi/CGIManager.cpp \
	./response/cgi/utils.cpp \
	
CC = g++

CPPFLAGS =  -std=c++98  -fsanitize=address  -g3 -Wall -Wextra -Werror 

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)
	@echo "\033[32m[Webserv] \033[0m\033[1;32mCompilation done\033[0m"

clean:
	@rm -rf $(OBJ)
	@echo "\033[32m[Webserv] \033[0m\033[1;32mObjects removed\033[0m"

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[32m[Webserv] \033[0m\033[1;32mExecutable removed\033[0m"

re: fclean all

.PHONY: all clean fclean re