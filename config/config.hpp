#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Location.hpp"
#include "../request.hpp"
#include "../Server.hpp"
#include "../response/response.hpp"

typedef std::vector<Server> servervect;

class config
{
	private:
		stringvect	_ports;
		servervect	_servers;
	public:
		void	conf(string conf);
		Server	&matchname(string &servername);
		void	setup_sockets();
		void	setup_cnx();
		void	must_fill();
};

string	get_words(string &line, stringvect &vector);
int isValidRequestURI(const std::string &uri);
int checkUriLength(const std::string &uri);
int checkRequestBodySize(const std::string &body, size_t max_allowed);
void parse(Request &server, string request);

#endif