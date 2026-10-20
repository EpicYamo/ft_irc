/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 06:53:07 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/20 06:53:38 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_invite(Client &client, const Message &msg)
{
	Client	*target;
	Channel	*channel;

	if ((msg.params.size() < 2) || (msg.params[0].empty())
		|| (msg.params[1].empty()))
	{
		reply(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return ;
	}
	target = find_client_by_nick(msg.params[0]);
	if ((target == NULL) || (!target->is_registered()))
	{
		reply(client, ERR_NOSUCHNICK, msg.params[0] + " :No such nick/channel");
		return ;
	}
	channel = find_channel(msg.params[1]);
	if (channel == NULL)
	{
		reply(client, ERR_NOSUCHCHANNEL, msg.params[1] + " :No such channel");
		return ;
	}
	if (!channel->has_member(&client))
	{
		reply(client, ERR_NOTONCHANNEL,
			channel->get_name() + " :You're not on that channel");
		return ;
	}
	if (!channel->is_operator(&client))
	{
		reply(client, ERR_CHANOPRIVSNEEDED,
			channel->get_name() + " :You're not channel operator");
		return ;
	}
	if (channel->has_member(target))
	{
		reply(client, ERR_USERONCHANNEL, target->get_nick() + " "
			+ channel->get_name() + " :is already on channel");
		return ;
	}
	if ((channel->is_invite_only()) || (channel->get_limit() > 0))
		channel->add_invite(target);
	reply(client, RPL_INVITING, target->get_nick() + " " + channel->get_name());
	queue_msg(*target, ":" + client.get_prefix() + " INVITE "
		+ target->get_nick() + " :" + channel->get_name());
}
