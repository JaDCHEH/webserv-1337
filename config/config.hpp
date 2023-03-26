#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <utility>
#include <string>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

typedef std::string	string;
typedef std::vector<string> stringvect;
typedef std::map<string, stringvect>	map;

class location
{
public:
	stringvect _location;
	map elements;
};

typedef std::vector<location> locationvect;

class server
{
public:
	map	elements;
	locationvect location;
};

typedef std::vector<server> servervect;

class config
{
public:
	stringvect	error_page;
	servervect servers;
	void	conf(string conf);
	void	check_conf();
};
string	get_words(string &line, stringvect &vector);

#endif