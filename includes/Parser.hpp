/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 00:33:40 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/13 02:10:18 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

struct Message
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
};

bool						parse_message(const std::string &line, Message &msg);
std::string					irc_lower(const std::string &str);
std::vector<std::string>	split_list(const std::string &str, char sep);

#endif
