CC = gcc
RM = rm -rf
TARGET = OSLINUX
CFLAG = -fcommon
SRC_DIR = source
OBJ_DIR = object
INCS = include

SRCS = directory.c utility.c main.c \
		stack.c user.c cat.c cd.c \
		chmod.c chown.c find.c mkdir.c \
		rm.c pwd.c ls.c
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $@ $^ -o $@ -I$(INCS) -lpthread

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAG) -c $< -o $@ -I$(INCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(TARGET)

re: fclean
	make all

.PHONY: all clean fclean re