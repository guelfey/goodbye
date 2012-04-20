/* goodbye - dead simple GTK/DBus shutdown dialog.
 * Copyright (C) 2012 Georg Reinke. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
	window = gtk_dialog_new_with_buttons("Bye", NULL, 0,
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
