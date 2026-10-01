/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 17:40:57 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/01 20:21:34 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

static bool	parse_port(const std::string &str, int &port);
static bool	valid_password(const std::string &pass);

int	main(int argc, char **argv)
{
	int	port;

	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	if (!parse_port(argv[1], port))
	{
		std::cerr << "Error: invalid port (1-65535)" << std::endl;
		return (1);
	}
	if (!valid_password(argv[2]))
	{
		std::cerr << "Error: invalid password (empty or has spaces)" << std::endl;
		return (1);
	}
	std::cout << "Port: " << port << " | Password: " << argv[2] << std::endl;
	return (0);
}

static bool	parse_port(const std::string &str, int &port)
{
	long	value;
	size_t	i;

	if ((str.empty()) || (str.size() > 5))
		return (false);
	i = 0;
	while (i < str.size())
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return (false);
		i++;
	}
	value = std::atol(str.c_str());
	if ((value < 1) || (value > 65535))
		return (false);
	port = static_cast<int>(value);
	return (true);
}

static bool	valid_password(const std::string &pass)
{
	size_t	i;

	if (pass.empty())
		return (false);
	i = 0;
	while (i < pass.size())
	{
		if (std::isspace(static_cast<unsigned char>(pass[i])))
			return (false);
		i++;
	}
	return (true);
}
