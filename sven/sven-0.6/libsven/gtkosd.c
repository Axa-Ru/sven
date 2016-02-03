#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include "gtkosd.h"
#include <math.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#define DEF_OSD_FONT "Sans 30"
#define DEF_OSD_COLOR  "red"
#define DEF_OSD_BGCOLOR "green"
#define DEF_OSD_TIME 5
#define DEF_OSD_BAR_LENGTH 50

#define SLIDER_SCALE 0.8
#define SLIDER_SCALE_ON 0.7

#define max(a, b)   ((a) >= (b) ? (a) : (b))

int BORDER_WIDTH=1;

//XErrorHandler DefErrHandler;

static void greate_osd_shape_combine_mask(GtkOSD *osd);
static void greate_osd_drawing(GtkOSD *osd);
static void greate_osd_transparent(GtkOSD *osd);
static void update_pos (GtkOSD * osd);
/*
void Xerror_handler(Display * d, XErrorEvent * ev)
{
	char buf[256];

	XGetErrorText(GDK_DISPLAY(), ev->error_code, buf, 256);
	printf( "[GTK_OSD] : X error: %s\n", buf);
}
*/
static gboolean draw_bar (GtkOSD *osd,GdkDrawable  *drawable,GdkGC *fg_gc, int x, int y, int percent, int is_slider,int set_color)
{
	int barw, barh, nbars;
	int on, i, xx;
	struct bar
	{
	int w, h, y;
	} bar[2];

	if(osd->bar_on!=1)
		return FALSE;

	/* bar size and dimension */
	bar[0].h = bar[1].h = barh =osd->extent.height/2;
	bar[0].w = bar[1].w = barw = barh / 2;
	bar[0].y = bar[1].y = y;

	bar[0].h /= 3;
	bar[1].w *= SLIDER_SCALE_ON;
	bar[0].w *= SLIDER_SCALE;
	bar[0].y += bar[0].h;

	   //check how to behave
	if (osd->bar_length <1)
	{
		nbars = (gdk_screen_width() * SLIDER_SCALE) / barw;
	}
	else
	{
			if((barw*osd->bar_length)>gdk_screen_width())
				nbars = (gdk_screen_width() * SLIDER_SCALE) / barw;
			else
				 nbars =osd->bar_length;
	}

	on = ((nbars - is_slider) * percent) / 100;
	
//	printf ("percent=%d, nbars=%d, on=%d", percent, nbars, on);
	
	/* Bar/Slider */
	for (xx = x, i = 0; i < nbars; xx += barw, i++)
	{
		struct bar *b = &(bar[is_slider ? (i == on) : (i < on)]);
		gdk_draw_rectangle (drawable,fg_gc,TRUE,xx, b->y, b->w, b->h);

		if(set_color==1)
		{
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx + BORDER_WIDTH, b->y, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx + BORDER_WIDTH, b->y + BORDER_WIDTH, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx, b->y + BORDER_WIDTH, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx - BORDER_WIDTH, b->y + BORDER_WIDTH, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx - BORDER_WIDTH, b->y, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx - BORDER_WIDTH, b->y - BORDER_WIDTH, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx, b->y - BORDER_WIDTH, b->w, b->h);
			gdk_draw_rectangle (drawable,fg_gc,TRUE,xx + BORDER_WIDTH, b->y - BORDER_WIDTH, b->w, b->h);
	
			if(osd->drop_shadow)
			{
				gdk_draw_rectangle (drawable,fg_gc,TRUE,xx +osd->drop_shadow_distance,b->y + osd->drop_shadow_distance, b->w, b->h);
			}
		}
	}
	
	return TRUE;
}

