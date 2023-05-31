#include "../../includes/Config/Server.hpp"

#include "../../includes/Response/Response.hpp"

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
		if(it->get_element("listen") == port && !def){
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
	res = new Response;
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
		client.request.fullpath  = "";
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
				string reqpath = it->request.path;
				it->request.fullpath =reqpath.replace(0, it->request._location.get_element("name").size(), it->request._location.get_element("root"));
				if (isValidRequestURI(it->request.path))
					it->request.code = "400";
				else if (checkUriLength(it->request.path))
					it->request.code = "414";
				else if (checkRequestBodySize(it->request.body, std::stoul(temp.get_element("bodysize"))))
					it->request.code = "413";
				it->isSending = false;
			}
			buffer.clear();
		}
		else if (FD_ISSET(it->socket, &writes))
		{
			if (!res->Create_Response(it->request, it->request.code))
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
