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

GtkWidget *
garcon_treemenu_window_new (void)
{
	GtkWidget *window;
	GtkWidget *treeview;
	GtkWidget *tree_scroll;
	GtkTreeModel *model, *filter_model;
	GarconMenu *menu = NULL;

	/* Window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Test menu.");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	/* Container */
	tree_scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(tree_scroll),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(tree_scroll),
	                                     GTK_SHADOW_IN);
	gtk_container_set_border_width (GTK_CONTAINER(tree_scroll), 2);

	gtk_container_add (GTK_CONTAINER(window), tree_scroll);

	/* Garcon treeview */
	treeview = garcon_tree_view_new ();

	/* Get menu and fill tree view */
	menu = garcon_menu_new_applications ();
	garcon_menu_load (menu, NULL, NULL);

	filter_model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
	model = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER(filter_model));
	garcon_fill_tree_view (model, NULL, menu);

	gtk_container_add (GTK_CONTAINER(tree_scroll), treeview);

	return window;
}

void
treemenu_plugin_show_test_dialog (void)
{
	GtkWidget *window;
	window = garcon_treemenu_window_new ();
	gtk_widget_show_all (window);
}