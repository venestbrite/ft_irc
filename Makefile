# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: stribuzi <stribuzi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/11 16:39:14 by stribuzi          #+#    #+#              #
#    Updated: 2022/02/15 12:01:29 by stribuzi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS	=	Channel.cpp utils.cpp handlers.cpp main.cpp

SRCS_BOT = 	bot.cpp

CXX		= clang++

CXXFLAGS = -Wall -Wextra -Werror

OBJS	= $(SRCS:.cpp=.o)

OBJS_BOT	= $(SRCS_BOT:.cpp=.o)

RM				= rm -f

NAME			= ircserv

BOT_NAME		= bot

all:			$(NAME)

$(NAME):		 $(OBJS)
					$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

bot:			$(OBJS_BOT)
					$(CXX) $(CXXFLAGS) $(OBJS_BOT) -o $(BOT_NAME)

clean:
				$(RM) $(OBJS) $(OBJS_BOT)

fclean:			clean
				$(RM) $(NAME) $(BOT_NAME)

re:				clean all

.PHONY:			all clean fclean re