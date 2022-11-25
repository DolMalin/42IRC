NAME = ircserv

INCLUDE_FILES = src/common.hpp src/Server.hpp src/Message.hpp src/User.hpp src/Channel.hpp src/Reply.hpp
INCLUDE_DIRS = src
SRC_FILES = Server.cpp Message.cpp User.cpp Channel.cpp Reply.cpp\
	common.cpp\
	commands/pass.cpp commands/nick.cpp commands/user.cpp\
	commands/join.cpp commands/ping_pong.cpp commands/quit.cpp commands/kill.cpp\
	commands/list.cpp commands/names.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)
SRC_DIR = src
OBJ_DIR = obj
CXX = c++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE_DIRS)
CXX_FLAGS += -g3
DEPENDENCIES = $(INCLUDE_FILES) Makefile

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPENDENCIES)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

$(NAME): obj/main.o $(addprefix $(OBJ_DIR)/,$(OBJ_FILES)) 
	$(CXX) $^ -o $@

test_client: obj/test_client.o $(addprefix $(OBJ_DIR)/,$(OBJ_FILES))
	$(CXX) $^ -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) -r $(NAME)

re: clean all

.PHONY: all clean fclean re
