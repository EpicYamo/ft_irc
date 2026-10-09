/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 02:51:12 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/09 02:51:18 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_user(Client &client, const Message &msg)
{
	if (client.is_registered())
	{
		reply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return ;
	}
	if ((msg.params.size() < 4) || (msg.params[0].empty()))
	{
		reply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	client.set_user(msg.params[0], msg.params[3]);
	try_register(client);
}
