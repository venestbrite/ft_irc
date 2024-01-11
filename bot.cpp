/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 11:05:46 by stribuzi          #+#    #+#             */
/*   Updated: 2022/02/15 11:50:40 by stribuzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <cstdlib>
#include <vector>

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

std::string random_joke(void)
{
	static int	random_seed = 12355;
	static int	random_num = 0;
	std::string	jokes[21];

	srand(random_seed);
	random_seed++;
	jokes[0] = " :What did the fish say when he swam into a wall? Dam.\n";
	jokes[1] = " :If my grandfather had three balls, he'd be a FLIPPER\n";
	jokes[2] = " :What do you call a fish with no eyes? A fsh.\n";
	jokes[3] = " :What do you call a can opener that doesn’t work? A can’t opener!\n";
	jokes[4] = " :Did you hear about the Italian chef who died? He pasta-way\n";
	jokes[5] = " :Two muffins were sitting in an oven. One turned to the other and said, 'Wow, it’s pretty hot in here.' The other one shouted, 'Wow, a talking muffin!'\n";
	jokes[6] = " :Did you hear about the guy who invented the knock-knock joke? He won the 'no-bell' prize.\n";
	jokes[7] = " :I invented a new word! Plagiarism!\n";
	jokes[8] = " :Hear about the new restaurant called Karma? There's no menu: You get what you deserve.\n";
	jokes[9] = " :Did you hear about the claustrophobic astronaut? He just needed a little space.\n";
	jokes[10] = " :A woman in labor suddenly shouted, 'Shouldn’t! Wouldn’t! Couldn’t! Didn’t! Can’t!' 'Don’t worry,' said the doc. 'Those are just contractions.'\n";
	jokes[11] = " :Why don’t scientists trust atoms? Because they make up everything!\n";
	jokes[12] = " :A man tells his doctor, 'Doc, help me. I’m addicted to Twitter!' The doctor replies, 'Sorry, I don’t follow you …'\n";
	jokes[13] = " :What do you call a boomerang that won’t come back? A stick.\n";
	jokes[14] = " :A toilet says to the other: you look a bit flushed!\n";
	jokes[15] = " : What do you call a dog magician? A labracadabrador\n";
	jokes[16] = " :Where would you find an elephant? The same place you lost her.\n";
	jokes[17] = " :How does a scientist freshen her breath? With experi-mints.\n";
	jokes[18] = " :What is a computer’s favorite snack? Computer chips.\n";
	jokes[19] = " :What did one volcano say to the other? I lava you.\n";
	jokes[20] = " :What animal is always at a baseball game? A bat\n";
	if (random_num == 20)
		random_num = 0;
	else
		random_num++;
  	return (jokes[random_num]);
}

void	send_msg(std::string message, int socket)
{
	send(socket, message.c_str(), message.length(), 0);
	sleep(1);
}

int		main(int argc, char **argv)
{
	int 						sock;
	int 						valread;
	struct sockaddr_in			serv_addr;
	int							port;
	std::string					password;
	std::string					channel_name;
	char						buffer[1024];
	std::vector<std::string>	split_res;

	// USER [username] [0] [host] : altramerda
	// NICK [nickname]
	// PASS :[password]

	sock = 0;
	if (argc != 4)
	{
		std::cout << "Error, wrong arguments. Usage: ./bot [port] [password] [channel_name]\n";
		return (0);
	}
	else
	{
		port = atoi(argv[1]);
		password.append(argv[2]);
		channel_name.append(argv[3]);
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Socket creation error \n";
		return (-1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
	{
		std::cout << "Invalid ip address \n";
		return (-1);
	}
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cout << "Failed to connect\n";
		return (-1);
	}
	fcntl(sock, F_SETFL, O_NONBLOCK);
	send_msg(std::string("CAP END\n"), sock);
	send_msg(std::string("PASS :" + password + "\n"), sock);
	send_msg(std::string("NICK bellofigobot\n"), sock);
	send_msg(std::string("USER sonobellocomeraulobot 0 127.0.0.1 :supersquack\n"), sock);
	send_msg(std::string("JOIN #" + channel_name + "\n"), sock);
	while (1)
	{
		valread = read(sock, buffer, 1024);
		buffer[valread] = '\0';
		std::string squack(buffer);
		split_res = ft_cpp_vec_split(squack, ' ');
		size_t i = 0;
		while (i < split_res.size())
		{
			if (split_res[i][0] == ':')
			{
				split_res[i] = split_res[i].substr(1, split_res[i].length());
				if (split_res[i].compare("JOKE") == 0)
					send_msg(std::string("PRIVMSG #" + channel_name + random_joke()), sock);
			}
			i++;
		}
		split_res.clear();
		squack.clear();
		bzero(buffer, strlen(buffer));
		sleep(1);
	}
	return 0;
}