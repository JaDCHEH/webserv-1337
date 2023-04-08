#ifndef SERVER_HPP
#define SERVER_HPP

#define PORT "8081"
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#define MAX_REQUEST_SIZE 2047

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <vector> // For storing connections
#include <algorithm> // For std::find
#include <set> // For managing file descriptors in select
#include <cstring> // For memset
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <map>
#include <cerrno>
#include "config/config.hpp"
#include "response/response.hpp"

class Request {
	public :
		std::string	method;
		std::string	path;
		std::string	http_version;
		Location	_location;
		Server		_server;
		string		_buffer;
		int			_first;
		size_t		_size_to_write;
		size_t		_amount_written;
		int			_fd;
		int			_buffer_state;
		size_t		_file_size;
		std::string host;
		std::string body;
		mapstring headers;
		int socket;
};

struct Client
{
		socklen_t address_length;
		struct sockaddr_storage address;
		int socket;
		char request[MAX_REQUEST_SIZE + 1];
		int received;
		bool isSending;
		std::string response;
};

#endif