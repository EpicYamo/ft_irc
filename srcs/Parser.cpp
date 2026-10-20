/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 00:36:52 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/21 01:59:50 by aaycan           ###   ########.fr       */
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
	{
		i = skip_spaces(line, read_word(line, i + 1, msg.prefix));
		if (msg.prefix.empty())
			return (false);
	}
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

std::string	irc_lower(const std::string &str)
{
	std::string	result;
	size_t		i;

	result = str;
	i = 0;
	while (i < result.size())
	{
		if (result[i] == '[')
			result[i] = '{';
		else if (result[i] == ']')
			result[i] = '}';
		else if (result[i] == '\\')
			result[i] = '|';
		else if (result[i] == '~')
			result[i] = '^';
		else
			result[i] = std::tolower(static_cast<unsigned char>(result[i]));
		i++;
	}
	return (result);
}

std::vector<std::string>	split_list(const std::string &str, char sep)
{
	std::vector<std::string>	result;
	size_t						start;
	size_t						end;

	start = 0;
	while (start <= str.size())
	{
		end = str.find(sep, start);
		if (end == std::string::npos)
			end = str.size();
		if (end > start)
			result.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	return (result);
}

bool	match_mask(const std::string &mask, const std::string &str)
{
	size_t	m;
	size_t	s;
	size_t	star;
	size_t	retry;

	m = 0;
	s = 0;
	star = std::string::npos;
	retry = 0;
	while (s < str.size())
	{
		if ((m < mask.size()) && ((mask[m] == '?') || (mask[m] == str[s])))
		{
			m++;
			s++;
		}
		else if ((m < mask.size()) && (mask[m] == '*'))
		{
			star = m;
			retry = s;
			m++;
		}
		else if (star != std::string::npos)
		{
			m = star + 1;
			retry++;
			s = retry;
		}
		else
			return (false);
	}
	while ((m < mask.size()) && (mask[m] == '*'))
		m++;
	return (m == mask.size());
}