static void greate_osd_shape_combine_mask(GtkOSD *osd)
{
	GdkPixmap* pixmap=NULL;
	GdkBitmap *bitmap=NULL;
	GdkGC *fg_gc=NULL;
	GdkGC *bg_gc=NULL;
	GdkColormap *palet=NULL;
	GdkColor fg_color;
	GdkColor bg_color;
	
	pixmap=gdk_pixmap_new(osd->darea->window,osd->width,osd->height,-1);

	fg_gc = gdk_gc_new (osd->darea->window);
	bg_gc = gdk_gc_new (osd->darea->window); 

	 palet = gdk_colormap_get_system();

	gdk_gc_copy  (fg_gc,osd->darea->style->fg_gc[GTK_STATE_NORMAL]);
	gdk_color_parse(osd->color, &fg_color);
	gdk_color_alloc(palet, &fg_color);
	gdk_gc_set_foreground(fg_gc,&fg_color);
	gdk_gc_set_foreground(bg_gc,&fg_color);

	
	gdk_gc_copy  (bg_gc,osd->darea->style->fg_gc[GTK_STATE_NORMAL]);
	gdk_color_parse(osd->bg_color, &bg_color);
	gdk_color_alloc(palet, &bg_color);;
	gdk_gc_set_background(bg_gc,&bg_color);
	gdk_gc_set_background(fg_gc,&bg_color);

	gdk_draw_rectangle (osd->darea->window,bg_gc,TRUE,0,0,osd->width,osd->height);
	gdk_draw_layout (osd->darea->window,fg_gc,osd->x,osd->y,osd->layout);
	
	if(osd->debug_frame)
		gdk_draw_rectangle (osd->darea->window,fg_gc,False,0, 0,osd->width - 1,osd->height - 1);

//((((osd->height/2)-((osd->extent.height/2)/3))/2)+(osd->height/2))

//printf("%d=%d=%d\n",((((osd->height/2)-((osd->extent.height/2)/3))/2)),(osd->height/2),((((osd->height/2)-((osd->extent.height/2)/3))/2)+(osd->height/2)));

	draw_bar (osd,osd->darea->window,fg_gc, 4,((((osd->height/2)-((osd->extent.height/2)/3))/4)+(osd->height/2)), osd->bar_set,osd->is_slider ,0);
	
	bitmap=gdk_pixmap_new(osd->darea->window,osd->width,osd->height,1);

	gdk_gc_unref( fg_gc );
	gdk_gc_unref( bg_gc );

	fg_gc = gdk_gc_new (bitmap); 
	bg_gc = gdk_gc_new (bitmap); 

	fg_color.pixel=-1;
	gdk_gc_set_foreground(fg_gc,(&fg_color));
	
	bg_color.pixel=0;
	gdk_gc_set_background(bg_gc,(&bg_color));

	gdk_draw_rectangle (bitmap,bg_gc,TRUE,0,0,osd->width,osd->height);

	draw_bar (osd,bitmap,fg_gc, 4,((((osd->height/2)-((osd->extent.height/2)/3))/4)+(osd->height/2)),osd->bar_set,osd->is_slider ,1);

	gdk_draw_layout (bitmap,fg_gc,osd->x,osd->y,osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x + BORDER_WIDTH,osd->y,osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x + BORDER_WIDTH, osd->y + BORDER_WIDTH, osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x, osd->y + BORDER_WIDTH,osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x - BORDER_WIDTH, osd->y + BORDER_WIDTH, osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x - BORDER_WIDTH, osd->y, osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x - BORDER_WIDTH, osd->y - BORDER_WIDTH,osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x, osd->y - BORDER_WIDTH, osd->layout);
	gdk_draw_layout (bitmap,fg_gc,osd->x + BORDER_WIDTH, osd->y - BORDER_WIDTH, osd->layout);

	if(osd->drop_shadow)
		gdk_draw_layout (bitmap,fg_gc,osd->x + osd->drop_shadow_distance,osd->y + osd->drop_shadow_distance,osd->layout);

	if(osd->debug_frame)
		gdk_draw_rectangle (bitmap,fg_gc,False,0, 0,osd->width - 1,osd->height - 1);


	gdk_window_set_back_pixmap (osd->darea->window, pixmap, FALSE);
//	gdk_window_shape_combine_mask(osd_window->window, bitmap,0,0);
	gtk_widget_shape_combine_mask (GTK_WIDGET(osd->osd_window), bitmap,0,0);
	
	gdk_gc_unref( fg_gc );
	gdk_gc_unref( bg_gc );	
	g_object_unref(pixmap);
	g_object_unref(bitmap);
}

