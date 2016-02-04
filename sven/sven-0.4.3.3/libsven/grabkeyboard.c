#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pcre.h>

#include "sven.h"
#include "key.h"
#include "utils.h"
#include "plugin.h"
#include "grabkeyboard.h"
#include "xlib.h"
#include "wininfo.h"
#include "load_symbol.h"
#include "rudict.h"
#include "endict.h"
#include "clipman.h"

static Display *dpy; int scr;

Window root, MainWin, win, focused, base_mask;
int revert;

Atom wm_del_win;

#define BASE(w)	(w & base_mask)

static WInfo def_info, *info, *tmp_info;

void GetAppWindow(Window win,Window *core);
WInfo* AddWindow(Window win,Sven *sven);
WInfo* win_add(Window w, Sven *sven);
void   win_free(WInfo *winf,Sven *sven);
WInfo* win_find(Window w,Sven *sven);
void descend_tree (Display *disp,Window w, unsigned long type);
gboolean get_key_win(gpointer data);
void dictionary_init(Sven *sven);
void save_init(gpointer data);
int auto_convert_text(Sven *sven,WInfo *info);
int auto_corect_text(Sven *sven,WInfo *info);

gboolean  scan_focus_wind (gpointer data)
{
	Sven *sven = (Sven *)data;

	XGetInputFocus(dpy, &focused, &revert);
	info = win_find(focused,sven);
	if(info)
	{
		descend_tree (info->dpy,focused, KeyPressMask);
		XSelectInput(info->dpy, focused, KeyPressMask);
	}
	else 
	{
		tmp_info = AddWindow(focused,sven);
		if (tmp_info) 
		{
			info = tmp_info; info->cur_group = sven->sxkb->def_group;
			XkbLockGroup(dpy, XkbUseCoreKbd, info->cur_group);
			update_flag(sven->sxkb->group2info[info->cur_group],sven);
			descend_tree (info->dpy,focused, KeyPressMask);
			XSelectInput(info->dpy, focused, KeyPressMask);
		}
	}	
	return TRUE;
}

void GetAppWindow(Window win,Window *core)
{
	Window rwin, parent, *childrens, *child; int n;
	if (!XQueryTree(dpy, win, &rwin, &parent, &childrens, &n)) return;
		child = childrens;
	while(n) 
	{
		if (BASE(*child) != BASE(win)) 
		{
			*core = *child;
			break;
		}
		GetAppWindow(*child, core);
		if (*core) break;
		child++; n--;
	}
	if (childrens) XFree(childrens);
}

/*Descend the client tree and set an event handler everywhere*/
void descend_tree (Display *disp,Window w, unsigned long type)
{
	int ret, i;
	static long int level = 0;
	unsigned int nchildren;
	Window sroot, parent, *children;

	//alarm (0);
	ret = XQueryTree (disp, w, &sroot, &parent, &children, &nchildren);
		
	if (ret == False) 
	{
		fprintf (stdout, "Sven:Query error level:%ld\n", level);
		return;
	}
	if (!nchildren)
		return;
		
	if (w == sroot) 
	{
		level = 0;
		XSelectInput (disp, sroot, type);
	}
	level++;
		
	for (i = 0; i < nchildren; i++) 
	{
		XSelectInput (disp, children[i], type);
		descend_tree (disp,children[i], type);
	}
		
	if (children)
		XFree (children);
}


WInfo* win_add(Window w, Sven *sven)
{
	WInfo *winf;
	winf= g_malloc0(sizeof (WInfo));
	
	
	if (winf) 
	{
		winf->dpy = XOpenDisplay(XDisplayName(NULL));
		winf->win = w;
		winf->text = "";
		winf->cur_group = sven->sxkb->def_group;
		winf->mode=11;
		winf->iterator=0;
		sven->keyboard->list = g_slist_append(sven->keyboard->list,(gpointer ) winf);
		return winf;
	}
	return NULL;
}

WInfo* win_find(Window w,Sven *sven)
{
	GSList *l = NULL;
	
	//printf("Lenght:%d\n",g_slist_length(sven->keyboard->list));
	for (l = sven->keyboard->list; l; l=l->next)
	{
		WInfo *winf = (WInfo *)l->data;
		if (winf->win == w)
			return winf;
	}
	return NULL;
 }
 
void   win_free(WInfo *winf,Sven *sven)
{
	//printf("Window Free\n");
	sven->keyboard->list = g_slist_remove (sven->keyboard->list, (gpointer ) winf);
	g_free (winf);
}




