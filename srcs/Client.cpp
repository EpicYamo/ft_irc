/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:07:23 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/05 23:32:46 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: _fd(-1), _ip(""), _recv_buf("") {}

Client::Client(int fd, const std::string &ip)
	: _fd(fd), _ip(ip), _recv_buf("") {}

Client::Client(const Client &other)
	: _fd(other._fd), _ip(other._ip), _recv_buf(other._recv_buf) {}

Client::~Client() {}

Client	&Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_ip = other._ip;
		_recv_buf = other._recv_buf;
	}
	return (*this);
}

int	Client::get_fd() const
{
	return (_fd);
}

const std::string	&Client::get_ip() const
{
	return (_ip);
}

void	Client::append_recv(const char *data, size_t len)
{
	_recv_buf.append(data, len);
}

bool	Client::extract_line(std::string &line)
{
	size_t	pos;

	pos = _recv_buf.find('\n');
	if (pos == std::string::npos)
		return (false);
	line = _recv_buf.substr(0, pos);
	_recv_buf.erase(0, pos + 1);
	if ((!line.empty()) && (line[line.size() - 1] == '\r'))
		line.erase(line.size() - 1);
	return (true);
}
