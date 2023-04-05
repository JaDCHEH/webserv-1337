/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   server.cpp										 :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: cjad <cjad@student.42.fr>				  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2023/03/26 14:57:56 by zihirri		   #+#	#+#			 */
/*   Updated: 2023/04/05 15:15:52 by cjad			 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "server.hpp"
#include <sstream>

// Checks if the URI contains a non allowed character c_400
int isValidRequestURI(const std::string &uri)
{
	const std::string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (uri.find_first_not_of(allowed_chars) == std::string::npos)
		return (0);
	return (400); //c_400 forbiden character in URI
}

int checkUriLength(const std::string &uri)
{
	int _len = uri.length();

	if (_len > 2048)
		return (414); //c_414 URI too long
	return (0);
}
int checkRequestBodySize(const std::string &body, size_t max_allowed)
{
	size_t _len = body.length();
	
	if(_len > max_allowed)
		return (413); //c_413 Entity too large
	return (0);
}

Request parse(char *request)
{
	// Convert the Request char array to an input string stream
	std::istringstream iss(request);
	// Create a Server object to hold the parsed Request data
	Request server;
	// Extract the HTTP method, path, and version from the Request
	iss >> server.method >> server.path >> server.http_version;
	// Extract the headers from the Request
	std::string header_key, header_value;
	while (std::getline(iss, header_key, ':') && std::getline(iss, header_value, '\r'))
	{
		iss.ignore(1); // ignore newline character
		server.headers[header_key] = header_value;
	}
	// Extract the body of the Request
	char *b = strstr(request, "\r\n\r\n"); // Find the end of the headers
	server.body = std::string(b + 4);	  // Set the body to everything after the headers
	// Return the parsed Server object
	server._buffer_state = 0;
	server._first = 0;
	return server;
}

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	struct addrinfo hints;
	struct addrinfo *bind_address;
	SOCKET socket_listen;
	config  conf;
	conf.conf(av[1]);
	std::cout << "Configuring local address..." << std::endl;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(0, PORT, &hints, &bind_address);

	std::cout << "Creating socket..." << std::endl;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	int sss=1;
	std::cout << "Binding socket to local address..." << std::endl;
	setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &sss, sizeof(int));
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "Failed to bind to port: " << PORT << " " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Listening..." << std::endl;
	if (listen(socket_listen, 10) < 0)
	{
		std::cout << "Failed to listen on socket."
				  << " " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set up the file descriptor sets for select
	fd_set master, reads;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	int max_socket = socket_listen;
	std::map <int ,Request> server;
	response	response;
	while (1)
	{
		reads = master;
		if (select(max_socket + 1, &reads, 0, 0, 0) == -1)
		{
			std::cout << "Failed to select. errno: "
					  << " " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		SOCKET i;
		for (i = 1; i <= max_socket; ++i)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == socket_listen)
				{
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);
					SOCKET socket_client = accept(socket_listen,
												  (struct sockaddr *)&client_address, &client_len);
					if (!ISVALIDSOCKET(socket_client))
					{
						std::cout << "Accept failed errno: "
								  << " " << strerror(errno) << std::endl;
						exit(EXIT_FAILURE);
					}
					FD_SET(socket_client, &master);
					if (socket_client > max_socket)
						max_socket = socket_client;

					char address_buffer[100];
					getnameinfo((struct sockaddr *)&client_address,
								client_len,
								address_buffer, sizeof(address_buffer), 0, 0,
								NI_NUMERICHOST);
					std::cout << "New connection from: " << address_buffer << std::endl;
				}
				else
				{
					char buffer[4096];
					memset(buffer, 0, sizeof(buffer));
					int bytes_received = recv(i, buffer, sizeof(buffer), 0);
					if (bytes_received < 1)
					{
						std::cout << "Disconnected errno : " << errno << std::endl;
						exit(EXIT_FAILURE);
					}
					else
					{
						std::cout << "Received " << bytes_received << " bytes from client\n";
						
						server[i] = parse(buffer);
						server[i].socket = i;
						server[i]._server = conf.matchname(server[i].host);
						server[i]._location = server[i]._server.matchlocation(server[i].path);
						if (isValidRequestURI(server[i].path))
							response.Create_response(server[i], "400");
						else if (checkUriLength(server[i].path))
							response.Create_response(server[i], "414");
						else if (checkRequestBodySize(server[i].body, std::stoul(server[i]._server.get_element("max_body_size"))))
							response.Create_response(server[i], "413");
						else 
							response.Create_response(server[i], "");
					}
					response.Create_response(server[i], "");
				}
			}
		}
	}
}
					// std::cout << "Method: " << server.method << std::endl;
					// std::cout << "Path: " << server.path << std::endl;
					// std::cout << "HTTP Version: " << server.http_version << std::endl;
					// std::cout << "Host: " << server.headers["Transfer-Encoding"] << std::endl;
					// std::cout << "Body: " << server.body << std::endl;
					//  std::cout.write(buffer, bytes_received);