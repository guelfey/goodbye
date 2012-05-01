include config.mk

all: $(TARGET)

options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	
$(TARGET): options $(OBJ)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(CPPFLAGS) -o $(TARGET) $(SRC) 

%.o: %.c
	@echo CC $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	@echo "RM $(TARGET) $(OBJ)"
	@rm -f $(TARGET) $(OBJ)

install:
	@echo "MKDIR $(PREFIX)/bin"
	@mkdir -p $(PREFIX)/bin
	@echo "INSTALL $(TARGET) $(PREFIX)/bin"
	@install -m 755 $(TARGET) $(PREFIX)/bin

.PHONY: all options clean install
