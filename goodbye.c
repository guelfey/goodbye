/* goodbye - dead simple GTK/DBus shutdown dialog.
 * Copyright (C) 2012 Georg Reinke. All rights reserved.
 *
 * See LICENSE for copying details.
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

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

int main(int argc, char *argv[]) {
	GtkWidget *window = NULL;
	gint result;
	GDBusConnection *connection = NULL;
	GDBusMessage *message = NULL, *reply = NULL;
	GError *error = NULL;
	const char *dest = NULL, *path = NULL, *interface = NULL, *method = NULL;

	gtk_init(&argc, &argv);

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
		gchar* status;
		message = g_dbus_message_new_method_call(NULL, path, interface, method);
		g_dbus_message_set_destination(message, dest);
		status = g_dbus_message_print(message, 0);
		g_printerr("sending following message:\n%s", status);
		g_free(status);
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
		} else {
			status = g_dbus_message_print(message, 0);
			g_printerr("got response:\n%s", status);
			g_free(status);
		}
	}
	g_object_unref(message);


	return 0;
}
