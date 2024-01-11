/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:01:24 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 12:38:17 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

# include "Channel.hpp"
# include <fcntl.h>

std::vector<std::string>    ft_cpp_vec_split (std::string str, char separator);
std::string                 find_nickname_by_fd(int fd, std::vector<User> users);
User                        get_user_by_fd(int fd, std::vector<User> users);
bool                        check_if_channel_exists(std::string name_to_check, std::vector<Channel> channels);
Channel                     *get_channel_by_name(std::string name_to_check, std::vector<Channel> channels);
void                        send_message_to_channel(int sending_fd, std::string channel_name_to_search, std::vector<User> users, std::vector<Channel> channels, std::string buffer);
void                        join_channel(int sd, std::string string_vec, std::vector<User> users, std::vector<Channel> *channels);
void                        join_multiple_channels(int sd, std::string to_split, std::vector<User> users, std::vector<Channel> *channels);
void                        erase_from_channel(int sd, std::string channel_to_erase, std::vector<User> users, std::vector<Channel>* channels);
void                        send_privmsg(std::string nick_to_search, std::vector<User> users, std::vector<Channel> channels, std::string buffer, int sd);
void                        nick_command(std::string nick_to_input, int fd_to_search, std::vector<User>* users);
bool						is_space(char to_check);

#endif