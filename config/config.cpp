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
		word.clear();
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

string 		Location::get_element(string key)
{
	return(_elements[key]);
}
int	Location::is_method_allowed(string method)
{
	if (std::find(_allowed_methods.begin(), _allowed_methods.end(), method) != _allowed_methods.end())
		return 1;
	return 0;
}

int	Location::find_element(string element)
{
	if (_elements.find(element) != _elements.end())
		return 1;
	return 0;
}

int Location::location_elements(const string &element)
{
	mapstring location_ele;

	location_ele["index"] = "";
	location_ele["autoindex"] = "";
	location_ele["root"] = "";
	location_ele["upload"] = "";
	location_ele["return"] = "";
	if(location_ele.find(element) != location_ele.end())
		return 1;
	return 0;
}

void	Location::check_validity()
{
	for (stringvect::iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); it++)
	{
		if ((*it) != "GET" && (*it) != "DELETE" && (*it) != "POST")
		{
			std::cerr << (*it) << " Unvalid method in the configuration file" << std::endl;
			exit(1);
		}
	}
}

void	Location::set_real(int a)
{
	_real = a;
}

int		Location::get_real()
{
	return _real;
}

void	Location::location_fill(std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;
	int allowed = 0;

	std::getline(ifs, line);
	_real = 1;
	word = get_words(line,vector);
	if (word != "{" || vector.size() >= 1)
	{
		std::cerr << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "allowed_methods")
		{
			if (allowed == 1)
			{
				std::cerr << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			_allowed_methods = vector;
			allowed = 1;
		}
		else if (location_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cerr << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			_elements[word] = vector[0];
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cerr << "unvalid element " << word << " or arguments in the location block" << std::endl;
			exit(0);
		}
	}
	if (word != "}") 
	{
		std::cerr << "missing ending for the location block" << std::endl;
		exit(0);
	}
	check_validity();
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
		std::cerr << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (serv_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cerr << word << " is already presented in this block" << std::endl;
				exit(0);
			}
			_elements[word] = vector[0];
		}
		else if (word == "location")
		{
				if (vector.size() != 1)
				{
					std::cerr << "location block is wrong" << std::endl;
					exit(0);
				}
				else{
					if(vector[0].back() != '/')
						vector[0] += '/';
					templocation.location_fill(ifs, line);
					_location[vector[0]] = templocation;
				}
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cerr << word <<" unvalid element in the server block" << std::endl;
			exit(0);
		}
	}
	if (word != "}")
	{
		std::cerr << "missing ending for the server block" << std::endl;
		exit(0);
	}
	return *this;
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
	int error_page = 0;
	_error_page.clear();
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "error_page")
		{
			if (vector.size() != 1 || error_page == 1)
			{
				std::cerr << "arguments of error page are invalid" << std::endl;
				exit(0);
			}
			_error_page = vector[0];
			error_page = 1;
		}
		else if (word == "server")
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

void Location::must_fill(const string &root)
{
	if (_elements.find("root") == _elements.end())
		_elements["root"] = root;
	if (_elements.find("auto_index") == _elements.end())
		_elements["auto_index"] = "off";
}

void Server::must_fill(const string &error_page)
{
	if (_elements.find("root") == _elements.end())
		_elements["root"] = "/";
	if (_elements.find("listen") == _elements.end())
		_elements["listen"] = "8080";
	if (_elements.find("max_body_size") == _elements.end())
		_elements["max_body_size"] = "2000000000";
	if (!(std::all_of(_elements["max_body_size"].begin(), _elements["max_body_size"].end(), ::isdigit)))
	{
		std::cerr << "the body size isn't only digits " << _elements["max_body_size"] << std::endl;
		exit(1);
	}
	if (_elements.find("error_page") == _elements.end())
		_elements["error_page"] = error_page;
	for (locationmap::iterator it = _location.begin(); it != _location.end(); it++)
		it->second.must_fill(_elements["root"]);
	std::ifstream ifs(_elements["error_page"]);
	if (!ifs)
	{
		std::cerr << "the error page in the conf file doesn't exist" << std::endl;
		exit(1);
	}
}

void config::must_fill()
{
	if (_error_page == "")
		_error_page = "database/Defaulterror.html";
	for (servervect::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->must_fill(_error_page);
	std::ifstream ifs(_error_page);
	if (!ifs)
	{
		std::cerr << "the error page in the conf file doesn't exist" << std::endl;
		exit(1);
	}
}

int	Server::find_element(string key)
{
	if (_elements.find(key) == _elements.end())
		return 1;
	return 0;
}

void	Server::set_element(string key, string &value)
{
	_elements[key] = value;
}

string Server::get_element(string key)
{
	return(_elements[key]);
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

Location	Server::matchlocation(string & uri)
{
	size_t match = 0;
	Location fake;
	fake.set_real(-1);
	const char *c_uri = uri.c_str();
	DIR *dir = opendir(c_uri);

    if (dir) {
        if (uri.back() != '/') {
            uri += "/";
        }
    } else {
        uri = uri.substr(0, uri.find_last_of("/"));
    }
	closedir(dir);
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (strncmp((*i).first.c_str(), uri.c_str(), (*i).first.size()) == 0){
			if (match < (*i).first.size())
				match = (*i).first.size();
		}
	}
	for (locationmap::iterator i2 = _location.begin(); i2 != _location.end(); i2++){
		if ((*i2).first.size() == match)
			return (*i2).second;
	}
	return fake;
}