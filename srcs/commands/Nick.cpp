/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 01:53:15 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/09 02:45:20 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cctype>

static bool	is_special(char c);
static bool	is_valid_nick(const std::string &nick);

void	Server::cmd_nick(Client &client, const Message &msg)
{
	Client		*other;
	std::string	old_prefix;

	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		reply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return ;
	}
	if (!is_valid_nick(msg.params[0]))
	{
		reply(client, ERR_ERRONEUSNICKNAME,
			msg.params[0] + " :Erroneous nickname");
		return ;
	}
	other = find_client_by_nick(msg.params[0]);
	if ((other != NULL) && (other != &client))
	{
		reply(client, ERR_NICKNAMEINUSE,
			msg.params[0] + " :Nickname is already in use");
		return ;
	}
	old_prefix = client.get_prefix();
	client.set_nick(msg.params[0]);
	if (client.is_registered())
		queue_msg(client, ":" + old_prefix + " NICK :" + client.get_nick());
	else
		try_register(client);
}

static bool	is_special(char c)
{
	return ((c == '[') || (c == ']') || (c == '\\') || (c == '`')
		|| (c == '_') || (c == '^') || (c == '{') || (c == '|')
		|| (c == '}'));
}

static bool	is_valid_nick(const std::string &nick)
{
	size_t	i;

	if (nick.size() > 9)
		return (false);
	if ((!std::isalpha(static_cast<unsigned char>(nick[0])))
		&& (!is_special(nick[0])))
		return (false);
	i = 1;
	while (i < nick.size())
	{
		if ((!std::isalnum(static_cast<unsigned char>(nick[i])))
			&& (!is_special(nick[i])) && (nick[i] != '-'))
			return (false);
		i++;
	}
	return (true);
}
