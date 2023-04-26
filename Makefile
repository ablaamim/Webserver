NAME = Webserv

SRC = ./MainInc/main.cpp \
	./parsing/ConfigurationParser.cpp \
	./parsing/debug.cpp \
	./parsing/libcpp.cpp \
	./Servers/Servers.cpp \
	./global_interface/Webserv.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

CC = g++

CFLAGS =  #-fsanitize=address -g3 -std=c++98 #-Wall -Wextra -Werror #-O2 -Werror=format-security -D_GLIBCXX_ASSERTIONS

##
# -D_GLIBCXX_ASSERTIONS enables additional C++ standard library hardening. 
# It is implemented in libstdc++ and described in the libstdc++ documentation.
##

##
# -Werror=format-security enables format string security checks.
##

##
# -O2 enables optimization.
##

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

