/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:04:53 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/04 21:23:24 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
	private:
		int			_fd;
		std::string	_ip;
		std::string	_recv_buf;

		Client();

	public:
		Client(int fd, const std::string &ip);
		Client(const Client &other);
		~Client();
		Client	&operator=(const Client &other);

		int					get_fd() const;
		const std::string	&get_ip() const;
		std::string			&get_recv_buf();
};

#endif