static void greate_osd_drawing(GtkOSD *osd)
{
	GdkGC *fg_gc=NULL;
	GdkColor fg_color;
	GdkColor bg_color;

//	gdk_window_merge_child_shapes (osd->osd_window->window);

	fg_gc = gdk_gc_new (osd->darea->window);
	gdk_gc_copy  (fg_gc,osd->darea->style->fg_gc[GTK_STATE_NORMAL]);
	gdk_color_parse(osd->color, &fg_color);
	gdk_color_alloc(gdk_colormap_get_system(), &fg_color);
	gdk_gc_set_foreground(fg_gc,&fg_color);

	gdk_color_parse(osd->bg_color, &bg_color);
	gdk_color_alloc(gdk_colormap_get_system(), &bg_color);;
	gdk_gc_set_background(fg_gc,&bg_color);

	gdk_draw_layout (osd->darea->window,fg_gc,osd->x,osd->y,osd->layout);
	
	if(osd->debug_frame)
		gdk_draw_rectangle (osd->darea->window,fg_gc,False,0, 0, osd->width - 1, osd->height - 1);

	draw_bar (osd,osd->darea->window,fg_gc, 4,((((osd->height/2)-((osd->extent.height/2)/3))/4)+(osd->height/2)), osd->bar_set,osd->is_slider ,0);
	

	gdk_gc_unref( fg_gc );	
}

static void greate_osd_transparent(GtkOSD *osd)
{
	GdkGC *fg_gc=NULL;
	GdkColor fg_color;
	GdkColor bg_color;
	
	GdkPixbuf *dest;
	GdkPixbuf *pixbuf;

	gint root_x=0;
        gint root_y=0;

	//gdk_window_merge_child_shapes (osd->osd_window->window);

	fg_gc = gdk_gc_new (osd->darea->window);
	gdk_gc_copy  (fg_gc,osd->darea->style->fg_gc[GTK_STATE_NORMAL]);
	gdk_color_parse(osd->color, &fg_color);
	gdk_color_alloc(gdk_colormap_get_system(), &fg_color);
	gdk_gc_set_foreground(fg_gc,&fg_color);

	gdk_color_parse(osd->bg_color, &bg_color);
	gdk_color_alloc(gdk_colormap_get_system(), &bg_color);;
	gdk_gc_set_background(fg_gc,&bg_color);

	gtk_window_get_position (GTK_WINDOW(osd->osd_window),&root_x, &root_y);
	//printf("RX:%d; RY:%d\n",root_x,root_y);
	
	pixbuf = gdk_pixbuf_get_from_drawable (NULL, gdk_get_default_root_window (), NULL, 0, 0, 0, 0, osd->width,osd->height);

	dest = gdk_pixbuf_composite_color_simple (pixbuf,osd->width,osd->height,GDK_INTERP_BILINEAR,150,16, 0xb01b07, 0xb01b07);

	gdk_draw_pixbuf (osd->darea->window,fg_gc,dest,0,0,0,0,osd->width,osd->height,GDK_RGB_DITHER_MAX,0,0);
	g_object_unref (dest);
	g_object_unref (pixbuf);
	
	gdk_draw_layout (osd->darea->window,fg_gc,osd->x, osd->y,osd->layout);
	
	if(osd->debug_frame)
		gdk_draw_rectangle (osd->darea->window,fg_gc,False,0, 0, osd->width - 1, osd->height - 1);

	draw_bar (osd,osd->darea->window,fg_gc, 4,((((osd->height/2)-((osd->extent.height/2)/3))/4)+(osd->height/2)), osd->bar_set,osd->is_slider ,0);

	gdk_gc_unref( fg_gc );

}

