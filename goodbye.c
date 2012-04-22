/* goodbye - dead simple GTK/DBus shutdown dialog.
 * Copyright (C) 2012 Georg Reinke. All rights reserved.
 *
 * See LICENSE for copying details.
 */

#define PROGNAME "goodbye"
#define VERSION "0.0-git"

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

const char* names[4][4] = {
	[Shutdown] = {
		"org.freedesktop.ConsoleKit",
		"/org/freedesktop/ConsoleKit/Manager",
		"org.freedesktop.ConsoleKit.Manager",
		"Stop"
	},
	[Reboot] = {
		"org.freedesktop.ConsoleKit",
		"/org/freedesktop/ConsoleKit/Manager",
		"org.freedesktop.ConsoleKit.Manager",
		"Restart"
	},
	[Suspend] = {
		"org.freedesktop.UPower",
		"/org/freedesktop/UPower",
		"org.freedesktop.UPower",
		"Suspend"
	},
	[Hibernate] = {
		"org.freedesktop.UPower",
		"/org/freedesktop/UPower",
		"org.freedesktop.UPower",
		"Hibernate"
	}
};

void version(int exit_val) {
	g_printerr("%s %s\n", PROGNAME, VERSION);
	g_printerr("(C) 2012 Georg Reinke, see LICENSE for details\n");

	exit(exit_val);
}

void usage(int exit_val) {
	g_printerr("USAGE: %s [OPTION]\n", PROGNAME);
	g_printerr("OPTIONS:\n");
	g_printerr("  -h|--help:    print this help\n");
	g_printerr("  -v|--verbose: show some more information\n");
	g_printerr("  --version:    show version and copyright notice\n");

	exit(exit_val);
}
int main(int argc, char *argv[]) {
	bool verbose = false;
	const char *dest = NULL, *path = NULL, *interface = NULL, *method = NULL;
	GDBusConnection *connection = NULL;
	GDBusMessage *message = NULL, *reply = NULL;
	GError *error = NULL;
	gint result;
	GtkWidget *window = NULL;

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

	/* DBus stuff */
	connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (!connection) {
		g_printerr("Failed to connect to system bus: %s\n", error->message);
		g_error_free(error);
		exit(1);
	}

	/* GTK stuff */
	window = gtk_dialog_new_with_buttons("Shutdown dialog", NULL, 0,
	                                     "Shutdown", Shutdown,
										 "Reboot", Reboot,
										 "Suspend", Suspend,
										 "Hibernate", Hibernate,
										  NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	result = gtk_dialog_run( (GtkDialog*) window);

	if (result >= 0) {
		dest = names[result][0];
		path = names[result][1];
		interface = names[result][2];
		method = names[result][3];
	}

	if (dest) {
		message = g_dbus_message_new_method_call(NULL, path, interface, method);
		g_dbus_message_set_destination(message, dest);
		if (verbose) {
			gchar* status = g_dbus_message_print(message, 0);
			g_printerr("sending following message:\n%s", status);
			g_free(status);
		}
		reply = g_dbus_connection_send_message_with_reply_sync(connection, 
			                                                   message,
				  						                       0,
															   -1,
															   NULL,
										                       NULL,
										                       &error);
		if (!reply) {
			g_printerr("Failed to send message: %s\n", error->message);
			g_error_free(error);
			exit(1);
		} else if (verbose) {
			gchar* status = g_dbus_message_print(message, 0);
			g_printerr("got response:\n%s", status);
			g_free(status);
		}
	}
	g_object_unref(message);


	return 0;
}
