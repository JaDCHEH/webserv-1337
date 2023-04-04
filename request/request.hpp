#ifndef Request_HPP
#define Request_HPP
#include "config/config.hpp"

class Request
{
public:
	string	METHOD;
	string	uri;
	string	version;
	string	host;
	int		_writing_status;
	int		_socked_fd;
	int		_header_status;
	int		_file_status;
	string	Buffer;
	
};

Request::Request(/* args */)
{
}

Request::~Request()
{
}


#endif