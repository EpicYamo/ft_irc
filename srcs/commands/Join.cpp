/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:00:21 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/14 05:44:03 by aaycan           ###   ########.fr       */
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

	(void)key;
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
	else
		channel->add_member(&client, false);
	broadcast(*channel, ":" + client.get_prefix() + " JOIN "
		+ channel->get_name(), NULL);
	if (!channel->get_topic().empty())
		reply(client, RPL_TOPIC, channel->get_name() + " :"
			+ channel->get_topic());
	send_names(client, *channel);
}

void	Server::part_all_channels(Client &client)
{
	(void)client;
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
