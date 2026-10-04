/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:04:56 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/04 20:11:13 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include <string>
# include <vector>
# include <csignal>
# include <poll.h>
# include <map>

class Server
{
	private:
		int								_port;
		std::string						_password;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		std::map<int, Client>			_clients;
		static volatile sig_atomic_t	_stop;

		Server();
		Server(const Server &other);
		Server	&operator=(const Server &other);

		void		setup_signals();
		void		setup_socket();
		static void	signal_handler(int sig);
		void		handle_events();
		void		accept_client();
		bool		read_client(size_t i);
		void		remove_client(size_t i);

	public:
		Server(int port, const std::string &password);
		~Server();

		void	run();
};

#endif
