/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2002  Peter Alm, Mikael Alm, Olle Hallnas,
 *                           Thomas Nilsson and 4Front Technologies
 *  Copyright (C) 1999-2002  Haavard Kvaalen
 *
 *
 * sven multimedia keyboard deamon
 * configfile.h - the prototypes function and structure
 *
 * Copyright (C) 2005 Eugene Morenko(More)
 * mailto: more@irpin.com
 * web: http://sven.linux.kiev.ua/
 * icq:325697438
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <glib.h>

typedef struct
{
	gchar *key;
	gchar *value;
}
ConfigLine;

typedef struct
{
	gchar *name;
	GList *lines;
}
ConfigSection;

typedef struct
{
	GList *sections;
}
ConfigFile;

#ifdef __cplusplus
extern "C" {
#endif

ConfigFile *sven_cfg_new(void);
ConfigFile *sven_cfg_open_file(gchar * filename);
gboolean sven_cfg_write_file(ConfigFile * cfg, gchar * filename);
void sven_cfg_free(ConfigFile * cfg);
gboolean sven_cfg_free_file(gchar * filename);
ConfigFile *sven_cfg_open_default_file(void);
gboolean sven_cfg_write_default_file(ConfigFile * cfg);

gboolean sven_cfg_read_string(ConfigFile * cfg, gchar * section, gchar * key, gchar ** value);
gboolean sven_cfg_read_int(ConfigFile * cfg, gchar * section, gchar * key, gint * value);
gboolean sven_cfg_read_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean * value);
gboolean sven_cfg_read_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat * value);
gboolean sven_cfg_read_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble * value);

void sven_cfg_write_string(ConfigFile * cfg, gchar * section, gchar * key, gchar * value);
void sven_cfg_write_int(ConfigFile * cfg, gchar * section, gchar * key, gint value);
void sven_cfg_write_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean value);
void sven_cfg_write_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat value);
void sven_cfg_write_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble value);

void sven_cfg_remove_key(ConfigFile * cfg, gchar * section, gchar * key);

ConfigSection *sven_cfg_create_section(ConfigFile * cfg, char * name);
ConfigLine *sven_cfg_create_string(ConfigSection * section, char * key, char * value);
ConfigSection *sven_cfg_find_section(ConfigFile * cfg, char * name);
ConfigLine *sven_cfg_find_string(ConfigSection * section, char * key);

#ifdef __cplusplus
};
#endif
