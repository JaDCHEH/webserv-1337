#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "../Request/Request.hpp"

class Response;

class Server
{
	private:
		mapstring				_elements;
		mapstring				_error_page;
		locationmap				_location;
		SOCKET					socket_listen;
		struct addrinfo 		hints;
		struct addrinfo 		*bind_address;
		Response				*res;
		int						r;
	public:
		std::vector<Client>		clients;
		void					reset();
		int						find_element(string key);
		void					set_element(string key, string &value);
		string 					get_element(string key);
		Location				matchlocation(string &location);
		void					setting_PORT(int i);
		void					recieve_cnx(fd_set &reads, fd_set &writes, std::vector<Server> servers);
		Server					server_fill(std::ifstream &ifs, string &line);
		void					must_fill();
		int						getSocket();
};

#endif