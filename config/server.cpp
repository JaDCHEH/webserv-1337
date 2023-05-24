#include "Server.hpp"
#include "../response/response.hpp"

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

int serv_elements(string element)
{
	std::map<string, string> serv_ele;

	serv_ele["index"] = "";
	serv_ele["error_page"] = "";
	serv_ele["bodysize"] = "";
	serv_ele["root"] = "";
	serv_ele["servername"] = "";
	serv_ele["listen"] = "";
	serv_ele["upload"] = "";
	if(serv_ele.find(element) != serv_ele.end())
	{
		return 1;
	}
	return 0;
}

string	Request::get_error_page(string code)
{
	mapstring::iterator it = _error_page.find(code);
	if (it == _error_page.end())
		return "";
	return it->second;
}

void	Server::setting_PORT()
{
	std::cout << "Configuring local address..." << std::endl;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(0, get_element("listen").c_str(), &hints, &bind_address);

	std::cout << "Creating socket..." << std::endl;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	int sss = 1;
	std::cout << "Binding socket to local address..." << std::endl;
	setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &sss, sizeof(int));
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "Failed to bind to port: " << PORT << " " << strerror(errno) << std::endl;
	//	exit(EXIT_FAILURE);
	}

	std::cout << "Listening..." << std::endl;
	if (listen(socket_listen, 1024) < 0)
	{
		std::cout << "Failed to listen on socket."
				  << " " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

Server	matchname(Request &req, string servername, std::vector<Server> _servers)
{
	servervect::iterator it = _servers.begin();
	servervect::iterator temp = _servers.begin();
	int def = 0;
	string name = servername.substr(0, servername.find(':'));
	string port = servername.substr(servername.find(':') + 1);
	while (it != _servers.end())
	{
		if(it->find_element("servername") && it->get_element("listen") == port && !def){
			def = 1;
			temp = it;
		}
		else{
			if (it->get_element("servername") == name && it->get_element("listen") == port)
				return *it;
		}
		it++;
	}
	if (!def)
		req.code = "404";
	return *temp;
}

void	Server::recieve_cnx(fd_set &reads, fd_set &writes, std::vector<Server> servers)
{
	res = new response;
	res->init();

	string buffer;
	if (FD_ISSET(socket_listen, &reads))
	{
		Client client;
		bzero(&client.address, sizeof(struct sockaddr_storage));
		char address_buffer[100];
		client.isSending = true;
		client.received = 0;
		client.request._amount_written = 0;
		client.request._size_to_write = 0;
		client.request._buffer_state = 0;
		client.request._size_recv = 0;
		client.request._req = "";
		client.address_length = sizeof(client.address);
		SOCKET socket_client = accept(socket_listen,
									  (struct sockaddr *)&client.address, &client.address_length);
		// std::cout << "new socket "<< socket_client << std::endl;
		if (!ISVALIDSOCKET(socket_client))
		{
			std::cout << "Accept failed errno: "
					  << " " << strerror(errno) << std::endl;
		}
		fcntl(socket_client, F_SETFL, O_NONBLOCK);
		client.socket = socket_client;
		clients.push_back(client);
		getnameinfo((struct sockaddr *)&client.address,
					client.address_length,
					address_buffer, sizeof(address_buffer), 0, 0,
					NI_NUMERICHOST);
	}
	// std::cout << clients.size() << std::endl;

	std::vector<Client>::iterator it = clients.begin();
	std::vector<Client>::iterator end = clients.end();
	while (it != end)
	{
		if (it->isSending && FD_ISSET(it->socket, &reads))
		{
			
			buffer.resize(2000);
			int bytes_received = recv(it->socket, &buffer[0], 2000, 0);
			if (bytes_received < 1)
			{
				if (bytes_received == 0)
					std::cout << "Connexion got dropped by the client! " << strerror(errno) << std::endl;
				else
					std::cout << "Disconnected errno : " << strerror(errno) << std::endl;
				CLOSESOCKET(it->socket);
				it->request._req.clear();
				it->request.body.clear();
				it = clients.erase(it);
				end = clients.end();
				continue;
			}
			it->request._size_recv += bytes_received;
			it->request._req += buffer;
			it->request._req.resize(it->request._size_recv);
			if (recv(it->socket, &buffer[0], 1, MSG_PEEK) <= 0)
			{
				Server temp;
				it->request.code = "";
				parse(it->request, it->request._req);
				it->request.socket = it->socket;
				it->request._error_page = _error_page;
				temp = matchname(it->request, it->request.getHeader("Host"), servers);
				it->request._location = temp.matchlocation(it->request.path);
				if (isValidRequestURI(it->request.path))
					it->request.code = "400";
				else if (checkUriLength(it->request.path))
					it->request.code = "414";
				else if (checkRequestBodySize(it->request.body, std::stoul(temp.get_element("max_body_size"))))
					it->request.code = "413";
				it->isSending = false;
			}
			buffer.clear();
		}
		else if (FD_ISSET(it->socket, &writes))
		{
			if (!res->Create_response(it->request, it->request.code))
			{
				CLOSESOCKET(it->socket);
				it->request._req.clear();
				it->request.body.clear();
				it = clients.erase(it);
				end = clients.end();
				continue;
			}
		}
		++it;
	}
	delete(res);
}

Server	Server::server_fill(std::ifstream &ifs, string &line)
{
	string word;
	stringvect vector;
	std::getline(ifs, line);
	Location templocation;
	word = get_words(line,vector);
	if (word != "{" || vector.size() == 1)
	{
		std::cerr << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		templocation.reset();
		word = get_words(line, vector);
		if (word == "error_page")
		{
			if (vector.size() != 2 || vector[0].find_first_not_of("0123456789") != string::npos)
			{
				std::cerr << "arguments of error page are invalid" << std::endl;
				exit(0);
			}
			std::ifstream file(vector[1]);
			if (!file)
			{
				std::cerr << "the error page does not exist in the server" << std::endl;
				exit(0);
			}
			_error_page[vector[0]] = vector[1];
		}
		else if (serv_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cerr << word << " is already presented in this block " << vector[0] << std::endl;
				exit(0);
			}
			_elements[word] = vector[0];

		}
		else if (word == "location")
		{
				if (vector.size() != 1)
				{
					std::cerr << "location block is wrong" << std::endl;
					exit(0);
				}
				else{
					if(vector[0].back() != '/')
						vector[0] += '/';
					templocation.location_fill(ifs, line);
					_location[vector[0]] = templocation;
				}
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cerr << word <<" unvalid element in the server block" << std::endl;
			exit(0);
		}
	}
	if (word != "}")
	{
		std::cerr << "missing ending for the server block" << std::endl;
		exit(0);
	}
	return *this;
}

