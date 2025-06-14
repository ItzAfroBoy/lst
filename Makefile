CC=gcc
SRC=src
INC=include
OBJ=lst.o ini.o
DEPS=$(wildcard $(INC)/*.h)
NAME=lst
CFLAGS=-Wall -std=c99 -I$(INC)
.PHONY: pkg clean

%.o: $(SRC)/%.c $(DEPS)
ifdef debug
	$(CC) -c $< $(CFLAGS) -g
else
	$(CC) -c $< $(CFLAGS) -O2
endif

$(NAME): $(OBJ)
ifdef debug
	$(CC) -o $@ $^ $(CFLAGS) -g
else
	$(CC) -o $@ $^ $(CFLAGS) -O2
endif

clean:
	@rm *.o $(NAME)
