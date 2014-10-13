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
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>

#include "garcon-cnp.h"
#include "garcon-treeview.h"

#define STR_IS_EMPTY(str) ((str) == NULL || *(str) == '\0')

/*
 * Columns in Tree view
 */
enum tree_columns {
	L_ICON_NAME,
	L_NODE_DATA,
	L_TOOLTIP,
	L_GARCON_ITEM,
	L_VISIBILE,
	N_L_COLUMNS
};

static gboolean
garcon_treeview_cmp (const gchar *haystack, const gchar *needle)
{
	gchar *haystack_down = NULL;
	gchar *needle_down = NULL;
	gboolean math = FALSE;

	haystack_down = g_ascii_strdown (haystack, -1);
	needle_down = g_ascii_strdown (needle, -1);

	math = (g_strrstr(haystack_down, needle_down) != NULL);

	g_free (haystack_down);
	g_free (needle_down);

	return math;
}

static void
garcon_tree_view_set_visible_parents (GtkTreeModel *model, GtkTreeIter *c_iter)
{
	GtkTreeIter t_iter, parent;

	t_iter = *c_iter;

	while (gtk_tree_model_iter_parent (model, &parent, &t_iter)) {
		gtk_tree_store_set (GTK_TREE_STORE(model), &parent,
		                    L_VISIBILE, TRUE, -1);
		t_iter = parent;
	}
}

static gboolean
garcon_treeview_any_parent_visible (GtkTreeModel *model,
                                    GtkTreeIter  *iter,
                                    const gchar  *needle)
{
	GtkTreeIter t_iter, parent;
	gchar *name = NULL;
	gboolean visible = FALSE;

	t_iter = *iter;

	while (gtk_tree_model_iter_parent (model, &parent, &t_iter)) {
		gtk_tree_model_get (model, &parent, L_NODE_DATA, &name, -1);
		visible = garcon_treeview_cmp (name, needle);
		g_free (name);

		if (visible)
			return TRUE;

		t_iter = parent;
	}

	return FALSE;
}

static gboolean
garcon_treeview_filter_tree_func (GtkTreeModel *model,
                                  GtkTreePath  *path,
                                  GtkTreeIter  *iter,
                                  gpointer      data)
{
	gboolean visible = FALSE;
	const gchar *name = NULL;
	const gchar *needle = data;

	gtk_tree_model_get (model, iter, L_NODE_DATA, &name, -1);

	/* Compare name */
	visible = garcon_treeview_cmp (name, needle);

	/* Check if any parent match. */
	if (!visible)
		visible = garcon_treeview_any_parent_visible (model, iter, needle);

	gtk_tree_store_set (GTK_TREE_STORE(model), iter,
	                    L_VISIBILE, visible, -1);

	if (visible)
		garcon_tree_view_set_visible_parents (model, iter);

	return FALSE;
}

static gboolean
_garcon_tree_view_set_all_visible (GtkTreeModel *model,
                                   GtkTreePath  *path,
                                   GtkTreeIter  *iter,
                                   gpointer      data)
{
	gtk_tree_store_set (GTK_TREE_STORE(model), iter,
	                    L_VISIBILE, TRUE, -1);
	return FALSE;
}

static void
garcon_tree_view_set_all_visible (GtkTreeModel *model)
{
	gtk_tree_model_foreach (GTK_TREE_MODEL(model),
	                        _garcon_tree_view_set_all_visible,
	                        NULL);
}

void
garcon_treeview_refilter (GtkWidget *treeview, const gchar *needle)
{
	GtkTreeModel *filter_model, *model;

	filter_model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
	model = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER(filter_model));

	if (STR_IS_EMPTY (needle)) {
		garcon_tree_view_set_all_visible (model);
		gtk_tree_view_collapse_all (GTK_TREE_VIEW(treeview));
	}
	else {
		gtk_tree_model_foreach (GTK_TREE_MODEL(model),
		                        garcon_treeview_filter_tree_func,
		                        (gpointer) needle);

		gtk_tree_view_expand_all (GTK_TREE_VIEW(treeview));
	}
}

static void
garcon_tree_view_row_activated_cb (GtkTreeView       *treeview,
                                   GtkTreePath       *path,
                                   GtkTreeViewColumn *column,
                                   gpointer           user_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GarconMenuItem *item;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));

	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, L_GARCON_ITEM, &item, -1);

	if (item) {
		garcon_gtk_menu_item_activate (item);
		g_object_unref (item);
	}

}

