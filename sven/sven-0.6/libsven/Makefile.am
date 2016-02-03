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
	trayicon.c trayicon.h\
	xlib.c xlib.h\
	emulate_input.c emulate_input.h\
	utils.c utils.h\
	configfile.c configfile.h\
	key.c key.h\
	mouse.c mouse.h\
	grabkeyboard.c grabkeyboard.h\
	gtkosd.c gtkosd.h\
	plugin.c plugin.h\
	pixmap.c pixmap.h\
	pref/prefswin.c pref/prefswin.h\
	pref/about.c\
	pref/confdevice.c\
	pref/configplugin.c\
	pref/configxosd.c
	
sveninclude_HEADERS = \
		trayicon.h\
		xlib.h\
		emulate_input.h\
		utils.h\
		configfile.h\
		key.h\
		mouse.h\
		grabkeyboard.h\
		gtkosd.h\
		plugin.h\
		pixmap.h\
		pref/prefswin.h
