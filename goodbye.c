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

static bool verbose = false;
static const char* names[4][4] = {
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
	const gchar* label;
	GDBusConnection *connection = NULL;
	GDBusMessage *message = NULL, *reply = NULL;
	GError *error = NULL;
	int action = -1;

	/* figure out which button was pressed */
	label = gtk_button_get_label(GTK_BUTTON(widget));
	for (int i = 0; i < 4; ++i) {
		if (!strcmp(label, names[i][3])) {
			action = i;
			break;
		}
	}

	assert(action >= 0);

	dest = names[action][0];
	path = names[action][1];
	interface = names[action][2];
	method = names[action][3];

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

	/* GTK stuff */
	//window = gtk_dialog_new_with_buttons("Shutdown dialog", NULL, 0,
	//         "Shutdown", Shutdown, "Reboot", Reboot, "Suspend", Suspend,
	//         "Hibernate", Hibernate, NULL);
	/* create the window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Goodbye");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);

	/* create the buttons */
	for (int i = 0; i < 4; ++i)
		buttons[i] = gtk_button_new_with_label(names[i][3]);

	/* create the box */
	box = gtk_hbutton_box_new();
	for (int i = 0; i < 4; ++i) 
		gtk_container_add(GTK_CONTAINER(box), buttons[i]);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_CENTER);
	gtk_container_add(GTK_CONTAINER(window), box);

	/* add the signals */
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	for (int i = 0; i < 4; ++i)
		g_signal_connect(buttons[i], "clicked", G_CALLBACK(handle_clicked), NULL);

	/* show window and quit */
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
