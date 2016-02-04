# Sven Multimedia keyboard deamon
# Makefile - the Makefile template
# Copyright (C) 2005 Eugene Morenko(More) <more@irpin.com>


prefix = /usr
bindir = ${prefix}/bin

CC = gcc
INSTALL = /usr/bin/install -c
INSTALL_DIR = $(INSTALL) -d -m 755 
INSTALL_PROGRAM = $(INSTALL) -m 755


all: sven-font

sven-font: sven-font.c
	$(CC) `gtk-config --cflags`  sven-font.c -o sven-font `gtk-config --libs`
	echo "The sven-font binary is succesfully created"
	echo "run make install"

clean:
	rm -f sven-font

patchclean: clean
	rm -f *.orig *.rej  

distclean: patchclean
	rm -f  *~ *.bak core

install: all
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) ./sven-font $(DESTDIR)$(bindir)/sven-font

uninstall:
	/bin/rm -f $(DESTDIR)$(bindir)/sven-font
