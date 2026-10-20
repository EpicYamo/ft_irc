/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 01:21:27 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/21 02:05:34 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static bool	is_who_match(const std::string &mask, Client &target);

void	Server::cmd_who(Client &client, const Message &msg)
{
	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		reply(client, ERR_NEEDMOREPARAMS, "WHO :Not enough parameters");
		return ;
	}
	if ((msg.params.size() > 1) && (msg.params[1] == "o"))
	{
		reply(client, RPL_ENDOFWHO, msg.params[0] + " :End of WHO list");
		return ;
	}
	if ((msg.params[0][0] == '#') || (msg.params[0][0] == '&'))
		who_channel(client, msg.params[0]);
	else
		who_mask(client, msg.params[0]);
}

void	Server::who_channel(Client &client, const std::string &name)
{
	Channel										*channel;
	std::map<Client *, bool>::const_iterator	mem_it;
	bool										show_all;

	channel = find_channel(name);
	if (channel != NULL)
	{
		show_all = channel->has_member(&client);
		mem_it = channel->get_members().begin();
		while (mem_it != channel->get_members().end())
		{
			if ((show_all) || (!mem_it->first->is_invisible()))
				send_whoreply(client, *mem_it->first, channel->get_name(),
					mem_it->second);
			mem_it++;
		}
	}
	reply(client, RPL_ENDOFWHO, name + " :End of WHO list");
}

void	Server::who_mask(Client &client, const std::string &mask)
{
	Client							*target;
	std::map<int, Client>::iterator	cl_it;

	target = find_client_by_nick(mask);
	if ((mask.find_first_of("*?") == std::string::npos) && (target != NULL)
		&& (target->is_registered()))
		send_who_user(client, *target);
	else
	{
		cl_it = _clients.begin();
		while (cl_it != _clients.end())
		{
			if ((cl_it->second.is_registered())
				&& (!cl_it->second.is_closing())
				&& (is_visible(client, cl_it->second))
				&& ((mask == "0") || (is_who_match(mask, cl_it->second))))
				send_who_user(client, cl_it->second);
			cl_it++;
		}
	}
	reply(client, RPL_ENDOFWHO, mask + " :End of WHO list");
}

void	Server::send_who_user(Client &client, Client &target)
{
	std::map<std::string, Channel>::iterator	ch_it;

	ch_it = _channels.begin();
	while ((ch_it != _channels.end()) && (!ch_it->second.has_member(&target)))
		ch_it++;
	if (ch_it == _channels.end())
		send_whoreply(client, target, "*", false);
	else
		send_whoreply(client, target, ch_it->second.get_name(),
			ch_it->second.is_operator(&target));
}

void	Server::send_whoreply(Client &client, Client &target,
	const std::string &chan_field, bool is_op)
{
	std::string	flags;

	flags = "H";
	if (is_op)
		flags += "@";
	reply(client, RPL_WHOREPLY, chan_field + " " + target.get_username()
		+ " " + target.get_ip() + " " SERVER_NAME " " + target.get_nick()
		+ " " + flags + " :0 " + target.get_realname());
}

static bool	is_who_match(const std::string &mask, Client &target)
{
	std::string	lower;

	lower = irc_lower(mask);
	return ((match_mask(lower, irc_lower(target.get_nick())))
		|| (match_mask(lower, irc_lower(target.get_username())))
		|| (match_mask(lower, target.get_ip()))
		|| (match_mask(lower, irc_lower(target.get_realname())))
		|| (match_mask(lower, SERVER_NAME)));
}
