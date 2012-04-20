bye - dead simple GTK/DBus shutdown dialog
------------------------------------------

`Bye` is a simple application that does nothing else but to show a shutdown
dialog. This dialog gives you the choice to
* shut down your system,
* reboot your system,
* suspend to RAM or to
* hibernate (suspend to disk).

Dependencies:
* GTK3 or GTK2
* DBus
* for shutting down and rebooting: ConsoleKit
* for suspending and hibernating: UPower

Compiling:
	$ make

A `make install` target will follow.

If you run `bye`, please make sure the following conditions are met:
1. Your session is registered with ConsoleKit.
2. `dbus-daemon` is running.
3. You are authorized by PolicyKit to perform the respective action.
