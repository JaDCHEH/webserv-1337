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

location	location_fill(std::ifstream &ifs, string &line, stringvect vect)
{
	string word;
	stringvect vector;
	location location;
	int try_files=0;
	int fastcgi_pass=0;
	int allowed_methods=0;

	location._location = vect;
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
			if (location.elements.find("try_files") != location.elements.end())
			{
				std::cout << "try_files is already presented in this block" << std::endl;
				exit(0);
			}
			location.elements["try_files"] = vector;
			try_files = 1;
		}
		else if (word == "allowed_method" && vector.size() >= 1)
		{
			if (location.elements.find("allowed_methods") != location.elements.end())
			{
				std::cout << "allowed_methods is already presented in this block" << std::endl;
				exit(0);
			}
			location.elements["allowed_methods"] = vector;
			allowed_methods = 1;
		}
		else if (word == "fastcgi_pass" && vector.size() >= 1)
		{
			if (location.elements.find("fastcgi_pass") != location.elements.end())
			{
				std::cout << "fastcgi_pass is already presented in this block" << std::endl;
				exit(0);
			}
			location.elements["fastcgi_pass"] = vector;
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

server	server_fill(std::ifstream &ifs, string &line)
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
			if (server.elements.find("listen") != server.elements.end())
			{
				std::cout << "listen is already presented in this block" << std::endl;
				exit(0);
			}
			server.elements["listen"] = vector;
			listen = 1;
		}
		else if (word == "root" && vector.size() == 1)
		{
			if (server.elements.find("root") != server.elements.end())
			{
				std::cout << "root is already presented in this block" << std::endl;
				exit(0);
			}
			server.elements["root"] = vector;
			root = 1;
		}
		else if (word == "index" && vector.size() == 1)
		{
			if (server.elements.find("index") != server.elements.end())
			{
				std::cout << "index is already presented in this block" << std::endl;
				exit(0);
			}
			server.elements["index"] = vector;
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
					server.location.push_back(location_fill(ifs ,line, vector));
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

void	config::conf(string conf)
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
			this->error_page = vector;
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
					this->servers.push_back(server_fill(ifs, line));
			}
		}
		else
		{
			std::cout << word << "  unvalid argument in the conf file" << std::endl;
			exit(0);
		}
	}
	
}