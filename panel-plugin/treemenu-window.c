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

#include "garcon-treeview.h"
#include "treemenu-window.h"

/*
 * GtkSearchEntry:
 */
static gboolean
gtk_search_entry_changed_handler (GtkEntry  *entry,
                                  GtkWidget *treeview)
{
	garcon_treeview_refilter (treeview, gtk_entry_get_text (entry));

	return FALSE;
}

static gboolean
gtk_search_entry_activated_handler (GtkEntry  *entry,
                                    GtkWidget *treeview)
{
	return FALSE;
}

static void
gtk_search_entry_icon_pressed_cb (GtkEntry       *entry,
                                  gint            position,
                                  GdkEventButton *event,
                                  gpointer       *data)
{
	if (position == GTK_ENTRY_ICON_SECONDARY) {
		gtk_entry_set_text (entry, "");
		gtk_widget_grab_focus(GTK_WIDGET(entry));

		g_signal_emit_by_name(G_OBJECT(entry), "activate");
	}
}

static void
gtk_search_entry_changed_cb (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);

	gboolean has_text = gtk_entry_get_text_length (entry) > 0;
	gtk_entry_set_icon_sensitive (entry, GTK_ENTRY_ICON_SECONDARY, has_text);
}

static GtkWidget *
gtk_search_entry_new (void)
{
	GtkWidget *search_entry;

	search_entry = gtk_entry_new ();

	gtk_entry_set_icon_from_icon_name (GTK_ENTRY(search_entry),
	                                   GTK_ENTRY_ICON_PRIMARY, "edit-find");
	gtk_entry_set_icon_from_icon_name (GTK_ENTRY(search_entry),
	                                   GTK_ENTRY_ICON_SECONDARY, "edit-clear");

	gtk_entry_set_icon_sensitive (GTK_ENTRY(search_entry),
	                              GTK_ENTRY_ICON_SECONDARY, FALSE);

	g_signal_connect (search_entry, "icon-press",
	                  G_CALLBACK (gtk_search_entry_icon_pressed_cb), NULL);
	g_signal_connect (search_entry, "changed",
	                  G_CALLBACK (gtk_search_entry_changed_cb), NULL);

	return search_entry;
}

/*
 * Dialog
 */
GtkWidget *
garcon_treemenu_window_new (void)
{
	GtkWidget *window;
	GtkWidget *vbox, *hbox;
	GtkWidget *entry, *button;
	GtkWidget *treeview;
	GtkWidget *tree_scroll;
	GtkTreeModel *model, *filter_model;
	GarconMenu *menu = NULL;

	/* Window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Test menu.");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	/* Vertical box */
	vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add (GTK_CONTAINER(window), vbox);

	/* Container */
	tree_scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(tree_scroll),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(tree_scroll),
	                                     GTK_SHADOW_IN);
	gtk_container_set_border_width (GTK_CONTAINER(tree_scroll), 2);

	gtk_box_pack_start (GTK_BOX(vbox), tree_scroll, TRUE, TRUE, 2);

	/* Garcon treeview */
	treeview = garcon_tree_view_new ();

	/* Get menu and fill tree view */
	menu = garcon_menu_new_applications ();
	garcon_menu_load (menu, NULL, NULL);

	filter_model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
	model = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER(filter_model));
	garcon_fill_tree_view (model, NULL, menu);

	gtk_container_add (GTK_CONTAINER(tree_scroll), treeview);

	/* Horitontal box */
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

	entry = gtk_search_entry_new ();
	g_signal_connect (G_OBJECT(entry), "changed",
	                  G_CALLBACK(gtk_search_entry_changed_handler), treeview);
	g_signal_connect (G_OBJECT(entry), "activate",
	                  G_CALLBACK(gtk_search_entry_activated_handler), treeview);
	gtk_box_pack_start (GTK_BOX(hbox), entry, TRUE, TRUE, 2);

	button = gtk_button_new_with_label ("Salir");
	gtk_box_pack_start (GTK_BOX(hbox), button, FALSE, FALSE, 2);

	return window;
}

void
treemenu_plugin_show_test_dialog (void)
{
	GtkWidget *window;
	window = garcon_treemenu_window_new ();
	gtk_widget_show_all (window);
}