CC=gcc

OPT=-O2
DEBUG=-g


CFLAGS=$(OPT) $(DEBUG) -fPIC -Wall `../../script/sven-config --cflags`
LIBS= ../../libsven/.libs/libsven.so


PLUGIN=time.so
PLUGINDIR= `../../script/sven-config --plugin-dir`

all : $(PLUGIN)

install : $(PLUGIN)
	mkdir -p $(DESTDIR)$(PLUGINDIR)
	install $(PLUGIN) $(DESTDIR)$(PLUGINDIR)


$(PLUGIN) : time.o
	$(CC) $(LIBS) -shared $^ -o $@


.c.o: $(SRCS)
	$(CC) $(CFLAGS) -c $^ -o $@

clean :
	rm -f $(PLUGIN) *.o


