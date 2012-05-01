/* goodbye - dead simple GTK/D-Bus shutdown dialog.
 * Copyright (C) 2012 Georg Reinke. All rights reserved.
 *
 * See LICENSE for copying details.
 */

#include <assert.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum {
	Shutdown = 0,
	Reboot,
	Suspend,
	Hibernate
};

/* The names of some things associated whith each button. */
enum {
	Destination = 0,
	Object,
	Interface,
	Method,
	Label,
	Icon
};

static const char* names[4][6] = {
	[Shutdown] = {
		[Destination] = "org.freedesktop.ConsoleKit",
		[Object] = "/org/freedesktop/ConsoleKit/Manager",
		[Interface] = "org.freedesktop.ConsoleKit.Manager",
		[Method] = "Stop",
		[Label] = "Shutdown",
		[Icon] = "system-shutdown"
	},
	[Reboot] = {
		[Destination] = "org.freedesktop.ConsoleKit",
		[Object] = "/org/freedesktop/ConsoleKit/Manager",
		[Interface] = "org.freedesktop.ConsoleKit.Manager",
		[Method] = "Restart",
		[Label] = "Reboot", 
		[Icon] = "system-restart"
	},
	[Suspend] = {
		[Destination] = "org.freedesktop.UPower",
		[Object] = "/org/freedesktop/UPower",
		[Interface] = "org.freedesktop.UPower",
		[Method] = "Suspend",
		[Label] = "Suspend",
		[Icon] = "system-suspend"
	},
	[Hibernate] = {
		[Destination] = "org.freedesktop.UPower",
		[Object] = "/org/freedesktop/UPower",
		[Interface] = "org.freedesktop.UPower",
		[Method] = "Hibernate",
		[Label] = "Hibernate",
		[Icon] = "system-suspend-hibernate"
	}
};

static bool verbose = false;

void version(int exit_val) {
	g_printerr("%s %s\n", PROGNAME, VERSION);
	g_printerr("(C) 2012 Georg Reinke, see LICENSE for details\n");

	exit(exit_val);
}

void usage(int exit_val) {
	g_printerr("%s - dead simple GTK/D-Bus shutdown dialog\n", PROGNAME);
	g_printerr("USAGE: %s [OPTION]\n", PROGNAME);
	g_printerr("OPTIONS:\n");
	g_printerr("  -h|--help:    print this help\n");
	g_printerr("  -v|--verbose: show some more information\n");
	g_printerr("  --version:    show version and copyright notice\n");

	exit(exit_val);
}

void handle_clicked(GtkWidget *widget, gpointer data) {
	const char *dest = NULL, *path = NULL, *interface = NULL, *method = NULL;
	GDBusConnection *connection = NULL;
	GDBusMessage *message = NULL, *reply = NULL;
	GError *error = NULL;
	int action = -1;

	action = GPOINTER_TO_INT(data);
	assert(action >= 0);

	dest = names[action][Destination];
	path = names[action][Object];
	interface = names[action][Interface];
	method = names[action][Method];

	/* DBus stuff */
	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (!connection) {
		g_printerr("Failed to connect to system bus: %s\n", error->message);
		g_error_free(error);
		exit(1);
	}

	message = g_dbus_message_new_method_call(NULL, path, interface, method);
	g_dbus_message_set_destination(message, dest);
	if (verbose) {
		gchar* status = g_dbus_message_print(message, 0);
		g_printerr("sending following message:\n%s", status);
		g_free(status);
	}
	reply = g_dbus_connection_send_message_with_reply_sync(connection, 
			message, 0, -1, NULL, NULL, &error);
	if (!reply) {
		g_printerr("Failed to send message: %s\n", error->message);
		g_error_free(error);
		exit(1);
	} else if (verbose) {
		gchar* status = g_dbus_message_print(message, 0);
		g_printerr("got response:\n%s", status);
		g_free(status);
	}

	g_object_unref(message);
	g_object_unref(connection);
	gtk_main_quit();
}

int main(int argc, char *argv[]) {
	GtkWidget *window, *box;
	GtkWidget *buttons[4];
	GtkWidget *icons[4];

	gtk_init(&argc, &argv);

	if (argc > 1) {
		if (argc > 2) 
			usage(1);
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			usage(0);
		} else if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--verbose")) {
			verbose = true;
		} else if (!strcmp(argv[1], "--version")) {
			version(0);
		} else {
			usage(1);
		}
	}

	/* create the window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Goodbye");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);

	/* create the buttons and their icons*/
	for (int i = 0; i < 4; ++i) {
		buttons[i] = gtk_button_new_with_label(names[i][Label]);
		icons[i] = gtk_image_new_from_icon_name(names[i][Icon], 
		GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(buttons[i]), icons[i]);
	}


	/* create the box */
#ifdef USE_GTK3
	box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
#else
	box = gtk_hbutton_box_new();
#endif /* USE_GTK3 */

	for (int i = 0; i < 4; ++i) 
		gtk_container_add(GTK_CONTAINER(box), buttons[i]);

	gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_CENTER);
	gtk_container_add(GTK_CONTAINER(window), box);

	/* add the signals */
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	for (int i = 0; i < 4; ++i)
		g_signal_connect(buttons[i], "clicked", G_CALLBACK(handle_clicked),
		                 GINT_TO_POINTER(i));

	/* show window and quit */
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
