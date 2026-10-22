/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaycan <aaycan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 20:04:53 by aaycan            #+#    #+#             */
/*   Updated: 2026/10/22 07:56:28 by aaycan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <ctime>

# define USERLEN 10
# define MAX_MSG_LEN 510
# define MAX_RECV_LEN 8192
# define MAX_SEND_LEN 262144

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
		bool		_closing;
		bool		_cap_negotiating;
		bool		_invisible;
		bool		_sendq_exceeded;
		bool		_ping_sent;
		time_t		_connect_time;
		time_t		_last_activity;
		time_t		_closing_time;
		time_t		_flood_timer;

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
		void				release_buffers();
		const std::string	&get_nick() const;
		const std::string	&get_username() const;
		const std::string	&get_realname() const;
		std::string			get_prefix() const;
		time_t				get_connect_time() const;
		time_t				get_last_activity() const;
		time_t				get_closing_time() const;
		time_t				get_flood_timer() const;

		bool				is_pass_ok() const;
		bool				is_registered() const;
		bool				is_closing() const;
		bool				is_cap_negotiating() const;
		bool				is_invisible() const;

		void				set_nick(const std::string &nick);
		void				set_user(const std::string &username,
								const std::string &realname);
		void				set_pass_ok(bool value);
		void				set_registered(bool value);
		void				set_closing(bool value);
		void				set_cap_negotiating(bool value);
		void				set_invisible(bool value);
		void				set_sendq_exceeded(bool value);
		void				set_ping_sent(bool value);
		void				set_last_activity(time_t value);
		void				set_flood_timer(time_t value);
		bool				is_recv_overflow() const;
		bool				is_sendq_exceeded() const;
		bool				is_ping_sent() const;
};

#endif
