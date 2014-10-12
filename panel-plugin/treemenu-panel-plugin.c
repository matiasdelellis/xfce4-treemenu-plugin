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

#include "treemenu-window.h"
#include "treemenu-panel-plugin.h"

/* prototypes */
static void
treemenu_construct (XfcePanelPlugin *plugin);

/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (treemenu_construct);

/*
 * 
 */
static void
treemenu_plugin_menu_activated (GtkButton       *widget,
                                TreemenuPlugin  *treemenu)
{
	treemenu_plugin_show_test_dialog ();
}

/*
 * Treemenu private signals.
 */

static gboolean
treemenu_size_changed (XfcePanelPlugin *plugin,
                       gint             panel_size,
                       TreemenuPlugin *treemenu)
{
	GtkOrientation panel_orientation;

	/* get the orientation of the plugin */
	panel_orientation = xfce_panel_plugin_get_orientation (plugin);

	/* set the widget size */
	if (panel_orientation == GTK_ORIENTATION_HORIZONTAL)
		gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, panel_size);
	else
		gtk_widget_set_size_request (GTK_WIDGET (plugin), panel_size, -1);

	/* we handled the orientation */
	return TRUE;
}

static void
treemenu_mode_changed (XfcePanelPlugin     *plugin,
                       XfcePanelPluginMode  mode,
                       TreemenuPlugin      *treemenu)
{
	GtkOrientation orientation;

	orientation = xfce_panel_plugin_get_orientation (plugin);

	if (mode == XFCE_PANEL_PLUGIN_MODE_DESKBAR)
	{
		//xfce_hvbox_set_orientation (XFCE_HVBOX (treemenu->hvbox), GTK_ORIENTATION_HORIZONTAL);
	}
	else
	{
		//xfce_hvbox_set_orientation (XFCE_HVBOX (treemenu->hvbox), orientation);
	}
}

/*
 * Treemenu plugin construction
 */

static void
treemenu_free (XfcePanelPlugin *plugin,
               TreemenuPlugin  *treemenu)
{
	GtkWidget *dialog;

	/* check if the dialog is still open. if so, destroy it */
	dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
	if (G_UNLIKELY (dialog != NULL))
		gtk_widget_destroy (dialog);

	/* destroy the panel widgets */
	gtk_widget_destroy (treemenu->button);

	/* free the plugin structure */
	panel_slice_free (TreemenuPlugin, treemenu);
}

static TreemenuPlugin *
treemenu_new (XfcePanelPlugin *plugin)
{
	TreemenuPlugin *treemenu;

	/* allocate memory for the plugin structure */
	treemenu = panel_slice_new0 (TreemenuPlugin);
	treemenu->plugin = plugin;

	/* Panel widget */
	treemenu->button = gtk_button_new_with_label ("Aplicaciones");
	g_signal_connect (G_OBJECT (treemenu->button), "activate",
	                  G_CALLBACK (treemenu_plugin_menu_activated), treemenu);

	return treemenu;
}

static void
treemenu_construct (XfcePanelPlugin *plugin)
{
	TreemenuPlugin *treemenu;

	/* setup transation domain */
	xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

	/* create the plugin */
	treemenu = treemenu_new (plugin);

	/* add the hvbox to the panel */
	gtk_container_add (GTK_CONTAINER (plugin), treemenu->button);
	gtk_widget_show (treemenu->button);

	/* connect plugin signals */
	g_signal_connect (G_OBJECT (plugin), "free-data",
	                  G_CALLBACK (treemenu_free), treemenu);

	g_signal_connect (G_OBJECT (plugin), "size-changed",
	                  G_CALLBACK (treemenu_size_changed), treemenu);

	g_signal_connect (G_OBJECT (plugin), "mode-changed",
	                  G_CALLBACK (treemenu_mode_changed), treemenu);

	/* show the about menu item and connect signal */
	/*xfce_panel_plugin_menu_show_about (plugin);
	g_signal_connect (G_OBJECT (plugin), "about",
				G_CALLBACK (treemenu_about), NULL);*/
}
