#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"

class response;
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

class Request;

class Server
{
	private:
		mapstring		_elements;
		mapstring		_error_page;
		locationmap		_location;
		SOCKET			socket_listen;
		struct addrinfo hints;
		struct addrinfo *bind_address;
		fd_set reads;
		fd_set writes;
		std::map<int, Request> server;
		response *res;
		std::vector<Client> clients;
	public:
		void		reset();
		int			find_element(string key);
		void		set_element(string key, string &value);
		string 		get_element(string key);
		string		get_error_page(string code);
		Location	matchlocation(string &location);
		void		setting_PORT();
		void		recieve_cnx();
		Server		server_fill(std::ifstream &ifs, string &line);
		void		must_fill();
};

class Request {

	public :
		string	method;
		string	path;
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
		mapstring headers;
		int socket;
};
int isValidRequestURI(const std::string &uri);
int checkUriLength(const std::string &uri);
int checkRequestBodySize(const std::string &body, size_t max_allowed);
void parse(Request &server, string request);

#endif