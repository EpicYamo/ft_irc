/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 22:29:40 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/21 01:40:50 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>
#include <cctype>

static bool		takes_param(char mode, bool adding);
static bool		apply_limit(Channel &channel, bool adding,
					const std::string &param, std::string &shown);
static size_t	parse_limit(const std::string &str);
static bool		is_ban_query(const std::string &modes);
static void		add_change(std::string &modes, std::string &values, char &sign,
					bool adding, char mode, const std::string &shown);

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
	Channel	*channel;

	channel = find_channel(msg.params[0]);
	if (channel == NULL)
	{
		reply(client, ERR_NOSUCHCHANNEL, msg.params[0] + " :No such channel");
		return ;
	}
	if (msg.params.size() < 2)
	{
		send_channel_modes(client, *channel);
		return ;
	}
	if ((msg.params.size() == 2) && (is_ban_query(msg.params[1])))
	{
		reply(client, RPL_ENDOFBANLIST,
			channel->get_name() + " :End of channel ban list");
		return ;
	}
	if (!channel->has_member(&client))
	{
		reply(client, ERR_NOTONCHANNEL,
			channel->get_name() + " :You're not on that channel");
		return ;
	}
	if (!channel->is_operator(&client))
	{
		reply(client, ERR_CHANOPRIVSNEEDED,
			channel->get_name() + " :You're not channel operator");
		return ;
	}
	apply_channel_modes(client, *channel, msg);
}

void	Server::send_channel_modes(Client &client, const Channel &channel)
{
	std::string			modes;
	std::string			values;
	std::ostringstream	limit;
	bool				is_member;

	is_member = channel.has_member(&client);
	modes = "+";
	if (channel.is_invite_only())
		modes += "i";
	if (channel.is_topic_restricted())
		modes += "t";
	if (!channel.get_key().empty())
	{
		modes += "k";
		if (is_member)
			values += " " + channel.get_key();
	}
	if (channel.get_limit() > 0)
	{
		modes += "l";
		limit << channel.get_limit();
		if (is_member)
			values += " " + limit.str();
	}
	reply(client, RPL_CHANNELMODEIS, channel.get_name() + " " + modes + values);
}

void	Server::apply_channel_modes(Client &client, Channel &channel,
	const Message &msg)
{
	std::string	modes;
	std::string	values;
	std::string	param;
	std::string	shown;
	char		sign;
	bool		adding;
	size_t		arg;
	size_t		param_count;
	size_t		i;

	sign = 0;
	adding = true;
	arg = 2;
	param_count = 0;
	i = 0;
	while (i < msg.params[1].size())
	{
		if ((msg.params[1][i] == '+') || (msg.params[1][i] == '-'))
			adding = (msg.params[1][i] == '+');
		else if (std::string("itklo").find(msg.params[1][i])
			== std::string::npos)
			reply(client, ERR_UNKNOWNMODE, std::string(1, msg.params[1][i])
				+ " :is unknown mode char to me for " + channel.get_name());
		else
		{
			param.clear();
			if (takes_param(msg.params[1][i], adding))
			{
				if (arg < msg.params.size())
					param = msg.params[arg];
				arg++;
				param_count++;
			}
			if ((param_count <= MAX_MODE_PARAMS) && (apply_mode(client,
				channel, msg.params[1][i], adding, param, shown)))
				add_change(modes, values, sign, adding, msg.params[1][i],
					shown);
		}
		i++;
	}
	if (!modes.empty())
		broadcast(channel, ":" + client.get_prefix() + " MODE "
			+ channel.get_name() + " " + modes + values, NULL);
}

bool	Server::apply_mode(Client &client, Channel &channel, char mode,
	bool adding, const std::string &param, std::string &shown)
{
	shown.clear();
	if (mode == 'i')
	{
		if (channel.is_invite_only() == adding)
			return (false);
		channel.set_invite_only(adding);
		return (true);
	}
	if (mode == 't')
	{
		if (channel.is_topic_restricted() == adding)
			return (false);
		channel.set_topic_restricted(adding);
		return (true);
	}
	if (mode == 'k')
		return (apply_key(client, channel, adding, param, shown));
	if (mode == 'l')
		return (apply_limit(channel, adding, param, shown));
	return (apply_operator(client, channel, adding, param, shown));
}

bool	Server::apply_key(Client &client, Channel &channel, bool adding,
	const std::string &param, std::string &shown)
{
	if (!adding)
	{
		if (channel.get_key().empty())
			return (false);
		channel.set_key("");
		shown = "*";
		return (true);
	}
	if ((param.empty()) || (param.find_first_of(" ,") != std::string::npos))
		return (false);
	if (!channel.get_key().empty())
	{
		reply(client, ERR_KEYSET,
			channel.get_name() + " :Channel key already set");
		return (false);
	}
	channel.set_key(param);
	shown = param;
	return (true);
}

bool	Server::apply_operator(Client &client, Channel &channel, bool adding,
	const std::string &nick, std::string &shown)
{
	Client	*target;

	if (nick.empty())
		return (false);
	target = find_client_by_nick(nick);
	if ((target == NULL) || (!target->is_registered()))
	{
		reply(client, ERR_NOSUCHNICK, nick + " :No such nick/channel");
		return (false);
	}
	if (!channel.has_member(target))
	{
		reply(client, ERR_USERNOTINCHANNEL, target->get_nick() + " "
			+ channel.get_name() + " :They aren't on that channel");
		return (false);
	}
	if (channel.is_operator(target) == adding)
		return (false);
	channel.set_operator(target, adding);
	shown = target->get_nick();
	return (true);
}

static bool	takes_param(char mode, bool adding)
{
	return ((mode == 'k') || (mode == 'o') || ((mode == 'l') && (adding)));
}

static bool	apply_limit(Channel &channel, bool adding,
	const std::string &param, std::string &shown)
{
	std::ostringstream	out;
	size_t				limit;

	if (!adding)
	{
		if (channel.get_limit() == 0)
			return (false);
		channel.set_limit(0);
		return (true);
	}
	limit = parse_limit(param);
	if ((limit == 0) || (limit == channel.get_limit()))
		return (false);
	channel.set_limit(limit);
	out << limit;
	shown = out.str();
	return (true);
}

static size_t	parse_limit(const std::string &str)
{
	size_t	value;
	size_t	i;

	if ((str.empty()) || (str.size() > 9))
		return (0);
	value = 0;
	i = 0;
	while (i < str.size())
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return (0);
		value = value * 10 + (str[i] - '0');
		i++;
	}
	return (value);
}

static void	add_change(std::string &modes, std::string &values, char &sign,
	bool adding, char mode, const std::string &shown)
{
	char	new_sign;

	new_sign = '-';
	if (adding)
		new_sign = '+';
	if (sign != new_sign)
	{
		modes += new_sign;
		sign = new_sign;
	}
	modes += mode;
	if (!shown.empty())
		values += " " + shown;
}

static bool	is_ban_query(const std::string &modes)
{
	return ((modes == "b") || (modes == "+b") || (modes == "-b"));
}
