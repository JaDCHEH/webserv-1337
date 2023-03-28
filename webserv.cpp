#include "config/config.hpp"

string firstline = "GET / HTTP/1.1";
string content_type = "text/html";
string content_lenght = "12321323";

int port=8080;

int main(int ac, char **av)
{
	config serverconf;
	if (ac != 2)
		return 1;
	serverconf.conf(av[1]);
}