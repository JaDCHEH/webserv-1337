#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "config/config.hpp"

class Request {

	public :
		string		method;
		string		path;
		string		_req;
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
		mapstring	headers;
		int			socket;
		string		getHeader(string &key);

};

#endif