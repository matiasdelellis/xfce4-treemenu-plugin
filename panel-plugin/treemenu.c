#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include "treemenu-widget.h"

int
main (int   argc,
      char *argv[])
{
	gtk_init (&argc, &argv);

	treemenu_plugin_show_test_dialog ();

	gtk_main ();

	return 0;
}