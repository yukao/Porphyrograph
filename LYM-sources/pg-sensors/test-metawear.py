from mbientlab.metawear import MetaWear, libmetawear, parse_value
from mbientlab.metawear.cbindings import *
from time import sleep

import platform
import sys

class State:
    def __init__(self, device, index):
        self.device = device
        self.samples = 0
        self.index = index

states = []
MAC_addresses = ['E9:0F:0A:C5:4D:66', 'C7:7D:6A:41:B4:15']
for i in range(len(MAC_addresses)):
    d = MetaWear(MAC_addresses[i])
    d.connect()
    print("Connected to " + d.address)
    states.append(State(d, i))

for s in states:
    print("Configuring device ", s.device.address)
    libmetawear.mbl_mw_settings_set_connection_parameters(s.device.board, 7.5, 7.5, 0, 6000)
    sleep(1.5)

for s in states:
	pattern= LedPattern(repeat_count= Const.LED_REPEAT_INDEFINITELY)
	libmetawear.mbl_mw_led_load_preset_pattern(byref(pattern), LedPreset.SOLID)
	if s.index == 0:
		libmetawear.mbl_mw_led_write_pattern(s.device.board, byref(pattern), LedColor.GREEN)
	else:
		libmetawear.mbl_mw_led_write_pattern(s.device.board, byref(pattern), LedColor.RED)
	libmetawear.mbl_mw_led_play(s.device.board)

	sleep(1.0)

	libmetawear.mbl_mw_led_stop_and_clear(s.device.board)
	libmetawear.mbl_mw_haptic_start_buzzer(s.device.board, 1000)
	sleep(1.0)

	s.device.disconnect()
	sleep(1.0)
