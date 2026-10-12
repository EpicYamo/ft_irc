/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 21:04:47 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/12 21:04:51 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::cmd_quit(Client &client, const Message &msg)
{
	std::string	reason;

	if ((!msg.params.empty()) && (!msg.params[0].empty()))
		reason = msg.params[0];
	else
		reason = client.get_nick();
	disconnect_client(client, "Quit: " + reason);
}
