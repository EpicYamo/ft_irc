/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:42:25 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/20 23:19:02 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <set>

void	Server::cmd_names(Client &client, const Message &msg)
{
	std::vector<std::string>	chan_names;
	Channel						*channel;
	size_t						i;

	if ((msg.params.size() > 1) && (msg.params[1] != SERVER_NAME))
	{
		reply(client, ERR_NOSUCHSERVER, msg.params[1] + " :No such server");
		return ;
	}
	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		send_all_names(client);
		return ;
	}
	chan_names = split_list(msg.params[0], ',');
	i = 0;
	while (i < chan_names.size())
	{
		channel = find_channel(chan_names[i]);
		if (channel != NULL)
			send_names(client, *channel);
		else
			reply(client, RPL_ENDOFNAMES, chan_names[i] + " :End of NAMES list");
		i++;
	}
}

void	Server::send_all_names(Client &client)
{
	std::map<std::string, Channel>::iterator	ch_it;
	std::map<int, Client>::iterator				cl_it;
	std::map<Client *, bool>::const_iterator	mem_it;
	std::set<Client *>							in_channel;
	std::vector<std::string>					no_channel;

	ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		send_list_reply(client, RPL_NAMREPLY, "= " + ch_it->second.get_name(),
			ch_it->second.get_member_names());
		mem_it = ch_it->second.get_members().begin();
		while (mem_it != ch_it->second.get_members().end())
		{
			in_channel.insert(mem_it->first);
			mem_it++;
		}
		ch_it++;
	}
	cl_it = _clients.begin();
	while (cl_it != _clients.end())
	{
		if ((cl_it->second.is_registered()) && (!cl_it->second.is_closing())
			&& (in_channel.count(&cl_it->second) == 0))
			no_channel.push_back(cl_it->second.get_nick());
		cl_it++;
	}
	send_list_reply(client, RPL_NAMREPLY, "* *", no_channel);
	reply(client, RPL_ENDOFNAMES, "* :End of NAMES list");
}

void	Server::send_names(Client &client, const Channel &channel)
{
	send_list_reply(client, RPL_NAMREPLY, "= " + channel.get_name(),
		channel.get_member_names());
	reply(client, RPL_ENDOFNAMES, channel.get_name() + " :End of NAMES list");
}
