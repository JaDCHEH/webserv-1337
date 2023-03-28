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
#include <filesystem>

typedef std::string	string;
typedef std::vector<string> stringvect;
typedef std::map<string, stringvect>	mapvect;
typedef std::map<string, string>	mapstring;

class location
{
public:
	mapvect elements;
};

typedef std::vector<location> locationvect;
typedef std::map<string, location> locationmap;

class server
{
public:
	mapstring	elements;
	locationmap location;
	locationmap::iterator	matchlocation(string &location, string &servername);
};

typedef std::vector<server> servervect;
typedef std::map<string, server>;

class config
{
public:
	stringvect	error_page;
	servervect servers;
	void	conf(string &conf);
	servervect::iterator	matchname(string &servername);
};
string	get_words(string &line, stringvect &vector);

#endif