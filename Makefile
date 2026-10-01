NAME		:= ircserv
SRC_DIR		:= srcs
OBJ_DIR		:= objs
INC_DIR		:= includes
SRCS		:= main.cpp
HDRS		:=

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
