#include <stdlib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

enum {
	bye_shutdown = 1,
	bye_reboot,
	bye_suspend,
	bye_hibernate,
};

int main(int argc, char *argv[]) {
	GtkWidget *window = NULL;
	gint result;
	GDBusConnection *connection = NULL;
	GDBusMessage *message = NULL, *reply = NULL;
	GError *error = NULL;
	char *dest = NULL, *path = NULL, *interface = NULL, *method = NULL;

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
	                                     "Shutdown", bye_shutdown,
										 "Reboot", bye_reboot,
										 "Suspend", bye_suspend,
										 "Hibernate", bye_hibernate,
										  NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	result = gtk_dialog_run( (GtkDialog*) window);

	switch (result) {
		case bye_shutdown:
			dest = "org.freedesktop.ConsoleKit";
			path = "/org/freedesktop/ConsoleKit/Manager";
			interface = "org.freedesktop.ConsoleKit.Manager";
			method = "Stop";
			break;
		case bye_reboot:
			dest = "org.freedesktop.ConsoleKit";
			path = "/org/freedesktop/ConsoleKit/Manager";
			interface = "org.freedesktop.ConsoleKit.Manager";
			method = "Restart";
			break;
		case bye_suspend:
			dest = "org.freedesktop.UPower";
			path = "/org/freedesktop/UPower";
			interface = "org.freedesktop.UPower";
			method = "Suspend";
			break;
		case bye_hibernate:
			dest = "org.freedesktop.UPower";
			path = "/org/freedesktop/UPower";
			interface = "org.freedesktop.UPower";
			method = "Hibernate";
	}

	if (dest) {
		message = g_dbus_message_new_method_call(NULL, path, interface, method);
		g_dbus_message_set_destination(message, dest);
		gchar* status = g_dbus_message_print(message, 0);
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
			gchar* status = g_dbus_message_print(message, 0);
			g_printerr("got response:\n%s", status);
			g_free(status);
		}
	}
//	g_unref(message);


	return 0;
}
