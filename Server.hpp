#ifndef SERVER_HPP
#define SERVER_HPP

#include "config/Location.hpp"
#include "response/response.hpp"
#include "request.hpp"

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
		response response;
		std::vector<Client> clients;
	public:
		int			find_element(string key);
		void		set_element(string key, string &value);
		string 		get_element(string key);
		string		get_error_page(string code);
		Location	matchlocation(string &location);
		void		setting_PORT();
		void		recieve_cnx();
		Server&		server_fill(std::ifstream &ifs, string &line);
		void		must_fill();
};

#endif