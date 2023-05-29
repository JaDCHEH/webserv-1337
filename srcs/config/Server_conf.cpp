#include "../../includes/Config/Server.hpp"

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

void Server::must_fill()
{
	if (_elements.find("root") == _elements.end())
		_elements["root"] = "/";
	if (_elements.find("listen") == _elements.end())
		_elements["listen"] = "8080";
	if (_elements.find("bodysize") == _elements.end())
		_elements["bodysize"] = "2000000000";
	if (!(std::all_of(_elements["max_body_size"].begin(), _elements["max_body_size"].end(), ::isdigit)))
	{
		std::cerr << "the body size isn't only digits " << _elements["max_body_size"] << std::endl;
		exit(1);
	};
	for (locationmap::iterator it = _location.begin(); it != _location.end(); it++)
		it->second.must_fill(_elements["root"]);
	r = 0;
}


Server	Server::server_fill(std::ifstream &ifs, string &line)
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
		templocation.reset();
		word = get_words(line, vector);
		if (word == "error_page")
		{
			if (vector.size() != 2 || vector[0].find_first_not_of("0123456789") != string::npos)
			{
				std::cerr << "arguments of error page are invalid" << std::endl;
				exit(0);
			}
			std::ifstream file(vector[1]);
			if (!file)
			{
				std::cerr << "the error page does not exist in the server" << std::endl;
				exit(0);
			}
			_error_page[vector[0]] = vector[1];
		}
		else if (serv_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cerr << word << " is already presented in this block " << vector[0] << std::endl;
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
					templocation.location_fill(ifs, line, vector[0]);
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

void Server::reset()
{
	_elements.clear();
	_error_page.clear();
	_location.clear();
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

Location	Server::matchlocation(string & uri)
{
	size_t match = 0;
	Location fake;
	fake.set_real(-1);
	string urii =  uri;
	string smatched;
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (strncmp(i->first.c_str(), urii.c_str(), i->first.size() - 1) == 0){
			if (match < i->first.size())
			{
				match = i->first.size();
				smatched = i->first;
			}
		}
	}
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (i->first.size() == match && i->first == smatched)
			return i->second;
	}
	return fake;
}

int		Server::getSocket(){
	return socket_listen;
}