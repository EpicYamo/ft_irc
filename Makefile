NAME		:= ircserv
SRC_DIR		:= srcs
OBJ_DIR		:= objs
INC_DIR		:= includes
SRCS		:= main.cpp Server.cpp Client.cpp Parser.cpp \
				commands/Cap.cpp commands/Pass.cpp commands/Nick.cpp \
				commands/User.cpp commands/Ping.cpp commands/Pong.cpp \
				commands/Quit.cpp commands/Privmsg.cpp commands/Join.cpp \
				commands/Names.cpp Channel.cpp commands/Part.cpp
HDRS		:= Server.hpp Client.hpp Parser.hpp Replies.hpp Channel.hpp
OBJS		:= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
HEADERS		:= $(addprefix $(INC_DIR)/, $(HDRS))
CPP			:= c++
CPPFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)
RM			:= rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