WInfo* AddWindow(Window win,Sven *sven)
{
	int ignore = 0;
	WInfo *info;
	XWindowAttributes attr;
	
	if(!XGetWindowAttributes(dpy, win, &attr)) 
	{
		return (WInfo*) 0;
	}
	
	info = win_find(win,sven);
	if (!info) 
	{
		info = win_add(win,sven);
		if (!info) return (WInfo*) 0;
	}
	
	XSelectInput(dpy, win, FocusChangeMask);
  
	return info;
}

void grab_keyboard_init (int   argc,char *argv[],gpointer data)
{
	Sven *sven = (Sven *)data;
	int  xkbEventType, xkbError, reason_rtrn, mjr, mnr;
	int timer; 
	int dummy;
	Atom protocols[1];
	  
	sven->keyboard = g_new0(Mkeyboard, 1);

	dpy = XkbOpenDisplay( "", &xkbEventType, &xkbError,NULL, NULL, &reason_rtrn);
	if (!dpy) 
	{
		printf("Can't connect to X-server: %s\n", getenv("DISPLAY"));
		switch (reason_rtrn) 
		{
			case XkbOD_BadLibraryVersion :
			case XkbOD_BadServerVersion :
				printf("xxkb was compiled with XKB version 	%d.%02d\n",XkbMajorVersion,XkbMinorVersion);
				printf("But %s uses incompatibl eversion %d.%02d\n",reason_rtrn == XkbOD_BadLibraryVersion ? "Xlib" : "Xserver",mjr,mnr);
			break;
			case XkbOD_ConnectionRefused :
				printf("Connection refused\n");
			break;
			case XkbOD_NonXkbServer:
				printf("XKB extension not present\n");
			break;
			default:
				printf("Unknown error %d from XkbOpenDisplay\n", reason_rtrn);
		}
		exit(1);
	}   
	
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	base_mask = ~(dpy->resource_mask);
	wm_del_win = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

	MainWin = GDK_WINDOW_XID (sven->wininfo->info_win->window);

	*protocols = wm_del_win;
	XSetWMProtocols(dpy, MainWin, protocols, 1);

	XkbSelectEventDetails(dpy, XkbUseCoreKbd, XkbStateNotify,
				XkbAllStateComponentsMask, XkbGroupStateMask);
	XSelectInput(dpy, root, SubstructureNotifyMask);
	
	def_info.win = MainWin;
	def_info.cur_group = sven->sxkb->def_group;
  
	{
		Window rwin, parent, *children, *child, app; int num; 
		
		XQueryTree(dpy, root, &rwin, &parent, &children, &num);
		child = children;
		while (num) 
		{
			app = (Window) 0;
			GetAppWindow(*child, &app);
			if (app) AddWindow(app,sven);
			child++; num--;
		}
		if (children) XFree(children);
		XGetInputFocus(dpy, &focused, &revert);
		info = win_find(focused,sven);
		if (!info) info = &def_info;
	}
	
	g_timeout_add (700,scan_focus_wind, sven);
	
	sven->keyboard->coun_case=0;
	
	load_symbol_init(sven);
	load_symbol_conv_to_ru(sven);
	load_symbol_translete(sven);
	convert_utf_init(sven);
	
	save_init(sven); 
	corect_init(sven);
	dictionary_init(sven);
}

void gslist_print (GSList* list)
{
	 GSList *l = NULL;
	printf("\nLength:%d\n",g_slist_length(list));

	for (l = list; l; l=l->next)
	{
		g_printf ("%s", l->data); 
	    
	}
	printf("\n");
}

void add_keysym_to_list(XEvent * event,WInfo *winf)
{
	Skeysym *sks;
	sks= g_malloc0(sizeof (Skeysym));
	
	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->mod= get_current_modifier(event);
		winf->ks = g_slist_append(winf->ks,(gpointer ) sks);
	}
	//gslist_print(winf->ks);
	return ;
}

void add_keysym_to_list_num(XEvent * event,WInfo *winf,int num)
{
	Skeysym *sks;
	sks= g_malloc0(sizeof (Skeysym));
	
	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->mod= get_current_modifier(event);
		winf->ks = g_slist_insert(winf->ks,(gpointer ) sks,num);
	}
	//gslist_print(winf->ks);
	return ;
}
void add_keysym_to_list_num_insert(XEvent * event,WInfo *winf,int num)
{
	Skeysym *sks;
	sks= g_malloc0(sizeof (Skeysym));
	
	if (sks) 
	{
		sks->key= get_current_keysym(event);
		sks->mod= get_current_modifier(event);
		winf->ks = g_slist_insert(winf->ks,(gpointer ) sks,num);
		winf->ks= g_slist_remove (winf->ks, (gpointer ) g_slist_nth_data(winf->ks,num+1));
	}
	//gslist_print(winf->ks);
	return ;
}

