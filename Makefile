NAME = ircserv
HOSTNAME:= $(shell hostname)
SRC = src/main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp src/Message.cpp src/Numerics.cpp

OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

CXX = @c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98 -DHOSTNAME=\"$(HOSTNAME)\"

RED		= \033[1;31m
GREEN	= \033[1;32m
RESET	= \033[0m


all: $(NAME)

$(NAME): $(OBJ)
	@echo "Compiler flags:		$(GREEN)$(CXXFLAGS)$(RESET)"
	@echo "\nCreated binary file:	$(GREEN)(+) $(NAME)$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@echo "Compiling .o file:	$(GREEN)(+) $@$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Removing dependancies:	$(RED)(-) $(OBJ) $(DEP)$(RESET)"
	@rm -f $(OBJ) $(DEP)

fclean: clean
	@echo "Removing binary file:	$(RED)(-) $(NAME)$(RESET)"
	@rm -f $(NAME)

g: CXXFLAGS += -g
g: re

re: fclean all

-include $(DEP)

.PHONY: all clean fclean re g
