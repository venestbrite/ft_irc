#include "handlers.hpp"

void	handle_mode(std::vector<std::string> string_vec, std::vector<Channel> *channels, std::vector<User> users, int sd)
{
	if ((string_vec.size() - 2) == 4)
	{
		if (string_vec[1][0] == '#')
		{
			std::string channel_name = string_vec[1].substr(1, string_vec[1].length());
			if (string_vec[2].compare("+o") == 0) // wants to make someone operator
			{
				std::string username_to_add = string_vec[3];
				for (std::vector<Channel>::iterator i = channels->begin(); i < channels->end(); i++)
				{
					if (i->get_name().compare(channel_name) == 0)
					{
						if (i->check_if_sd_is_operator(sd))
							i->make_user_operator(username_to_add, users, sd);
						break ;
					}
				}
			}
            else if (string_vec[2].compare("-o") == 0)
            {
                std::string username_to_add = string_vec[3];
                for (std::vector<Channel>::iterator i = channels->begin(); i < channels->end(); i++)
				{
					if (i->get_name().compare(channel_name) == 0)
					{
						if (i->check_if_sd_is_operator(sd))
							i->remove_user_from_operator(username_to_add, users, sd);
						break ;
					}
				}
            }
		}
	}
}

void	init_all(int *master_socket, int port, int *addrlen, int opt, struct sockaddr_in *address, struct timeval *tv)
{
	tv->tv_sec = 7;
	tv->tv_usec = 0;
	if (((*master_socket) = socket(AF_INET, SOCK_STREAM, 0)) == 0) //create a master socket 
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt((*master_socket), SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)  //set master socket to allow multiple connections 
	{  
		perror("setsockopt");  
		exit(EXIT_FAILURE);  
	}  
	address->sin_family = AF_INET;  //type of socket created 
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons(port);
	if (bind((*master_socket), (struct sockaddr *)address, sizeof(*address)) < 0)//bind the socket to localhost port 
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "Listening on port " << port << '\n';
	if (listen((*master_socket), 3) < 0) //try to specify maximum of 3 pending connections for the master socket 
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	*addrlen = sizeof(address);  //accept the incoming connection 
	std::cout << "Server started\n";
}

void kick_user(std::vector<std::string> string_vec, std::vector<Channel> *channels, std::vector<User> users, int sd)
{
	if (string_vec[1][0] == '#')
	{
		std::string channel_name = string_vec[1].substr(1, string_vec[1].length());
		int user_sd = -1;
		if (check_if_channel_exists(channel_name, *channels))
		{
			std::string name_to_search = string_vec[2];
			for (std::vector<Channel>::iterator i = channels->begin(); i < channels->end(); i++)
			{
				if (i->get_name().compare(channel_name) == 0)
				{
					for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
					{
						if (i->get_nickname().compare(name_to_search) == 0)
						{
							user_sd = i->get_private_fd();
							break ;
						}
					}
					if (user_sd > 0)
						i->kick_user(user_sd, sd);
					break ;
				}
			}
		}
	}
}

void 	handle_part(std::string channel, int sd, std::vector<User> users, std::vector<Channel> *channels, char *buffer)
{
	User temp = get_user_by_fd(sd, users);
	std::string message_to_send = ":" + temp.get_identifier() + " " + buffer;
	if (channel[0] == '#')
		send_message_to_channel(sd, channel, users, *channels, std::string(buffer));
	send(sd, message_to_send.c_str(), message_to_send.length(), 0);
	erase_from_channel(sd, channel.substr(1, channel.length()), users, channels);
}

void	handle_first_validation(int sd, char *buffer, std::string global_password, std::vector<User> *users)
{
	std::vector<std::string> second_split = ft_cpp_vec_split(std::string(buffer), ' ');
	size_t i = 0;
	while (i < second_split.size())
	{
		if (second_split[i].compare("PASS") == 0)
		{
			std::string password = second_split[i + 1].substr(1, second_split[i + 1].size() - 1);
			if (global_password.compare(password) != 0) //error
				std::cout << "wrong password!\n";
			else
			{
				for (std::vector<User>::iterator i = users->begin(); i < users->end(); i++)
				{
					if (i->get_private_fd() == sd)
						i->validate_user();
				}
			}
		}
		else if (second_split[i].compare("NICK") == 0)
		{
			for (std::vector<User>::iterator it = users->begin(); it < users->end(); it++)
			{
				if (it->get_private_fd() == sd)
					it->set_nickname(second_split[i + 1]);
			}
		}
		else if (second_split[i].compare("USER") == 0)
		{
			for (std::vector<User>::iterator it = users->begin(); it < users->end(); it++)
			{
				if (it->get_private_fd() == sd)
				{
					it->set_username(second_split[i + 1]);
					it->set_host(second_split[i + 3]); 
					it->send_confirmation();
				}
			}
		}
		i++;
	}
}

