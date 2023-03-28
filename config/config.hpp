#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <utility>
#include <string>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>

typedef std::string	string;
typedef std::vector<string> stringvect;
typedef std::map<string, stringvect>	mapvect;
typedef std::map<string, string>	mapstring;

class Location
{
public:
	mapvect _elements;
	void location_fill(std::ifstream &ifs, string &line);
	int location_elements(const string &element);
};

typedef std::vector<Location> locationvect;
typedef std::map<string, Location> locationmap;

class Server
{
public:
	mapstring	_elements;
	locationmap _location;
	locationmap::iterator	matchlocation(string &location, string &servername);
	Server&	server_fill(std::ifstream &ifs, string &line);
};

typedef std::vector<Server> servervect;

class config
{
public:
	stringvect	_error_page;
	servervect _servers;
	void	conf(string conf);
	servervect::iterator	matchname(string &servername);
};
string	get_words(string &line, stringvect &vector);

#endif