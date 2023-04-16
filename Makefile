NAME = webserv

CC = @c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

CPP_FILES = server.cpp\
			 config/config.cpp\
			 config/Location.cpp\
			 config/server.cpp\
			 response/response.cpp\
			 response/getmethod.cpp\
			 response/deletemethod.cpp

all : $(NAME)

$(NAME) : $(CPP_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES) -o $(NAME)

clean :
	@rm -rf $(NAME)

re : clean all