/*
 *  Copyright (c) 2014 matias <mati86dl@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <garcon/garcon.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include "garcon-cnp.h"

#define STR_IS_EMPTY(str) ((str) == NULL || *(str) == '\0')

static void
garcon_gtk_menu_append_quoted (GString     *string,
                               const gchar *unquoted)
{
	gchar *quoted;

	quoted = g_shell_quote (unquoted);
	g_string_append (string, quoted);
	g_free (quoted);
}

void
garcon_gtk_menu_item_activate (GarconMenuItem *item)
{
	GString      *string;
	const gchar  *command;
	const gchar  *p;
	const gchar  *tmp;
	gchar       **argv;
	gboolean      result = FALSE;
	gchar        *uri;
	GError       *error = NULL;

	g_return_if_fail (GARCON_IS_MENU_ITEM (item));

	command = garcon_menu_item_get_command (item);
	if (STR_IS_EMPTY (command))
		return;

	string = g_string_sized_new (100);

	if (garcon_menu_item_requires_terminal (item))
	g_string_append (string, "exo-open --launch TerminalEmulator ");

	/* expand the field codes */
	for (p = command; *p != '\0'; ++p) {
		if (G_UNLIKELY (p[0] == '%' && p[1] != '\0')) {
			switch (*++p) {
				case 'f': case 'F':
				case 'u': case 'U':
					/* TODO for dnd, not a regression, xfdesktop never had this */
					break;
				case 'i':
					tmp = garcon_menu_item_get_icon_name (item);
					if (!STR_IS_EMPTY (tmp)) {
						g_string_append (string, "--icon ");
						garcon_gtk_menu_append_quoted (string, tmp);
					}
					break;
				case 'c':
					tmp = garcon_menu_item_get_name (item);
					if (!STR_IS_EMPTY (tmp))
						garcon_gtk_menu_append_quoted (string, tmp);
					break;
				case 'k':
					uri = garcon_menu_item_get_uri (item);
					if (!STR_IS_EMPTY (uri))
						garcon_gtk_menu_append_quoted (string, uri);
					g_free (uri);
					break;
				case '%':
					g_string_append_c (string, '%');
					break;
			}
		}
		else {
			g_string_append_c (string, *p);
		}
	}

	/* parse and spawn command */
	if (g_shell_parse_argv (string->str, NULL, &argv, &error)) {
		result = xfce_spawn_on_screen (NULL,
		                               garcon_menu_item_get_path (item),
		                               argv, NULL, G_SPAWN_SEARCH_PATH,
		                               garcon_menu_item_supports_startup_notification (item),
		                               gtk_get_current_event_time (),
		                               garcon_menu_item_get_icon_name (item),
		                               &error);
		g_strfreev (argv);
	}

	if (G_UNLIKELY (!result)) {
		xfce_dialog_show_error (NULL, error, _("Failed to execute command \"%s\"."), command);
		g_error_free (error);
	}

	g_string_free (string, TRUE);
}
