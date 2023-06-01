NAME = webserv

CC = @c++

CFLAGS =  -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

CPP_FILES = webserv.cpp\
			srcs/Config/Config.cpp\
			srcs/Config/Location.cpp\
			srcs/Server/Server.cpp\
			srcs/Config/Server_conf.cpp\
			srcs/Response/Response.cpp\
			srcs/Response/GetMethod.cpp\
			srcs/Response/DeleteMethod.cpp\
			srcs/Response/PostMethod.cpp\
			srcs/Request/Request.cpp \
			cgi-bin/HandleCgi.cpp\
			cgi-bin/cgi.cpp

all : $(NAME)

$(NAME) : $(CPP_FILES)
	$(CC) $(CFLAGS) $(CPP_FILES) -o $(NAME)

clean :
	@rm -rf $(NAME)

fclean:	clean

re : clean all