/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:11:02 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/20 23:19:16 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <set>

static std::string	sys_error(const std::string &func);
static bool			is_registration_cmd(const std::string &cmd);
static bool			prefix_matches(const Client &client, const std::string &prefix);

volatile sig_atomic_t	Server::_stop = 0;

Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _listen_fd(-1)
{
	_commands["CAP"] = &Server::cmd_cap;
	_commands["PASS"] = &Server::cmd_pass;
	_commands["NICK"] = &Server::cmd_nick;
	_commands["USER"] = &Server::cmd_user;
	_commands["PING"] = &Server::cmd_ping;
	_commands["PONG"] = &Server::cmd_pong;
	_commands["QUIT"] = &Server::cmd_quit;
	_commands["PRIVMSG"] = &Server::cmd_privmsg;
	_commands["NOTICE"] = &Server::cmd_notice;
	_commands["JOIN"] = &Server::cmd_join;
	_commands["NAMES"] = &Server::cmd_names;
	_commands["PART"] = &Server::cmd_part;
	_commands["MODE"] = &Server::cmd_mode;
	_commands["TOPIC"] = &Server::cmd_topic;
	_commands["KICK"] = &Server::cmd_kick;
	_commands["INVITE"] = &Server::cmd_invite;
	_commands["WHOIS"] = &Server::cmd_whois;
}

Server::~Server()
{
	size_t	i;

	i = 0;
	while (i < _pfds.size())
	{
		if (_pfds[i].fd != _listen_fd)
			close(_pfds[i].fd);
		i++;
	}
	if (_listen_fd >= 0)
		close(_listen_fd);
}

void	Server::signal_handler(int sig)
{
	(void)sig;
	_stop = 1;
}

void	Server::run()
{
	pollfd	pfd;

	setup_signals();
	setup_socket();
	pfd.fd = _listen_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	std::cout << "Server listening on port " << _port << std::endl;
	while (!_stop)
	{
		if (poll(&_pfds[0], _pfds.size(), -1) < 0)
		{
			if (_stop)
				break ;
			throw std::runtime_error(sys_error("poll"));
		}
		handle_events();
	}
	std::cout << std::endl << "Server shutting down" << std::endl;
}

void	Server::setup_signals()
{
	signal(SIGINT, Server::signal_handler);
	signal(SIGQUIT, Server::signal_handler);
	signal(SIGTERM, Server::signal_handler);
	signal(SIGPIPE, SIG_IGN);
}

void	Server::setup_socket()
{
	sockaddr_in	addr;
	int			opt;

	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw std::runtime_error(sys_error("socket"));
	opt = 1;
	if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error(sys_error("setsockopt"));
	if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error(sys_error("fcntl"));
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);
	if (bind(_listen_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw std::runtime_error(sys_error("bind"));
	if (listen(_listen_fd, SOMAXCONN) < 0)
		throw std::runtime_error(sys_error("listen"));
}

void	Server::handle_events()
{
	size_t	i;
	bool	removed;

	i = 0;
	while (i < _pfds.size())
	{
		removed = false;
		if (_pfds[i].fd == _listen_fd)
		{
			if (_pfds[i].revents & POLLIN)
				accept_client();
		}
		else
		{
			if (_pfds[i].revents & POLLIN)
				removed = !read_client(i);
			else if (_pfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				removed = true;
			if ((!removed) && (_pfds[i].revents & POLLOUT))
				removed = !write_client(i);
		}
		if (removed)
			remove_client(i);
		else
			i++;
	}
}

void	Server::accept_client()
{
	sockaddr_in	addr;
	socklen_t	len;
	int			fd;
	pollfd		pfd;

	len = sizeof(addr);
	fd = accept(_listen_fd, reinterpret_cast<sockaddr *>(&addr), &len);
	if (fd < 0)
		return ;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd);
		return ;
	}
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);
	_clients.insert(std::make_pair(fd, Client(fd, inet_ntoa(addr.sin_addr))));
	std::cout << "Client " << fd << " connected from "
		<< inet_ntoa(addr.sin_addr) << std::endl;
}

