#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Config/Location.hpp"

class Request {

	public :
		string		method;
		string		path;
		string		_req;
		string		query_str;
		string		http_version;
		Location	_location;
		mapstring	_error_page;
		string		fullpath;
		string		_buffer;
		int			_first;
		size_t		_size_to_write;
		size_t		_amount_written;
		size_t		_size_recv;
		string		get_error_page(string code);
		int			_fd;
		int			_buffer_state;
		int			socket;
		string		code;
		size_t		_file_size;
		string		host;
		string		body;
		mapstring	headers;
		string		getHeader(string key);
};
int isValidRequestURI(const std::string &uri);
int checkUriLength(const std::string &uri);
int checkRequestBodySize(const std::string &body, size_t max_allowed);
size_t convert(string num);
string	parsingChunked(string new_req);
void	post_parse(Request & server);
void parse(Request &server, string request);

struct Client
{
	socklen_t				address_length;
	struct sockaddr_storage	address;
	int						socket;
	int						received;
	bool					isSending;
	Request					request;
};

#endif