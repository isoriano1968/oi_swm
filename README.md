OpenIndiana (Solaris) Software Manager

The idea is of having a simple package manager for OpenIndiana (Solaris) with a GUI that handles package management via local package json catalogs and interacts with the python pkg utility via dbus messages.

Areas that still need investigation:

- The gateway to the python pkg utility (/usr/lib/python3.9/vendor-packages/pkg/client/api.py) does not exist at all and would need a dbus interface to communicate with the OI SWM via messages
- GUI is just a prototype
- C code is a mess
- etc.

Wann help? Sure feel free to contribute and get in touch or join our OI working sessions every Sunday 17:00h CEST on https://meet.chaostreffbern.ch/OpenIndianaCoWorkingSessions

Cheers

Iggi
  