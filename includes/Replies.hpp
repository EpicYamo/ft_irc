/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 22:01:32 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/08 22:03:54 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

# define SERVER_NAME			"ircserv"

# define RPL_WELCOME			"001"
# define RPL_YOURHOST			"002"
# define RPL_CREATED			"003"
# define RPL_MYINFO				"004"

# define ERR_UNKNOWNCOMMAND		"421"
# define ERR_NONICKNAMEGIVEN	"431"
# define ERR_ERRONEUSNICKNAME	"432"
# define ERR_NICKNAMEINUSE		"433"
# define ERR_NOTREGISTERED		"451"
# define ERR_NEEDMOREPARAMS		"461"
# define ERR_ALREADYREGISTRED	"462"
# define ERR_PASSWDMISMATCH		"464"

#endif
