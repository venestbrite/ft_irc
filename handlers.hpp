/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 15:10:26 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 12:02:49 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "utils.hpp"
#include <string.h>

typedef struct				s_program
{
	int						port; 
	int 					opt; 			// true
	int						master_socket;
	int						addrlen;
	int						new_socket;
	int						activity;
	int						valread;
	int						sd;
	int						max_sd;
	char					buffer[1025]; 
	fd_set					readfds;		//set of socket descriptors 
	struct sockaddr_in		address;
	struct timeval			tv;
	std::string 			global_password;
	std::vector<int>		client_socket;
	std::vector<User>		users;
	std::vector<Channel>	channels;
}							t_program;

void	handle_mode(std::vector<std::string> string_vec, std::vector<Channel> *channels, std::vector<User> users, int sd);
void	init_all(int *master_socket, int port, int *addrlen, int opt, struct sockaddr_in *address, struct timeval *tv);
void    kick_user(std::vector<std::string> string_vec, std::vector<Channel> *channels, std::vector<User> users, int sd);
void 	handle_part(std::string channel, int sd, std::vector<User> users, std::vector<Channel> *channels, char *buffer);
void    handle_quit(int sd, std::vector<User> *users, std::vector<Channel> *channels, std::vector<int> *client_socket);
void	super_mega_swag_if(std::string global_password, char *buffer, int sd, int valread, std::vector<User> *users, std::vector<Channel> *channels, std::vector<int> *client_socket);

#endif