void	handle_quit(int sd, std::vector<User> *users, std::vector<Channel> *channels, std::vector<int> *client_socket)
{
	std::string					msg_to_send;
	std::vector<int>::iterator	it;
	std::vector<User>::iterator	user_ite;
	User						temp = get_user_by_fd(sd, *users);

	for (std::vector<Channel>::iterator i = channels->begin(); i < channels->end(); i++)
	{
		msg_to_send.clear();
		msg_to_send.append(":" + temp.get_identifier() + " PART #" + i->get_name() + " :Connection Closed\n");
		if (i->check_if_user_is_present(temp))
		{
			i->remove_member_by_sd(sd);
			i->remove_operator_by_sd(sd);
			std::vector<User> temp_vec = i->get_members();
			for (std::vector<User>::iterator it = temp_vec.begin(); it < temp_vec.end(); it++)
				send(it->get_private_fd(), msg_to_send.c_str(), msg_to_send.length(), 0);
			temp_vec.clear();
			temp_vec = i->get_operators();
			for (std::vector<User>::iterator it = temp_vec.begin(); it < temp_vec.end(); it++)
				send(it->get_private_fd(), msg_to_send.c_str(), msg_to_send.length(), 0);
		}
	}
	for (it = client_socket->begin(); it < client_socket->end(); it++)
	{
		if (*it == sd)
			break ;
	}
	for (user_ite = users->begin(); user_ite < users->end(); user_ite++)
	{
		if (user_ite->get_private_fd() == sd)
			break ;
	}
	users->erase(user_ite);
	close(sd);  
	client_socket->erase(it);
}

void	handle_cap(std::string string_vec, int sd, char *buffer, std::string global_password, std::vector<User> *users)
{
	std::string message;

	if (string_vec.compare("LS") == 0)
	{
		message.append("CAP * LS :multi-prefix sasl\n");
		send(sd, message.c_str(), message.length(), 0);
	}
	else if (string_vec.compare("REQ") == 0)
	{
		message.append("CAP * ACK multi-prefix\n");
		send(sd, message.c_str(), message.length(), 0);
	}
	else if (string_vec.compare("END") == 0)
		handle_first_validation(sd, buffer, global_password, users);
}

void topic_handler(int sd, std::vector<User> users, std::vector<Channel> *channels, char *buffer)
{
	size_t i = 5; //end of TOPIC 
	User sender = get_user_by_fd(sd, users);

	if (!sender.is_validated())
	{
		std::cout << "User is not validated!\n";
		return ;
	}
	std::string channel_name;
	std::string topic_to_set;
	while (is_space(buffer[i]) && buffer[i] != '\0')
		i++;
	if (buffer[i] == '#')
	{
		i++;
		channel_name.append(&buffer[i]);
		int len = 0;
		while (buffer[i + len])
		{
			if (is_space(buffer[i + len]))
				break ;
			len++;
		}
		channel_name = channel_name.substr(channel_name.find(buffer[i]), len);
		std::cout << channel_name << '\n';
		i += len;
		if (i == strlen(buffer) - 2) // returns topic
		{
			std::string message_to_send = "332 #" + channel_name + " :" + get_channel_by_name(channel_name, *channels)->get_topic() + "\n";
			send(sd, message_to_send.c_str(), message_to_send.length(), 0);
			return ;
		}
		else
		{
			while (is_space(buffer[i]) && buffer[i] != '\0')
				i++;
			if (buffer[i] == ':')
			{
				i++;
				topic_to_set.append(&buffer[i]);
				for (std::vector<Channel>::iterator i = channels->begin(); i < channels->end(); i++)
				{
					if (i->get_name().compare(channel_name) == 0)
					{
						i->set_topic(topic_to_set);
						std::string message_to_send = ":" + sender.get_identifier() + " " + buffer;
						std::vector<User> operators = i->get_members();
						for (std::vector<User>::iterator i = operators.begin(); i != operators.end(); i++)
								send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
						operators = i->get_operators();
						for (std::vector<User>::iterator i = operators.begin(); i != operators.end(); i++)
								send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
						return ;
					}
				}
				std::string message_to_send = "442 #" + channel_name + " :You're not in that channel\n";
				send(sd, message_to_send.c_str(), message_to_send.length(), 0);
			}
		}
	}
}

void	super_mega_swag_if(std::string global_password, char *buffer, int sd, int valread, std::vector<User> *users, std::vector<Channel> *channels, std::vector<int> *client_socket)
{
	std::string parser_string;
	std::vector<std::string> string_vec;

	buffer[valread] = '\0';
	parser_string.append(buffer);
	string_vec = ft_cpp_vec_split(parser_string, ' ');
	if (string_vec[0].compare("PRIVMSG") == 0 || string_vec[0].compare("NOTICE") == 0)
		send_privmsg(string_vec[1], *users, *channels, std::string(buffer), sd);
	else if (string_vec[0].compare("NICK") == 0)
		nick_command(string_vec[1], sd, users);
	else if (string_vec[0].compare("JOIN") == 0)
		join_multiple_channels(sd, string_vec[1], *users, channels);
	else if (string_vec[0].compare("PART") == 0)
		handle_part(string_vec[1], sd, *users, channels, buffer);
	else if (string_vec[0].compare("QUIT") == 0)
		handle_quit(sd, users, channels, client_socket);
	else if (string_vec[0].compare("MODE") == 0)
		handle_mode(string_vec, channels, *users, sd);
	else if (string_vec[0].compare("KICK") == 0)
		kick_user(string_vec, channels, *users, sd);
	else if (string_vec[0].compare("TOPIC") == 0)
		topic_handler(sd, *users, channels, buffer);
	else if (string_vec[0].compare("CAP") == 0)
		handle_cap(string_vec[1], sd, buffer, global_password, users);
	else if (string_vec[0].compare("PASS") == 0)
		handle_first_validation(sd, buffer, global_password, users);
	bzero(buffer, strlen(buffer));
}