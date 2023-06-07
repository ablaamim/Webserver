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
	./response/error.cpp \
	./response/sendResponse.cpp \
	./response/methods/get.cpp \
	./response/methods/post.cpp \
	./response/methods/delete.cpp \
	


CC = clang++

#CFLAGS =  -std=c++98 #-Wall -Wextra -Werror #-g3 -fsanitize=address

CFLAGS =  -std=c++98  -g3 #-fsanitize=address

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME):
	@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
	@echo "\033[32m[Webserv] Compiled\033[0m"

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)
	@echo "\033[32m[Webserv] Cleaned\033[0m"

fclean: clean
		@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re