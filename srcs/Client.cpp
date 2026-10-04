/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:07:23 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/04 21:16:44 by aaycan           ###   ########.fr       */
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

std::string	&Client::get_recv_buf()
{
	return (_recv_buf);
}
