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
	if (ac != 2)
		return 1;
	config conf;
	conf.conf(av[1]);
	conf.setup_sockets();

	while (1)
	{
		conf.setup_cnx();
	}
}
