# OI Software Manager python service to interact with api.py
# (c) 2024 Iggi

import dbus
import dbus.service
import dbus.mainloop.glib
from gi.repository import GLib

class SoftwareManagerService(dbus.service.Object):
    def __init__(self, bus_name, object_path):
        dbus.service.Object.__init__(self, bus_name, object_path)

    @dbus.service.method('org.openindiana.swm', in_signature='s', out_signature='s')
    def pkg_install(self,packageName):
        print(f"Install message received for package: {packageName}")
        return f"Installation of {packageName} successful"

    @dbus.service.method('org.openindiana.swm', in_signature='s', out_signature='s')
    def pkg_uninstall(self,packageName):
        print(f"Uninstall message received for package: {packageName}")
        return f"Uninstall of {packageName} successful"

    @dbus.service.method('org.openindiana.swm', in_signature='s', out_signature='s')
    def pkg_update(self,packageName):
        print(f"Update message received for package: {packageName}")
        return f"{packageName} updated"

def main():
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus_name = dbus.service.BusName('org.openindiana.swm', bus=dbus.SessionBus())
    object_path = '/org/openindiana/SoftwareManager'
    sm_service = SoftwareManagerService(bus_name, object_path)
    loop = GLib.MainLoop()
    print("Starting the Software Manager DBus Service...")
    loop.run()

if __name__ == '__main__':
    main()

