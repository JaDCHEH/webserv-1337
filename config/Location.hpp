#ifndef LOCATION_HPP
#define LOCATION_HPP

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
#include <fcntl.h>
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
typedef std::vector<Location> locationvect;
typedef std::map<string, Location> locationmap;

class Location {
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

#endif