static gint osd_expose (GtkWidget *widget, GdkEventExpose *event,GtkOSD *osd)
{
	if(osd->osd_on==0)
	{
		gtk_widget_hide(GTK_WIDGET(osd->osd_window));
		return FALSE;
	}

	gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

	//printf("Expose\n");

	if(osd->layout==NULL)
	{
		osd->layout = pango_layout_new (gdk_pango_context_get ());
		pango_layout_set_justify(osd->layout,False);
		
		gtk_osd_set_font(osd,osd->font);
		gtk_osd_pango_set_alignment(osd,osd->alignment);
		gtk_osd_set_ellipsize(osd,osd->ellipsize);
		gtk_osd_set_max_width(osd,0);
		gtk_osd_set_pos(osd,osd->pos);
		gtk_osd_set_align (osd, osd->align);
	}

	 gtk_osd_set_max_width(osd,0);

	if(osd->bg_color_on==1)
		greate_osd_drawing(osd);
	else if(osd->bg_color_on==2)
		 greate_osd_transparent(osd);
	else
		greate_osd_shape_combine_mask(osd);
		

	return TRUE;
}


gboolean gtk_osd_set_font(GtkOSD *osd,const char *font)
{
	if((osd==NULL)||(font==NULL))
		return FALSE;

	PangoFontDescription *desc = pango_font_description_from_string(font);
	pango_layout_set_font_description (osd->layout, desc);
	osd->psize=pango_font_description_get_size(desc);
	pango_font_description_free (desc);

	if(g_ascii_strcasecmp(osd->font,font))
	{	
		if(osd->font)
			g_free(osd->font);
	
		osd->font=g_strdup(font);
	}

	gtk_widget_queue_draw (osd->darea);

	 return TRUE;
}

gboolean gtk_osd_set_color (GtkOSD * osd, const char *color)
{
	if ((osd == NULL)||(color == NULL))
		return FALSE;

	if(osd->color)
	{
		g_free(osd->color);
		osd->color=NULL;
	}

	osd->color=g_strdup(color);

	gtk_widget_queue_draw (osd->darea);	

	return TRUE;
}

gboolean gtk_osd_set_bg_color (GtkOSD * osd, const char *color)
{
 	if ((osd == NULL)||(color == NULL))
		return FALSE;

	if(osd->bg_color)
	{
		g_free(osd->bg_color);
		osd->bg_color=NULL;
	}

	osd->bg_color=g_strdup(color);

	if(osd->darea)
	{
		GdkColor color;

		gdk_color_parse(osd->bg_color,&color);
		gtk_widget_modify_bg (osd->darea, GTK_STATE_NORMAL,&color);
	}

	//gtk_widget_queue_draw (osd->darea);		

	return TRUE;
}

gboolean gtk_osd_set_bg_color_on (GtkOSD * osd,gboolean on)
{
 	if (osd == NULL)
		return FALSE;

	osd->bg_color_on=on;

	gtk_widget_queue_draw (osd->darea);	

	return TRUE;
}

gboolean gtk_osd_pango_set_alignment(GtkOSD *osd,PangoAlignment alignment)
{
	if((osd==NULL))
		return FALSE;

	pango_layout_set_alignment(osd->layout,alignment);
	osd->alignment = alignment;

	return TRUE;
}

gboolean gtk_osd_set_markup(GtkOSD *osd,const char *text)
{
	if((osd==NULL)||(text==NULL))
		return FALSE;

	pango_layout_set_markup (osd->layout,text, -1);	

/*	if(osd->text)
		g_free(osd->text);

	osd->text=g_strdup(text);*/

	gtk_widget_queue_draw (osd->darea);

	return TRUE;
}

gboolean gtk_osd_set_text(GtkOSD *osd,const char *text)
{
	if((osd==NULL)||(text==NULL))
		return FALSE;

	pango_layout_set_text (osd->layout,text, -1);	

	/*if(osd->text)
		g_free(osd->text);

	osd->text=g_strdup(text);*/

	gtk_widget_queue_draw (osd->darea);

	return TRUE;
}

gboolean gtk_osd_set_ellipsize(GtkOSD *osd,PangoEllipsizeMode  ellipsize)
{
	if(osd==NULL)
		return FALSE;

	pango_layout_set_ellipsize(osd->layout,ellipsize);

	if(ellipsize!=PANGO_ELLIPSIZE_NONE)
			pango_layout_set_wrap(osd->layout,PANGO_WRAP_WORD);

	osd->ellipsize=ellipsize;
	
	return TRUE;
}