void dell_keysym_to_list(WInfo *winf,int num)
{
	if (winf->ks) 
		winf->ks= g_slist_remove (winf->ks, (gpointer ) g_slist_nth_data(winf->ks,num-1));
	
	//gslist_print(winf->ks);
}

void free_keysym_to_list(WInfo *winf)
{
	if (! winf->ks)
		return;

	GSList *l = NULL ;

	for (l = winf->ks; l; l=l->next)
	{
		if (l->data)
			g_free (l->data);
	}
	g_slist_free (l);
	
	g_slist_free (winf->ks);
	winf->ks = NULL;
	
	//gslist_print(winf->ks);
 }

void dictionary_init(Sven *sven) 
{
	const char *error;
	int erroffset;
	sven->keyboard->russian_regc = pcre_compile(RUDICTIONARY,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!sven->keyboard->russian_regc) 
	{
		printf("dictionary_init_RU, ERROR, %s\n",error);
	}
	sven->keyboard->russian_regc1 = pcre_compile(RUDICTIONARY1,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!sven->keyboard->russian_regc1) 
	{
		printf("dictionary_init_RU, ERROR, %s\n",error);
	}
	
	sven->keyboard->english_regc = pcre_compile(ENDICTIONARY,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!sven->keyboard->english_regc) 
	{
		printf("dictionary_init_EN, ERROR, %s\n",error);
	}
	sven->keyboard->english_regc1 = pcre_compile(ENDICTIONARY1,PCRE_CASELESS, &error,&erroffset,NULL);
	if (!sven->keyboard->english_regc1) 
	{
		printf("dictionary_init_EN, ERROR, %s\n",error);
	}
}

void corect_init(gpointer data) 
{
	gchar *filename;
	Sven *sven = (Sven *)data;
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/en.cor", NULL);
	
	if (!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"en.cor", NULL);
	}
	sven->keyboard->corr_en = sven_cfg_open_file(filename);
	if (!sven->keyboard->corr_en)
			sven->keyboard->corr_en = sven_cfg_new();
	else
	//	printf("corr_en OK Load\n");
			
	filename= g_strconcat(g_get_home_dir(),"/.sven/ru.cor", NULL);
	
	if (!g_file_test(filename, G_FILE_TEST_EXISTS)) 
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"ru.cor", NULL);
	}
	sven->keyboard->corr_ru = sven_cfg_open_file(filename);
	if (!sven->keyboard->corr_ru)
			sven->keyboard->corr_ru = sven_cfg_new();
	else
	//	printf("corr_ru OK Load\n");
			
	g_free(filename);
}

