/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:38:13 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/20 04:21:41 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
	: _name(""), _topic(""), _members(), _topic_restricted(false),
	_invite_only(false), _key(""), _limit(0) {}

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _members(), _topic_restricted(false),
	_invite_only(false), _key(""), _limit(0) {}

Channel::Channel(const Channel &other)
	: _name(other._name), _topic(other._topic), _members(other._members),
	_topic_restricted(other._topic_restricted),
	_invite_only(other._invite_only), _key(other._key), _limit(other._limit) {}

Channel::~Channel() {}

Channel	&Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_members = other._members;
		_topic_restricted = other._topic_restricted;
		_invite_only = other._invite_only;
		_key = other._key;
		_limit = other._limit;
	}
	return (*this);
}

const std::string	&Channel::get_name() const
{
	return (_name);
}

const std::string	&Channel::get_topic() const
{
	return (_topic);
}

const std::map<Client *, bool>	&Channel::get_members() const
{
	return (_members);
}

bool	Channel::has_member(Client *client) const
{
	return (_members.find(client) != _members.end());
}

void	Channel::add_member(Client *client, bool is_operator)
{
	_members.insert(std::make_pair(client, is_operator));
}

void	Channel::remove_member(Client *client)
{
	_members.erase(client);
}

bool	Channel::is_empty() const
{
	return (_members.empty());
}

std::vector<std::string>	Channel::get_member_names() const
{
	std::vector<std::string>					names;
	std::map<Client *, bool>::const_iterator	it;

	it = _members.begin();
	while (it != _members.end())
	{
		if (it->second)
			names.push_back("@" + it->first->get_nick());
		else
			names.push_back(it->first->get_nick());
		it++;
	}
	return (names);
}

bool	Channel::is_operator(Client *client) const
{
	std::map<Client *, bool>::const_iterator	it;

	it = _members.find(client);
	if (it == _members.end())
		return (false);
	return (it->second);
}

bool	Channel::is_topic_restricted() const
{
	return (_topic_restricted);
}

void	Channel::set_topic(const std::string &topic)
{
	_topic = topic;
}

void	Channel::set_topic_restricted(bool value)
{
	_topic_restricted = value;
}

bool	Channel::is_invite_only() const
{
	return (_invite_only);
}

const std::string	&Channel::get_key() const
{
	return (_key);
}

size_t	Channel::get_limit() const
{
	return (_limit);
}

void	Channel::set_invite_only(bool value)
{
	_invite_only = value;
}

void	Channel::set_key(const std::string &key)
{
	_key = key;
}

void	Channel::set_limit(size_t limit)
{
	_limit = limit;
}

void	Channel::set_operator(Client *client, bool value)
{
	std::map<Client *, bool>::iterator	it;

	it = _members.find(client);
	if (it != _members.end())
		it->second = value;
}
