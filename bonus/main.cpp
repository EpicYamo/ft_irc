/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 21:46:43 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 21:46:45 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <iostream>
#include <cstdlib>
#include <exception>

static bool	parse_port(const std::string &str, int &port);

int	main(int argc, char **argv)
{
	int	port;

	if (argc != 4)
	{
		std::cerr << "Usage: ./ircbot <port> <password> <#channel>"
			<< std::endl;
		return (1);
	}
	if ((!parse_port(argv[1], port)) || (argv[3][0] != '#'))
	{
		std::cerr << "Error: invalid port or channel" << std::endl;
		return (1);
	}
	try
	{
		Bot	bot(port, argv[2], argv[3]);
		bot.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}

static bool	parse_port(const std::string &str, int &port)
{
	size_t	i;

	if ((str.empty()) || (str.size() > 5))
		return (false);
	i = 0;
	while (i < str.size())
	{
		if ((str[i] < '0') || (str[i] > '9'))
			return (false);
		i++;
	}
	port = std::atoi(str.c_str());
	return ((port > 0) && (port <= 65535));
}
