#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "autoconvert.h"
#include "flag.h"
#include "../../libsven/sven.h"

/*Start Greate Image*/
#include "flag/sven_flags.h"

typedef struct {
	const gchar *name;
	const gchar *lang;
	const guint8 *data;
} Tflagpix;

static Tflagpix tfp[] = {
	{"United Arab Emirates","ae",ae},
	{"Armenian","am",am},
	{"Argentina","ar",ar},
	{"Austria","at",at},
	{"Azerbaijan","az",az},
	{"Belgian","be",be},
	{"Cyrillic","bg",bg},
	{"Bahrain","bh",bh},
	{"Brazilian","br",br},
	{"Belarusian","by",by},
	{"Canadian","ca",ca},
	{"Switzerland","ch",ch},
	{"Cuba","cu",cu},
	{"Czech","cz",cz},
	{"German","de",de},
	{"Djibouti","dj",dj},
	{"Danish","dk",dk},
	{"Dzongkha","dz",dz},
	{"Estonian","ee",ee},
	{"Egypt","eg",eg},
	{"Spanish","es",es},
	{"Finnish","fi",fi},
	{"French","fr",fr},
	{"Great Britain","gb",gb},
	{"Georgian","ge",ge},
	{"ISO8859-7","gr",gr},
	{"Croatian","hr",hr},
	{"Croatian(US)","hr",hr},
	{"Hungarian","hu",hu},
	{"Israelian","il",il},
	{"Iraq","iq",iq},
	{"Icelandic","is",is},
	{"Italian","it",it},
	{"Jordan","jo",jo},
	{"Japanese","jp",jp},
	{"Comoros","km",km},
	{"South Korea","kr",kr},
	{"Kuwait","kw",kw},
	{"Latin American","la",la},
	{"Lebanon","lb",lb},
	{"Lithuanian","lt",lt},
	{"Libya","ly",ly},
	{"Morocco","ma",ma},
	{"Macedonian","mk",mk},
	{"Mongolia","mn",mn},
	{"Mexico","mx",mx},
	{"Nederland","nl",nl},
	{"Norwegian","no",no},
	{"S�egiella","se",no},
	{"Oman","om",om},
	{"Polish","pl",pl},
	{"Palestinian Territory","ps",ps},
	{"Portuguese","pt",pt},
	{"Qatar","qa",qa},
	{"Romanian","ro",ro},
	{"Russian","ru",ru},
	{"Russia - Winkeys", "ru", ru},
	{"Saudi Arabia","sa",sa},
	{"Sudan","sd",sd},
	{"Swedish","se",se},
	{"Slovene","si",si},
	{"Slovak","sk",sk},
	{"Somalia","so",so},
	{"Serbian Cyrillic","sr",sr},
	{"Serbian","sr",sr},
	{"Syria","sy",sy},
	{"Thai (Kedmanee)","th",th},
	{"Thai (Pattachote)","th",th},
	{"Thai (TIS-820.2538)","th",th},
	{"Tunisia","tn",tn},
	{"Turkish","tr",tr},
	{"Ukrainian","ua",ua},
	{"Maltese (US layout)","mt",us},
	{"US/ASCII","us",us},
	{"en_US","us",us},
	{"U.S. English", "us", us},
	{"English","us",us},
	{"Uruguay","uy",uy},
	{"Vietnamese","vn",vn},
	{"Yemen","ye",ye},
	{"Yugoslavia","yu",yu},
	{"fr-latin9","in9",zz},
	{"TSCII","tml",zz},
	{"Telugu","tel",zz},
	{"Syriac","syr",zz},
	{"Oriya","ori",zz},
	{"Ogham","ham",zz},
	{"Lao","lo",zz},
	{"Inuktitut","iu",zz},
	{"Dvorak","dvo",zz},
	{"Gurmukhi","gur",zz},
	{"Gujarati","guj",zz},
	{"Bengali","ben",zz},
	{"Arabic","ar",zz},
	{"Kannada","kan",zz},
	{"ISO9995-3","zz",zz},
	{"NULL","zz",zz},
};

/* This is an internally used function to create pixmaps. */
GdkPixbuf *create_pixbuf_flag(gchar *langs, int width, int height)
{
	gint i;
	GdkPixbuf *pixbuf;
	/*gchar *lang;
	lang=g_strndup(&langs[0],2);*/
//	printf("Lang:%s\n",langs);
		for (i=0;i<(sizeof(tfp)/sizeof(Tflagpix));i++) {
			if(g_strcasecmp (tfp[i].name,langs)==0) {
				break;
			}
		}
		if (i >= sizeof(tfp)/sizeof(Tflagpix)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tfp)/sizeof(Tflagpix)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tfp[i].data,FALSE,NULL);
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
 
