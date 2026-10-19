/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 22:48:24 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/19 22:48:28 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_topic(Client &client, const Message &msg)
{
	Channel	*channel;

	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		reply(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return ;
	}
	channel = find_channel(msg.params[0]);
	if (channel == NULL)
	{
		reply(client, ERR_NOSUCHCHANNEL, msg.params[0] + " :No such channel");
		return ;
	}
	if (msg.params.size() < 2)
	{
		if (channel->get_topic().empty())
			reply(client, RPL_NOTOPIC, channel->get_name() + " :No topic is set");
		else
			reply(client, RPL_TOPIC, channel->get_name() + " :"
				+ channel->get_topic());
		return ;
	}
	if (!channel->has_member(&client))
	{
		reply(client, ERR_NOTONCHANNEL,
			channel->get_name() + " :You're not on that channel");
		return ;
	}
	if ((channel->is_topic_restricted()) && (!channel->is_operator(&client)))
	{
		reply(client, ERR_CHANOPRIVSNEEDED,
			channel->get_name() + " :You're not channel operator");
		return ;
	}
	channel->set_topic(msg.params[1]);
	broadcast(*channel, ":" + client.get_prefix() + " TOPIC "
		+ channel->get_name() + " :" + msg.params[1], NULL);
}
