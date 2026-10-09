/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:04:56 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/09 00:09:45 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Parser.hpp"
# include "Replies.hpp"
# include <string>
# include <vector>
# include <csignal>
# include <poll.h>
# include <map>

class Server
{
	private:
		typedef void	(Server::*t_cmd)(Client &client, const Message &msg);

		int								_port;
		std::string						_password;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		std::map<int, Client>			_clients;
		std::map<std::string, t_cmd>	_commands;
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
		void		handle_line(Client &client, const std::string &line);
		bool		write_client(size_t i);
		void		queue_msg(Client &client, const std::string &msg);
		void		reply(Client &client, const std::string &code,
						const std::string &text);
		void		try_register(Client &client);
		Client		*find_client_by_nick(const std::string &nick);

		void		cmd_cap(Client &client, const Message &msg);
		void		cmd_pass(Client &client, const Message &msg);
		void		cmd_nick(Client &client, const Message &msg);
		void		cmd_user(Client &client, const Message &msg);

	public:
		Server(int port, const std::string &password);
		~Server();

		void	run();
};

#endif
