# this script can help to find files that are actually not in use, run it from the basedir:
#
#for file in inline_images/*.png; do
#	BASE=`echo $file | sed -e 's/.png//' -e 's?inline_images/??'`
#	if ! egrep -q "[\\t ]*\{[0-9]+, ${BASE}\}" src/pixmap.c ; then
#		if grep -q "^[^#]*${BASE} ${BASE}.png" inline_images/Makefile; then
#			echo $BASE
#		fi
#	fi
#done

TOOLBAR = \
	sven sven.png\
	topleft top-left.png\
	top top.png\
	topright top-right.png\
	bottomleft bottom-left.png\
	bottom bottom.png\
	bottomright bottom-right.png\
	left left.png\
	centre centre.png\
	right right.png\
	configure configure.png\
	data_visualization_section data_visualization_section.png\
	key_bindings key_bindings.png\
	klipper_dock klipper_dock.png\
	messagebox_info messagebox_info.png\
	plugins plugins.png\
	terminal terminal.png\
	cdaudio cdaudio.png\
	cdplay cd-play.png\
	cdprev cd-prev.png\
	cdstop cd-stop.png\
	cdpause cd-pause.png\
	cdnext cd-next.png\
	cdeject cd-eject.png\
	volup vol-up.png\
	voldown vol-down.png\
	volmute vol-mute.png\
	vol vol.png\
	mouse mouse.png\
	locale locale.png
	

all:
	gdk-pixbuf-csource --raw --static --build-list $(TOOLBAR) > sven_icons.h




