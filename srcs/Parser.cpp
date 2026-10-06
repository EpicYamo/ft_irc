/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 00:36:52 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/06 18:55:57 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <cctype>

static size_t	skip_spaces(const std::string &line, size_t i);
static size_t	read_word(const std::string &line, size_t i, std::string &word);
static void		to_upper(std::string &str);

bool	parse_message(const std::string &line, Message &msg)
{
	size_t		i;
	std::string	word;

	msg.prefix.clear();
	msg.command.clear();
	msg.params.clear();
	i = skip_spaces(line, 0);
	if ((i < line.size()) && (line[i] == ':'))
		i = skip_spaces(line, read_word(line, i + 1, msg.prefix));
	i = skip_spaces(line, read_word(line, i, msg.command));
	if (msg.command.empty())
		return (false);
	to_upper(msg.command);
	while (i < line.size())
	{
		if (line[i] == ':')
		{
			msg.params.push_back(line.substr(i + 1));
			i = line.size();
		}
		else
		{
			i = skip_spaces(line, read_word(line, i, word));
			msg.params.push_back(word);
		}
	}
	return (true);
}

static size_t	skip_spaces(const std::string &line, size_t i)
{
	while ((i < line.size()) && (line[i] == ' '))
		i++;
	return (i);
}

static size_t	read_word(const std::string &line, size_t i, std::string &word)
{
	size_t	start;

	start = i;
	while ((i < line.size()) && (line[i] != ' '))
		i++;
	word = line.substr(start, i - start);
	return (i);
}

static void	to_upper(std::string &str)
{
	size_t	i;

	i = 0;
	while (i < str.size())
	{
		str[i] = std::toupper(static_cast<unsigned char>(str[i]));
		i++;
	}
}
