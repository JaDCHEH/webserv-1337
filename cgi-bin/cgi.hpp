#ifndef CGI_HPP
#define CGI_HPP

#include "../includes/Response/Response.hpp"

#define MAX_INPUT 1024

class Cgi {
	private:
		mapstring _env;
	public:
		Cgi( void );
		~Cgi();
		void 	fill_env(string, Request &);
};

Cgi::Cgi( void ) {}

Cgi::~Cgi() {}

#endif