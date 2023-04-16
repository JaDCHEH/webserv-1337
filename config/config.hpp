#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"

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

#endif