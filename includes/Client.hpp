/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:04:53 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/08 21:52:47 by aaycan           ###   ########.fr       */
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
		std::string	_send_buf;
		std::string	_nick;
		std::string	_username;
		std::string	_realname;
		bool		_pass_ok;
		bool		_registered;

		Client();

	public:
		Client(int fd, const std::string &ip);
		Client(const Client &other);
		~Client();
		Client	&operator=(const Client &other);

		int					get_fd() const;
		const std::string	&get_ip() const;
		void				append_recv(const char *data, size_t len);
		bool				extract_line(std::string &line);
		void				append_send(const std::string &data);
		const std::string	&get_send_buf() const;
		void				erase_send(size_t len);
		const std::string	&get_nick() const;
		const std::string	&get_username() const;
		const std::string	&get_realname() const;
		std::string			get_prefix() const;
		bool				is_pass_ok() const;
		bool				is_registered() const;
		void				set_nick(const std::string &nick);
		void				set_user(const std::string &username,
								const std::string &realname);
		void				set_pass_ok(bool value);
		void				set_registered(bool value);
};

#endif
