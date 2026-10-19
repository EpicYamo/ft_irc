/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:04:56 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/19 22:57:02 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Parser.hpp"
# include "Replies.hpp"
# include "Channel.hpp"
# include <string>
# include <vector>
# include <csignal>
# include <poll.h>
# include <map>
# include <cstddef>

# define MAX_MSG_LEN 510

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
		std::map<std::string, Channel>	_channels;
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
		void		disconnect_client(Client &client, const std::string &reason);
		void		deliver_message(Client &client, const Message &msg,
						bool is_notice);
		void		join_channel(Client &client, const std::string &name,
						const std::string &key);
		void		part_all_channels(Client &client);
		void		part_channel(Client &client, Channel &channel,
						const std::string &reason);
		void		broadcast(const Channel &channel, const std::string &msg,
						Client *except);
		void		send_names(Client &client, const Channel &channel);
		void		send_to_user(Client &sender, const std::string &command,
						const std::string &target, const std::string &text,
						bool is_notice);
		void		send_to_channel(Client &sender, const std::string &command,
						const std::string &target, const std::string &text,
						bool is_notice);
		void		reply(Client &client, const std::string &code,
						const std::string &text);
		void		try_register(Client &client);
		void		leave_all_channels(Client &client, const std::string &reason);
		void		send_to_neighbors(Client &client, const std::string &msg,
						bool include_self);
		void		send_all_names(Client &client);
		void		send_namreply(Client &client, const std::string &field,
						const std::vector<std::string> &entries);
		void		kick_user(Client &op, const std::string &chan_name,
						const std::string &nick, const std::string &reason);
		void		user_mode(Client &client, const Message &msg);
		void		channel_mode(Client &client, const Message &msg);
		Client		*find_client_by_nick(const std::string &nick);
		Channel		*find_channel(const std::string &name);

		void		cmd_cap(Client &client, const Message &msg);
		void		cmd_pass(Client &client, const Message &msg);
		void		cmd_nick(Client &client, const Message &msg);
		void		cmd_user(Client &client, const Message &msg);
		void		cmd_ping(Client &client, const Message &msg);
		void		cmd_pong(Client &client, const Message &msg);
		void		cmd_quit(Client &client, const Message &msg);
		void		cmd_privmsg(Client &client, const Message &msg);
		void		cmd_notice(Client &client, const Message &msg);
		void		cmd_join(Client &client, const Message &msg);
		void		cmd_names(Client &client, const Message &msg);
		void		cmd_part(Client &client, const Message &msg);
		void		cmd_mode(Client &client, const Message &msg);
		void		cmd_topic(Client &client, const Message &msg);
		void		cmd_kick(Client &client, const Message &msg);

	public:
		Server(int port, const std::string &password);
		~Server();

		void	run();
};

#endif