GtkWidget *create_pixmap_flag(gchar *langs, int width, int height) 
{
	GtkWidget *wid;
	gint i;
	GdkPixbuf *pixbuf;
	/*gchar *lang;
	lang=g_strndup(&langs[0],2);
	printf("Lang:%s\n",lang);*/
		for (i=0;i<(sizeof(tfp)/sizeof(Tflagpix));i++) {
			if(g_strcasecmp (tfp[i].name,langs)==0) {
				break;
			}
		}
		if (i >= sizeof(tfp)/sizeof(Tflagpix)) {
			DEBUG_MSG(_("new_pixmap, requested unknown type %d\n"), type);
			i = sizeof(tfp)/sizeof(Tflagpix)-1;
		}
		pixbuf = gdk_pixbuf_new_from_inline(-1,tfp[i].data,FALSE,NULL);
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


gboolean timer_hide_win(gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	
	//printf("timer_hide_win\n");
	if(aconv->sxkb->timeId>0)
		gtk_widget_hide_all (aconv->sxkb->flag_win);
  
	return FALSE;
} 

void greate_win_flag(xkb_info *k,gpointer data)
{
	//int timer;
	AConvert *aconv = (AConvert *)data;
	int revert_to;
	Window focuswin;
	XWindowAttributes win_attributes;
	Window junkwin;
	int rx, ry, pos_x,pos_y=0;
	
	if(aconv->sxkb->view_flag==0)
		return;

	XGetInputFocus(GDK_DISPLAY(), &focuswin, &revert_to);
	if (focuswin) 
	{
		XGetWindowAttributes(GDK_DISPLAY(), focuswin, &win_attributes);

		XTranslateCoordinates (GDK_DISPLAY(), focuswin, win_attributes.root, 
				-win_attributes.border_width,-win_attributes.border_width,
				&rx, &ry, &junkwin);
		
		pos_x=rx;//((win_attributes.width/2)+rx);
		pos_y=ry;//((win_attributes.height/2)+ry);
	}
	else
	{
		pos_x=0;//200;
		pos_y=0;//200;
	}

	if(!aconv->sxkb->flag_win)
	{
		aconv->sxkb->flag_win = gtk_window_new (GTK_WINDOW_POPUP);
		gtk_widget_set_size_request (GTK_WIDGET(aconv->sxkb->flag_win), 32, 24);
		gtk_widget_set_uposition(GTK_WIDGET(aconv->sxkb->flag_win),pos_x,pos_y);
		//gtk_window_set_position (GTK_WINDOW (sven->sxkb->flag_win), GTK_WIN_POS_CENTER);
		
		aconv->sxkb->flag_image = gtk_image_new(); 
		gtk_widget_show (aconv->sxkb->flag_image);
		gtk_container_add (GTK_CONTAINER (aconv->sxkb->flag_win), aconv->sxkb->flag_image);
		gtk_widget_show_all (aconv->sxkb->flag_win);
	}
	else
	{
		if (k->flag_img)
		{
			GdkPixbuf *pixbuf=gdk_pixbuf_scale_simple (k->flag_img,32,24,GDK_INTERP_BILINEAR);
			gtk_image_set_from_pixbuf(GTK_IMAGE(aconv->sxkb->flag_image),pixbuf);
			g_object_unref(pixbuf);
		
		}
		else
		{
			GdkPixbuf *pixbuf=create_pixbuf_flag("zz",32,24);
			gtk_image_set_from_pixbuf(GTK_IMAGE(aconv->sxkb->flag_image),pixbuf);
			g_object_unref(pixbuf);
		}
		
		gtk_widget_set_uposition(GTK_WIDGET(aconv->sxkb->flag_win),pos_x,pos_y);
		gtk_widget_show_all (aconv->sxkb->flag_win);
	}
	
	 if (aconv->sxkb->timeId != 0)
            g_source_remove(aconv->sxkb->timeId);
	    
	 aconv->sxkb->timeId = g_timeout_add (800,timer_hide_win, aconv);
}

void update_flag(xkb_info *k,gpointer data)
{
	AConvert *aconv = (AConvert *)data;
	Sven *sven = (Sven *)aconv->sven;
	
	if((aconv->sxkb->view_flag_tray==1)&&(sven->tray_icon!=NULL))
	{
		//gtk_tooltips_set_tip (sven->tray->tooltip,GTK_WIDGET (sven->tray->eggtray),k->name,NULL);
		gtk_status_icon_set_tooltip (sven->tray_icon, k->name);
		if (k->flag_img)
		{
			gtk_status_icon_set_from_pixbuf(sven->tray_icon,k->flag_img);
		}
		else
		{
			GdkPixbuf *pixbuf=create_pixbuf_flag("zz",25,20);
			gtk_status_icon_set_from_pixbuf(sven->tray_icon,pixbuf);
			//gtk_image_set_from_pixbuf(GTK_IMAGE(sven->tray->image),pixbuf);
			g_object_unref(pixbuf);
		}
	}
	if(aconv->sxkb->view_flag==1)
		greate_win_flag(k,aconv);
}