gboolean gtk_osd_set_max_width(GtkOSD *osd,gint max_width)
{
	int MAX_WIDTH=0;

	if(osd==NULL)
		return FALSE;

	if(max_width==0)
	{
		MAX_WIDTH=(gdk_screen_width()-8);
	}
	else
	{
		MAX_WIDTH=(max_width-8);
	}

	pango_layout_set_width(osd->layout,PANGO_SCALE*MAX_WIDTH);

	pango_layout_get_pixel_extents (osd->layout, NULL, &osd->extent);
	pango_layout_get_pixel_size (osd->layout, &osd->width, &osd->height);

	osd->x =  BORDER_WIDTH*2;
	osd->y = BORDER_WIDTH*2;
	
	if(osd->alignment==PANGO_ALIGN_CENTER)
		osd->x -= MAX_WIDTH/2 - osd->width/2;
	else if(osd->alignment==PANGO_ALIGN_RIGHT)
		osd->x -= MAX_WIDTH - osd->width;

/*if(osd->align==GTK_OSD_CENTER)
		osd->x -= MAX_WIDTH/2+ osd->width/2;	*/

//		printf("%d-%d=%d\n",osd->width,osd->height,osd->x);
	if(osd->bar_on)
	{

		
		int nbars=0;

		if (osd->bar_length <1)
			nbars = (gdk_screen_width() * SLIDER_SCALE_ON) / (osd->extent.height / 4);
		else
		{
			if(((osd->extent.height / 2)*osd->bar_length)>gdk_screen_width())
				nbars = (gdk_screen_width() * SLIDER_SCALE) / ((osd->extent.height / 1.5)/2);
			else
				 nbars =osd->bar_length;
		}
//printf("OSD_W=H:%d=%d=%d\n",osd->width,osd->height,nbars);
//printf("OSD_W=H2:%d=%d\n",osd->extent.width,osd->extent.height);

		osd->width  =((osd->extent.height /2.8)*nbars);
		osd->height = osd->height*1.8;
		
//		printf("OSD_W=H1:%d=%d\n",osd->width,osd->height);

	}

		osd->width += BORDER_WIDTH*4;
		osd->height += BORDER_WIDTH*4;

		if(osd->drop_shadow)
		{
			osd->drop_shadow_distance = max(2,(osd->psize/PANGO_SCALE*0.1));
			osd->width += osd->drop_shadow_distance;
			osd->height += osd->drop_shadow_distance;
		}

	gtk_widget_set_size_request (GTK_WIDGET(osd->osd_window),osd->width,osd->height);
	update_pos (osd);
//	gtk_widget_set_size_request(GTK_WIDGET(osd->darea),osd->width,osd->height);
	gtk_widget_realize(GTK_WIDGET(osd->darea));

	osd->max_width=max_width;

	return TRUE;
}

gboolean gtk_osd_set_drop_shadow(GtkOSD *osd,gboolean data)
{
	if(osd==NULL)
		return FALSE;

	osd->drop_shadow=data;

	return TRUE;
}

gboolean gtk_osd_set_debug_frame(GtkOSD *osd,gboolean data)
{
	if(osd==NULL)
		return FALSE;

	osd->debug_frame=data;

	return TRUE;
}


gboolean gtk_osd_set_timeout (GtkOSD * osd, int timeout)
{
	if (osd == NULL)
		return  FALSE;

	osd->timeout = timeout;

	return TRUE;
}

gboolean gtk_osd_hide (gpointer data)
{
	GtkOSD *osd = (GtkOSD *)data;
	
	if (osd == NULL)
		return FALSE;

	if (osd->osd_window)
	{
		gtk_widget_hide(GTK_WIDGET(osd->osd_window));
		return TRUE;
	}
	return FALSE;
}

gboolean gtk_osd_show (GtkOSD * osd)
{
	if (osd == NULL)
		return FALSE;

	if (osd->osd_window)
	{
		gtk_widget_show(GTK_WIDGET(osd->osd_window));
		
		if (osd->timeId != 0)
			g_source_remove(osd->timeId);
			osd->timeId = g_timeout_add ((osd->timeout*1000),gtk_osd_hide, osd);
		return TRUE;
	}
	
	return FALSE;
}


