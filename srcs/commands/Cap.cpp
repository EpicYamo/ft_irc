/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 01:48:04 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/12 19:20:39 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Replies.hpp"

void	Server::cmd_cap(Client &client, const Message &msg)
{
	std::string	sub;
	std::string	caps;

	if (msg.params.empty())
	{
		reply(client, ERR_NEEDMOREPARAMS, "CAP :Not enough parameters");
		return ;
	}
	sub = msg.params[0];
	if ((sub == "LS") || (sub == "REQ"))
	{
		if (!client.is_registered())
			client.set_cap_negotiating(true);
		if (sub == "LS")
			reply(client, "CAP", "LS :");
		else
		{
			if (msg.params.size() > 1)
				caps = msg.params[1];
			reply(client, "CAP", "NAK :" + caps);
		}
	}
	else if (sub == "LIST")
		reply(client, "CAP", "LIST :");
	else if (sub == "END")
	{
		if (client.is_cap_negotiating())
		{
			client.set_cap_negotiating(false);
			try_register(client);
		}
	}
	else
		reply(client, ERR_INVALIDCAPCMD, sub + " :Invalid CAP command");
}
