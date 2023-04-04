#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../config/config.hpp"
#include "../server.hpp"

class Request;

class response
{
private:
	string		_initial_line;
	string		_headers;
	string		_body;
	Server		_server;
	Location	_location;
	mapstring	_code_map;
	mapstring	_extention_map;
public:
	void	fill_initial_line(const string version, const string error_code);
	string	&get_initial_line();
	string	&get_headers();
	void	fill_header(const string header, const string value);
	void	reset_values();
	void	_codes();
	void	unvalid_response(Request &Request, string code);
	void	send_to_socket();
	string	get_content_type(string extention);
	string	get_extention(string content_type);
	void	_extentions();
	void	Create_response(Request & Request, config & config, string code);
};

#endif