goodbye - dead simple GTK/D-Bus shutdown dialog
------------------------------------------

`goodbye` is a simple application that does nothing else but to show a shutdown
dialog. This dialog gives you the choice to
* shut down your system,
* reboot your system,
* suspend to RAM or to
* hibernate (suspend to disk).

### Builtime dependencies:
* a C compiler 
* Make
* GTK2 (GTK3 should also work, but no support is guaranteed)

### Runtime dependencies:
* GTK2 / GTK3
* D-Bus
* for shutting down and rebooting: ConsoleKit
* for suspending and hibernating: UPower

### Compiling/Installing:
```
$ make
# make install
```

### Running:
If you run `goodbye`, please make sure the following conditions are met:

1. Your session is registered with ConsoleKit.

2. The D-Bus system bus is accessible (`dbus-daemon --system` is running).

3. You are authorized by PolicyKit to perform the respective action.

### License:
This software is licensed under the MIT/X Consortium License.
See LICENSE for more details.

### Contact:
Georg Reinke `guelfey at googlemail dot com`
