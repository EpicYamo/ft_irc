/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:04:56 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/02 02:35:03 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <csignal>
# include <poll.h>

class Server
{
	private:
		int								_port;
		std::string						_password;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		static volatile sig_atomic_t	_stop;

		Server();
		Server(const Server &other);
		Server	&operator=(const Server &other);

		void		setup_signals();
		void		setup_socket();
		static void	signal_handler(int sig);

	public:
		Server(int port, const std::string &password);
		~Server();

		void	run();
};

#endif
