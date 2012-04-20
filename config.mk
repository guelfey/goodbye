# customize this
PREFIX=/usr/local

# for GTK2:
LIBS=`pkg-config --libs --cflags gtk+-2.0`

# for GTK3:
#LIBS=`pkg-config --libs --cflags gtk+-3.0`

CFLAGS = $(LIBS) -Wall -pedantic -g
LDFLAGS = $(LIBS) -Wall -pedantic -g
