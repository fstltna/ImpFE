# Makefile for ImpFE (Linux, GTK 3)
#
# ImpFE is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# Needs the GTK 3 development files, e.g. on Debian/Ubuntu:
#   sudo apt install build-essential pkg-config libgtk-3-dev
#
# Usage:
#   make                build impfe
#   make install        install into $(BINDIR) and $(MANDIR)
#   make clean          remove build output
#
# Any setting below can be changed on the command line, for example:
#   make install PREFIX=/usr DESTDIR=/tmp/pkgroot

# =================== Settings you may want to change

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man
DESTDIR ?=

# Number of planets/ships per sector expected (for optimizations)
NUMSECTPLAN = 15
NUMSECTSHIP = 25

CC ?= cc
CFLAGS ?= -O2 -g
PKG_CONFIG ?= pkg-config
INSTALL ?= install

# =================== Nothing below here should normally need changing

GTK_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS := $(shell $(PKG_CONFIG) --libs gtk+-3.0 2>/dev/null)

ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifeq ($(strip $(GTK_LIBS)),)
$(error GTK 3 not found - install the GTK 3 development package (libgtk-3-dev))
endif
endif

# Always on: these catch mistakes that newer compilers refuse outright
WARNFLAGS = -Wall -Wno-unused-variable -Wno-unused-but-set-variable \
	-Werror=implicit-function-declaration -Werror=implicit-int \
	-Werror=incompatible-pointer-types -Werror=int-conversion

ALL_CFLAGS = $(CFLAGS) $(WARNFLAGS) $(GTK_CFLAGS) \
	-DMAX_SECT_PLAN=$(NUMSECTPLAN) -DMAX_SECT_SHIP=$(NUMSECTSHIP) -MMD -MP

FE_OBJS = ImpFeGui.o gtkui.o gui_cb.o impcon.o main.o parse.o feconfig.o \
	database.o cmnd.o femap.o help.o ship.o planet.o

all: impfe

impfe: $(FE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(FE_OBJS) $(GTK_LIBS) -lm

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

# help.h is generated from the help text
help.h: help.txt help_gen
	./help_gen
help.o: help.h

install: impfe
	$(INSTALL) -d $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	$(INSTALL) -m 755 impfe $(DESTDIR)$(BINDIR)/impfe
	$(INSTALL) -m 644 impfe.1 $(DESTDIR)$(MANDIR)/man1/impfe.1

clean:
	-rm -f *.o *.d impfe help.h

.PHONY: all install clean

-include $(FE_OBJS:.o=.d)
