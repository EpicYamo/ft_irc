/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:38:13 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/17 01:43:37 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
	: _name(""), _topic(""), _members() {}

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _members() {}

Channel::Channel(const Channel &other)
	: _name(other._name), _topic(other._topic), _members(other._members) {}

Channel::~Channel() {}

Channel	&Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_members = other._members;
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
