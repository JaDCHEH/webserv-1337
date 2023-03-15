#include "config.hpp"
#include <fstream>
#include <iostream>

string	get_words(string &line, stringvect &vector)
{
	string::iterator it = line.begin();
	string word;
	string key;
	int flag = 0;

	vector.clear();
	while (it != line.end())
	{
		word = "";
		while ((*it == ' ' || *it == '\t') && it != line.end())
			it++;
		while ((*it != ' ' && *it != '\t') && it != line.end())
		{
			if (*it)
				word += (*it);
			it++;
		}
		if (word != "" && flag == 0)
		{
			key = word;
			flag = 1;
		}
		else if (word != "")
			vector.push_back(word);
	}
	return key;
}

location	location_fill(server &serv, std::ifstream &ifs, string &line, stringvect vect)
{
	string word;
	stringvect vector;
	location location;
	int try_files=0;
	int fastcgi_pass=0;
	int fastcgi_params=0;

	location.name = Pairstring("location", vect);
	std::getline(ifs, line);
	word = get_words(line,vector);
	if (word != "{" || vector.size() > 1)
	{
		std::cout << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "try_files" && vector.size() >= 1)
		{
			if (try_files == 1)
			{
				std::cout << "try_files is already presented in this block" << std::endl;
				exit(0);
			}
			location.try_files = Pairstring(word , vector);
			try_files = 1;
		}
		else if (word == "fastcgi_param" && vector.size() >= 1)
		{
			if (fastcgi_params == 1)
			{
				std::cout << "fastcgi_param is already presented in this block" << std::endl;
				exit(0);
			}
			location.fastcgi_param = Pairstring(word , vector);
			fastcgi_params = 1;
		}
		else if (word == "fastcgi_pass" && vector.size() >= 1)
		{
			if (fastcgi_pass == 1)
			{
				std::cout << "fastcgi_pass is already presented in this block" << std::endl;
				exit(0);
			}
			location.fastcgi_pass = Pairstring(word , vector);
			fastcgi_pass = 1;
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cout << "unvalid element in the location block" << std::endl;
			exit(0);
		}
	}
	if (word != "}")
	{
		std::cout << "missing ending for the location block" << std::endl;
		exit(0);
	}
	return location;
}

server	server_fill(config &conf, std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;
	server	server;
	int	listen = 0;
	int	root = 0;
	int	index = 0;
	std::getline(ifs, line);
	word = get_words(line,vector);
	if (word != "{" || vector.size() > 1)
	{
		std::cout << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "listen" && vector.size() >= 1)
		{
			if (listen == 1)
			{
				std::cout << "listen is already presented in this block" << std::endl;
				exit(0);
			}
			server.listen = Pairstring(word , vector);
			listen = 1;
		}
		else if (word == "root" && vector.size() == 1)
		{
			if (root == 1)
			{
				std::cout << "root is already presented in this block" << std::endl;
				exit(0);
			}
			server.listen = Pairstring(word, vector);
			root = 1;
		}
		else if (word == "index" && vector.size() == 1)
		{
			if (index == 1)
			{
				std::cout << "index is already presented in this block" << std::endl;
				exit(0);
			}
			server.listen = Pairstring(word, vector);
			index = 1;
		}
		else if (word == "location")
		{
				if (vector.size() != 1)
				{
					std::cout << "location block is wrong" << std::endl;
					exit(0);
				}
				else
					server.location.push_back(location_fill(server, ifs ,line, vector));
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cout << "unvalid element in the server block" << std::endl;
			exit(0);
		}
	}
	if (word != "}")
	{
		std::cout << "missing ending for the server block" << std::endl;
		exit(0);
	}
	return (server);
}

void	conf(string conf)
{
	std::ifstream ifs(conf);
	if (!ifs)
	{
		std::cout << "This file doesn't exist!" << std::endl;
		exit(0);
	}
	string line;
	string word;
	stringvect	vector;
	config con;
	int error_page = 0;
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "error_page")
		{
			if (vector.size() != 1 || error_page == 1)
			{
				std::cout << "arguments of error page are invalid" << std::endl;
				exit(0);
			}
			con.error_page = Pairstring(word, vector);
			error_page = 1;
		}
		else if (word == "server")
		{
			if (word == "server")
			{
				if (vector.size() > 0)
				{
					std::cout << "server block is wrong" << std::endl;
					exit(0);
				}
				else
					con.servers.push_back(server_fill(con, ifs, line));
			}
		}
		else
		{
			std::cout << word << "  unvalid argument in the conf file" << std::endl;
			exit(0);
		}
	}
	
}

int main(int ac, char **av)
{
	conf(av[1]);
}