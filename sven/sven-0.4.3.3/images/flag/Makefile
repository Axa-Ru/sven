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
	ae ae.png\
	am am.png\
	ar ar.png\
	at at.png\
	az az.png\
	be be.png\
	bg bg.png\
	bh bh.png\
	br br.png\
	by by.png\
	ca ca.png\
	ch ch.png\
	cu cu.png\
 	cz cz.png\
	de de.png\
	dj dj.png\
	dk dk.png\
	dz dz.png\
	ee ee.png\
	eg eg.png\
	es es.png\
	eu eu.png\
	fi fi.png\
	fr fr.png\
	gb gb.png\
	ge ge.png\
	gr gr.png\
	hr hr.png\
	hu hu.png\
	il il.png\
	iq iq.png\
	is is.png\
	it it.png\
	jo jo.png\
	jp jp.png\
	km km.png\
	kr kr.png\
	kw kw.png\
	la la.png\
	lb lb.png\
	lt lt.png\
	ly ly.png\
	ma ma.png\
	mk mk.png\
	mn mn.png\
	mx mx.png\
	nl nl.png\
	no no.png\
	om om.png\
	pl pl.png\
	ps ps.png\
	pt pt.png\
	qa qa.png\
	qc qc.png\
	ro ro.png\
	ru ru.png\
	sa sa.png\
	sd sd.png\
	se se.png\
	si si.png\
	sk sk.png\
	so so.png\
	sr sr.png\
	sy sy.png\
	th th.png\
	tn tn.png\
	tr tr.png\
	ua ua.png\
	uk uk.png\
	us us.png\
	un un.png\
	uy uy.png\
	vn vn.png\
	ye ye.png\
	yu yu.png\
	zz zz.png

all:
	gdk-pixbuf-csource --raw --static --build-list $(TOOLBAR) > sven_flags.h




