/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 22:29:40 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/19 22:31:19 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_mode(Client &client, const Message &msg)
{
	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		reply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return ;
	}
	if ((msg.params[0][0] == '#') || (msg.params[0][0] == '&'))
		channel_mode(client, msg);
	else
		user_mode(client, msg);
}

void	Server::user_mode(Client &client, const Message &msg)
{
	Client		*target;
	std::string	modes;
	bool		was_invisible;
	bool		adding;
	bool		unknown;
	size_t		i;

	target = find_client_by_nick(msg.params[0]);
	if ((target == NULL) || (!target->is_registered()))
	{
		reply(client, ERR_NOSUCHNICK, msg.params[0] + " :No such nick/channel");
		return ;
	}
	if (target != &client)
	{
		reply(client, ERR_USERSDONTMATCH, ":Cannot change mode for other users");
		return ;
	}
	if (msg.params.size() < 2)
	{
		if (client.is_invisible())
			reply(client, RPL_UMODEIS, "+i");
		else
			reply(client, RPL_UMODEIS, "+");
		return ;
	}
	modes = msg.params[1];
	was_invisible = client.is_invisible();
	adding = true;
	unknown = false;
	i = 0;
	while (i < modes.size())
	{
		if (modes[i] == '+')
			adding = true;
		else if (modes[i] == '-')
			adding = false;
		else if (modes[i] == 'i')
			client.set_invisible(adding);
		else
			unknown = true;
		i++;
	}
	if (client.is_invisible() != was_invisible)
	{
		if (client.is_invisible())
			queue_msg(client, ":" + client.get_prefix() + " MODE "
				+ client.get_nick() + " :+i");
		else
			queue_msg(client, ":" + client.get_prefix() + " MODE "
				+ client.get_nick() + " :-i");
	}
	if (unknown)
		reply(client, ERR_UMODEUNKNOWNFLAG, ":Unknown MODE flag");
}

void	Server::channel_mode(Client &client, const Message &msg)
{
	(void)client;
	(void)msg;
}
