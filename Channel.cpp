#include "Channel.hpp"

void	Channel::remove_member_by_sd(const int sd)
{
	std::vector<User> temp;
	for (std::vector<User>::iterator i = this->_members.begin(); i < this->_members.end(); i++)
	{
		if (i->get_private_fd() != sd)
			temp.push_back(*i);	
	}
	this->set_members(temp);
}

void	Channel::remove_operator_by_sd(const int sd)
{
	std::vector<User> temp;
	for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
	{
		if (i->get_private_fd() != sd)
			temp.push_back(*i);	
	}
	this->set_operators(temp);
}

bool	Channel::check_if_sd_is_operator(int sd)
{
	for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
	{
		if (i->get_private_fd() == sd)
			return (true);
	}
	return (false);
}


User get_user_by_fd(int fd, std::vector<User> users);

void	Channel::make_user_operator(const std::string &to_add, std::vector<User> users, int adding_fd)
{
	int new_operator_sd = -1;
	std::string message_to_send;
	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
	{
		if (i->get_nickname().compare(to_add) == 0)
		{
			new_operator_sd = i->get_private_fd();
			break ;
		}
	}
	if (new_operator_sd < 0)
		return ;
	User temp = get_user_by_fd(new_operator_sd, users);
	User adding_user = get_user_by_fd(adding_fd, users);
	this->remove_member_by_sd(new_operator_sd);
	this->add_operator(temp);
	// "MODE #" + channel_to_join + " +o " + get_user_by_adding_fd(sd, users)._nickname + "\n";
	message_to_send = ":" + adding_user.get_nickname() + " MODE #" + this->get_name() + " +o " + to_add + "\n";
	for (std::vector<User>::iterator i = this->_members.begin(); i < this->_members.end(); i++)
		send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
	for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
		send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
}

void	Channel::remove_user_from_operator(const std::string &to_remove, std::vector<User> users, int removing_fd)
{
	std::string message_to_send;
	int			to_remove_sd = -1;
	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
	{
		if (i->get_nickname().compare(to_remove) == 0)
		{
			to_remove_sd = i->get_private_fd();
			break ;
		}
	}
	if (to_remove_sd < 0)
		return ;
	User temp = get_user_by_fd(to_remove_sd, users);
	User removing_user = get_user_by_fd(removing_fd, users);
	this->remove_operator_by_sd(to_remove_sd);
	this->add_member(temp);
	message_to_send = ":" + removing_user.get_nickname() + " MODE #" + this->get_name() + " -o " + to_remove + "\n";
	for (std::vector<User>::iterator i = this->_members.begin(); i < this->_members.end(); i++)
		send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
	for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
		send(i->get_private_fd(), message_to_send.c_str(), message_to_send.length(), 0);
}

bool	Channel::kick_user(int sd_to_remove, int sd_to_send)
{
	User user_to_remove;
	User user_kicking;
	bool found = false;
	bool removed = false;
	bool is_operator = false;

	if (sd_to_remove == sd_to_send)
		return (false);
	for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
	{
		if (i->get_private_fd() == sd_to_send)
		{
			user_kicking = *i;
			is_operator = true;
			break ;
		}
	}
	if (!is_operator)
	{
		std::string msg = "482 " + this->get_name() + " :Operator privilege is needed!\n";
		send(sd_to_send, msg.c_str(), msg.length(), 0);
		return (false);
	}
	for (std::vector<User>::iterator i = this->_members.begin(); i < this->_members.end(); i++)
	{
		if (i->get_private_fd() == sd_to_remove)
		{
			user_to_remove = *i;
			found = true;
			break ;
		}
	}
	if (!found)
	{
		for (std::vector<User>::iterator i = this->_operators.begin(); i < this->_operators.end(); i++)
		{
			if (i->get_private_fd() == sd_to_remove)
			{
				user_to_remove = *i;
				found = true;
				this->remove_operator_by_sd(sd_to_remove);
				removed = true;
				break ;
			}
		}
	}
	else
	{
		this->remove_member_by_sd(user_to_remove.get_private_fd());
		removed = true;
	}
	if (removed)
	{
		std::string message_to_send = ":" + user_kicking.get_nickname() + " KICK #" + _channel_name +  " " + user_to_remove.get_nickname() + "\n";
		send(sd_to_send, message_to_send.c_str(), message_to_send.size(), 0);
		send(sd_to_remove, message_to_send.c_str(), message_to_send.size(), 0);
	}
	return (removed);
}

bool	Channel::check_if_user_is_present(User user_to_check)
{
	std::vector<User> users = this->get_members();

	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
	{
		if (user_to_check.get_private_fd() == i->get_private_fd())
			return (true);
	}
	users = this->get_operators();
	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++)
	{
		if (user_to_check.get_private_fd() == i->get_private_fd())
			return (true);
	}
	return (false);
}