gboolean
garcon_fill_tree_view (GtkTreeModel *model,
                       GtkTreeIter  *p_iter,
                       GarconMenu   *garcon_menu)
{
	GList               *elements, *li;
	const gchar         *name, *icon_name;
	const gchar         *comment;
	gboolean             has_children = FALSE;
	GarconMenuDirectory *directory;
	GtkTreeIter          iter;

	g_return_val_if_fail (GARCON_IS_MENU (garcon_menu), FALSE);

	elements = garcon_menu_get_elements (garcon_menu);
	for (li = elements; li != NULL; li = li->next) {
		if (GARCON_IS_MENU_ITEM (li->data) && p_iter != NULL) {
			/* skip invisible items */
			if (!garcon_menu_element_get_visible (li->data))
				continue;

			/* get element name */
			name = NULL;
			if (name == NULL)
				name = garcon_menu_item_get_name (li->data);

			icon_name = NULL;
			icon_name = garcon_menu_element_get_icon_name (li->data);
			if (STR_IS_EMPTY (icon_name))
				icon_name = "applications-other";

			if (G_UNLIKELY (name == NULL))
				continue;

			comment = garcon_menu_item_get_comment (li->data);

			gtk_tree_store_append (GTK_TREE_STORE(model), &iter, p_iter);
			gtk_tree_store_set (GTK_TREE_STORE(model), &iter,
			                    L_ICON_NAME, icon_name,
			                    L_NODE_DATA, name,
			                    L_TOOLTIP, comment,
			                    L_GARCON_ITEM, li->data,
			                    L_VISIBILE, TRUE,
			                    -1);

			has_children = TRUE;
		}
		else if (GARCON_IS_MENU_SEPARATOR (li->data)) {
			//g_print ("Separador!!! !\n");
		}
		else if (GARCON_IS_MENU (li->data)) {
			directory = garcon_menu_get_directory (li->data);
			if (directory != NULL && !garcon_menu_directory_get_visible (directory))
				continue;

			icon_name = NULL;
			icon_name = garcon_menu_element_get_icon_name (li->data);
			if (icon_name == NULL)
				icon_name = "applications-other";

			name = garcon_menu_element_get_name (li->data);

			gtk_tree_store_append (GTK_TREE_STORE(model), &iter, p_iter);
			gtk_tree_store_set (GTK_TREE_STORE(model), &iter,
			                    L_ICON_NAME, icon_name,
			                    L_NODE_DATA, name,
			                    L_TOOLTIP, name,
			                    L_VISIBILE, TRUE,
			                    -1);

			garcon_fill_tree_view (model, &iter, li->data);
		}
		else {
			g_print("Otros\n");
        }
    }

  g_list_free (elements);

  return has_children;
}

GtkWidget *
garcon_tree_view_new (void)
{
	GtkWidget *treeview;
	GtkTreeStore *store;
	GtkTreeModel *filter_tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/*
	 * Tree store.
	 */
	store = gtk_tree_store_new (N_L_COLUMNS,
	                            G_TYPE_STRING,   /* Icon name */
	                            G_TYPE_STRING,   /* Name */
	                            G_TYPE_STRING,   /* Tooltip */
	                            G_TYPE_OBJECT,   /* Garcon item */
	                            G_TYPE_BOOLEAN); /* Row visibility */
	/*
	 * Tree view
	 */
	filter_tree = gtk_tree_model_filter_new (GTK_TREE_MODEL(store), NULL);
	gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_tree), L_VISIBILE);

	/* Create the tree view */
	treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(filter_tree));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(treeview), FALSE);
	gtk_tree_view_set_show_expanders (GTK_TREE_VIEW(treeview), TRUE);

	column = gtk_tree_view_column_new();

	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column,      renderer,
	                                     "icon-name", L_ICON_NAME,
	                                     NULL);

	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
	                                     "text", L_NODE_DATA,
	                                     NULL);

	g_object_set (G_OBJECT(renderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);
	g_object_unref (filter_tree);

	gtk_tree_view_set_tooltip_column (GTK_TREE_VIEW(treeview), L_TOOLTIP);

	g_signal_connect (G_OBJECT(treeview), "row-activated",
	                  G_CALLBACK(garcon_tree_view_row_activated_cb), NULL);

	gtk_widget_show_all(GTK_WIDGET(treeview));

	return treeview;
}