void Server::reset()
{
	_elements.clear();
	_error_page.clear();
	_location.clear();
}

void Server::must_fill()
{
	if (_elements.find("root") == _elements.end())
		_elements["root"] = "/";
	if (_elements.find("listen") == _elements.end())
		_elements["listen"] = "8080";
	if (_elements.find("max_body_size") == _elements.end())
		_elements["max_body_size"] = "2000000000";
	if (!(std::all_of(_elements["max_body_size"].begin(), _elements["max_body_size"].end(), ::isdigit)))
	{
		std::cerr << "the body size isn't only digits " << _elements["max_body_size"] << std::endl;
		exit(1);
	};
	for (locationmap::iterator it = _location.begin(); it != _location.end(); it++)
		it->second.must_fill(_elements["root"]);
	r = 0;
}

int	Server::find_element(string key)
{
	if (_elements.find(key) == _elements.end())
		return 1;
	return 0;
}

void	Server::set_element(string key, string &value)
{
	_elements[key] = value;
}

string Server::get_element(string key)
{
	return(_elements[key]);
}

Location	Server::matchlocation(string & uri)
{
	size_t match = 0;
	Location fake;
	fake.set_real(-1);
	string urii =  uri;
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (strncmp(i->first.c_str(), urii.c_str(), i->first.size() - 1) == 0){
			if (match < i->first.size())
				match = i->first.size();
		}
	}
	for (locationmap::iterator i = _location.begin(); i != _location.end(); i++){
		if (i->first.size() == match)
			return i->second;
	}
	return fake;
}

int		Server::getSocket(){
	return socket_listen;
}