void save_init(gpointer data) 
{
	gchar *filename;
	Sven *sven = (Sven *)data;
	ConfigFile *cfgfile;
	int key=0;
	int mod=0;
	
	sven->keyboard->corr_en_init=1;
	sven->keyboard->corr_ru_init=1;
	sven->keyboard->auto_switch=1;
	sven->keyboard->conv_lang="Pause";
	sven->keyboard->conv_focus="Shift+Pause";
	sven->keyboard->conv_case="Scroll_Lock";
	
	cfgfile = sven_cfg_open_file(g_strconcat(g_get_home_dir(),"/.sven/config", NULL));
	if (!cfgfile)
		cfgfile = sven_cfg_new();
	
	if (cfgfile!= NULL)
	{
		sven_cfg_read_int (cfgfile, "keyboard", "corr_en", &sven->keyboard->corr_en_init);
		sven_cfg_read_int (cfgfile, "keyboard", "corr_ru", &sven->keyboard->corr_ru_init);
		sven_cfg_read_int (cfgfile, "keyboard", "auto_switch", &sven->keyboard->auto_switch);
		sven_cfg_read_string (cfgfile, "keyboard", "conv_lang", &sven->keyboard->conv_lang);
		sven_cfg_read_string (cfgfile, "keyboard", "conv_focus", &sven->keyboard->conv_focus);
		sven_cfg_read_string (cfgfile, "keyboard", "conv_case", &sven->keyboard->conv_case);
		sven_cfg_free (cfgfile);
	}
	
	if (sven->keyboard->conv_lang!= NULL)
	{
		key=StringToModifier_Key(g_strdup(sven->keyboard->conv_lang),&mod);
		grab_key (sven->screen,key,mod);
	}
	if (sven->keyboard->conv_focus!= NULL)
	{
		key=StringToModifier_Key(g_strdup(sven->keyboard->conv_focus),&mod);
		grab_key (sven->screen,key,mod);
	}
	if (sven->keyboard->conv_case!= NULL)
	{
		key=StringToModifier_Key(g_strdup(sven->keyboard->conv_case),&mod);
		grab_key (sven->screen,key,mod);
	}
}
void add_char_to_win(Sven *sven,XEvent * xev,WInfo *info)
{
	size_t slen;
	char ch;

	
	ch=XKeycodeToKeysym (dpy,xev->xkey.keycode,get_current_modifier(xev));

	if (ch == '\n')
		return;
		
	slen = strlen(info->text);
	
	if((ch ==' ') && (info->text[strlen(info->text)-1] !=' '))
	{
		if(slen>0)
			sven->wininfo->words_count++;
		gtk_label_set_markup(GTK_LABEL(sven->wininfo->words),g_strdup_printf ("%d", sven->wininfo->words_count));
	}
	
	if(strlen(info->text)>STRING_SIZE)
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
	}
	
	if(info->text[strlen(info->text)-1] ==' ')
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
	}
	
	if (info->iterator <= 0) 
	{
		if (info->mode == KLB_INSERT_MODE) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen),(g_strdup_printf("%c",ch)),NULL);
				add_keysym_to_list_num(xev,info,g_slist_length(info->ks));
			}
			else
			{
				info->text=g_strconcat(info->text,(g_strdup_printf("%c",ch)),NULL);
				add_keysym_to_list(xev,info);
			}
		}
		else
		{
			info->text=g_strconcat(info->text,(g_strdup_printf("%c",ch)),NULL);
			add_keysym_to_list(xev,info);
		}
	}
	else if (info->iterator > 0) 
	{
		if (info->mode == KLB_INSERT_MODE) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),(g_strdup_printf("%c",ch)),g_strndup(&info->text[(slen-info->iterator)+1],slen),NULL);
				add_keysym_to_list_num_insert(xev,info,g_slist_length(info->ks)-info->iterator);
				if(info->iterator > 0)
					info->iterator--;
				else
					info->iterator=0;
			}
			else
			{
				info->text=g_strconcat(info->text,(g_strdup_printf("%c",ch)),NULL);
				add_keysym_to_list(xev,info);
			}
		}
		else
		{
			info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),(g_strdup_printf("%c",ch)),g_strndup(&info->text[(slen-info->iterator)],slen),NULL);
			add_keysym_to_list_num(xev,info,g_slist_length(info->ks)-info->iterator);
		}
	}
	else 
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
	}
}

