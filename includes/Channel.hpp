/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 05:35:02 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/14 05:35:10 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <map>

class	Client;

class Channel
{
	private:
		std::string					_name;
		std::string					_topic;
		std::map<Client *, bool>	_members;

		Channel();

	public:
		Channel(const std::string &name);
		Channel(const Channel &other);
		~Channel();
		Channel	&operator=(const Channel &other);

		const std::string				&get_name() const;
		const std::string				&get_topic() const;
		const std::map<Client *, bool>	&get_members() const;
		bool							has_member(Client *client) const;
		void							add_member(Client *client, bool is_operator);
};

#endif
