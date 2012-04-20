include config.mk

TARGET = goodbye
OBJS = goodbye.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

clean:
	@echo Cleaning up...
	@rm $(TARGET) $(OBJS)

install:
	@echo Installing into $(PREFIX)/bin
	@mkdir -p $(PREFIX)/bin
	@install -m 755 bye $(PREFIX)/bin

.PHONY: all clean