gboolean get_key_win(gpointer data)
{
	Sven *sven = (Sven *)data;
	XEvent xev;
	int i=1;
	

	if (XPending(dpy))
	{
		XSync (dpy, False);	/* Alt key repeats forever in XFree without this */
		XNextEvent (dpy, &xev);

		if (xev.type ==  sven->sxkb->xkb_event_type) 
		{
			XkbEvent xkbev = (XkbEvent ) xev;

			if (xkbev.any.xkb_type == XkbStateNotify) 
			{
				sven->sxkb->cur_group = xkbev.state.group;
				{
					XGetInputFocus(dpy, &focused, &revert);
					if((focused == None) || (focused == PointerRoot))
						return 0;
					info = win_find(focused,sven);
					{
						info->cur_group = sven->sxkb->cur_group;
					}
				}
				if (sven->sxkb->cur_group < sven->sxkb->ngroups)
					update_flag(sven->sxkb->group2info[sven->sxkb->cur_group],sven);
			}
			else if (xkbev.any.xkb_type == XkbNewKeyboardNotify) 
			{        
				read_kbd_description(sven);
				sven->sxkb->cur_group = sven->sxkb->def_group;
				update_flag(sven->sxkb->group2info[sven->sxkb->cur_group],sven);
				flag_menu_destroy(sven);
				flag_menu_create(sven); 
			}
			return 1;
		}
		else
		switch (xev.type) 
		{     
			case FocusOut:
				//printf("FocusOut\n");
				XGetInputFocus(dpy, &focused, &revert);
				if((focused == xev.xfocus.window) || (focused == None) || (focused == PointerRoot))
					break;

				info = win_find(focused,sven);
				if(info)
				{
					{
						info->text="";
						free_keysym_to_list(info);
					 	info->iterator=0;
					}
					XkbLockGroup(dpy, XkbUseCoreKbd, info->cur_group);
					update_flag(sven->sxkb->group2info[info->cur_group],sven);
					break;
				}
				
			break;
			case DestroyNotify:
				//printf("DestroyNotify\n");
				if (xev.xdestroywindow.window == root) break;
		
				win = xev.xdestroywindow.window;
				tmp_info= win_find(win,sven);
				if (tmp_info) 
				{
					win_free(tmp_info,sven);
					if (tmp_info == info) XkbLockGroup(dpy, XkbUseCoreKbd, sven->sxkb->def_group);
					break;
				}
			break;
			case ClientMessage:
				//printf("ClientMessage\n");
				win = xev.xclient.window;
				if((win == MainWin) && (xev.xclient.data.l[0] == wm_del_win))
				sven_exit(NULL,sven);
			break;
			default:
				//printf("Unknown event %d\n",xev.type);
			break;
		}
		
	}
	else
	{
		XGetInputFocus(dpy, &focused, &revert);
		
		info = win_find(focused,sven);
		if(info)
		{
			
			if (XPending(info->dpy))
			{
				XSync (info->dpy, False);	/* Alt key repeats forever in XFree without this */
				XNextEvent (info->dpy, &xev);
if(xev.type==KeyPress)
{
	int action=0;
	size_t slen;
	
	if (xev.xkey.send_event == TRUE)
		return ;
	
	if(!info)
	{
		return -1;
	}

	action=get_key_action (&xev);
	slen = strlen(info->text);
	
	switch (action) 
	{
		case KLB_NO_ACTION:
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
		return ;
		case KLB_CLEAR:
			sven->wininfo->char_count++;
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		return ;
		case KLB_ENTER:
			sven->wininfo->char_count++;
				if(slen>0)
					sven->wininfo->words_count++;
		
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->words),g_strdup_printf ("%d", sven->wininfo->words_count));
		return ;
		case KLB_FLUSH:
			sven->wininfo->char_count++;
			add_char_to_win(sven,&xev,info);
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		break;
		case KLB_SWITCH_MODE:
			sven->wininfo->char_count++;	
			if (KLB_INSERT_MODE == info->mode)
				info->mode = KLB_REPLACE_MODE;
			else
				info->mode = KLB_INSERT_MODE;
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		return ;
		case KLB_MOVE_LEFT:
			sven->wininfo->char_count++;
			if ((slen >0) && (info->iterator <= slen))
				info->iterator++;
			else 
			{
				info->text="";
				free_keysym_to_list(info);
				info->iterator=0;
			}
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		return ;
		case KLB_MOVE_RIGHT:
			sven->wininfo->char_count++;
			if (info->iterator <= slen)
				info->iterator--;
			else 
			{
				info->text="";
				free_keysym_to_list(info);
				info->iterator=0;
			}
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
			return;
		case KLB_DEL_SYM_LEFT:
			sven->wininfo->char_count++;
			if (info->iterator <= 0) 
			{
				if(slen>0)
				{
					info->text=g_strconcat(g_strndup(&info->text[0],slen-1),NULL);
					dell_keysym_to_list(info,g_slist_length(info->ks));
				}
			}
			else if (info->iterator > 0) 
			{
				if(slen>0)
					info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator-1),g_strndup(&info->text[(slen-info->iterator)],slen),NULL);
					dell_keysym_to_list(info,g_slist_length(info->ks)-info->iterator);
			}
			else 
			{
				info->text="";
				free_keysym_to_list(info);
				info->iterator=0;
			}
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		break;
		case KLB_DEL_SYM_RIGHT:
			sven->wininfo->char_count++;
			if (info->iterator == 0) 
			{
				info->text="";
				free_keysym_to_list(info);
				info->iterator=0;
			}
			else if (info->iterator > 0) 
			{
				if(slen>0)
					info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),g_strndup(&info->text[(slen-info->iterator)+1],slen),NULL);
				dell_keysym_to_list(info,(g_slist_length(info->ks)-info->iterator)+1);
				info->iterator--;
			}
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		break;
		case KLB_ADD_SYM:
			sven->wininfo->char_count++;
			add_char_to_win(sven,&xev,info);
			gtk_label_set_markup(GTK_LABEL(sven->wininfo->characters),g_strdup_printf ("%d", sven->wininfo->char_count));
		break;
		default:
		break;
	}

	if(auto_corect_text(sven,info)==1)
		return;
		
	auto_convert_text(sven,info);
}

			}
		}
	}

	return TRUE;
}

