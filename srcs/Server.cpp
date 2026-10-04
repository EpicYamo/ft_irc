/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:11:02 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/04 23:03:48 by aaycan           ###   ########.fr       */
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

static std::string	sys_error(const std::string &func);

volatile sig_atomic_t	Server::_stop = 0;

Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _listen_fd(-1) {}

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
		else if (_pfds[i].revents & POLLIN)
			removed = !read_client(i);
		else if (_pfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			removed = true;
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
	char	buf[512];
	ssize_t	bytes;
	int		fd;

	fd = _pfds[i].fd;
	bytes = recv(fd, buf, sizeof(buf), 0);
	if (bytes <= 0)
		return (false);
	_clients.find(fd)->second.get_recv_buf().append(buf, bytes);
	std::cout << "[" << fd << "] " << std::string(buf, bytes);
	return (true);
}

void	Server::remove_client(size_t i)
{
	std::cout << "Client " << _pfds[i].fd << " disconnected" << std::endl;
	close(_pfds[i].fd);
	_clients.erase(_pfds[i].fd);
	_pfds.erase(_pfds.begin() + i);
}

static std::string	sys_error(const std::string &func)
{
	return (func + "() failed: " + std::strerror(errno));
}
