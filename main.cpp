/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:01:33 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 11:35:11 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "handlers.hpp"

void	do_everything(char **argv)
{
	t_program	program;

	program.opt = 1;
	program.port = atoi(argv[1]);
	program.client_socket.reserve(30);
	program.global_password.append(argv[2]);
	init_all(&program.master_socket, program.port, &program.addrlen, program.opt, &program.address, &program.tv);
	while(1)
	{
		FD_ZERO(&program.readfds); //clear the socket set 
		FD_SET(program.master_socket, &program.readfds); //add master socket to set 
		program.max_sd = program.master_socket;
		for (size_t i = 0 ; i < program.client_socket.size() ; i++) //add child sockets to set 
		{
			program.sd = program.client_socket[i]; //socket descriptor 
			if (program.sd > 0) //if valid socket descriptor then add to read list 
				FD_SET(program.sd , &program.readfds);
			if (program.sd > program.max_sd) //highest file descriptor number, need it for the   function 
				program.max_sd = program.sd;
		}
		program.activity = select(program.max_sd + 1 , &program.readfds , NULL , NULL , &program.tv); //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely 
		if (FD_ISSET(program.master_socket, &program.readfds)) //If something happened on the master socket , then its an incoming connection 
		{
			if ((program.new_socket = accept(program.master_socket, (struct sockaddr *)&program.address, (socklen_t*)&program.addrlen)) < 0)  
				std::cout << "Accept error\n";
			User temp(program.new_socket);
			program.users.push_back(temp);
			program.client_socket.push_back(program.new_socket);
		}
		for (size_t i = 0; i < program.client_socket.size(); i++) //else its some IO operation on some other socket
		{
			program.sd = program.client_socket[i];
			if (FD_ISSET(program.sd, &program.readfds))
			{
				if ((program.valread = recv(program.sd, program.buffer, 1024, 0)) == 0)
					handle_quit(program.sd, &program.users, &program.channels, &program.client_socket);
				else
					super_mega_swag_if(program.global_password, program.buffer, program.sd, program.valread, &program.users, &program.channels, &program.client_socket);
			}
		}
	}
}

int		main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv [port] [password]\n";
		return 0;
	}
	do_everything(argv);
	return 0;
}