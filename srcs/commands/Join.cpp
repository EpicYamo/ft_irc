/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:00:21 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/20 05:41:26 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static bool	is_valid_channel_name(const std::string &name);

void	Server::cmd_join(Client &client, const Message &msg)
{
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;
	std::string					key;
	size_t						i;

	if ((!msg.params.empty()) && (msg.params[0] == "0"))
	{
		part_all_channels(client);
		return ;
	}
	if (!msg.params.empty())
		channels = split_list(msg.params[0], ',');
	if (channels.empty())
	{
		reply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return ;
	}
	if (msg.params.size() > 1)
		keys = split_list(msg.params[1], ',');
	i = 0;
	while (i < channels.size())
	{
		key.clear();
		if (i < keys.size())
			key = keys[i];
		join_channel(client, channels[i], key);
		i++;
	}
}

void	Server::join_channel(Client &client, const std::string &name,
	const std::string &key)
{
	Channel	*channel;

	if (!is_valid_channel_name(name))
	{
		reply(client, ERR_NOSUCHCHANNEL, name + " :No such channel");
		return ;
	}
	channel = find_channel(name);
	if (channel == NULL)
	{
		_channels.insert(std::make_pair(irc_lower(name), Channel(name)));
		channel = find_channel(name);
		channel->add_member(&client, true);
	}
	else if (channel->has_member(&client))
		return ;
	else if (!can_join(client, *channel, key))
		return ;
	else
	{
		channel->add_member(&client, false);
		channel->remove_invite(&client);
	}
	broadcast(*channel, ":" + client.get_prefix() + " JOIN "
		+ channel->get_name(), NULL);
	if (!channel->get_topic().empty())
		reply(client, RPL_TOPIC, channel->get_name() + " :"
			+ channel->get_topic());
	send_names(client, *channel);
}

bool	Server::can_join(Client &client, const Channel &channel,
	const std::string &key)
{
	if ((!channel.get_key().empty()) && (key != channel.get_key()))
	{
		reply(client, ERR_BADCHANNELKEY,
			channel.get_name() + " :Cannot join channel (+k)");
		return (false);
	}
	if ((channel.is_invite_only()) && (!channel.is_invited(&client)))
	{
		reply(client, ERR_INVITEONLYCHAN,
			channel.get_name() + " :Cannot join channel (+i)");
		return (false);
	}
	if ((channel.get_limit() > 0)
		&& (channel.get_members().size() >= channel.get_limit())
		&& (!channel.is_invited(&client)))
	{
		reply(client, ERR_CHANNELISFULL,
			channel.get_name() + " :Cannot join channel (+l)");
		return (false);
	}
	return (true);
}

void	Server::part_all_channels(Client &client)
{
	std::map<std::string, Channel>::iterator	ch_it;
	Channel										*channel;

	ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		channel = &(ch_it->second);
		ch_it++;
		if (channel->has_member(&client))
			part_channel(client, *channel, client.get_nick());
	}
}

static bool	is_valid_channel_name(const std::string &name)
{
	size_t	i;

	if ((name.size() < 2) || (name.size() > 50))
		return (false);
	if ((name[0] != '#') && (name[0] != '&'))
		return (false);
	i = 1;
	while (i < name.size())
	{
		if ((name[i] == ' ') || (name[i] == ',') || (name[i] == '\x07')
			|| (name[i] == ':'))
			return (false);
		i++;
	}
	return (true);
}
