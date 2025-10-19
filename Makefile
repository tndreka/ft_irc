NAME = IRC

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer -std=c++98 -I$(INC_DIR)

MAKEFLAGS += -s

#Folders
SRC_DIR = src

OBJ_DIR = obj

INC_DIR = include

#Source and object
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME) : $(OBJS)
	@printf "[.]   📦 Compiling '\033[33m$(NAME)\033[0m'...\r"
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "🚀 '\033[33m$(NAME)\033[0m' compiled \033[32msuccessfully\033[0m!"

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) 
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -rf $(OBJ_DIR)

fclean: clean
	@printf "[.]   🧹 Removing '\033[33m$(NAME)\033[0m' build...\r"
	rm -f $(NAME)
	printf "[✅]  🧹 Removed '\033[33m$(NAME)\033[0m' build...  \n"

re: fclean all

.PHONY: all clean fclean re
