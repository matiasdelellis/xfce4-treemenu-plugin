AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-treemenu-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	-DBASEICONDIR=\"$(datadir)/icons/hicolor\" \
	$(PLATFORM_CPPFLAGS)

#
# Test menu app
#
bin_PROGRAMS = treemenu

treemenu_SOURCES = \
	treemenu-window.c \
	treemenu-window.h \
	garcon-treeview.c \
	garcon-treeview.h \
	garcon-cnp.c \
	garcon-cnp.h \
	treemenu.c

treemenu_CFLAGS = \
	$(DBUS_GLIB_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(GARCON_CFLAGS) \
	$(PLATFORM_CFLAGS)

treemenu_LDADD = \
	$(DBUS_GLIB_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(GARCON_LIBS) \
	-lm

#
# Treemenu plugin
#
plugin_LTLIBRARIES = \
	libtreemenu.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libtreemenu_la_SOURCES = \
	treemenu-panel-plugin.c \
	treemenu-panel-plugin.h \
	treemenu-window.c \
	treemenu-window.h \
	garcon-treeview.c \
	garcon-treeview.h \
	garcon-cnp.c \
	garcon-cnp.h

libtreemenu_la_CFLAGS = \
	$(DBUS_GLIB_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(GARCON_CFLAGS) \
	$(PLATFORM_CFLAGS)

libtreemenu_la_LDFLAGS = 					\
	-avoid-version 						\
	-module 						\
	-no-undefined 						\
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)'

libtreemenu_la_LIBADD = \
	$(DBUS_GLIB_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	$(GARCON_LIBS) \
	-lm

#
# Desktop file
#
desktopdir =								\
	$(datadir)/xfce4/panel/plugins

desktop_in_in_files =							\
	treemenu.desktop.in.in

desktop_in_files =							\
	$(desktop_in_in_files:.desktop.in.in=.desktop.in)

treemenu.desktop.in: treemenu.desktop.in.in
	sed -e "s,\@libdir\@,$(libdir),g" < $< > $@

desktop_DATA =								\
	$(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	$(desktop_in_in_files)

CLEANFILES =								\
	$(desktop_in_files)						\
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
