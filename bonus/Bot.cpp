/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 21:42:11 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 22:46:13 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static bool	is_join_error(const std::string &code);

volatile sig_atomic_t	Bot::_stop = 0;

Bot::Bot(int port, const std::string &password, const std::string &channel)
	: _port(port), _password(password), _channel(channel), _fd(-1),
	_recv_buf(""), _send_buf("") {}

Bot::~Bot()
{
	if (_fd >= 0)
		close(_fd);
}

void	Bot::signal_handler(int sig)
{
	(void)sig;
	_stop = 1;
}

void	Bot::run()
{
	pollfd	pfd;

	signal(SIGINT, Bot::signal_handler);
	signal(SIGTERM, Bot::signal_handler);
	signal(SIGPIPE, SIG_IGN);
	connect_server();
	send_line("PASS " + _password);
	send_line("NICK " BOT_NICK);
	send_line("USER bot 0 * :ft_irc welcome bot");
	while (!_stop)
	{
		pfd.fd = _fd;
		pfd.events = POLLIN;
		if (!_send_buf.empty())
			pfd.events |= POLLOUT;
		pfd.revents = 0;
		if (poll(&pfd, 1, -1) < 0)
		{
			if (_stop)
				break ;
			throw std::runtime_error("poll failed");
		}
		if ((pfd.revents & POLLIN) && (!read_server()))
			return ;
		if ((pfd.revents & POLLOUT) && (!write_server()))
			return ;
		if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
			return ;
	}
	_send_buf = "QUIT :Bot shutting down\r\n";
	write_server();
}

void	Bot::connect_server()
{
	sockaddr_in	addr;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw std::runtime_error("socket failed");
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(_port);
	if (connect(_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw std::runtime_error("cannot connect to the server");
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl failed");
}

bool	Bot::read_server()
{
	char		buf[512];
	ssize_t		bytes;
	size_t		pos;
	std::string	line;

	bytes = recv(_fd, buf, sizeof(buf), 0);
	if (bytes <= 0)
	{
		std::cerr << "Server closed the connection" << std::endl;
		return (false);
	}
	_recv_buf.append(buf, bytes);
	pos = _recv_buf.find('\n');
	while (pos != std::string::npos)
	{
		line = _recv_buf.substr(0, pos);
		_recv_buf.erase(0, pos + 1);
		if ((!line.empty()) && (line[line.size() - 1] == '\r'))
			line.erase(line.size() - 1);
		handle_line(line);
		pos = _recv_buf.find('\n');
	}
	return (true);
}

bool	Bot::write_server()
{
	ssize_t	bytes;

	bytes = send(_fd, _send_buf.c_str(), _send_buf.size(), 0);
	if (bytes < 0)
		return (false);
	_send_buf.erase(0, bytes);
	return (true);
}

void	Bot::send_line(const std::string &line)
{
	_send_buf += line + "\r\n";
}

void	Bot::handle_line(const std::string &line)
{
	Message		msg;
	std::string	nick;

	if (!parse_message(line, msg))
		return ;
	nick = msg.prefix.substr(0, msg.prefix.find('!'));
	if ((msg.command == "PING") && (!msg.params.empty()))
		send_line("PONG :" + msg.params[0]);
	else if ((is_join_error(msg.command)) && (msg.params.size() > 2))
	{
		std::cerr << "Error: cannot join " << msg.params[1] << ": "
			<< msg.params[2] << std::endl;
		_stop = 1;
	}
	else if (msg.command == "JOIN")
		on_join(msg, nick);
	else if (msg.command == "001")
		send_line("JOIN " + _channel);
	else if (msg.command == "433")
	{
		std::cerr << "Error: nickname " BOT_NICK " is already in use"
			<< std::endl;
		_stop = 1;
	}
	else if (msg.command == "JOIN")
		on_join(msg, nick);
	else if (msg.command == "PRIVMSG")
		on_privmsg(msg, nick);
	else if (msg.command == "ERROR")
		_stop = 1;
}

void	Bot::on_join(const Message &msg, const std::string &nick)
{
	if ((msg.params.empty()) || (irc_lower(nick) == BOT_NICK))
		return ;
	send_line("NOTICE " + msg.params[0] + " :Welcome to " + msg.params[0]
		+ ", " + nick + "!");
}

void	Bot::on_privmsg(const Message &msg, const std::string &nick)
{
	std::string	target;

	if ((msg.params.size() < 2) || (msg.params[1] != "!help"))
		return ;
	target = msg.params[0];
	if (irc_lower(target) == BOT_NICK)
		target = nick;
	send_line("NOTICE " + target + " :I greet everyone who joins "
		+ _channel + ". Commands: !help");
}

static bool	is_join_error(const std::string &code)
{
	return ((code == "403") || (code == "405") || (code == "471")
		|| (code == "473") || (code == "475"));
}
