#include "config/config.hpp"
#include "response/ResponseHeader.hpp"

string firstline = "GET / HTTP/1.1";
string content_type = "text/html";
string content_lenght = "12321323";

int port=8080;

int main(int ac, char **av)
{
	config serverconf;
	ResponseHeader header;
	if (ac != 2)
		return 1;
	serverconf.conf(av[1]);
	stringvect line;
	get_words(firstline, line);
	
}