static void update_pos (GtkOSD * osd)
{
	if((osd==NULL)||(osd->osd_window==NULL))
		return;
		
	switch (osd->pos)
	{
		case GTK_OSD_BOTTOM:
			osd->y_pos =(gdk_screen_height() - osd->height) - osd->voffset-5;
		break;
		case GTK_OSD_MIDDLE:
			osd->y_pos = ((gdk_screen_height() /2) - osd->height) - osd->voffset;
		break;
		case GTK_OSD_TOP:
		default:
			osd->y_pos = osd->voffset;
		break;
	}

	switch (osd->align)
	{
		case GTK_OSD_LEFT:
			osd->x_pos =  osd->hoffset;
		break;
		case GTK_OSD_CENTER:
			osd->x_pos = (((gdk_screen_width() / 2) - osd->hoffset)-(osd->width/2));
		break;
		case GTK_OSD_RIGHT:
			osd->x_pos = (gdk_screen_width() - osd->width) - osd->hoffset;
		break;
		default:
		osd->x_pos = 0;
		break;
	}
		if(osd->x_pos<0)
			osd->x_pos=gdk_screen_height() - osd->width - osd->hoffset;

		gtk_window_move (GTK_WINDOW(osd->osd_window), osd->x_pos, osd->y_pos);
		gdk_flush();
}

gboolean gtk_osd_set_vertical_offset (GtkOSD * osd, int voffset)
{
	if (osd == NULL)
		return FALSE;

	osd->voffset = voffset;
	update_pos (osd);

	return TRUE;
}

gboolean gtk_osd_set_horizontal_offset (GtkOSD * osd, int hoffset)
{
	if (osd == NULL)
		return FALSE;
	osd->hoffset = hoffset;
	update_pos (osd);

	return TRUE;
}

gboolean gtk_osd_set_pos (GtkOSD * osd, Gtk_Osd_Pos pos)
{
	if (osd == NULL)
		return  FALSE;

	osd->pos = pos;
	update_pos (osd);

	return TRUE;
}

gboolean gtk_osd_set_align (GtkOSD * osd, Gtk_Osd_Align align)
{
	if (osd == NULL)
		return FALSE ;

	osd->align = align;
	update_pos (osd);

	return TRUE;
}

gboolean gtk_osd_set_bar_length(GtkOSD * osd, int length)
{
	if (osd == NULL)
		return FALSE;

	if (length < 1)
		return FALSE;

	osd->bar_length = length;

	gtk_widget_queue_draw (osd->darea);	

	return TRUE;
}

gboolean gtk_osd_set_is_slider(GtkOSD * osd, Gtk_Osd_Command command)
{
	if (osd == NULL)
		return FALSE;

	osd->is_slider = command;
	osd->bar_on=1;
	gtk_widget_queue_draw (osd->darea);
	return TRUE;
}

gboolean gtk_osd_set_bar_set(GtkOSD * osd, gint bar)
{
	if (osd == NULL)
		return FALSE;

	osd->bar_set = (bar < 0) ? 0 : (bar > 100) ? 100 : bar;
	osd->bar_on=1;
	gtk_widget_queue_draw (osd->darea);

	return TRUE;
}

gboolean gtk_osd_set_bar_on(GtkOSD * osd, gboolean on)
{
	if (osd == NULL)
		return FALSE;

	osd->bar_on=on;
	gtk_widget_queue_draw (osd->darea);
	return TRUE;
}

gboolean gtk_osd_set_osd_on(GtkOSD * osd, gboolean on)
{
	if (osd == NULL)
		return FALSE;

	osd->osd_on=on;
	if(on)
		gtk_osd_show(osd);
	else
		gtk_osd_hide(osd);
	return TRUE;
}