bool	Server::read_client(size_t i)
{
	char		buf[512];
	ssize_t		bytes;
	Client		*client;
	std::string	line;

	bytes = recv(_pfds[i].fd, buf, sizeof(buf), 0);
	if (bytes <= 0)
		return (false);
	client = &(_clients.find(_pfds[i].fd)->second);
	client->append_recv(buf, bytes);
	while ((!client->is_closing()) && (client->extract_line(line)))
		handle_line(*client, line);
	return (true);
}

bool	Server::write_client(size_t i)
{
	Client	*client;
	ssize_t	bytes;

	client = &(_clients.find(_pfds[i].fd)->second);
	if (!client->get_send_buf().empty())
	{
		bytes = send(_pfds[i].fd, client->get_send_buf().c_str(),
				client->get_send_buf().size(), 0);
		if (bytes < 0)
			return (false);
		client->erase_send(bytes);
	}
	if (client->get_send_buf().empty())
	{
		if (client->is_closing())
			return (false);
		_pfds[i].events = POLLIN;
	}
	return (true);
}

void	Server::remove_client(size_t i)
{
	Client	*client;

	client = &(_clients.find(_pfds[i].fd)->second);
	leave_all_channels(*client, "Remote host closed the connection");
	std::cout << "Client " << _pfds[i].fd << " disconnected" << std::endl;
	close(_pfds[i].fd);
	_clients.erase(_pfds[i].fd);
	_pfds.erase(_pfds.begin() + i);
}

