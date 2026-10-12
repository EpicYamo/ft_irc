/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:59:39 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/12 20:59:45 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_pong(Client &client, const Message &msg)
{
	if ((msg.params.empty()) || (msg.params[0].empty()))
	{
		reply(client, ERR_NOORIGIN, ":No origin specified");
		return ;
	}
	if ((msg.params.size() > 1) && (msg.params[1] != SERVER_NAME))
		reply(client, ERR_NOSUCHSERVER, msg.params[1] + " :No such server");
}