void xConvert_text_eng (gpointer data)
{
	Sven *sven = (Sven *)data;
	int i=0;
	int mod=0;
	size_t slen;
	XKeyEvent kevent;
	
	XGetInputFocus(dpy, &focused, &revert);
	
	info = win_find(focused,sven);
		if (!info) info = &def_info;
	
	slen = strlen(info->text);

	if((slen <=0) && g_slist_length (info->ks))
		return ;
	
	if(slen!=g_slist_length (info->ks))
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
		return ;
	}
	
	sven->wininfo->chars_edits_count++;
	gtk_label_set_markup(GTK_LABEL(sven->wininfo->chars_edits),g_strdup_printf ("%d", sven->wininfo->chars_edits_count));
	
	for (i = 0; i < sven->sxkb->ngroups; i++) 
	{
		if((g_strcasecmp (sven->sxkb->group2info[i]->name,"English")==0) || (g_strcasecmp (sven->sxkb->group2info[i]->name,"US/ASCII")==0) || (g_strcasecmp (sven->sxkb->group2info[i]->name,"en_US")==0) || (g_strcasecmp (sven->sxkb->group2info[i]->name,"Maltese (US layout)")==0))
		{
			info->cur_group=i;
			break;
		}
	}
		
	mod=(8192*info->cur_group);
	XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
	update_flag(sven->sxkb->group2info[info->cur_group],sven);
	
	if (info->iterator > 0) 
	{
		if(slen >0)
		{
			info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
			for (i =0; i <= slen-info->iterator; i++) 
			{
				dell_keysym_to_list(info,g_slist_length (info->ks));
			}
			info->iterator=0;
		}
	}
	
	slen = strlen(info->text);
	
	delete_char_in_win(info->win,slen);
	
	for (i = 0; i < slen; i++) 
	{
		Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);

		send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod+sks->mod);

	}
}

void xConvert_text_rus (gpointer data)
{
	Sven *sven = (Sven *)data;
	int i=0;
	int mod=0;
	size_t slen;
	XKeyEvent kevent;
	
	XGetInputFocus(dpy, &focused, &revert);
	
	info = win_find(focused,sven);
		if (!info) info = &def_info;
	
	slen = strlen(info->text);

	if((slen <=0) && g_slist_length (info->ks))
		return ;
	
	if(slen!=g_slist_length (info->ks))
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
		return ;
	}
	
	sven->wininfo->chars_edits_count++;
	gtk_label_set_markup(GTK_LABEL(sven->wininfo->chars_edits),g_strdup_printf ("%d", sven->wininfo->chars_edits_count));
	
	for (i = 0; i < sven->sxkb->ngroups; i++) 
	{
		if((g_strcasecmp (sven->sxkb->group2info[i]->name,"Russian")==0) ||  (g_strcasecmp (sven->sxkb->group2info[i]->name,"Russian(phonetic)")==0))
		{
			info->cur_group=i;
			break;
		}
	}
		
	mod=(8192*info->cur_group);
	XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
	update_flag(sven->sxkb->group2info[info->cur_group],sven);
	
	if (info->iterator > 0) 
	{
		if(slen >0)
		{
			info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
			for (i =0; i <= slen-info->iterator; i++) 
			{
				dell_keysym_to_list(info,g_slist_length (info->ks));
			}
			info->iterator=0;
		}
	}
	
	slen = strlen(info->text);
	
	delete_char_in_win(info->win,slen);
	
	for (i = 0; i < slen; i++) 
	{
		Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);

		send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod+sks->mod);
	}
}

void xConvert_text (gpointer data)
{
	Sven *sven = (Sven *)data;
	int i=0;
	int mod=0;
	size_t slen;
	XKeyEvent kevent;
	
	XGetInputFocus(dpy, &focused, &revert);
		
	info = win_find(focused,sven);
	if(!info)
		if (!info) info = &def_info;
	
	{
	
		slen = strlen(info->text);
	
		if((slen <=0) || g_slist_length (info->ks)<=0)
			return ;
		
		if(slen!=g_slist_length (info->ks))
		{
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
			return ;
		}
	
		sven->wininfo->chars_edits_count++;
		gtk_label_set_markup(GTK_LABEL(sven->wininfo->chars_edits),g_strdup_printf ("%d", sven->wininfo->chars_edits_count));
		
		if(info->cur_group==(sven->sxkb->ngroups-1))
			info->cur_group=0;
		else if (info->cur_group < (sven->sxkb->ngroups-1))
			info->cur_group++;
		else
			info->cur_group=0;
			
		mod=(8192*info->cur_group);
		XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
		update_flag(sven->sxkb->group2info[info->cur_group],sven);
	
		if (info->iterator > 0) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
				for (i =0; i <= slen-info->iterator; i++) 
				{
					dell_keysym_to_list(info,g_slist_length (info->ks));
				}
				info->iterator=0;
			}
		}
	
		slen = strlen(info->text);
		
		delete_char_in_win(info->win,slen);
		
		for (i = 0; i < slen; i++) 
		{
			Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);
	
			send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod+sks->mod);
	
		}
	}
}

