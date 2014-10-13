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

#ifndef __GARCON_TREE_VIEW_H__
#define __GARCON_TREE_VIEW_H__

#include <gtk/gtk.h>

void       garcon_treeview_refilter (GtkWidget   *treeview,
                                     const gchar *filter);

gboolean   garcon_fill_tree_view    (GtkTreeModel *model,
                                     GtkTreeIter  *p_iter,
                                     GarconMenu   *garcon_menu);

GtkWidget *garcon_tree_view_new     (void);

#endif /* !__GARCON_TREE_VIEW_H__ */