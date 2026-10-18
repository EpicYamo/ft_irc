/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/18 19:05:49 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/18 19:10:31 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_part(Client &client, const Message &msg)
{
	std::vector<std::string>	chan_names;
	std::string					reason;
	Channel						*channel;
	size_t						i;

	if (!msg.params.empty())
		chan_names = split_list(msg.params[0], ',');
	if (chan_names.empty())
	{
		reply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return ;
	}
	if ((msg.params.size() > 1) && (!msg.params[1].empty()))
		reason = msg.params[1];
	else
		reason = client.get_nick();
	i = 0;
	while (i < chan_names.size())
	{
		channel = find_channel(chan_names[i]);
		if (channel == NULL)
			reply(client, ERR_NOSUCHCHANNEL,
				chan_names[i] + " :No such channel");
		else if (!channel->has_member(&client))
			reply(client, ERR_NOTONCHANNEL,
				channel->get_name() + " :You're not on that channel");
		else
			part_channel(client, *channel, reason);
		i++;
	}
}

void	Server::part_channel(Client &client, Channel &channel,
	const std::string &reason)
{
	broadcast(channel, ":" + client.get_prefix() + " PART "
		+ channel.get_name() + " :" + reason, NULL);
	channel.remove_member(&client);
	if (channel.is_empty())
		_channels.erase(irc_lower(channel.get_name()));
}
