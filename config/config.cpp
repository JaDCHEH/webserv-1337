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

int location_elements(string element)
{
	mapstring location_ele;

	location_ele["index"] = "";
	location_ele["allowed_methods"] = "";
	location_ele["autoindex"] = "";
	location_ele["root"] = "";
	location_ele["upload"] = "";
	if(location_ele.find(element) != location_ele.end())
		return 1;
	return 0;
}

location	location_fill(std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;
	location location;

	std::getline(ifs, line);
	word = get_words(line,vector);
	if (word != "{" || vector.size() >= 1)
	{
		std::cout << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (location_elements(word) && vector.size() >= 1)
		{
			if (location.elements.find(word) != location.elements.end())
			{
				std::cout << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			location.elements[word] = vector;
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

int serv_elements(string element)
{
	std::map<string, string> serv_ele;

	serv_ele["index"] = "";
	serv_ele["error_page"] = "";
	serv_ele["bodysize"] = "";
	serv_ele["root"] = "";
	serv_ele["servername"] = "";
	serv_ele["upload"] = "";
	if(serv_ele.find(element) != serv_ele.end())
		return 1;
	return 0;
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
		if (serv_elements(word) && vector.size() > 1)
		{
			if (server.elements.find(word) != server.elements.end())
			{
				std::cout << word << " is already presented in this block" << std::endl;
				exit(0);
			}
			server.elements[word] = vector[0];
			listen = 1;
		}
		else if (word == "location")
		{
				if (vector.size() != 1)
				{
					std::cout << "location block is wrong" << std::endl;
					exit(0);
				}
				else{
					if(vector[1].back() != '/')
						vector[1] += '/';
					server.location[vector[1]] = location_fill(ifs ,line);
				}
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

void	config::conf(string &conf)
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

servervect::iterator	config::matchname(string &servername)
{
	servervect::iterator it = servers.begin();
	servervect::iterator temp = servers.begin();
	int i = 0;
	while (it != servers.end())
	{
		if((*it).elements.find("servername") == (*it).elements.end() && !i){
			i = 1;
			temp = it;
		}
		else{
			if ((*it).elements["servername"] == servername)
				return it;
		}
		it++;
	}
	return temp;
}

locationmap::iterator	server::matchlocation(string & uri, string &servername)
{
	int match = 0;

    if (std::filesystem::is_directory(uri)) {
        uri += "/";
    } else {
        uri = uri.substr(0, uri.find_last_of("/"));
    }
	for (locationmap::iterator i = location.begin(); i != location.end(); i++){
		if (strncmp((*i).first.c_str(), uri.c_str(), (*i).first.size()) == 0){
			if (match < (*i).first.size())
				match = (*i).first.size();
		}
	}
	for (locationmap::iterator i2 = location.begin(); i2 != location.end(); i2++){
		if ((*i2).first.size() == match)
			return i2;
	}
	return location.end();
}