void	Server::handle_line(Client &client, const std::string &line)
{
	Message									msg;
	std::map<std::string, t_cmd>::iterator	it;

	std::cout << "<< [" << client.get_fd() << "] " << line << std::endl;
	if (!parse_message(line, msg))
		return ;
	if ((!msg.prefix.empty()) && (!prefix_matches(client, msg.prefix)))
		return ;
	if ((!client.is_registered()) && (!is_registration_cmd(msg.command)))
	{
		reply(client, ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}
	it = _commands.find(msg.command);
	if (it == _commands.end())
	{
		reply(client, ERR_UNKNOWNCOMMAND, msg.command + " :Unknown command");
		return ;
	}
	(this->*(it->second))(client, msg);
}

void	Server::queue_msg(Client &client, const std::string &msg)
{
	size_t	i;

	if (client.is_closing())
		return ;
	std::cout << ">> [" << client.get_fd() << "] " << msg << std::endl;
	client.append_send(msg + "\r\n");
	i = 0;
	while (i < _pfds.size())
	{
		if (_pfds[i].fd == client.get_fd())
			_pfds[i].events |= POLLOUT;
		i++;
	}
}

void	Server::reply(Client &client, const std::string &code,
	const std::string &text)
{
	std::string	target;

	target = client.get_nick();
	if (target.empty())
		target = "*";
	queue_msg(client, ":" SERVER_NAME " " + code + " " + target + " " + text);
}

void	Server::send_list_reply(Client &client, const std::string &code,
	const std::string &field, const std::vector<std::string> &entries)
{
	std::string	header;
	std::string	list;
	size_t		i;

	header = ":" SERVER_NAME " " + code + " " + client.get_nick()
		+ " " + field + " :";
	i = 0;
	while (i < entries.size())
	{
		if ((!list.empty())
			&& (header.size() + list.size() + 1 + entries[i].size()
				> MAX_MSG_LEN))
		{
			reply(client, code, field + " :" + list);
			list.clear();
		}
		if (!list.empty())
			list += " ";
		list += entries[i];
		i++;
	}
	if (!list.empty())
		reply(client, code, field + " :" + list);
}

void	Server::disconnect_client(Client &client, const std::string &reason)
{
	size_t	i;

	if (client.is_closing())
		return ;
	queue_msg(client, "ERROR :Closing Link: " + client.get_ip()
		+ " (" + reason + ")");
	client.set_closing(true);
	leave_all_channels(client, reason);
	i = 0;
	while (i < _pfds.size())
	{
		if (_pfds[i].fd == client.get_fd())
			_pfds[i].events = POLLOUT;
		i++;
	}
}

void	Server::try_register(Client &client)
{
	if ((client.is_registered()) || (client.is_cap_negotiating()))
		return ;
	if ((client.get_nick().empty()) || (client.get_username().empty()))
		return ;
	if (!client.is_pass_ok())
	{
		reply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		disconnect_client(client, "Password incorrect");
		return ;
	}
	client.set_registered(true);
	reply(client, RPL_WELCOME, ":Welcome to the IRC network, "
		+ client.get_prefix());
	reply(client, RPL_YOURHOST, ":Your host is " SERVER_NAME
		", running version 1.0");
	reply(client, RPL_CREATED, ":This server was created for 42");
	reply(client, RPL_MYINFO, SERVER_NAME " 1.0 i itkol");
}

Client	*Server::find_client_by_nick(const std::string &nick)
{
	std::map<int, Client>::iterator	it;

	it = _clients.begin();
	while (it != _clients.end())
	{
		if ((!it->second.is_closing())
			&& (irc_lower(it->second.get_nick()) == irc_lower(nick)))
			return (&(it->second));
		it++;
	}
	return (NULL);
}

Channel	*Server::find_channel(const std::string &name)
{
	std::map<std::string, Channel>::iterator	it;

	it = _channels.find(irc_lower(name));
	if (it == _channels.end())
		return (NULL);
	return (&(it->second));
}

void	Server::broadcast(const Channel &channel, const std::string &msg,
	Client *except)
{
	std::map<Client *, bool>::const_iterator	it;

	it = channel.get_members().begin();
	while (it != channel.get_members().end())
	{
		if (it->first != except)
			queue_msg(*(it->first), msg);
		it++;
	}
}

void	Server::leave_all_channels(Client &client, const std::string &reason)
{
	std::map<std::string, Channel>::iterator	it;

	send_to_neighbors(client, ":" + client.get_prefix() + " QUIT :" + reason,
		false);
	it = _channels.begin();
	while (it != _channels.end())
	{
		it->second.remove_member(&client);
		if (it->second.is_empty())
			_channels.erase(it++);
		else
			it++;
	}
}

void	Server::send_to_neighbors(Client &client, const std::string &msg,
	bool include_self)
{
	std::set<Client *>							recipients;
	std::set<Client *>::iterator				rcp_it;
	std::map<std::string, Channel>::iterator	ch_it;
	std::map<Client *, bool>::const_iterator	mem_it;

	if (include_self)
		recipients.insert(&client);
	ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		if (ch_it->second.has_member(&client))
		{
			mem_it = ch_it->second.get_members().begin();
			while (mem_it != ch_it->second.get_members().end())
			{
				if (mem_it->first != &client)
					recipients.insert(mem_it->first);
				mem_it++;
			}
		}
		ch_it++;
	}
	rcp_it = recipients.begin();
	while (rcp_it != recipients.end())
	{
		queue_msg(**rcp_it, msg);
		rcp_it++;
	}
}

static std::string	sys_error(const std::string &func)
{
	return (func + "() failed: " + std::strerror(errno));
}

static bool	is_registration_cmd(const std::string &cmd)
{
	return ((cmd == "CAP") || (cmd == "PASS") || (cmd == "NICK")
		|| (cmd == "USER") || (cmd == "PING") || (cmd == "QUIT")
		|| (cmd == "PONG"));
}

static bool	prefix_matches(const Client &client, const std::string &prefix)
{
	std::string	nick;

	if (client.get_nick().empty())
		return (false);
	nick = prefix.substr(0, prefix.find_first_of("!@"));
	return (irc_lower(nick) == irc_lower(client.get_nick()));
}
