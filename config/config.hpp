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
private:
	int			_real;
	stringvect	_allowed_methods;
	mapstring	_elements;
public:
	void		location_fill(std::ifstream &ifs, string &line);
	int			location_elements(const string &element);
	void		set_real(int a);
	int			get_real();
	int			find_element(string element);
	string 		get_element(string key);
	int			is_method_allowed(string method);
	void		check_validity();
	void		must_fill(const string &root);
};

typedef std::vector<Location> locationvect;
typedef std::map<string, Location> locationmap;

class Server
{
private:
	mapstring	_elements;
	locationmap	_location;
public:
	int			find_element(string key);
	void		set_element(string key, string &value);
	string 		get_element(string key);
	Location	&matchlocation(string &location);
	Server&		server_fill(std::ifstream &ifs, string &line);
	void		must_fill(const string &error_page);
};

typedef std::vector<Server> servervect;

class config
{
private:
	string		_error_page;
	servervect	_servers;
public:
	void	conf(string conf);
	Server	&matchname(string &servername);
	void	must_fill();
};
string	get_words(string &line, stringvect &vector);

#endif