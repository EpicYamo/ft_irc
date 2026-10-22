/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:07:23 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 07:58:09 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: _fd(-1), _ip(""), _recv_buf(""), _send_buf(""), _nick(""),
	_username(""), _realname(""), _pass_ok(false), _registered(false),
	_closing(false), _cap_negotiating(false), _invisible(false),
	_sendq_exceeded(false), _ping_sent(false), _connect_time(0),
	_last_activity(0), _closing_time(0), _flood_timer(0) {}

Client::Client(int fd, const std::string &ip)
	: _fd(fd), _ip(ip), _recv_buf(""), _send_buf(""), _nick(""),
	_username(""), _realname(""), _pass_ok(false), _registered(false),
	_closing(false), _cap_negotiating(false), _invisible(false),
	_sendq_exceeded(false), _ping_sent(false), _connect_time(time(NULL)),
	_last_activity(time(NULL)), _closing_time(0), _flood_timer(0) {}

Client::Client(const Client &other)
	: _fd(other._fd), _ip(other._ip), _recv_buf(other._recv_buf),
	_send_buf(other._send_buf), _nick(other._nick),
	_username(other._username), _realname(other._realname),
	_pass_ok(other._pass_ok), _registered(other._registered),
	_closing(other._closing), _cap_negotiating(other._cap_negotiating),
	_invisible(other._invisible), _sendq_exceeded(other._sendq_exceeded),
	_ping_sent(other._ping_sent), _connect_time(other._connect_time),
	_last_activity(other._last_activity), _closing_time(other._closing_time),
	_flood_timer(other._flood_timer) {}

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
		_closing = other._closing;
		_cap_negotiating = other._cap_negotiating;
		_invisible = other._invisible;
		_sendq_exceeded = other._sendq_exceeded;
		_ping_sent = other._ping_sent;
		_connect_time = other._connect_time;
		_last_activity = other._last_activity;
		_closing_time = other._closing_time;
		_flood_timer = other._flood_timer;
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

	_recv_buf.erase(0, _recv_buf.find_first_not_of("\r\n"));
	pos = _recv_buf.find_first_of("\r\n");
	if (pos == std::string::npos)
		return (false);
	line = _recv_buf.substr(0, pos);
	_recv_buf.erase(0, pos + 1);
	pos = line.find('\0');
	if (pos != std::string::npos)
		line.erase(pos);
	if (line.size() > MAX_MSG_LEN)
		line.erase(MAX_MSG_LEN);
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

void	Client::release_buffers()
{
	std::string().swap(_recv_buf);
	std::string().swap(_send_buf);
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

time_t	Client::get_connect_time() const
{
	return (_connect_time);
}

time_t	Client::get_last_activity() const
{
	return (_last_activity);
}

time_t	Client::get_closing_time() const
{
	return (_closing_time);
}

time_t	Client::get_flood_timer() const
{
	return (_flood_timer);
}

bool	Client::is_pass_ok() const
{
	return (_pass_ok);
}

bool	Client::is_registered() const
{
	return (_registered);
}

bool	Client::is_closing() const
{
	return (_closing);
}

bool	Client::is_cap_negotiating() const
{
	return (_cap_negotiating);
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

void	Client::set_closing(bool value)
{
	_closing = value;
	if (value)
		_closing_time = time(NULL);
}

void	Client::set_cap_negotiating(bool value)
{
	_cap_negotiating = value;
}

bool	Client::is_invisible() const
{
	return (_invisible);
}

bool	Client::is_recv_overflow() const
{
	return (_recv_buf.size() > MAX_RECV_LEN);
}

bool	Client::is_sendq_exceeded() const
{
	return (_sendq_exceeded);
}

bool	Client::is_ping_sent() const
{
	return (_ping_sent);
}

void	Client::set_invisible(bool value)
{
	_invisible = value;
}

void	Client::set_sendq_exceeded(bool value)
{
	_sendq_exceeded = value;
}

void	Client::set_ping_sent(bool value)
{
	_ping_sent = value;
}

void	Client::set_last_activity(time_t value)
{
	_last_activity = value;
}

void	Client::set_flood_timer(time_t value)
{
	_flood_timer = value;
}
