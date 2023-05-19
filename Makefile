CC = gcc
RM = rm -rf
TARGET = OSLINUX
CFLAG = -fcommon
SRC_DIR = source
INCS = include

SRCS = directory.c utility.c main.c \
		stack.c user.c cat.c cd.c \
		chmod.c chown.c find.c mkdir.c \
		rm.c pwd.c ls.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $@ $^ -o $@ -I$(INCS) -lpthread

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAG) -c $< -o $@ -I$(INCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(TARGET)

re: fclean
	make all

.PHONY: all clean fclean re