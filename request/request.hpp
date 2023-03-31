#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "config/config.hpp"

class request
{
public:
	string METHOD;
	string uri;
	string version;
	string host;
};

request::request(/* args */)
{
}

request::~request()
{
}


#endif