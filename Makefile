NAME = webserv

CC = @c++

CFLAGS =  -Wall -Wextra -Werror -std=c++98

CPP_FILES = webserv.cpp\
			config/config.cpp\
			config/Location.cpp\
			config/server.cpp\
			response/response.cpp\
			response/getmethod.cpp\
			response/deletemethod.cpp\
			response/postmethod.cpp\
			cgi-bin/HandleCgi.cpp

all : $(NAME)

$(NAME) : $(CPP_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES) -o $(NAME)

clean :
	@rm -rf $(NAME)

re : clean all