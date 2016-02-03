## Process this file with automake to produce Makefile.in
#svenincludedir = $(includedir)/sven

bin_PROGRAMS = sven

gnulocaledir = $(prefix)/share/locale
pkgdatadir = @pkgdatadir@

INCLUDES = \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DLIBDIR=\""@plugindir@"\" \
	-DPKGDATADIR=\""$(pkgdatadir)"\" \
	@PACKAGE_CFLAGS@ @X_CFLAGS@ 


#sven_LDFLAGS =  ${GTK_CFLAGS}

sven_SOURCES = sven.c

#sven_LDADD = @PACKAGE_LIBS@  $(INTLLIBS)
#AM_CFLAGS = ${GTK_CFLAGS}	
	
sven_LDADD = @PACKAGE_LIBS@  ../libsven/*.o @X_LIBS@ -lgmodule-2.0 -lX11

