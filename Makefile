include config.mk

all: $(TARGET)

$(TARGET): $(SRC)
	@echo CC $(TARGET)
	@$(CC) $(LDFLAGS) -o $(TARGET) $(SRC) -DPROGNAME=\"$(TARGET)\" \
	-DVERSION=\"$(VERSION)\"

clean:
	@rm -f $(TARGET)

install:
	@mkdir -p $(PREFIX)/bin
	@install -m 755 $(TARGET) $(PREFIX)/bin

.PHONY: all clean install
