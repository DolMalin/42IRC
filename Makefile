NAME = ircserv
INCLUDE_FILES =	src/common.hpp src/Socket.hpp
INCLUDE_DIRS = src
SRC_FILES = Socket.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
SRC_DIR = src
OBJ_DIR = obj
CXX = c++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE_DIRS)
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
