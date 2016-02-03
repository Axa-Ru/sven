
#ifndef __GTK_OSD_H__
#define __GTK_OSD_H__

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The type of data that can be displayed. */
  typedef enum
  {
    GTK_OSD_PERCENTAGE= 0,		/* Percentage bar (like a progress bar) */
    GTK_OSD_SLIDER,			/* Slider (like a volume control) */
    GTK_OSD_TEXT,		/* Text */
    GTK_OSD_INT,		/* Text */
    GTK_OSD_ENTER
  } Gtk_Osd_Command;

/* Position of the display */
typedef enum
{
	GTK_OSD_TOP = 0,		/* Top of the screen. */
	GTK_OSD_BOTTOM,		/* Bottom of the screen. */
	GTK_OSD_MIDDLE		/* middle of the screen. */
} Gtk_Osd_Pos;

/* Alignment of the display */
  typedef enum
  {
    GTK_OSD_LEFT = 0,
    GTK_OSD_CENTER,
    GTK_OSD_RIGHT
  } Gtk_Osd_Align;

typedef struct _GtkOSD       GtkOSD;

struct _GtkOSD
{
	GtkWidget *osd_window;
	GtkWidget *darea;
	PangoLayout *layout;

	gint drop_shadow : 1;
	gint debug_frame : 1;

	PangoRectangle extent;

	int max_width;
	int width;
	int height;
	int x;
	int y;
	int x_pos;
	int y_pos;
	int voffset;
	int hoffset;

	int drop_shadow_distance;
	int psize;
	
	gboolean osd_on;
	gchar  *font;
	gchar  *color;
	gchar  *bg_color;
	int bg_color_on ;
	int timeout;
	int bar_length;
	int bar_set;
	int is_slider;
	int bar_on;
//	int osd_enable;
	PangoEllipsizeMode  ellipsize;
	PangoAlignment alignment;
	Gtk_Osd_Pos pos;
	Gtk_Osd_Align align;

	gint timeId;

};

GtkOSD* gtk_osd_new (void);
GtkOSD* gtk_osd_new_from_data (const char *font,const char *color,const char *bg_color,int bg_color_on,gboolean shadow,gboolean frame,int timeout,Gtk_Osd_Pos pos,Gtk_Osd_Align align,int voffset,int hoffset,int osd_on);
gboolean gtk_osd_set_font(GtkOSD *osd,const char *font);
gboolean gtk_osd_set_color (GtkOSD * osd, const char *color);
gboolean gtk_osd_set_bg_color (GtkOSD * osd, const char *color);
gboolean gtk_osd_set_bg_color_on (GtkOSD * osd, gboolean on);
gboolean gtk_osd_pango_set_alignment(GtkOSD *osd,PangoAlignment alignment);
gboolean gtk_osd_set_markup(GtkOSD *osd,const char *text);
gboolean gtk_osd_set_text(GtkOSD *osd,const char *text);
gboolean gtk_osd_set_ellipsize(GtkOSD *osd,PangoEllipsizeMode  ellipsize);
gboolean gtk_osd_set_max_width(GtkOSD *osd,gint max_width);
gboolean gtk_osd_set_drop_shadow(GtkOSD *osd,gboolean data);
gboolean gtk_osd_set_debug_frame(GtkOSD *osd,gboolean data);
gboolean gtk_osd_set_timeout (GtkOSD * osd, int timeout);
gboolean gtk_osd_set_vertical_offset (GtkOSD * osd, int voffset);
gboolean gtk_osd_set_horizontal_offset (GtkOSD * osd, int hoffset);
gboolean gtk_osd_set_pos (GtkOSD * osd, Gtk_Osd_Pos pos);
gboolean gtk_osd_set_align (GtkOSD * osd, Gtk_Osd_Align align);
gboolean gtk_osd_set_bar_length(GtkOSD * osd, int length);
gboolean gtk_osd_set_is_slider(GtkOSD * osd, Gtk_Osd_Command command);
gboolean gtk_osd_set_bar_set(GtkOSD * osd, gint bar);
gboolean gtk_osd_set_bar_on(GtkOSD * osd, gboolean on);
gboolean gtk_osd_set_osd_on(GtkOSD * osd, gboolean on);

gboolean gtk_osd_hide (gpointer data);
gboolean gtk_osd_show (GtkOSD * osd);

gboolean gtk_osd_print (GtkOSD *osd,...);

gboolean  gtk_osd_destroy(GtkOSD *osd);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_OSD_H__ */
