NAME		:= ircserv
SRC_DIR		:= srcs
OBJ_DIR		:= objs
INC_DIR		:= includes
SRCS		:= main.cpp Server.cpp Client.cpp Parser.cpp Channel.cpp \
				commands/Cap.cpp commands/Pass.cpp commands/Nick.cpp \
				commands/User.cpp commands/Ping.cpp commands/Pong.cpp \
				commands/Quit.cpp commands/Privmsg.cpp commands/Join.cpp \
				commands/Names.cpp commands/Part.cpp commands/Mode.cpp \
				commands/Topic.cpp commands/Kick.cpp commands/Invite.cpp \
				commands/Whois.cpp commands/Who.cpp
HDRS		:= Server.hpp Client.hpp Parser.hpp Replies.hpp Channel.hpp
OBJS		:= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
HEADERS		:= $(addprefix $(INC_DIR)/, $(HDRS))
BONUS_NAME	:= ircbot
BONUS_DIR	:= bonus
BONUS_SRCS	:= main.cpp Bot.cpp
BONUS_OBJS	:= $(addprefix $(OBJ_DIR)/$(BONUS_DIR)/, $(BONUS_SRCS:.cpp=.o)) \
				$(OBJ_DIR)/Parser.o
CPP			:= c++
CPPFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)
RM			:= rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CPP) $(CPPFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS)

$(OBJ_DIR)/$(BONUS_DIR)/%.o: $(BONUS_DIR)/%.cpp $(BONUS_DIR)/Bot.hpp \
		$(INC_DIR)/Parser.hpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -I$(BONUS_DIR) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all bonus clean fclean re
