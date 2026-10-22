/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 01:44:31 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 20:30:31 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_privmsg(Client &client, const Message &msg)
{
	deliver_message(client, msg, false);
}

void	Server::cmd_notice(Client &client, const Message &msg)
{
	deliver_message(client, msg, true);
}

void	Server::deliver_message(Client &client, const Message &msg,
	bool is_notice)
{
	std::vector<std::string>	targets;
	size_t						i;

	if (!msg.params.empty())
		targets = split_list(msg.params[0], ',');
	if (targets.empty())
	{
		if (!is_notice)
			reply(client, ERR_NORECIPIENT,
				":No recipient given (" + msg.command + ")");
		return ;
	}
	if ((msg.params.size() < 2) || (msg.params[1].empty()))
	{
		if (!is_notice)
			reply(client, ERR_NOTEXTTOSEND, ":No text to send");
		return ;
	}
	client.set_last_message(time(NULL));
	i = 0;
	while ((i < targets.size()) && (i < MAX_TARGETS))
	{
		if ((targets[i][0] == '#') || (targets[i][0] == '&'))
			send_to_channel(client, msg.command, targets[i], msg.params[1],
				is_notice);
		else
			send_to_user(client, msg.command, targets[i], msg.params[1],
				is_notice);
		i++;
	}
}

void	Server::send_to_user(Client &sender, const std::string &command,
	const std::string &target, const std::string &text, bool is_notice)
{
	Client	*recipient;

	recipient = find_client_by_nick(target);
	if ((recipient == NULL) || (!recipient->is_registered()))
	{
		if (!is_notice)
			reply(sender, ERR_NOSUCHNICK, target + " :No such nick/channel");
		return ;
	}
	queue_msg(*recipient, ":" + sender.get_prefix() + " " + command + " "
		+ recipient->get_nick() + " :" + text);
}

void	Server::send_to_channel(Client &sender, const std::string &command,
	const std::string &target, const std::string &text, bool is_notice)
{
	Channel	*channel;

	channel = find_channel(target);
	if (channel == NULL)
	{
		if (!is_notice)
			reply(sender, ERR_NOSUCHNICK, target + " :No such nick/channel");
		return ;
	}
	broadcast(*channel, ":" + sender.get_prefix() + " " + command + " "
		+ channel->get_name() + " :" + text, &sender);
}
