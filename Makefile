include config.mk

TARGET = bye
OBJS = bye.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

clean:
	@rm $(TARGET) $(OBJS)

.PHONY: all clean
