#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

/*
 * Apply font change and close dialog.
 */
static int
font_dialog_ok (GtkButton * button, gpointer user_data)
{
  GtkWidget *font_dialog = user_data;
  char *tmp_font;

  assert (GTK_IS_FONT_SELECTION_DIALOG (font_dialog));

  tmp_font = gtk_font_selection_dialog_get_font_name
    (GTK_FONT_SELECTION_DIALOG (font_dialog));

  //gtk_entry_set_text (GTK_ENTRY (font_entry), tmp_font);
  g_print("%s\n",tmp_font);

  gtk_widget_destroy (font_dialog);
	 gtk_main_quit();
	exit (1);

  return 0;
}

static void dialog_close_lcb(GtkObject *object,GtkWidget *win) 
{
	gtk_widget_destroy(win);
	gtk_main_quit();
	exit (1);
}
/*
 * Create dialog window for font selection.
 */
void font_dialog_window (void)
{
  GtkWidget *font_dialog;
  GtkWidget *vbox;
  GtkWidget *cancel_button, *apply_button, *ok_button;
  GList *children;

 // g_print("font_dialog_window");
  font_dialog = gtk_font_selection_dialog_new ("Font selection");
  gtk_signal_connect (GTK_OBJECT (font_dialog), "destroy",GTK_SIGNAL_FUNC (dialog_close_lcb), NULL);

  assert (font_dialog);

    children = gtk_container_children (GTK_CONTAINER (font_dialog));

  vbox = GTK_WIDGET (children->data);

  children = gtk_container_children (GTK_CONTAINER (vbox));

  vbox = GTK_WIDGET (children->next->data);
  children = gtk_container_children (GTK_CONTAINER (vbox));
  ok_button = GTK_WIDGET (children->data);

  apply_button = GTK_WIDGET (children->next->data);

  cancel_button = GTK_WIDGET (children->next->next->data);

  gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy),
			     GTK_OBJECT (font_dialog));

  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
		      GTK_SIGNAL_FUNC (font_dialog_ok), font_dialog);
  gtk_signal_connect (GTK_OBJECT (apply_button), "clicked",
		      GTK_SIGNAL_FUNC (font_dialog_ok), font_dialog);


  gtk_widget_show_all (font_dialog);
}

int
main (int argc, char *argv[])
{

  gtk_init (&argc, &argv);

   font_dialog_window();

   gtk_main ();
  return 0;
}
