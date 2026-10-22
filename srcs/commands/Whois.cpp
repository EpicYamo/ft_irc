/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 23:19:51 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 20:36:10 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>

void	Server::cmd_whois(Client &client, const Message &msg)
{
	std::vector<std::string>	nicks;
	size_t						i;

	if (msg.params.size() > 1)
	{
		if ((msg.params[0] != SERVER_NAME)
			&& (find_client_by_nick(msg.params[0]) == NULL))
		{
			reply(client, ERR_NOSUCHSERVER, msg.params[0]
				+ " :No such server");
			return ;
		}
		nicks = split_list(msg.params[1], ',');
	}
	else if (!msg.params.empty())
		nicks = split_list(msg.params[0], ',');
	if (nicks.empty())
	{
		reply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return ;
	}
	i = 0;
	while ((i < nicks.size()) && (i < MAX_TARGETS))
	{
		whois_user(client, nicks[i]);
		i++;
	}
}

void	Server::whois_user(Client &client, const std::string &nick)
{
	Client										*target;
	std::map<std::string, Channel>::iterator	ch_it;
	std::vector<std::string>					chans;
	std::ostringstream							idle_info;

	target = find_client_by_nick(nick);
	if ((target == NULL) || (!target->is_registered()))
	{
		reply(client, ERR_NOSUCHNICK, nick + " :No such nick/channel");
		reply(client, RPL_ENDOFWHOIS, nick + " :End of WHOIS list");
		return ;
	}
	reply(client, RPL_WHOISUSER, target->get_nick() + " "
		+ target->get_username() + " " + target->get_ip() + " * :"
		+ target->get_realname());
	ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		if (ch_it->second.is_operator(target))
			chans.push_back("@" + ch_it->second.get_name());
		else if (ch_it->second.has_member(target))
			chans.push_back(ch_it->second.get_name());
		ch_it++;
	}
	send_list_reply(client, RPL_WHOISCHANNELS, target->get_nick(), chans);
	reply(client, RPL_WHOISSERVER, target->get_nick() + " " SERVER_NAME
		" :ft_irc server");
	idle_info << (time(NULL) - target->get_last_message()) << " "
		<< target->get_connect_time();
	reply(client, RPL_WHOISIDLE, target->get_nick() + " " + idle_info.str()
		+ " :seconds idle, signon time");
	reply(client, RPL_ENDOFWHOIS, nick + " :End of WHOIS list");
}
