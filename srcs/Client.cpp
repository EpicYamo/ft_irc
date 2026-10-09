/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:07:23 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/08 21:57:45 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: _fd(-1), _ip(""), _recv_buf(""), _send_buf(""), _nick(""),
	_username(""), _realname(""), _pass_ok(false), _registered(false) {}

Client::Client(int fd, const std::string &ip)
	: _fd(fd), _ip(ip), _recv_buf(""), _send_buf(""), _nick(""),
	_username(""), _realname(""), _pass_ok(false), _registered(false) {}

Client::Client(const Client &other)
	: _fd(other._fd), _ip(other._ip), _recv_buf(other._recv_buf),
	_send_buf(other._send_buf), _nick(other._nick),
	_username(other._username), _realname(other._realname),
	_pass_ok(other._pass_ok), _registered(other._registered) {}

Client::~Client() {}

Client	&Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_ip = other._ip;
		_recv_buf = other._recv_buf;
		_send_buf = other._send_buf;
		_nick = other._nick;
		_username = other._username;
		_realname = other._realname;
		_pass_ok = other._pass_ok;
		_registered = other._registered;
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

void	Client::append_send(const std::string &data)
{
	_send_buf += data;
}

const std::string	&Client::get_send_buf() const
{
	return (_send_buf);
}

void	Client::erase_send(size_t len)
{
	_send_buf.erase(0, len);
}

const std::string	&Client::get_nick() const
{
	return (_nick);
}

const std::string	&Client::get_username() const
{
	return (_username);
}

const std::string	&Client::get_realname() const
{
	return (_realname);
}

std::string	Client::get_prefix() const
{
	return (_nick + "!" + _username + "@" + _ip);
}

bool	Client::is_pass_ok() const
{
	return (_pass_ok);
}

bool	Client::is_registered() const
{
	return (_registered);
}

void	Client::set_nick(const std::string &nick)
{
	_nick = nick;
}

void	Client::set_user(const std::string &username, const std::string &realname)
{
	_username = username;
	_realname = realname;
}

void	Client::set_pass_ok(bool value)
{
	_pass_ok = value;
}

void	Client::set_registered(bool value)
{
	_registered = value;
}
