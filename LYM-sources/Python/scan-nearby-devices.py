from mbientlab.warble import *
from mbientlab.metawear import *
from threading import Event

e = Event()
address = None
def device_discover_task(result):
    global address
    if (result.has_service_uuid(MetaWear.GATT_SERVICE)):
        # grab the first discovered metawear device
        address = result.mac
        e.set()

BleScanner.set_handler(device_discover_task)
BleScanner.start()
e.wait()

BleScanner.stop()
