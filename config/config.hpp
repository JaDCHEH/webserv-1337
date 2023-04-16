#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <utility>
#include <string>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib> 
#include <netdb.h>
#define PORT "8081"
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#define MAX_REQUEST_SIZE 2047
typedef	std::string	string;
typedef	std::vector<string> stringvect;
typedef	std::map<string, stringvect>	mapvect;
typedef	std::map<string, string>	mapstring;
#include "../request.hpp"

struct Client
{
		socklen_t address_length;
		struct sockaddr_storage address;
		int socket;
		char request[MAX_REQUEST_SIZE + 1];
		int received;
		bool isSending;
		std::string response;
};

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
	void		reset();
	int			is_method_allowed(string method);
	void		check_validity();
	void		must_fill(const string &root);
};

typedef std::vector<Location> locationvect;
typedef std::map<string, Location> locationmap;

class Server
{
private:
	mapstring		_elements;
	mapstring		_error_page;
	locationmap		_location;
	SOCKET			socket_listen;
	struct addrinfo hints;
	struct addrinfo *bind_address;
	fd_set reads;
	fd_set writes;
	std::map<int, Request> server;
	response response;
	std::vector<Client> clients;
public:
	int			find_element(string key);
	void		set_element(string key, string &value);
	string 		get_element(string key);
	string		get_error_page(string code);
	Location	matchlocation(string &location);
	void		setting_PORT();
	void		recieve_cnx();
	Server&		server_fill(std::ifstream &ifs, string &line);
	void		must_fill();
};

typedef std::vector<Server> servervect;

class config
{
private:
	stringvect	_ports;
	servervect	_servers;
public:
	void	conf(string conf);
	Server	&matchname(string &servername);
	void	setup_sockets();
	void	setup_cnx();
	void	must_fill();
};

string	get_words(string &line, stringvect &vector);
int isValidRequestURI(const std::string &uri);
int checkUriLength(const std::string &uri);
int checkRequestBodySize(const std::string &body, size_t max_allowed);
void parse(Request &server, string request);

#endif