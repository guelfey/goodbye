include config.mk

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      $(TARGET)"
	@$(CC) $(LDFLAGS) -o $(TARGET) $(SRC) -DPROGNAME=\"$(TARGET)\" \
	-DVERSION=\"$(VERSION)\"

clean:
	@echo "RM      $(TARGET)"
	@rm -f $(TARGET)

install:
	@echo "MKDIR   $(PREFIX)/bin"
	@mkdir -p $(PREFIX)/bin
	@echo "INSTALL $(TARGET) $(PREFIX)/bin"
	@install -m 755 $(TARGET) $(PREFIX)/bin

.PHONY: all clean install
