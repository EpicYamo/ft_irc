/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 01:50:48 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/09 01:50:53 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_pass(Client &client, const Message &msg)
{
	if (client.is_registered())
	{
		reply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return ;
	}
	if (msg.params.empty())
	{
		reply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return ;
	}
	if (msg.params[0] != _password)
	{
		reply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		return ;
	}
	client.set_pass_ok(true);
}
