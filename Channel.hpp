/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:01:35 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 11:26:25 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>//close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

#include <iostream>

#include <string>
#include <vector>

class User
{
	private:
		std::string 	_nickname;
		std::string		_username;
		std::string		_host;
		int				_private_fd;
		bool			_is_validated;
	public:
		User(std::string nickname, int fd) : _nickname(nickname), _private_fd(fd) {};
		User(std::string nickname, std::string username, std::string host, int fd) : _nickname(nickname), _username(username), _host(host), _private_fd(fd) { this->_is_validated = true; };
		User(int fd) { this->_private_fd = fd; this->_is_validated = false; }
		User() {};
		std::string get_identifier() { return  (this->_nickname + "!" + this->_username + "@" + this->_host); };
		std::string get_username() { return this->_username; };
		std::string get_nickname() { return this->_nickname; };
		int			get_private_fd() { return this->_private_fd; };
		void		set_nickname(std::string nickname) { this->_nickname = nickname; };
		void		set_username(std::string username) { this->_username = username; };
		void		set_host(std::string host) { this->_host = host; };
		void		send_confirmation()
		{ 	
			if (!_is_validated)
				return ;
			std::string welcome_str = "001 " + this->_nickname + " Welcome to FavelasIrc " + this->_nickname + "!" + this->_username + "@" + this->_host + "\n";
			send(this->_private_fd, welcome_str.c_str(), welcome_str.length(), 0);	// send welcome msg // 
		}
		void		validate_user() { this->_is_validated = true; };
		bool		is_validated() { return this->_is_validated; };
};

class Channel
{
	private:
		std::string			_channel_name;
		std::string         _topic;
		std::vector<User>   _members;
		std::vector<User>	_operators;
	public:
		Channel(std::string name, std::string topic, std::vector<User>members, std::vector<User> operators) : _channel_name(name), _topic(topic), _members(members), _operators(operators) {};
		void				set_topic(std::string topic_to_set) { this->_topic = topic_to_set; }
		void 				add_member(const User &user_to_add) { this->_members.push_back(user_to_add); }
		void				add_operator(const User &user_to_add) { this->_operators.push_back(user_to_add); }
		std::string 		get_topic() { return (this->_topic); }
		std::string			get_name() { return (this->_channel_name); }
		std::vector<User>   get_members() { return this->_members; }
		std::vector<User>	get_operators() { return this->_operators; }
		void				set_members(std::vector<User> to_set) { this->_members.swap(to_set); }
		void				set_operators(std::vector<User> to_set) { this->_operators.swap(to_set); }
		void				remove_operator_by_sd(int sd);
		void				remove_member_by_sd(int sd);
		bool				check_if_sd_is_operator(int sd);
		void				make_user_operator(const std::string &to_add, std::vector<User> users, int fd);
		void				remove_user_from_operator(const std::string &to_add, std::vector<User> users, int fd);
		bool				kick_user(int sd_to_remove, int fd_to_send);
		bool				check_if_user_is_present(User user_to_check);
		int					get_size() { return (this->_members.size() + this->_operators.size()); };
};

#endif