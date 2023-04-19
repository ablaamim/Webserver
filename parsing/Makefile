NAME = Webserv

SRC = main.cpp \
	ConfigurationParser.cpp \
	debug.cpp \
	libcpp.cpp \
	../Servers/Servers.cpp \
	../Kqueue/Kqueue.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

CC = clang++

CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3 -std=c++98 #-O2 -Werror=format-security -D_GLIBCXX_ASSERTIONS

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

