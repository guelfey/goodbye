VERSION = 0.1
TARGET = goodbye

# customize this
PREFIX=/usr/local
# for GTK2:
CFLAGS   := `pkg-config --cflags gtk+-2.0`
LDFLAGS  := `pkg-config --cflags --libs gtk+-2.0`
CPPFLAGS := -DUSE_GTK2
# for GTK3:
#CFLAGS   := `pkg-config --cflags gtk+-3.0`
#LDFLAGS  := `pkg-config --cflags --libs gtk+-3.0`
#CPPFLAGS := -DUSE_GTK3

CFLAGS += -Wall -pedantic -g -std=c99
CPPFLAGS += -DVERSION=\"$(VERSION)\" -DPROGNAME=\"$(TARGET)\"
LDFLAGS += -Wall -pedantic -g -std=c99

SRC = goodbye.c
OBJ = $(SRC:.c=.o)
