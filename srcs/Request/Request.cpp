#include "../../includes/Request/Request.hpp"

// Checks if the URI contains a non allowed character c_400
int isValidRequestURI(const std::string &uri)
{
	const std::string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (uri.find_first_not_of(allowed_chars) == std::string::npos)
		return (0);
	return (400); // c_400 forbiden character in URI
}

int checkUriLength(const std::string &uri)
{
	int _len = uri.length();

	if (_len > 2048)
		return (414); // c_414 URI too long
	return (0);
}
int checkRequestBodySize(const std::string &body, size_t max_allowed)
{
	size_t _len = body.length();

	if (_len > max_allowed)
		return (413); // c_413 Entity too large
	return (0);
}

size_t convert(string num) {
    int len = num.length();
    int base = 1;
    int temp = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] >= '0' && num[i] <= '9') {
            temp += (num[i] - '0') * base;
            base = base * 16;
        }
        else if (num[i] >= 'a' && num[i] <= 'f') {
            temp += (num[i] - 'a' + 10) * base;
            base = base * 16;
        }
        else
            return (0);
    }
    return temp;
}

string	parsingChunked(string new_req)
{
	std::istringstream iss(new_req);
    string s, str;
	size_t str_length;
    while (std::getline(iss, str, '\r')) {
        iss.ignore(1); // to ignore "\n"
		if (str[0] == '0') // 0 means no more line to read
			break ;
		str_length = convert(str);// convert length from hex to decimal
        string body;
		body.resize(str_length);
		iss.read(&body[0], str_length);
		s += body;
		body.clear();
    }
	return s;
}

void	post_parse(Request & server)
{
	string key, value;
	int	tflag = 0, cflag = 0;

	if (server.getHeader("Transfer-Encoding") == "chunked")
	{
		tflag = 1;
		server.body = parsingChunked(server.body);
		server.code = ""; // Valid request
	}
	else if (server.getHeader("Transfer-Encoding") != "")
	{
		server.code = "501";// "501 Not Implemented" If the Transfer-Encoding header specifies an encoding mechanism other than "chunked"
		return ;
	}
	if (server.getHeader("Content-Length") != "")
		cflag = 1;
	if (!tflag && !cflag)
		server.code = "400"; // "Bad request" If both the Content-Length and Transfer-Encoding headers are missing or not specified
}

void parse(Request &server, string request)
{
	// Convert the Request char array to an input string stream
	std::istringstream iss(request);
	// Create a Server object to hold the parsed Request data
	// Extract the HTTP method, path, and version from the Request
	iss >> server.method >> server.path >> server.http_version;
	
	// Filling the query string CGI
	if (server.path.find("?") != std::string::npos)
	{
		server.query_str = server.path.substr(server.path.find("?") + 1);
		server.path = server.path.substr(0, server.path.find("?"));
	}

	// Extract the headers from the Request
	std::string header_key, header_value;
	while (std::getline(iss, header_key, ':') && std::getline(iss, header_value, '\r'))
	{
		iss.ignore(1); // ignore newline character
		while (header_key.front() == '\n' || header_key.front() == '\r')
			header_key = header_key.substr(1);
		while (header_value.front() == ' ')
			header_value = header_value.substr(1);
		if (server.headers.find(header_key) == server.headers.end())	
			server.headers[header_key] = header_value;
	}
	server.body = request.substr(request.find("\r\n\r\n") + 4); // Set the body to everything after the headers
	if (server.method == "POST")
	{
		post_parse(server);
	}
	// std::cout << "check code : " << server.code << std::endl;
	// Extract the body of the Request
	// Return the parsed Server object
	server._buffer_state = 0;
	server._first = 0;
	server._amount_written = 0;
}

string	Request::getHeader(string key)
{
	mapstring::iterator it = headers.find(key);
    if (it != headers.end())
        return it->second;
	return "";
}

string	Request::get_error_page(string code)
{
	mapstring::iterator it = _error_page.find(code);
	if (it == _error_page.end())
		return "";
	return it->second;
}
