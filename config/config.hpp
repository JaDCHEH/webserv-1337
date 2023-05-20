#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"

typedef std::vector<Server> servervect;

class config
{
	public:
		void	conf(string conf);
		Server	&matchname(string &servername);
		void	setup_sockets();
		void	setup_cnx(fd_set &reads, fd_set &writes);
		void	must_fill();
		servervect	_servers;
		stringvect	_ports;
};

#endif