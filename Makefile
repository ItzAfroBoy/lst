CC=gcc
SRC=src
INC=include
OBJ=lst.o ini.o
BIN=/usr/local/bin
DEPS=$(wildcard $(INC)/*.h)
NAME=lst
CFLAGS=-Wall -std=c99 -I$(INC) -g

.PHONY: install purge clean

%.o: $(SRC)/%.c $(DEPS)
	@$(CC) -c $< $(CFLAGS)

$(NAME): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)

install:
	@sudo cp $(NAME) $(BIN)

clean:
	@rm *.o $(NAME)

purge: clean
	@sudo rm $(BIN)/$(NAME)