void gtk_osd_print_valist (GtkOSD *osd,va_list	 var_args)
{
	
	gint column;
	gchar *temp;
	gchar *string=NULL;
	gchar *string1=NULL;
	gint is_slider,percent,on=0;
	gint in=0;
  
	if(osd==NULL)
		return;

	column = va_arg (var_args, gint);

	while (column != -1)
	{
		switch (column)
		{
			case GTK_OSD_TEXT:
				temp = va_arg (var_args, gchar *);
				if(temp)
				{
					if(string==NULL)
						string= g_strdup_printf("%s",temp);
					else
					{
						string1=string;
						string= g_strdup_printf("%s%s",string1,temp);
						g_free(string1);
					}
				}
			break;
			case GTK_OSD_INT:
				in = va_arg (var_args, gint);
				if(string==NULL)
					string= g_strdup_printf("%d",in);
				else
				{
					string1=string;
					string= g_strdup_printf("%s%d",string1,in);
					g_free(string1);
				}
			break;
			case GTK_OSD_ENTER:
				//temp = va_arg (var_args, char *);
				if(string==NULL)
					string= g_strdup_printf("\n");
				else
				{
					string1=string;
					string= g_strdup_printf("%s\n",string);
					g_free(string1);
				}
			break;
			case GTK_OSD_SLIDER:
				percent = va_arg (var_args, gint);
				is_slider=GTK_OSD_SLIDER;
				on=1;
			break;
			case GTK_OSD_PERCENTAGE:
				percent = va_arg (var_args, gint);
				is_slider=GTK_OSD_PERCENTAGE;
				on=1;
			break;
		}
		column = va_arg (var_args, gint);
	}

	if(string)
		gtk_osd_set_markup(osd,string);

//	printf("BAR:%d\n",on);

	if(on==1)
	{
		gtk_osd_set_is_slider(osd, is_slider);
		gtk_osd_set_bar_set(osd,percent);
	}
	else
		gtk_osd_set_bar_on(osd,0);

	gtk_osd_show(osd);

	g_free(string);
}


gboolean gtk_osd_print (GtkOSD *osd,...)
{
	va_list var_args;

 	if(osd==NULL)
		return FALSE;

	va_start (var_args, osd);
	gtk_osd_print_valist (osd,var_args);
	va_end (var_args);

	return TRUE;
}

GtkOSD* gtk_osd_new (void)
{
	GtkOSD *osd;
	GdkColor color;

	//DefErrHandler = XSetErrorHandler((XErrorHandler) Xerror_handler);

	osd =g_slice_new0 (GtkOSD);

	osd->font=g_strdup(DEF_OSD_FONT);
	osd->color=g_strdup(DEF_OSD_COLOR);
	osd->bg_color=g_strdup(DEF_OSD_BGCOLOR);
	osd->timeout=DEF_OSD_TIME;
	osd->bar_length=DEF_OSD_BAR_LENGTH;
	osd->is_slider=GTK_OSD_PERCENTAGE;
	//osd->bar_on=1;
	osd->drop_shadow=TRUE;
	osd->debug_frame=TRUE;
	//osd->bg_color_on=1;
	osd->osd_on=1;

	osd->osd_window = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_widget_set_events(osd->osd_window,GDK_POINTER_MOTION_MASK);

	osd->darea=gtk_drawing_area_new();
	gdk_color_parse(osd->bg_color,&color);
	gtk_widget_modify_bg (osd->darea, GTK_STATE_NORMAL,&color);
	gtk_widget_set_size_request (osd->darea, 0, 0);	
	gtk_container_add(GTK_CONTAINER(osd->osd_window),osd->darea);
	
	if(osd->layout==NULL)
	{
		osd->layout = pango_layout_new (gdk_pango_context_get ());
		pango_layout_set_justify(osd->layout,False);
		
		gtk_osd_set_font(osd,osd->font);
		gtk_osd_pango_set_alignment(osd,PANGO_ALIGN_CENTER);
		gtk_osd_set_ellipsize(osd,PANGO_ELLIPSIZE_END);
		gtk_osd_set_max_width(osd,0);
		gtk_osd_set_pos(osd,GTK_OSD_MIDDLE);
		gtk_osd_set_align (osd, GTK_OSD_LEFT);
	}
	
	g_signal_connect (G_OBJECT (osd->darea), "expose_event",(GCallback) osd_expose,osd);
	gtk_widget_show((osd->darea));

	return osd;
}

