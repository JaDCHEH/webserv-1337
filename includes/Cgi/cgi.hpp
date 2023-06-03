#ifndef CGI_HPP
#define CGI_HPP

#include "../Response/Response.hpp"

#define MAX_INPUT 1024
class Cgi {
	private:
		Request		_req;
		string		_file;
		string		_exten;
		string		_init_line;
		string 		buf;
    	mapstring   _env;
    	string		resp;
		string		execut;
    	string 		username;
    	char 		**env;
    	char 		**av;
	public:
		Cgi( Request&, string, string, Response& );
		~Cgi( void );
		void	init( void );
		char 	**getEnvir( void );
		char 	**getArgv( void );
		void 	fill_env( string , Request &, mapstring &);
		void    free_all(char **, char **);
		char**	convertMapToCharArray(mapstring& );
		bool		_flag;
};

#endif