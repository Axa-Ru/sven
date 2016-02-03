#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sven.h"
/*Start Greate Image*/
#include "../images/sven_icons.h"

typedef struct {
	const guint id;
	const guint8 *data;
} Tpixmap;

static Tpixmap tp[] = {
	{1,key_bindings},
	{2,mouse},
	{3,plugins},
	{4,messagebox_info},
	{5,wind_pic},
	{6,configure_pic},
	{7,topleft},
	{8,top},
	{9,topright},
	{10,bottomleft},
	{11,bottom},
	{12,bottomright},
	{13,left},
	{14,centre},
	{15,right},
	{16,confgktosd},
	{17,keyboardim},
	{18,cdaudio},
	{19,klipper_dock},
	{20,locale_im},
	{21,terminal},
	{22,vol},
	{23,vol_down},
	{24,vol_up},
	{25,vol_mute},
};
/* This is an internally used function to create pixmaps. */
GdkPixbuf *create_pixbuf_type(gint type, int width, int height)
{
	gint i;
	GdkPixbuf *pixbuf;

		for (i=0;i<(sizeof(tp)/sizeof(Tpixmap));i++) {
			if (tp[i].id == type) {
				break;
			}
		}
		if (i >= sizeof(tp)/sizeof(Tpixmap)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tp)/sizeof(Tpixmap)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	g_object_unref(pixbuf);
	pixbuf=scaled;
    }
	return pixbuf;
}

GtkWidget *create_pixmap_type(gint type, int width, int height) 
{
	GtkWidget *wid;
	gint i;
	GdkPixbuf *pixbuf;

		for (i=0;i<(sizeof(tp)/sizeof(Tpixmap));i++) {
			if (tp[i].id == type) {
				break;
			}
		}
		if (i >= sizeof(tp)/sizeof(Tpixmap)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tp)/sizeof(Tpixmap)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tp[i].data,FALSE,NULL);
		    g_assert (pixbuf);

    if ((width > 0 && height > 0)
	|| (gdk_pixbuf_get_width (pixbuf) != width
	    && gdk_pixbuf_get_height (pixbuf) != height))
    {
	GdkPixbuf *scaled;

	scaled =
	    gdk_pixbuf_scale_simple (pixbuf,
				     width >
				     0 ? width : gdk_pixbuf_get_width (pixbuf),
				     height >
				     0 ? height :
				     gdk_pixbuf_get_height (pixbuf),
				     GDK_INTERP_BILINEAR);
	g_object_unref(pixbuf);
	pixbuf=scaled;
    }
	wid = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	return wid;
}
/*END Greate Image*/

GdkPixbuf *create_pixbuf_from_file (gchar *file, gint width, gint height)
{
	GdkPixbuf *pixbuf;
    
	DEBUG_MSG ("create_pixbuf_from_file: path: %s\n", file);
    
	if (g_file_test (file, G_FILE_TEST_EXISTS)) 
	{
		GError *error=NULL;
    
		pixbuf=gdk_pixbuf_new_from_file (file, &error);
		
		if ((width > 0 && height > 0) || (gdk_pixbuf_get_width (pixbuf) != width  && gdk_pixbuf_get_height (pixbuf) != height))
		{
			GdkPixbuf *scaled;
			scaled = gdk_pixbuf_scale_simple (pixbuf,
				width > 0 ? width : gdk_pixbuf_get_width (pixbuf),
				height > 0 ? height : gdk_pixbuf_get_height (pixbuf),
				GDK_INTERP_BILINEAR);
			pixbuf=scaled;
		}
		
		return pixbuf;
	}
	return NULL;
}


GtkWidget *create_pixmap_from_file(gchar *file,int width, int height) 
{
	GtkWidget *wid;
	GdkPixbuf *pixbuf;
	GError *error=NULL;
    
	if (g_file_test (file, G_FILE_TEST_EXISTS)) 
	{
	
		pixbuf=gdk_pixbuf_new_from_file (file, &error);
		g_assert (pixbuf);
	
		if ((width > 0 && height > 0) || (gdk_pixbuf_get_width (pixbuf) != width  && gdk_pixbuf_get_height (pixbuf) != height))
		{
			GdkPixbuf *scaled;
			scaled = gdk_pixbuf_scale_simple (pixbuf,
				width > 0 ? width : gdk_pixbuf_get_width (pixbuf),
				height > 0 ? height : gdk_pixbuf_get_height (pixbuf),
				GDK_INTERP_BILINEAR);
			pixbuf=scaled;
		}
		wid = gtk_image_new_from_pixbuf(pixbuf);
		g_object_unref(pixbuf);
		return wid;
	}
	return NULL;
}