void xConvert_text_focus (gpointer data)
{
	Sven *sven = (Sven *)data;
	int i=0;
	int mod=0;
	gchar *newtext=NULL;
	gchar *temp=NULL;
	size_t slen;
	XKeyEvent kevent;
	
	temp=get_text_select();
	newtext=conv_utfstr_to_en (sven,temp);
	
	XGetInputFocus(dpy, &focused, &revert);
		
	info = win_find(focused,sven);
	if(!info)
		if (!info) info = &def_info;
	
	if((newtext!=NULL)&&(strlen(newtext)>0))
	{
		sven->wininfo->chars_edits_count++;
		gtk_label_set_markup(GTK_LABEL(sven->wininfo->chars_edits),g_strdup_printf ("%d", sven->wininfo->chars_edits_count));
		

		gchar *lang=sven->sxkb->group2info[info->cur_group]->name;
		
		if(info->cur_group==(sven->sxkb->ngroups-1))
			info->cur_group=0;
		else if (info->cur_group < (sven->sxkb->ngroups-1))
			info->cur_group++;
		else
			info->cur_group=0;
		
		mod=(8192*info->cur_group);
		XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
		update_flag(sven->sxkb->group2info[info->cur_group],sven);

		slen = strlen(newtext);
	
		for (i = 0; i < slen; i++) 
		{
			int modifer=0;
			
			if(g_unichar_isupper(newtext[i]))
				modifer=mod+1;
			else
				modifer=mod;
				
			if(get_modifer_to_char (sven,g_strdup_printf("%c",newtext[i]),lang)==1)
				modifer=mod+1;
			
			if (newtext[i] == '\n')
			{
				send_key_to_win(focused,XKeysymToKeycode(dpy,XK_Return),0);
				//printf("Enter\n");
			}
			else if (newtext[i] == '\t')
			{
				send_key_to_win(focused,XKeysymToKeycode(dpy,XK_Tab),0);
				//printf("Tab\n");
			}
			else
				send_key_to_win(focused,XKeysymToKeycode(dpy,newtext[i]),modifer);
		}
	}
}

void xConvert_text_case (gpointer data)
{
	Sven *sven = (Sven *)data;
	int i=0;
	int mod=0;
	size_t slen;
	XKeyEvent kevent;
	
	slen = strlen(info->text);
	
	if((slen <=0) && (g_slist_length (info->ks)<=0))
		return ;

	XGetInputFocus(dpy, &focused, &revert);
	
	info = win_find(focused,sven);
	if(!info)
		if (!info) info = &def_info;
		
	if(slen!=g_slist_length (info->ks))
	{
		info->text="";
		free_keysym_to_list(info);
		info->iterator=0;
		return ;
	}
	if(sven->keyboard->coun_case > 2)
		   sven->keyboard->coun_case=0;
	
	
	if(sven->keyboard->coun_case==0)
	{
		sven->keyboard->coun_case++;
	
		mod=(8192*info->cur_group);

		if (info->iterator > 0) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
				for (i =0; i <= slen-info->iterator; i++) 
				{
					dell_keysym_to_list(info,g_slist_length (info->ks));
				}
				info->iterator=0;
			}
		}
	
		slen = strlen(info->text);
	
		delete_char_in_win(info->win,slen);
	
		for (i = 0; i < slen; i++) 
		{
			Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);
	
			send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod+1);
	
		}
		return ;
	}
	else if(sven->keyboard->coun_case==1)
	{
		sven->keyboard->coun_case++;
	
		mod=(8192*info->cur_group);

		if (info->iterator > 0) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
				for (i =0; i <= slen-info->iterator; i++) 
				{
					dell_keysym_to_list(info,g_slist_length (info->ks));
				}
				info->iterator=0;
			}
		}
	
		slen = strlen(info->text);
	
		delete_char_in_win(info->win,slen);
	
		{
			Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,0);
			send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod+1);
		}
		
		for (i = 1; i < slen; i++) 
		{
			Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);
			send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod);
	
		}
	}
	else if(sven->keyboard->coun_case==2)
	{
		sven->keyboard->coun_case++;
	
		mod=(8192*info->cur_group);

		if (info->iterator > 0) 
		{
			if(slen >0)
			{
				info->text=g_strconcat(g_strndup(&info->text[0],slen-info->iterator),NULL);
				for (i =0; i <= slen-info->iterator; i++) 
				{
					dell_keysym_to_list(info,g_slist_length (info->ks));
				}
				info->iterator=0;
			}
		}
	
		slen = strlen(info->text);
	
		delete_char_in_win(info->win,slen);
	
		for (i = 0; i < slen; i++) 
		{
			Skeysym *sks = (Skeysym *)g_slist_nth_data(info->ks,i);
	
			send_key_to_win(info->win,XKeysymToKeycode(dpy,sks->key),mod);
	
		}
		return ;
	}
	else
	   sven->keyboard->coun_case=0;
}


