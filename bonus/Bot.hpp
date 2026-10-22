/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 21:43:43 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 22:16:26 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include "Parser.hpp"
# include <string>
# include <csignal>

# define BOT_NICK "ircbot"

class Bot
{
	private:
		int								_port;
		std::string						_password;
		std::string						_channel;
		int								_fd;
		std::string						_recv_buf;
		std::string						_send_buf;
		static volatile sig_atomic_t	_stop;

		Bot();
		Bot(const Bot &other);
		Bot	&operator=(const Bot &other);

		static void	signal_handler(int sig);
		void		connect_server();
		bool		read_server();
		bool		write_server();
		void		send_line(const std::string &line);
		void		handle_line(const std::string &line);
		void		on_join(const Message &msg, const std::string &nick);
		void		on_privmsg(const Message &msg, const std::string &nick);

	public:
		Bot(int port, const std::string &password, const std::string &channel);
		~Bot();

		void	run();
};

#endif
