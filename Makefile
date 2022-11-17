NAME = ircserv
INCLUDE_FILES = 
INCLUDE_DIRS = src
SRC_FILES = main.cpp
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

$(NAME): $(addprefix $(OBJ_DIR)/,$(OBJ_FILES))
	$(CXX) $^ -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: clean all

.PHONY: all clean fclean re
