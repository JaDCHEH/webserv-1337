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

#include "config/config.hpp"
#include <sstream>

int main(int ac, char **av)
{
	signal(SIGPIPE, SIG_IGN);
	if (ac != 2)
		return 1;
	config conf;
	conf.conf(av[1]);
	conf.setup_sockets();
	while (1)
	{
		int max_socket = -1;
		fd_set	reads;
		fd_set	writes;

		FD_ZERO(&reads);
		FD_ZERO(&writes);
		for (size_t i = 0; i < conf._servers.size(); i++)
		{
			FD_SET(conf._servers[i].getSocket(), &reads);
			if (conf._servers[i].getSocket() > max_socket)
				max_socket = conf._servers[i].getSocket();
			for (size_t j = 0; j < conf._servers[i].clients.size(); j++)
			{
				if (conf._servers[i].clients[j].isSending == true)
					FD_SET(conf._servers[i].clients[j].socket, &reads);
				else
					FD_SET(conf._servers[i].clients[j].socket, &writes);
				if (conf._servers[i].clients[j].socket > max_socket)
					max_socket = conf._servers[i].clients[j].socket;
			}
		}
		if (select(max_socket + 1, &reads, &writes, 0, 0) == -1)
		{
			std::cout << "Failed to select. errno: "
					  << " " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		conf.setup_cnx(reads, writes);
	}
}
