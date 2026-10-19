/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 22:57:59 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/19 22:58:05 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_kick(Client &client, const Message &msg)
{
	std::vector<std::string>	chans;
	std::vector<std::string>	nicks;
	std::string					reason;
	size_t						i;

	if (msg.params.size() >= 2)
	{
		chans = split_list(msg.params[0], ',');
		nicks = split_list(msg.params[1], ',');
	}
	if ((chans.empty()) || (nicks.empty())
		|| ((chans.size() != 1) && (chans.size() != nicks.size())))
	{
		reply(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return ;
	}
	if ((msg.params.size() > 2) && (!msg.params[2].empty()))
		reason = msg.params[2];
	else
		reason = client.get_nick();
	i = 0;
	while (i < nicks.size())
	{
		if (chans.size() == 1)
			kick_user(client, chans[0], nicks[i], reason);
		else
			kick_user(client, chans[i], nicks[i], reason);
		i++;
	}
}

void	Server::kick_user(Client &op, const std::string &chan_name,
	const std::string &nick, const std::string &reason)
{
	Channel	*channel;
	Client	*target;

	channel = find_channel(chan_name);
	if (channel == NULL)
	{
		reply(op, ERR_NOSUCHCHANNEL, chan_name + " :No such channel");
		return ;
	}
	if (!channel->has_member(&op))
	{
		reply(op, ERR_NOTONCHANNEL,
			channel->get_name() + " :You're not on that channel");
		return ;
	}
	if (!channel->is_operator(&op))
	{
		reply(op, ERR_CHANOPRIVSNEEDED,
			channel->get_name() + " :You're not channel operator");
		return ;
	}
	target = find_client_by_nick(nick);
	if ((target == NULL) || (!target->is_registered()))
	{
		reply(op, ERR_NOSUCHNICK, nick + " :No such nick/channel");
		return ;
	}
	if (!channel->has_member(target))
	{
		reply(op, ERR_USERNOTINCHANNEL, target->get_nick() + " "
			+ channel->get_name() + " :They aren't on that channel");
		return ;
	}
	broadcast(*channel, ":" + op.get_prefix() + " KICK " + channel->get_name()
		+ " " + target->get_nick() + " :" + reason, NULL);
	channel->remove_member(target);
	if (channel->is_empty())
		_channels.erase(irc_lower(channel->get_name()));
}
