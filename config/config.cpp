#include "config.hpp"
#include <fstream>
#include <iostream>

void	config::setup_sockets()
{
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].setting_PORT();
}

void	config::setup_cnx()
{
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].recieve_cnx();
}

void	config::conf(string conf)
{
	std::ifstream ifs(conf);
	if (!ifs)
	{
		std::cerr << "This file doesn't exist!" << std::endl;
		exit(0);
	}
	string line;
	string word;
	stringvect	vector;
	Server serv;
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		serv.reset();
		if (word == "server")
		{
			if (word == "server")
			{
				if (vector.size() > 0)
				{
					std::cerr << "server block is wrong" << std::endl;
					exit(0);
				}
				else
					_servers.push_back(serv.server_fill(ifs, line));
			}
		}
		else
		{
			std::cerr << word << "  unvalid argument in the conf file" << std::endl;
			exit(1);
		}
	}
	if (!_servers.size())
	{
		std::cerr <<  "conf file must have at least 1 server block" << std::endl;
		exit(1);
	}
	must_fill();
}

void config::must_fill()
{
	for (servervect::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		it->must_fill();
		_ports.push_back(it->get_element("listen"));
	}
}

Server&	config::matchname(string &servername)
{
	servervect::iterator it = _servers.begin();
	servervect::iterator temp = _servers.begin();
	int i = 0;
	while (it != _servers.end())
	{
		if(it->find_element("servername") && !i){
			i = 1;
			temp = it;
		}
		else{
			if (it->get_element("servername") == servername)
				return *it;
		}
		it++;
	}
	return *temp;
}