int auto_convert_text(Sven *sven,WInfo *info)
{
	int ovector[30],ret=0;

	if(sven->keyboard->auto_switch==0)
		return -1;
	
	/*printf("Text:%s\n",info->text);
	printf("Translete:%s\n",convert_symbol_to_ru(sven,info->text));*/
		
	if((g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Russian")==0) ||  (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Russian(phonetic)")==0)) 
	{
		ret =pcre_exec(sven->keyboard->english_regc,NULL,(sven,info->text), strlen(info->text),0,PCRE_ANCHORED,ovector,30);
		if(ret > 0)
		{
			xConvert_text_eng(sven);		
		}
		else 
		{
			ret =pcre_exec(sven->keyboard->english_regc1,NULL,(sven,info->text),strlen(info->text),0,PCRE_ANCHORED,ovector,30);
			if (ret > 0)
			{
				xConvert_text_eng(sven);		
			}
			else 
			{
				//printf("NOT Found_RUS:%d\n",ret);
			}
		}
	}	
	else if ((g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"English")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"US/ASCII")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"en_US")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Maltese (US layout)")==0))
	{
		ret =pcre_exec(sven->keyboard->russian_regc,NULL,(sven,info->text),strlen(info->text),0,PCRE_ANCHORED,ovector,30);
		if(ret > 0)
			xConvert_text_rus(sven);		
		else 
		{
			ret =pcre_exec(sven->keyboard->russian_regc1,NULL,(sven,info->text),strlen(info->text),0,PCRE_ANCHORED,ovector,30);
			if (ret > 0)
			{
				xConvert_text_rus(sven);		
			}
			else 
			{
				//printf("NOT Found_ENG:%d\n",ret);
			}
		}
	}	
	return 1;
}

int auto_corect_text(Sven *sven,WInfo *info)
{
	ConfigSection *sect;
	ConfigLine *line;
	gchar *newtext="";
	size_t slen;
	int i=0;
	int mod=0;

	
if(((g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Russian")==0) ||  (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Russian(phonetic)")==0)) && (sven->keyboard->corr_ru_init==1))
{
	
	g_return_val_if_fail(sven->keyboard->corr_ru != NULL, FALSE);
				
	if (!(sect = sven_cfg_find_section(sven->keyboard->corr_ru, "orfo")))
		return -1;
	if (!(line = sven_cfg_find_string(sect, locale_to_utf8(convert_symbol_to_ru(sven,info->text)))))
		return -1;
	
	newtext=locale_from_utf8(g_strdup(line->value));
	
	if(newtext!=NULL)
	{
		slen = strlen(info->text);
		delete_char_in_win(info->win,slen);
			
		slen = strlen(newtext);
		mod=(8192*info->cur_group);
		XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
		update_flag(sven->sxkb->group2info[info->cur_group],sven);
		
		for (i = 0; i < slen; i++) 
		{
			send_key_to_win(info->win,XKeysymToKeycode(dpy,get_load_symbol(sven,newtext[i])),mod);
			
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
		}
	}
	return 1;
}
else if (((g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"English")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"US/ASCII")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"en_US")==0) || (g_strcasecmp (sven->sxkb->group2info[info->cur_group]->name,"Maltese (US layout)")==0)) && (sven->keyboard->corr_en_init==1))
{
		g_return_val_if_fail(sven->keyboard->corr_en != NULL, FALSE);
				
	if (!(sect = sven_cfg_find_section(sven->keyboard->corr_en, "orfo")))
		return -1;
	if (!(line = sven_cfg_find_string(sect, locale_to_utf8(info->text))))
		return -1;
	
	newtext=locale_from_utf8(g_strdup(line->value));
	
	if(newtext!=NULL)
	{
		slen = strlen(info->text);
		delete_char_in_win(info->win,slen);
	
	
		slen = strlen(newtext);
		mod=(8192*info->cur_group);
		XkbLockGroup(dpy, XkbUseCoreKbd,info->cur_group);
		update_flag(sven->sxkb->group2info[info->cur_group],sven);
		
		for (i = 0; i < slen; i++) 
		{
			send_key_to_win(info->win,XKeysymToKeycode(dpy,get_load_symbol(sven,newtext[i])),mod);
			
			info->text="";
			free_keysym_to_list(info);
			info->iterator=0;
		}
	}
	return 1;
}
	return 0;
}

