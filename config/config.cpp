#include "config.hpp"
#include <fstream>
#include <iostream>

string	get_words(string &line, stringvect& vector)
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

int Location::location_elements(const string &element)
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

void	Location::location_fill(std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;

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
			if (_elements.find(word) != _elements.end())
			{
				std::cout << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			_elements[word] = vector;
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
}

int serv_elements(string element)
{
	std::map<string, string> serv_ele;

	serv_ele["index"] = "";
	serv_ele["error_page"] = "";
	serv_ele["bodysize"] = "";
	serv_ele["root"] = "";
	serv_ele["servername"] = "";
	serv_ele["listen"] = "";
	serv_ele["upload"] = "";
	if(serv_ele.find(element) != serv_ele.end())
	{
		return 1;
	}
	return 0;
}

Server&	Server::server_fill(std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;
	std::getline(ifs, line);
	Location templocation;
	word = get_words(line,vector);
	if (word != "{" || vector.size() == 1)
	{
		std::cout << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (serv_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cout << word << " is already presented in this block" << std::endl;
				exit(0);
			}
			_elements[word] = vector[0];
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
					templocation.location_fill(ifs, line);
					_location[vector[1]] = templocation;
				}
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cout << word <<" unvalid element in the server block" << std::endl;
			exit(0);
		}
	}
	if (word != "}")
	{
		std::cout << "missing ending for the server block" << std::endl;
		exit(0);
	}
	return *this;
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
	Server serv;
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
			this->_error_page = vector;
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
					_servers.push_back(serv.server_fill(ifs, line));
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
	servervect::iterator it = _servers.begin();
	servervect::iterator temp = _servers.begin();
	int i = 0;
	while (it != _servers.end())
	{
		if((*it)._elements.find("servername") == (*it)._elements.end() && !i){
			i = 1;
			temp = it;
		}
		else{
			if ((*it)._elements["servername"] == servername)
				return it;
		}
		it++;
	}
	return temp;
}

locationmap::iterator	Server::matchlocation(string & uri, string &servername)
{
	int match = 0;
	const char *c_uri = uri.c_str();
	DIR *dir = opendir(c_uri);

    if (dir) {
        if (uri.back() != '/') {
            uri += "/";
        }
    } else {
        uri = uri.substr(0, uri.find_last_of("/"));
    }
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (strncmp((*i).first.c_str(), uri.c_str(), (*i).first.size()) == 0){
			if (match < (*i).first.size())
				match = (*i).first.size();
		}
	}
	for (locationmap::iterator i2 = _location.begin(); i2 != _location.end(); i2++){
		if ((*i2).first.size() == match)
			return i2;
	}
	return _location.end();
}