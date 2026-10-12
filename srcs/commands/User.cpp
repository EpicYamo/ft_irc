/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 02:51:12 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/12 20:33:32 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static std::string	sanitize_username(const std::string &raw);

void	Server::cmd_user(Client &client, const Message &msg)
{
	std::string	username;

	if (client.is_registered())
	{
		reply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return ;
	}
	if (msg.params.size() < 4)
	{
		reply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	username = sanitize_username(msg.params[0]);
	if (username.empty())
	{
		reply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	client.set_user(username, msg.params[3]);
	try_register(client);
}

static std::string	sanitize_username(const std::string &raw)
{
	std::string	result;
	size_t		i;

	i = 0;
	while ((i < raw.size()) && (result.size() < USERLEN))
	{
		if (raw[i] != '@')
			result += raw[i];
		i++;
	}
	return (result);
}
