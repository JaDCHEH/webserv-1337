#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <utility>
#include <string>

typedef std::string	string;
typedef std::vector<string> stringvect;
typedef std::pair<string, stringvect>	Pairstring;

class location
{
public:
	Pairstring	name;
	Pairstring	try_files;
	Pairstring	fastcgi_pass;
	Pairstring	fastcgi_param;
};

typedef std::vector<location> locationvect;

class server
{
public:
	Pairstring	listen;
	Pairstring	root;
	Pairstring	index;
	locationvect location;
};

typedef std::vector<server> servervect;

class config
{
public:
	Pairstring	error_page;
	servervect servers;
};

#endif