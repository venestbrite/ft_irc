/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 11:05:42 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 12:04:42 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::vector<std::string> ft_cpp_vec_split (std::string str, char separator)  
{  
	int							current_index;
	int							start_index;
	int							end_index;
	size_t						i;  
	std::vector<std::string>	to_ret;

	current_index = 0;
	i = 0;
	start_index = 0;
	end_index = 0;
	while (i <= str.length())  
	{
		if (str[i] == separator || i == str.length() || str[i] == '\r' || str[i] == '\n' || str[i] == '\t' || str[i] == '\v' || str[i] == '\f')  
		{  
			end_index = i;  
			std::string sub_str = "";  
			sub_str.append(str, start_index, end_index - start_index);  
			to_ret.push_back(sub_str);
			current_index += 1;  
			start_index = end_index + 1;  
		}  
		i++;  
	}  
	return (to_ret);
}   

std::string find_nickname_by_fd(int fd, std::vector<User> users)
{
	for(std::vector<User>::iterator i = users.begin(); i != users.end(); i++)
	{
		if (i->get_private_fd() == fd)
		{
			return (i->get_nickname());
		}
	}
	return (std::string());
}

User get_user_by_fd(int fd, std::vector<User> users)
{
	std::vector<User>::iterator i;
	for(i = users.begin(); i != users.end(); i++)
	{
		if (i->get_private_fd() == fd)
		{
			return (*i);
		}
	}
	return *i;
}

bool 	check_if_channel_exists(std::string name_to_check, std::vector<Channel> channels)
{
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if (it->get_name().compare(name_to_check) == 0 || it->get_name().compare(name_to_check) == 1)
		{
			return true;
		}
	}
	return false;
}

Channel *get_channel_by_name(std::string name_to_check, std::vector<Channel> channels)
{
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if (it->get_name().compare(name_to_check) == 0)
			return (&(*it));
	}
	return (nullptr);
}

