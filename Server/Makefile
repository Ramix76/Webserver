# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: framos-p <framos-p@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/25 16:46:08 by framos-p          #+#    #+#              #
#    Updated: 2024/04/18 15:33:32 by framos-p         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME				=	webserver

INC					=	includes/
SRCS				=	srcs/


SRCS_FILES			=	$(SRCS)main.cpp \
						$(SRCS)SocketServer.cpp \
						$(SRCS)HTTPRequest.cpp
						
CC					=	g++
CPPFLAGS			=	-O0 -g -I$(INC) -Wall -Wextra -Werror -MMD -std=c++98
LDLIBS      		= 	-lstdc++
RM 					=	rm -rf

OBJS				=	$(SRCS_FILES:%.cpp=%.o)

DEPS				=	$(SRCS_FILES:%.cpp=%.d)

# Colors

NOCOLOR				=	\033[0m
BOLD_PURPLE			=	\033[1;35m
BOLD_CYAN			=	\033[1;36m
RED 				=	\033[0;91m
YELLOW 				=	\033[5;93m
BLUE 				=	\033[0;94m
GREEN				=	\033[0;32m

all: $(NAME)

$(NAME)::			$(OBJS)
					echo "\n$(YELLOW)Linking...$(GREEN)\n"
					$(CC) $(CPPFLAGS) $(LDLIBS) $(OBJS) -o $(NAME)

$(NAME)::
					echo "\n$(BLUE)WEBSERVER COMPILED! ✅$(DEF_COLOR)\n"

clean:
					$(RM) $(OBJS) $(NAME)
					$(RM) $(DEPS) 
					echo "\n✅$(YELLOW)Clean: $(RED)Removed WebServer.a \n$(NOCOLOR)"
					echo "\n☑️ $(YELLOW)Clean: $(RED)Removed all the \".o\ and \".d\" files \n$(NOCOLOR)"

fclean: clean

re:	fclean all

-include $(DEPS)

.PHONY: all clean fclean re

.SILENT:
