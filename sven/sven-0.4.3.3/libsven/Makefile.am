ACLOCAL_AMFLAGS = -I ../m4
AUTOMAKE_OPTIONS = foreign

svenincludedir = $(includedir)/sven

lib_LTLIBRARIES = libsven.la

libsven_la_LDFLAGS = -export-dynamic -version-info $(LIBSVEN_MAJOR_VERSION):$(LIBSVEN_MINOR_VERSION):$(LIBSVEN_MICRO_VERSION)
libsven_la_LIBADD =  @PACKAGE_LIBS@ @X_LIBS@


INCLUDES = \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DLIBDIR=\""@plugindir@"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	@PACKAGE_CFLAGS@ @X_CFLAGS@

libsven_la_SOURCES = \
	about.c about.h\
	cdpl.c cdpl.h\
	clipman.c clipman.h\
	configfile.c configfile.h\
	confdevice.c confdevice.h\
	configxosd.c configxosd.h\
	configany.c configany.h\
	configplugin.c configplugin.h\
	eggtrayicon.c eggtrayicon.h\
	key.c key.h\
	mouse.c mouse.h\
	grabkeyboard.c grabkeyboard.h\
	mixer.c mixer.h\
	prefswin.c prefswin.h\
	plugin.c plugin.h\
	utils.c utils.h\
	xosd.c xosd.h\
	wininfo.c wininfo.h\
	sxkb.c sxkb.h\
	xlib.c xlib.h\
	load_symbol.c load_symbol.h\
	confcorrect.c confcorrect.h\
	confswitch.c confswitch.h

sveninclude_HEADERS = \
	about.h\
	cdpl.h\
	clipman.h\
	configfile.h\
	confdevice.h\
	configxosd.h\
	configany.h\
	configplugin.h\
	eggtrayicon.h\
	key.h\
	mouse.h\
	grabkeyboard.h\
	mixer.h\
	prefswin.h\
	plugin.h\
	sven.h\
	utils.h\
	xosd.h\
	wininfo.h\
	sxkb.h\
	xlib.h\
	load_symbol.h\
	confcorrect.h\
	confswitch.h


