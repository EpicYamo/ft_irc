/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:42:25 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/14 06:05:30 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::send_names(Client &client, const Channel &channel)
{
	std::map<Client *, bool>::const_iterator	it;
	std::string									header;
	std::string									names;
	std::string									entry;

	header = ":" SERVER_NAME " " RPL_NAMREPLY " " + client.get_nick()
		+ " = " + channel.get_name() + " :";
	it = channel.get_members().begin();
	while (it != channel.get_members().end())
	{
		entry = it->first->get_nick();
		if (it->second)
			entry = "@" + entry;
		if ((!names.empty())
			&& (header.size() + names.size() + 1 + entry.size() > MAX_MSG_LEN))
		{
			reply(client, RPL_NAMREPLY, "= " + channel.get_name() + " :" + names);
			names.clear();
		}
		if (!names.empty())
			names += " ";
		names += entry;
		it++;
	}
	if (!names.empty())
		reply(client, RPL_NAMREPLY, "= " + channel.get_name() + " :" + names);
	reply(client, RPL_ENDOFNAMES, channel.get_name() + " :End of NAMES list");
}
