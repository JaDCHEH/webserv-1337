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
	stringvect _allowed_methods;
	mapstring _elements;
	void location_fill(std::ifstream &ifs, string &line);
	int location_elements(const string &element);
	void check_validity();
	void must_fill(const string &root);
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
	void must_fill(const string &error_page);
};

typedef std::vector<Server> servervect;

class config
{
public:
	string	_error_page;
	servervect _servers;
	void	conf(string conf);
	servervect::iterator	matchname(string &servername);
	void must_fill();
};
string	get_words(string &line, stringvect &vector);

#endif