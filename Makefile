NAME = game
CFLAGS = -std=c99 -Wall -Wextra -Werror -g
LIBFLAGS = -lGL -lglfw -lGLEW -lm
SRC_DIR := src
SUBDIRS := $(SRC_DIR) $(SRC_DIR)/game $(SRC_DIR)/platform $(SRC_DIR)/core

SRC := $(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.c))
OBJS := $(SRC:.c=.o)


all: $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFLAGS)


clean:
	rm $(NAME) $(OBJS)
