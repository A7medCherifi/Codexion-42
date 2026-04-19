NAME = codexion
CC = gcc 
CFLAGS = -g -Wall -Wextra -Werror -pthread
RM = rm -f

HEADER = coders/codexion.h
INCLUDES = -I coders

SRC = parsing.c ft_atoi.c utils.c codexion.c get_time.c

OBJ = $(SRC:%.c=obj/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "Done"

obj/%.o: coders/%.c $(HEADER)
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@$(RM) -rf obj/

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: clean