void send_message_to_channel(int sending_fd, std::string channel_name_to_search, std::vector<User> users, std::vector<Channel> channels, std::string buffer)
{
	std::string channel_name = channel_name_to_search.substr(1, channel_name_to_search.length() - 1);
	User current_user = get_user_by_fd(sending_fd, users);
	if (!current_user.is_validated())
		return ;
	if (!check_if_channel_exists(channel_name, channels))
	{
		// send reply no such channel // 403 channel_name:No such channel
		std::string reply = "403 " + channel_name + ":No such channel\n";
		send(sending_fd, reply.c_str(), reply.length(), 0);
		return ;
	}	
	Channel current_channel = *get_channel_by_name(channel_name, channels);

	std::vector<User> current_channel_users = current_channel.get_members();
	std::vector<User> current_channel_operators = current_channel.get_operators();

	if (!current_channel.check_if_user_is_present(current_user))
		return ;
	std::string message_to_send = ":" + current_user.get_identifier() + " " + buffer;
	for (std::vector<User>::iterator it = current_channel_operators.begin(); it != current_channel_operators.end(); it++)
	{
		if (sending_fd != it->get_private_fd())
			send(it->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
	}
	for (std::vector<User>::iterator it = current_channel_users.begin(); it != current_channel_users.end(); it++)
	{
		if (sending_fd != it->get_private_fd())
			send(it->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
	}
}

void	join_multiple_channels(int sd, std::string to_split, std::vector<User> users, std::vector<Channel> *channels)
{
	std::vector<std::string> res = ft_cpp_vec_split(to_split, ',');
	if (res.size() > 0)
	{
		for (size_t i = 0; i < res.size(); i++)
			join_channel(sd, res[i], users, channels);
	}
}

void	join_channel(int sd, std::string string_vec, std::vector<User> users, std::vector<Channel> *channels)
{
	std::string channel_to_join = string_vec.substr(1, string_vec.length());
	if (channel_to_join.back() == '\n')
		channel_to_join = channel_to_join.substr(0, channel_to_join.length() - 1);
	User temp = get_user_by_fd(sd, users);
	if (!temp.is_validated())
		return ;
	//std::string message_to_send = ":" + find_nickname_by_fd(sd, users) + "!" + find_nickname_by_fd(sd, users) + "@foo.example.com " + "JOIN #" + channel_to_join + "\n";
	std::string message_to_send = ":" + temp.get_identifier() +  " JOIN #" + channel_to_join + "\n";
	bool channel_exists = check_if_channel_exists(channel_to_join, *channels);
	if (channel_exists)
	{
		// Channel to_join = get_channel_by_name(channel_to_join, channels);
		std::vector<Channel>::iterator it;
		for (it = channels->begin(); it != channels->end(); it++)
		{
			if (it->get_name().compare(channel_to_join) == 0)
			{
				it->add_member(get_user_by_fd(sd, users));
				send(sd, message_to_send.c_str(), message_to_send.length(), 0);
				// now we need to send this user the current list of users inside channel //
				std::string users_list = ":127.0.0.1 332 " + get_user_by_fd(sd, users).get_nickname() + " #" +  channel_to_join + " :" + it->get_topic() + "\n"; // topic 
				send(sd, users_list.c_str(), users_list.length(), 0);
				users_list = ":127.0.0.1 353 " + get_user_by_fd(sd, users).get_nickname() + " = #" + channel_to_join + " :";
				std::vector<User> temp_vec = it->get_members();
				for (std::vector<User>::iterator i = temp_vec.begin(); i != temp_vec.end(); i++)
				{
					users_list.append(i->get_nickname());
					users_list.append(" ");
				}
				std::vector<User> temp_vec_operators = it->get_operators();
				for (std::vector<User>::iterator i = temp_vec_operators.begin(); i != temp_vec_operators.end(); i++)
				{
					users_list.append("@");
					users_list.append(i->get_nickname());
					users_list.append(" ");
				}
				users_list.append("\n");
				send(sd, users_list.c_str(), users_list.length(), 0);
				users_list = ":127.0.0.1 366 " + get_user_by_fd(sd, users).get_nickname() + " #" + channel_to_join + " :end of NAMES list\n";
				send(sd, users_list.c_str(), users_list.length(), 0);
				break ;
			}
		}
		std::vector<User> channel_users = it->get_members();
		std::vector<User> channel_operators = it->get_operators();
		for (std::vector<User>::iterator it = channel_operators.begin(); it != channel_operators.end(); it++)
		{
			if (it->get_private_fd() != sd)
				send(it->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
		}
		for (std::vector<User>::iterator it = channel_users.begin(); it != channel_users.end(); it++)
		{
			if (it->get_private_fd() != sd)
				send(it->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
		}
	}
	else
	{
		std::vector<User> initializer;
		// initializer.push_back(get_user_by_fd(sd, users));
		std::vector<User> operators;
		operators.push_back(get_user_by_fd(sd, users));
		channels->push_back(Channel(channel_to_join, "test topic", initializer, operators));
		send(sd, message_to_send.c_str(), message_to_send.length(), 0);
		std::string operator_notif = "MODE #" + channel_to_join + " +o " + get_user_by_fd(sd, users).get_nickname() + "\n";
		send(sd, operator_notif.c_str(), operator_notif.length(), 0);
	}
}

void	erase_from_channel(int sd, std::string channel_to_erase, std::vector<User> users, std::vector<Channel>* channels)
{
	User temp = get_user_by_fd(sd, users);
	if (!temp.is_validated())
		return ;
	std::vector<User> temp_vec;
	for (std::vector<Channel>::iterator i = channels->begin(); i != channels->end(); i++)
	{
		if (i->get_name().compare(channel_to_erase) == 0)
		{
			for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
			{
				if (i->get_private_fd() != sd)
					temp_vec.push_back(*i);
			}
			i->set_members(temp_vec);
			break ;
		}
	}
}

void	send_privmsg(std::string nickname_to_search, std::vector<User> users, std::vector<Channel> channels, std::string buffer, int sd)
{
	if (nickname_to_search[0] == '#')
		send_message_to_channel(sd, nickname_to_search, users, channels, std::string(buffer));
	else
	{
		int fd_to_send = -1;
		for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
		{
			if (i->get_nickname().compare(nickname_to_search) == 0 || i->get_nickname().compare(nickname_to_search) == 1)
			{
				fd_to_send = i->get_private_fd();
				break ;
			}
		}
		std::vector<User>::iterator i;
		std::string sender_nickname;
		User temp = get_user_by_fd(sd, users);
		if (!temp.is_validated())
			return ;
		// std::string message = ":" + sender_nickname + "!" + sender_nickname + "@foo.example.com " + buffer; 
		std::string message = ":" + temp.get_identifier() + " " + buffer;
		// FORMAT FOR MESSAGES IS:  ":[nickname]![identifier] [MESSAGGIO]" // 
		// [identifier] = nick@foo.example.com <-- questo formato
		send(fd_to_send, (void *)message.c_str(), message.length(), 0);
		message.clear();
	}
}

bool	username_already_in_use(int fd_to_search, std::string nickname_to_check, std::vector<User> users)
{
	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
	{
		if (i->get_nickname().compare(nickname_to_check) == 0 && i->get_private_fd() != fd_to_search)
			return (true);
	}
	return (false);
}

void	nick_command(std::string nick_to_input, int fd_to_search, std::vector<User>* users)
{
	std::string	old_username;
	std::string message_to_send;
	int			fd_to_send = -1;

	for (std::vector<User>::iterator i = users->begin(); i < users->end(); i++)
	{
		if (i->get_private_fd() == fd_to_search)
		{
			if (!i->is_validated())
				return ;
			fd_to_send = i->get_private_fd();
			old_username = i->get_nickname();
			if (nick_to_input.back() == '\n')
				i->set_nickname(nick_to_input.erase(nick_to_input.length() - 1));
			else
				i->set_nickname(nick_to_input);
			break ;
		}
	}
	if (username_already_in_use(fd_to_search, nick_to_input, *users)) // 433 <nick> :Nickname is already in use
	{
		message_to_send = "433 " + nick_to_input + " :Nickname is already in use\n";
		send(fd_to_send, message_to_send.c_str(), message_to_send.length(), 0);
		return ;
	}
	message_to_send = ":" + old_username + " NICK :" + nick_to_input + "\n"; // FORMAT IS ":[current_nick] NICK :[new_nick]\n" // 
	send(fd_to_send, message_to_send.c_str(), message_to_send.length(), 0);
}

bool	is_space(char to_check)
{
	if (to_check == ' ' || to_check == '\t' || to_check == '\n' || to_check == '\v' || to_check == '\f' || to_check == '\r')
		return true;
	return false;
}