include config.mk

TARGET = bye
OBJS = bye.o

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)
