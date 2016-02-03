CC=gcc

OPT=-O3
DEBUG= -ggdb3


CFLAGS=$(OPT) $(DEBUG) -fPIC -Wall `../../script/sven-config --cflags` `pcre-config --cflags`
LIBS= -L../../libsven/.libs -lsven `pcre-config --libs`


PLUGIN=autoconvert.so
PLUGINDIR= `../../script/sven-config --plugin-dir`

all : $(PLUGIN)

install : $(PLUGIN)
	mkdir -p $(DESTDIR)$(PLUGINDIR)
	install $(PLUGIN) $(DESTDIR)$(PLUGINDIR)

$(PLUGIN) : autoconvert.o sxkb.o xutil.o flag.o wlist.o keyboard.o convert.o glist.o configure.o
	$(CC) -shared $^ -o $@ $(LIBS)


.c.o: $(SRCS)
	$(CC) $(CFLAGS) -c $^ -o $@

distclean :
	rm -f $(PLUGIN) *.o   *.so
	
clean :
	rm -f $(PLUGIN) *.o  *.so


