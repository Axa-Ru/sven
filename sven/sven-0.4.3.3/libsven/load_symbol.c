/* Sven multimedia keyboard deamon
 * load_symbol.c
 *
 *Copyright (C) 2005 Eugene Morenko(More)
 *mailto: more@irpin.com
 *web: http://sven.linux.kiev.ua/
 *icq:325697438
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
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
#include <unistd.h>

#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sven.h"
#include "load_symbol.h"
#include "grabkeyboard.h"

void load_symbol_init(gpointer data)
{
	Sven *sven = (Sven *)data;

	sven->keyboard->htsym = g_hash_table_new (g_str_hash, g_str_equal);
	
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_GHE_bar)),(gpointer) XK_Cyrillic_GHE_bar);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ZHE_descender)),(gpointer)XK_Cyrillic_ZHE_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_zhe_descender)),(gpointer)XK_Cyrillic_zhe_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_KA_descender)),(gpointer)XK_Cyrillic_KA_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ka_descender)),(gpointer)XK_Cyrillic_ka_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_KA_vertstroke)),(gpointer)XK_Cyrillic_KA_vertstroke);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ka_vertstroke)),(gpointer)XK_Cyrillic_ka_vertstroke);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_EN_descender)),(gpointer)XK_Cyrillic_EN_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_en_descender)),(gpointer)XK_Cyrillic_en_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_U_straight)),(gpointer)XK_Cyrillic_U_straight);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_u_straight)),(gpointer)XK_Cyrillic_u_straight);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_U_straight_bar)),(gpointer)XK_Cyrillic_U_straight_bar);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_u_straight_bar)),(gpointer)XK_Cyrillic_u_straight_bar);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_HA_descender)),(gpointer)XK_Cyrillic_HA_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ha_descender)),(gpointer)XK_Cyrillic_ha_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_CHE_descender)),(gpointer)XK_Cyrillic_CHE_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_che_descender)),(gpointer)XK_Cyrillic_che_descender);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_CHE_vertstroke)),(gpointer)XK_Cyrillic_CHE_vertstroke);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_che_vertstroke)),(gpointer)XK_Cyrillic_che_vertstroke);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SHHA)),(gpointer)XK_Cyrillic_SHHA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_shha)),(gpointer)XK_Cyrillic_shha);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SCHWA)),(gpointer)XK_Cyrillic_SCHWA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_schwa)),(gpointer)XK_Cyrillic_schwa);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_I_macron)),(gpointer)XK_Cyrillic_I_macron);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_i_macron)),(gpointer)XK_Cyrillic_i_macron);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_O_bar)),(gpointer)XK_Cyrillic_O_bar);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_o_bar)),(gpointer)XK_Cyrillic_o_bar);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_U_macron)),(gpointer)XK_Cyrillic_U_macron);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_u_macron)),(gpointer)XK_Cyrillic_u_macron);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_dje)),(gpointer)XK_Serbian_dje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_gje)),(gpointer)XK_Macedonia_gje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_io)),(gpointer)XK_Cyrillic_io);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_ie)),(gpointer)XK_Ukrainian_ie);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_je)),(gpointer)XK_Ukranian_je);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_dse)),(gpointer)XK_Macedonia_dse);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_i)),(gpointer)XK_Ukrainian_i);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_i)),(gpointer)XK_Ukranian_i);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_yi)),(gpointer)XK_Ukrainian_yi);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_yi)),(gpointer)XK_Ukranian_yi);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_je)),(gpointer)XK_Cyrillic_je);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_je)),(gpointer)XK_Serbian_je);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_lje)),(gpointer)XK_Cyrillic_lje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_lje)),(gpointer)XK_Serbian_lje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_nje)),(gpointer)XK_Cyrillic_nje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_nje)),(gpointer)XK_Serbian_nje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_tshe)),(gpointer)XK_Serbian_tshe);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_kje)),(gpointer)XK_Macedonia_kje);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_ghe_with_upturn)),(gpointer)XK_Ukrainian_ghe_with_upturn);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Byelorussian_shortu)),(gpointer)XK_Byelorussian_shortu);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_dzhe)),(gpointer)XK_Cyrillic_dzhe);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_dze)),(gpointer)XK_Serbian_dze);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_numerosign)),(gpointer)XK_numerosign);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_DJE)),(gpointer)XK_Serbian_DJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_GJE)),(gpointer)XK_Macedonia_GJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_IO)),(gpointer)XK_Cyrillic_IO);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_IE)),(gpointer)XK_Ukrainian_IE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_JE)),(gpointer)XK_Ukranian_JE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_DSE)),(gpointer)XK_Macedonia_DSE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_I)),(gpointer)XK_Ukrainian_I);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_I)),(gpointer)XK_Ukranian_I);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_YI)),(gpointer)XK_Ukrainian_YI);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukranian_YI)),(gpointer)XK_Ukranian_YI);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_JE)),(gpointer)XK_Cyrillic_JE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_JE)),(gpointer)XK_Serbian_JE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_LJE)),(gpointer)XK_Cyrillic_LJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_LJE)),(gpointer)XK_Serbian_LJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_NJE)),(gpointer)XK_Cyrillic_NJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_NJE)),(gpointer)XK_Serbian_NJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_TSHE)),(gpointer)XK_Serbian_TSHE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Macedonia_KJE)),(gpointer)XK_Macedonia_KJE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Ukrainian_GHE_WITH_UPTURN)),(gpointer)XK_Ukrainian_GHE_WITH_UPTURN);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Byelorussian_SHORTU)),(gpointer)XK_Byelorussian_SHORTU);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_DZHE)),(gpointer)XK_Cyrillic_DZHE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Serbian_DZE)),(gpointer)XK_Serbian_DZE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_yu)),(gpointer)XK_Cyrillic_yu);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_a)),(gpointer)XK_Cyrillic_a);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_be)),(gpointer)XK_Cyrillic_be);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_tse)),(gpointer)XK_Cyrillic_tse);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_de)),(gpointer)XK_Cyrillic_de);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ie)),(gpointer)XK_Cyrillic_ie);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ef)),(gpointer)XK_Cyrillic_ef);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ghe)),(gpointer)XK_Cyrillic_ghe);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ha)),(gpointer)XK_Cyrillic_ha);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_i)),(gpointer)XK_Cyrillic_i);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_shorti)),(gpointer)XK_Cyrillic_shorti);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ka)),(gpointer)XK_Cyrillic_ka);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_el)),(gpointer)XK_Cyrillic_el);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_em)),(gpointer)XK_Cyrillic_em);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_en)),(gpointer)XK_Cyrillic_en);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_o)),(gpointer)XK_Cyrillic_o);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_pe)),(gpointer)XK_Cyrillic_pe);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ya)),(gpointer)XK_Cyrillic_ya);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_er)),(gpointer)XK_Cyrillic_er);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_es)),(gpointer)XK_Cyrillic_es);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_te)),(gpointer)XK_Cyrillic_te);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_u)),(gpointer)XK_Cyrillic_u);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_zhe)),(gpointer)XK_Cyrillic_zhe);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ve)),(gpointer)XK_Cyrillic_ve);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_softsign)),(gpointer)XK_Cyrillic_softsign);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_yeru)),(gpointer)XK_Cyrillic_yeru);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ze)),(gpointer)XK_Cyrillic_ze);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_sha)),(gpointer)XK_Cyrillic_sha);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_e)),(gpointer)XK_Cyrillic_e);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_shcha)),(gpointer)XK_Cyrillic_shcha);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_che)),(gpointer)XK_Cyrillic_che);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_hardsign)),(gpointer)XK_Cyrillic_hardsign);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_YU)),(gpointer)XK_Cyrillic_YU);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_A)),(gpointer)XK_Cyrillic_A);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_BE)),(gpointer)XK_Cyrillic_BE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_TSE)),(gpointer)XK_Cyrillic_TSE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_DE)),(gpointer)XK_Cyrillic_DE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_IE)),(gpointer)XK_Cyrillic_IE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_EF)),(gpointer)XK_Cyrillic_EF);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_GHE)),(gpointer)XK_Cyrillic_GHE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_HA)),(gpointer)XK_Cyrillic_HA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_I)),(gpointer)XK_Cyrillic_I);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SHORTI)),(gpointer)XK_Cyrillic_SHORTI);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_KA)),(gpointer)XK_Cyrillic_KA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_EL)),(gpointer)XK_Cyrillic_EL);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_EM)),(gpointer)XK_Cyrillic_EM);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_EN)),(gpointer)XK_Cyrillic_EN);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_O)),(gpointer)XK_Cyrillic_O);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_PE)),(gpointer)XK_Cyrillic_PE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_YA)),(gpointer)XK_Cyrillic_YA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ER)),(gpointer)XK_Cyrillic_ER);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ES)),(gpointer)XK_Cyrillic_ES);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_TE)),(gpointer)XK_Cyrillic_TE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_U)),(gpointer)XK_Cyrillic_U);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ZHE)),(gpointer)XK_Cyrillic_ZHE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_VE)),(gpointer)XK_Cyrillic_VE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SOFTSIGN)),(gpointer)XK_Cyrillic_SOFTSIGN);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_YERU)),(gpointer)XK_Cyrillic_YERU);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_ZE)),(gpointer)XK_Cyrillic_ZE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SHA)),(gpointer)XK_Cyrillic_SHA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_E)),(gpointer)XK_Cyrillic_E);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_SHCHA)),(gpointer)XK_Cyrillic_SHCHA);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_CHE)),(gpointer)XK_Cyrillic_CHE);
g_hash_table_insert (sven->keyboard->htsym, (g_strdup_printf("%c",XK_Cyrillic_HARDSIGN)),(gpointer)XK_Cyrillic_HARDSIGN);
g_hash_table_insert (sven->keyboard->htsym,"\"",(gpointer)XK_quotedbl);
g_hash_table_insert (sven->keyboard->htsym,"\'",(gpointer)XK_apostrophe);
g_hash_table_insert (sven->keyboard->htsym,"{",(gpointer)XK_braceleft);
g_hash_table_insert (sven->keyboard->htsym,"}",(gpointer)XK_braceright);
g_hash_table_insert (sven->keyboard->htsym,",",(gpointer)XK_comma);
g_hash_table_insert (sven->keyboard->htsym,".",(gpointer)XK_period);
g_hash_table_insert (sven->keyboard->htsym,":",(gpointer)XK_colon);
g_hash_table_insert (sven->keyboard->htsym,";",(gpointer)XK_semicolon);
g_hash_table_insert (sven->keyboard->htsym,"<",(gpointer)XK_less);
g_hash_table_insert (sven->keyboard->htsym,">",(gpointer)XK_greater);
g_hash_table_insert (sven->keyboard->htsym,"[",(gpointer)XK_bracketleft);
g_hash_table_insert (sven->keyboard->htsym,"]",(gpointer)XK_bracketright);
g_hash_table_insert (sven->keyboard->htsym," ",(gpointer)XK_space);
g_hash_table_insert (sven->keyboard->htsym,"!",(gpointer)XK_exclam);
g_hash_table_insert (sven->keyboard->htsym,"@",(gpointer)XK_at);
g_hash_table_insert (sven->keyboard->htsym,"#",(gpointer)XK_numbersign);
g_hash_table_insert (sven->keyboard->htsym,"#",(gpointer)XK_ssharp);
g_hash_table_insert (sven->keyboard->htsym,"$",(gpointer)XK_dollar);
g_hash_table_insert (sven->keyboard->htsym,"%",(gpointer)XK_percent);
g_hash_table_insert (sven->keyboard->htsym,"^",(gpointer)XK_degree);
g_hash_table_insert (sven->keyboard->htsym,"&",(gpointer)XK_ampersand);
g_hash_table_insert (sven->keyboard->htsym,"*",(gpointer)XK_asterisk);
g_hash_table_insert (sven->keyboard->htsym,"(",(gpointer)XK_parenleft);
g_hash_table_insert (sven->keyboard->htsym,")",(gpointer)XK_parenright);
g_hash_table_insert (sven->keyboard->htsym,"_",(gpointer)XK_underscore);
g_hash_table_insert (sven->keyboard->htsym,"=",(gpointer)XK_equal);
g_hash_table_insert (sven->keyboard->htsym,"+",(gpointer)XK_plus);
g_hash_table_insert (sven->keyboard->htsym,"-",(gpointer)XK_minus);
g_hash_table_insert (sven->keyboard->htsym,"/",(gpointer)XK_slash);
g_hash_table_insert (sven->keyboard->htsym,"|",(gpointer)XK_bar);
g_hash_table_insert (sven->keyboard->htsym,"\\",(gpointer)XK_backslash);
g_hash_table_insert (sven->keyboard->htsym,"?",(gpointer)XK_question);
g_hash_table_insert (sven->keyboard->htsym,"~",(gpointer)XK_dead_tilde);
g_hash_table_insert (sven->keyboard->htsym,"`",(gpointer)XK_Armenian_accent);
}

void load_symbol_translete(gpointer data)
{
	Sven *sven = (Sven *)data;
	
	sven->keyboard->httrans = g_hash_table_new (g_str_hash, g_str_equal);

g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_E)),"\"");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_e)),"'");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_HA)),"{");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_HARDSIGN)),"}");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_be)),",");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_yu)),".");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ZHE)),":");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_zhe)),";");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_BE)),"<");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_YU)),">");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ha)),"[");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_hardsign)),"]");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_EF)),"A");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_I)),"B");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ES)),"C");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_VE)),"D");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_U)),"E");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_A)),"F");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_PE)),"G");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ER)),"H");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_SHA)),"I");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_O)),"J");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_EL)),"K");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_DE)),"L");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_SOFTSIGN)),"M");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_TE)),"N");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_SHCHA)),"O");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ZE)),"P");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_SHORTI)),"Q");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_KA)),"R");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_YERU)),"S");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_IE)),"T");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_GHE)),"U");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_EM)),"V");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_TSE)),"W");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_CHE)),"X");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_EN)),"Y");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_YA)),"Z");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ef)),"a");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_i)),"b");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_es)),"c");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ve)),"d");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_u)),"e");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_a)),"f");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_pe)),"g");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_er)),"h");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_sha)),"i");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_o)),"j");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_el)),"k");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_de)),"l");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_softsign)),"m");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_te)),"n");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_shcha)),"o");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ze)),"p");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_shorti)),"q");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ka)),"r");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_yeru)),"s");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ie)),"t");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ghe)),"u");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_em)),"v");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_tse)),"w");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_che)),"x");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_en)),"y");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Cyrillic_ya)),"z");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukrainian_i)),"s");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukranian_i)),"s");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukrainian_yi)),"]");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukranian_yi)),"]");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukrainian_I)),"S");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukranian_I)),"S");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukrainian_YI)),"}");
g_hash_table_insert (sven->keyboard->httrans, (g_strdup_printf("%c",XK_Ukranian_YI)),"}");
g_hash_table_insert (sven->keyboard->httrans, "[","{");
}

void load_symbol_conv_to_ru(gpointer data)
{
	Sven *sven = (Sven *)data;
	
	sven->keyboard->ht_convru = g_hash_table_new (g_str_hash, g_str_equal);

g_hash_table_insert (sven->keyboard->ht_convru,"\"",(g_strdup_printf("%c",XK_Cyrillic_E)));
g_hash_table_insert (sven->keyboard->ht_convru,"'",(g_strdup_printf("%c",XK_Cyrillic_e)));
g_hash_table_insert (sven->keyboard->ht_convru,"{",(g_strdup_printf("%c",XK_Cyrillic_HA)));
g_hash_table_insert (sven->keyboard->ht_convru,"}",(g_strdup_printf("%c",XK_Cyrillic_HARDSIGN)));
g_hash_table_insert (sven->keyboard->ht_convru,",",(g_strdup_printf("%c",XK_Cyrillic_be)));
g_hash_table_insert (sven->keyboard->ht_convru,".",(g_strdup_printf("%c",XK_Cyrillic_yu)));
g_hash_table_insert (sven->keyboard->ht_convru,":", (g_strdup_printf("%c",XK_Cyrillic_ZHE)));
g_hash_table_insert (sven->keyboard->ht_convru,";", (g_strdup_printf("%c",XK_Cyrillic_zhe)));
g_hash_table_insert (sven->keyboard->ht_convru,"<", (g_strdup_printf("%c",XK_Cyrillic_BE)));
g_hash_table_insert (sven->keyboard->ht_convru,">", (g_strdup_printf("%c",XK_Cyrillic_YU)));
g_hash_table_insert (sven->keyboard->ht_convru,"[", (g_strdup_printf("%c",XK_Cyrillic_ha)));
g_hash_table_insert (sven->keyboard->ht_convru,"]", (g_strdup_printf("%c",XK_Cyrillic_hardsign)));
g_hash_table_insert (sven->keyboard->ht_convru,"A", (g_strdup_printf("%c",XK_Cyrillic_EF)));
g_hash_table_insert (sven->keyboard->ht_convru,"B", (g_strdup_printf("%c",XK_Cyrillic_I)));
g_hash_table_insert (sven->keyboard->ht_convru,"C", (g_strdup_printf("%c",XK_Cyrillic_ES)));
g_hash_table_insert (sven->keyboard->ht_convru,"D", (g_strdup_printf("%c",XK_Cyrillic_VE)));
g_hash_table_insert (sven->keyboard->ht_convru,"E", (g_strdup_printf("%c",XK_Cyrillic_U)));
g_hash_table_insert (sven->keyboard->ht_convru,"F", (g_strdup_printf("%c",XK_Cyrillic_A)));
g_hash_table_insert (sven->keyboard->ht_convru,"G", (g_strdup_printf("%c",XK_Cyrillic_PE)));
g_hash_table_insert (sven->keyboard->ht_convru,"H", (g_strdup_printf("%c",XK_Cyrillic_ER)));
g_hash_table_insert (sven->keyboard->ht_convru,"I", (g_strdup_printf("%c",XK_Cyrillic_SHA)));
g_hash_table_insert (sven->keyboard->ht_convru,"J", (g_strdup_printf("%c",XK_Cyrillic_O)));
g_hash_table_insert (sven->keyboard->ht_convru,"K", (g_strdup_printf("%c",XK_Cyrillic_EL)));
g_hash_table_insert (sven->keyboard->ht_convru,"L", (g_strdup_printf("%c",XK_Cyrillic_DE)));
g_hash_table_insert (sven->keyboard->ht_convru,"M", (g_strdup_printf("%c",XK_Cyrillic_SOFTSIGN)));
g_hash_table_insert (sven->keyboard->ht_convru,"N", (g_strdup_printf("%c",XK_Cyrillic_TE)));
g_hash_table_insert (sven->keyboard->ht_convru,"O", (g_strdup_printf("%c",XK_Cyrillic_SHCHA)));
g_hash_table_insert (sven->keyboard->ht_convru,"P", (g_strdup_printf("%c",XK_Cyrillic_ZE)));
g_hash_table_insert (sven->keyboard->ht_convru,"Q", (g_strdup_printf("%c",XK_Cyrillic_SHORTI)));
g_hash_table_insert (sven->keyboard->ht_convru,"R", (g_strdup_printf("%c",XK_Cyrillic_KA)));
g_hash_table_insert (sven->keyboard->ht_convru,"S", (g_strdup_printf("%c",XK_Cyrillic_YERU)));
g_hash_table_insert (sven->keyboard->ht_convru,"T", (g_strdup_printf("%c",XK_Cyrillic_IE)));
g_hash_table_insert (sven->keyboard->ht_convru,"U", (g_strdup_printf("%c",XK_Cyrillic_GHE)));
g_hash_table_insert (sven->keyboard->ht_convru,"V", (g_strdup_printf("%c",XK_Cyrillic_EM)));
g_hash_table_insert (sven->keyboard->ht_convru,"W", (g_strdup_printf("%c",XK_Cyrillic_TSE)));
g_hash_table_insert (sven->keyboard->ht_convru,"X", (g_strdup_printf("%c",XK_Cyrillic_CHE)));
g_hash_table_insert (sven->keyboard->ht_convru,"Y", (g_strdup_printf("%c",XK_Cyrillic_EN)));
g_hash_table_insert (sven->keyboard->ht_convru,"Z", (g_strdup_printf("%c",XK_Cyrillic_YA)));
g_hash_table_insert (sven->keyboard->ht_convru,"a", (g_strdup_printf("%c",XK_Cyrillic_ef)));
g_hash_table_insert (sven->keyboard->ht_convru,"b", (g_strdup_printf("%c",XK_Cyrillic_i)));
g_hash_table_insert (sven->keyboard->ht_convru,"c", (g_strdup_printf("%c",XK_Cyrillic_es)));
g_hash_table_insert (sven->keyboard->ht_convru,"d", (g_strdup_printf("%c",XK_Cyrillic_ve)));
g_hash_table_insert (sven->keyboard->ht_convru,"e", (g_strdup_printf("%c",XK_Cyrillic_u)));
g_hash_table_insert (sven->keyboard->ht_convru,"f", (g_strdup_printf("%c",XK_Cyrillic_a)));
g_hash_table_insert (sven->keyboard->ht_convru,"g", (g_strdup_printf("%c",XK_Cyrillic_pe)));
g_hash_table_insert (sven->keyboard->ht_convru,"h", (g_strdup_printf("%c",XK_Cyrillic_er)));
g_hash_table_insert (sven->keyboard->ht_convru,"i", (g_strdup_printf("%c",XK_Cyrillic_sha)));
g_hash_table_insert (sven->keyboard->ht_convru,"j", (g_strdup_printf("%c",XK_Cyrillic_o)));
g_hash_table_insert (sven->keyboard->ht_convru,"k", (g_strdup_printf("%c",XK_Cyrillic_el)));
g_hash_table_insert (sven->keyboard->ht_convru,"l", (g_strdup_printf("%c",XK_Cyrillic_de)));
g_hash_table_insert (sven->keyboard->ht_convru,"m", (g_strdup_printf("%c",XK_Cyrillic_softsign)));
g_hash_table_insert (sven->keyboard->ht_convru,"n", (g_strdup_printf("%c",XK_Cyrillic_te)));
g_hash_table_insert (sven->keyboard->ht_convru,"o", (g_strdup_printf("%c",XK_Cyrillic_shcha)));
g_hash_table_insert (sven->keyboard->ht_convru,"p", (g_strdup_printf("%c",XK_Cyrillic_ze)));
g_hash_table_insert (sven->keyboard->ht_convru,"q", (g_strdup_printf("%c",XK_Cyrillic_shorti)));
g_hash_table_insert (sven->keyboard->ht_convru,"r", (g_strdup_printf("%c",XK_Cyrillic_ka)));
g_hash_table_insert (sven->keyboard->ht_convru,"s", (g_strdup_printf("%c",XK_Cyrillic_yeru)));
g_hash_table_insert (sven->keyboard->ht_convru,"t", (g_strdup_printf("%c",XK_Cyrillic_ie)));
g_hash_table_insert (sven->keyboard->ht_convru,"u", (g_strdup_printf("%c",XK_Cyrillic_ghe)));
g_hash_table_insert (sven->keyboard->ht_convru,"v", (g_strdup_printf("%c",XK_Cyrillic_em)));
g_hash_table_insert (sven->keyboard->ht_convru,"w", (g_strdup_printf("%c",XK_Cyrillic_tse)));
g_hash_table_insert (sven->keyboard->ht_convru,"x", (g_strdup_printf("%c",XK_Cyrillic_che)));
g_hash_table_insert (sven->keyboard->ht_convru,"y", (g_strdup_printf("%c",XK_Cyrillic_en)));
g_hash_table_insert (sven->keyboard->ht_convru,"z", (g_strdup_printf("%c",XK_Cyrillic_ya)));

}

char *get_load_symbol_translete(gpointer data,gchar ch)
{
	Sven *sven = (Sven *)data;
	
	char * key = g_hash_table_lookup (sven->keyboard->httrans,(g_strdup_printf("%c",ch)));
	if(key==NULL)
	{
		return g_strdup_printf("%c",ch);
	}
	
	return key;
}

gchar *convert_symbol_to_translete(gpointer data,gchar *text)
{
	Sven *sven = (Sven *)data;
	size_t slen;
	int i=0;
	gchar *newtext="";
	gchar *temp="";
	
	if(text==NULL)
		return NULL;
	
	slen = strlen(text);
	
	for (i = 0; i < slen; i++) 
	{
		newtext=g_strconcat(newtext,get_load_symbol_translete(sven,text[i]),NULL);
	}
	
	return newtext;
}


char *get_load_symbol_conv_to_ru(gpointer data,gchar ch)
{
	Sven *sven = (Sven *)data;
	
	char * key = g_hash_table_lookup (sven->keyboard->ht_convru,(g_strdup_printf("%c",ch)));
	if(key==NULL)
	{
		return g_strdup_printf("%c",ch);
	}
	
	return key;
}

gchar *convert_symbol_to_ru(gpointer data,gchar *text)
{
	Sven *sven = (Sven *)data;
	size_t slen;
	int i=0;
	gchar *newtext="";
	gchar *temp="";
	
	if(text==NULL)
		return NULL;
	
	slen = strlen(text);
	
	for (i = 0; i < slen; i++) 
	{
		newtext=g_strconcat(newtext,get_load_symbol_conv_to_ru(sven,text[i]),NULL);
	}
	
	return newtext;
}

KeySym get_load_symbol(gpointer data,gchar ch)
{
	Sven *sven = (Sven *)data;
	int i=0;
	
	KeySym key = (KeySym ) g_hash_table_lookup (sven->keyboard->htsym,(g_strdup_printf("%c",ch)));

	if(key==NULL)
	{
		return XStringToKeysym(g_strdup_printf("%c",ch));
	}
	
	return key;
}


void convert_utf_init(gpointer data) 
{
	gchar *filename;
	Sven *sven = (Sven *)data;
	
	filename= g_strconcat(g_get_home_dir(),"/.sven/utf.conv", NULL);
	
	if (!g_file_test(filename, G_FILE_TEST_EXISTS))
	{
		g_free(filename);
		filename= g_strconcat(PKGDATADIR,"utf.conv", NULL);
	}
	sven->keyboard->utf_char = sven_cfg_open_file(filename);
	if (!sven->keyboard->utf_char)
			sven->keyboard->utf_char = sven_cfg_new();
	//else
		//printf("utf_char OK Load\n");
			

}

gchar* conv_utfchar_to_en (gpointer data,gchar *buf)
{
	Sven *sven = (Sven *)data;
	ConfigSection *sect;
	ConfigLine *line;
	

	if (! buf)
		return NULL;

	g_return_val_if_fail(sven->keyboard->utf_char != NULL, FALSE);
				
	if (!(sect = sven_cfg_find_section(sven->keyboard->utf_char, "utf->eng")))
		return buf;
		
	if (!(line = sven_cfg_find_string(sect,buf)))
		return buf;
	
	return g_strdup(line->value);
}
int get_modifer_to_char (gpointer data,gchar *ch,gchar *flag)
{
	Sven *sven = (Sven *)data;
	int modifer=0;
	
	if (! ch)
		return 0;

	if((g_strcasecmp (flag,"Russian")==0) ||  (g_strcasecmp (flag,"Russian(phonetic)")==0) || (g_strcasecmp (flag,"Ukrainian")==0)) 
	{
		sven_cfg_read_int(sven->keyboard->utf_char,"ru->modifer",ch,&modifer);
		
		return modifer;
	}
	else
	{
		sven_cfg_read_int(sven->keyboard->utf_char,"eng->modifer",ch,&modifer);
		
		return modifer;
	}
	
	return 0;
}


gchar* conv_utfstr_to_en (gpointer data,gchar *buf)
{
	if (! buf)
		return NULL;
     
	Sven *sven = (Sven *)data;
	gchar *newtext="";	
	gchar *p = buf;
	gunichar unichar = g_utf8_get_char (p);
  
	while ( unichar )
        {
		GString* gs = g_string_sized_new (2);
		gs = g_string_append_unichar (gs, unichar);      
		newtext=g_strconcat(newtext,conv_utfchar_to_en(sven,gs->str),NULL);
		p = g_utf8_next_char (p);
		unichar = g_utf8_get_char (p);
		g_string_free (gs, FALSE);
        }
  
	return newtext;
}

