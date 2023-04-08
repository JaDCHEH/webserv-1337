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
        server.body = std::string(b + 4);         // Set the body to everything after the headers
        // Return the parsed Server object
        server._buffer_state = 0;
        server._first = 0;
		server._amount_written = 0;
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
        fd_set reads, writes;

        std::map <int ,Request> server;
        response        response;
        std::vector <Client> clients;
        while (1)
        {
                FD_ZERO(&reads);
                FD_ZERO(&writes);
                FD_SET(socket_listen, &reads);
                int max_socket = socket_listen;
                for (size_t i = 0; i < clients.size(); i++)
                {
                        if(clients[i].isSending == true)
                                FD_SET(clients[i].socket, &reads);
                        else
                                FD_SET(clients[i].socket, &writes);
                        if (clients[i].socket > max_socket)
                                max_socket = clients[i].socket;
                }
                if (select(max_socket + 1, &reads, &writes, 0, 0) == -1)
                {
                        std::cout << "Failed to select. errno: "
                                          << " " << strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                }
                // SOCKET i;

                if (FD_ISSET(socket_listen, &reads))
                {
                        Client client;
                        bzero(&client, sizeof(Client));
                        client.isSending = true;
                        client.address_length = sizeof(client.address);
                        SOCKET socket_client = accept(socket_listen,
                                                                                  (struct sockaddr *)&client.address, &client.address_length);
                        if (!ISVALIDSOCKET(socket_client))
                        {
                                std::cout << "Accept failed errno: "
                                                  << " " << strerror(errno) << std::endl;
                                continue;
                        }
                        client.socket = socket_client;
                        clients.push_back(client);
                        FD_SET(socket_client, &reads);
                        char address_buffer[100];
                        getnameinfo((struct sockaddr *)&client.address,
                                                client.address_length,
                                                address_buffer, sizeof(address_buffer), 0, 0,
                                                NI_NUMERICHOST);
                        std::cout << "New connection from: " << address_buffer << std::endl;
                }

                for (size_t i = 0; i < clients.size(); i++)
                {
                        if (clients[i].isSending && FD_ISSET(clients[i].socket, &reads))
                        {
                                std::cout << "Been here" << std::endl;

                                char buffer[4096];
                                memset(buffer, 0, sizeof(buffer));
                                int bytes_received = recv(clients[i].socket, buffer, sizeof(buffer), 0);
                                if (bytes_received < 1)
                                {
                                        std::cout << "Disconnected errno : " << strerror(errno) << std::endl;

                                }
                                else
                                {
                                        std::cout << "Received " << bytes_received << " bytes from client\n";

                                        server[clients[i].socket] = parse(buffer);
                                        server[clients[i].socket].socket = clients[i].socket;
                                        server[clients[i].socket]._server = conf.matchname(server[clients[i].socket].host);
                                        server[clients[i].socket]._location = server[clients[i].socket]._server.matchlocation(server[clients[i].socket].path);
                                        clients[i].isSending = false;

                                }
                        }
                        else if (FD_ISSET(clients[i].socket, &writes))
                        {
                                if (isValidRequestURI(server[clients[i].socket].path))
                                        response.Create_response(server[clients[i].socket], "400");
                                else if (checkUriLength(server[clients[i].socket].path))
                                        response.Create_response(server[clients[i].socket], "414");
                                else if (checkRequestBodySize(server[clients[i].socket].body, std::stoul(server[clients[i].socket]._server.get_element("max_body_size"))))
                                        response.Create_response(server[clients[i].socket], "413");
                                else
                                        response.Create_response(server[clients[i].socket], "");
                        }
                }
        }
}
