NAME = codexion
CC = cc 
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm -f

HEADER = coders/codexion.h

SRC = parsing.c ft_atoi.c utils.c codexion.c utils2.c monitor.c \
	thread_manager.c heap_manager.c thread_helper.c all_checks.c 

OBJ = $(SRC:%.c=obj/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "Done"

obj/%.o: coders/%.c $(HEADER)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@$(RM) -rf obj/

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: clean

#-fsanitize=thread  -g3 -O0 -fsanitize=address -fsanitize=undefined       -g -fsanitize=address 