GtkOSD* gtk_osd_new_from_data (const char *font,const char *color,const char *bg_color,int bg_color_on,gboolean shadow,gboolean frame,int timeout,Gtk_Osd_Pos pos,Gtk_Osd_Align align,int voffset,int hoffset,int osd_on)
{
	GtkOSD *osd;
	GdkColor bcolor;

//	DefErrHandler = XSetErrorHandler((XErrorHandler) Xerror_handler);

	osd =g_slice_new0 (GtkOSD);

	if(font)
		osd->font=g_strdup(font);
	else
		osd->font=g_strdup(DEF_OSD_FONT);

	if(color)
		osd->color=g_strdup(color);
	else
		osd->color=g_strdup(DEF_OSD_COLOR);
	
	if(bg_color)
		osd->bg_color=g_strdup(bg_color);
	else
	osd->bg_color=g_strdup(DEF_OSD_BGCOLOR);
	osd->bg_color_on=bg_color_on;
	osd->timeout=timeout;
	osd->bar_length=DEF_OSD_BAR_LENGTH;
	osd->is_slider=GTK_OSD_PERCENTAGE;
//	osd->bar_on=1;
	osd->drop_shadow=shadow;
	osd->debug_frame=frame;
	osd->osd_on=osd_on;
	gtk_osd_set_vertical_offset (osd, voffset);
	gtk_osd_set_horizontal_offset (osd,hoffset);

	osd->osd_window = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_widget_set_events(osd->osd_window,GDK_POINTER_MOTION_MASK);

	osd->darea=gtk_drawing_area_new();
	gdk_color_parse(osd->bg_color,&bcolor);
	gtk_widget_modify_bg (osd->darea, GTK_STATE_NORMAL,&bcolor);
	gtk_widget_set_size_request (osd->darea, 0, 0);	
	gtk_container_add(GTK_CONTAINER(osd->osd_window),osd->darea);
	
	if(osd->layout==NULL)
	{
		osd->layout = pango_layout_new (gdk_pango_context_get ());
		pango_layout_set_justify(osd->layout,False);
		
		gtk_osd_set_font(osd,osd->font);
		gtk_osd_pango_set_alignment(osd,PANGO_ALIGN_CENTER);
		gtk_osd_set_ellipsize(osd,PANGO_ELLIPSIZE_END);
		gtk_osd_set_max_width(osd,0);
		gtk_osd_set_pos(osd,pos);
		gtk_osd_set_align (osd, align);
	}
	
	g_signal_connect (G_OBJECT (osd->darea), "expose_event",(GCallback) osd_expose,osd);
	gtk_widget_show((osd->darea));

	return osd;
}

gboolean  gtk_osd_destroy(GtkOSD *osd)
{
	if(osd==NULL)
		return FALSE;

	g_source_remove(osd->timeId);
	gtk_widget_destroy(osd->darea);
	gtk_widget_destroy(osd->osd_window);
	g_object_unref(osd->layout);
	g_free(osd->font);
	g_free(osd->color);
	g_free(osd->bg_color);
	g_slice_free(GtkOSD,osd);

	return TRUE;
}

/*
gboolean  update_window_key_event(gpointer data)
{
	GtkOSD *osd=data;
	printf("FGEuyw\n");
//	gtk_osd_set_font(osd,"Sans 52");
//	gtk_osd_set_text(osd,"Morenko: ");

//	test=0;

//	gtk_osd_set_is_slider(osd,GTK_OSD_SLIDER);

//gtk_osd_set_bar_set(osd,osd->bar_set+2);
//gtk_osd_print (osd,GTK_OSD_TEXT,"Text:",GTK_OSD_INT,count,GTK_OSD_ENTER,GTK_OSD_SLIDER,count,-1);

//	count++;
	if(count++>2)
		count=0;

	gtk_osd_destroy(osd);

//	printf("count:%d\n",count);
	return False;
}

int main( int   argc,  char *argv[] )
{
	GtkOSD *osd;
	
	gtk_init (&argc, &argv);
	
	osd=gtk_osd_new ();	
	gtk_osd_print (osd,GTK_OSD_TEXT,"Text:",GTK_OSD_INT,23,GTK_OSD_ENTER,GTK_OSD_PERCENTAGE,32,-1);
	gtk_osd_set_timeout (osd,100);
	gtk_osd_show(osd);

	g_timeout_add (8000,update_window_key_event, osd);	

	gtk_main ();

	return 0;
}*/

