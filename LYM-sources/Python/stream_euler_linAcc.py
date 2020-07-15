# usage: python stream_acc.py [mac1] [mac2] ... [mac(n)]
from __future__ import print_function
from mbientlab.metawear import MetaWear, libmetawear, parse_value
from mbientlab.metawear.cbindings import *
from time import sleep
from time import time
from threading import Event

from signal import signal, SIGINT
from sys import exit

import platform
import sys

#OSC
from pythonosc import udp_client

if sys.version_info[0] == 2:
    range = xrange

MAC_ADDRESSES = ['E9:0F:0A:C5:4D:66', 'C7:7D:6A:41:B4:15']
DEVICE_IDS = ['mw1', 'mw2']
# MAC_ADDRESSES = ['C7:7D:6A:41:B4:15']
# DEVICE_IDS = ['mw2']

SAMPLE_FREQUENCY = 0.15   # 60 FPS

class State:
    def __init__(self, device, index):
        self.device = device
        self.index = index
        self.samples_EULER_ANGLE = 0
        self.samples_LINEAR_ACC = 0
        self.callback = FnVoid_VoidP_DataP(self.data_handler)
        self.last_posting_time_EULER_ANGLE = time()
        self.last_posting_time_LINEAR_ACC = time()

    def data_handler(self, ctx, data):
        # from cbindings: 
        # class EULERAngles(Structure):
        #     _fields_ = [
        #         ("heading" , c_float),
        #         ("pitch" , c_float),
        #         ("roll" , c_float),
        #         ("yaw" , c_float)
        #     ]
        # class CartesianFloat(Structure):
        #     _fields_ = [
        #         ("x" , c_float),
        #         ("y" , c_float),
        #         ("z" , c_float)
        #     ]
        values = parse_value(data, n_elem = 1)
        curTime = time()
        if values.__repr__().find("heading") >= 0:
            post_interval_EULER_ANGLE = curTime - self.last_posting_time_EULER_ANGLE
            if post_interval_EULER_ANGLE >= SAMPLE_FREQUENCY:
                self.last_posting_time_EULER_ANGLE = curTime;
                # print("device: %s, EULER %s\n" % (self.device.address, values))
                print("device: %s (%d), EULER %f %f %f %f\n" % (DEVICE_IDS[self.index], self.index, values.heading, values.pitch, values.roll, values.yaw))
                client.send_message("/mw_euler", [self.index, values.heading, values.pitch, values.roll, values.yaw])
                self.samples_EULER_ANGLE += 1
        else:
            post_interval_LINEAR_ACC = curTime - self.last_posting_time_LINEAR_ACC
            if post_interval_LINEAR_ACC >= SAMPLE_FREQUENCY:
                self.last_posting_time_LINEAR_ACC = curTime;
                # print("device: %s, ACCEL %s\n" % (self.device.address, values))
                print("device: %s (%d), ACCEL %f %f %f\n" % (DEVICE_IDS[self.index], self.index, values.x, values.y, values.z))
                client.send_message("/mw_linAcc", [self.index, values.x, values.y, values.z])
                self.samples_LINEAR_ACC += 1

def handlerForInterruption(signal_received, frame):
    # Handle breaks
    print('SIGINT or CTRL-C detected. Exiting gracefully')
    disconnect()
    exit(0)

def disconnect():
    for s in states:
        libmetawear.mbl_mw_sensor_fusion_stop(s.device.board);

        signal_EULER_ANGLE = libmetawear.mbl_mw_sensor_fusion_get_data_signal(s.device.board, SensorFusionData.EULER_ANGLE);
        signal_LINEAR_ACC = libmetawear.mbl_mw_sensor_fusion_get_data_signal(s.device.board, SensorFusionData.LINEAR_ACC);
        libmetawear.mbl_mw_datasignal_unsubscribe(signal_EULER_ANGLE)
        libmetawear.mbl_mw_datasignal_unsubscribe(signal_LINEAR_ACC)
        libmetawear.mbl_mw_debug_disconnect(s.device.board)

    print("Total samples emitted")
    for s in states:
        print("    %s -> %d" % (s.device.address, s.samples_EULER_ANGLE + s.samples_LINEAR_ACC))

if __name__ == '__main__':
    # Tell Python to run the handlerForInterruption() function when SIGINT is recieved
    signal(SIGINT, handlerForInterruption)

    client = udp_client.SimpleUDPClient('127.0.0.1', 9000)

    states = []
    for i in range(len(MAC_ADDRESSES)):
        d = MetaWear(MAC_ADDRESSES[i])
        d.connect()
        print("Connected to " + d.address)
        states.append(State(d, i))

    for s in states:
        print("Configuring device")
        libmetawear.mbl_mw_settings_set_connection_parameters(s.device.board, 7.5, 7.5, 0, 6000)
        sleep(1.5)

        libmetawear.mbl_mw_sensor_fusion_set_mode(s.device.board, SensorFusionMode.NDOF);
        #  * Supported acceleration ranges (sensitivity of the sensor)
        #     MBL_MW_SENSOR_FUSION_ACC_RANGE_2G = 0,
        #     MBL_MW_SENSOR_FUSION_ACC_RANGE_4G,
        #     MBL_MW_SENSOR_FUSION_ACC_RANGE_8G,
        #     MBL_MW_SENSOR_FUSION_ACC_RANGE_16G
        libmetawear.mbl_mw_sensor_fusion_set_acc_range(s.device.board, SensorFusionAccRange._8G)
        #  * Supported rotation ranges (degrees per second)
        #     MBL_MW_SENSOR_FUSION_GYRO_RANGE_2000DPS = 0,
        #     MBL_MW_SENSOR_FUSION_GYRO_RANGE_1000DPS,
        #     MBL_MW_SENSOR_FUSION_GYRO_RANGE_500DPS,
        #     MBL_MW_SENSOR_FUSION_GYRO_RANGE_250DPS
        libmetawear.mbl_mw_sensor_fusion_set_gyro_range(s.device.board, SensorFusionGyroRange._2000DPS)
        libmetawear.mbl_mw_sensor_fusion_write_config(s.device.board)

        signal_EULER_ANGLE = libmetawear.mbl_mw_sensor_fusion_get_data_signal(s.device.board, SensorFusionData.EULER_ANGLE);
        libmetawear.mbl_mw_datasignal_subscribe(signal_EULER_ANGLE, None, s.callback)

        signal_LINEAR_ACC = libmetawear.mbl_mw_sensor_fusion_get_data_signal(s.device.board, SensorFusionData.LINEAR_ACC);
        libmetawear.mbl_mw_datasignal_subscribe(signal_LINEAR_ACC, None, s.callback)

        # SensorFusionData.CORRECTED_ACC: False,
        # SensorFusionData.CORRECTED_GYRO: False,
        # SensorFusionData.CORRECTED_MAG: False,
        # SensorFusionData.QUATERNION: False,
        # SensorFusionData.EULER_ANGLE: False,
        # SensorFusionData.GRAVITY_VECTOR: False
        # SensorFusionData.LINEAR_ACC: False,
        libmetawear.mbl_mw_sensor_fusion_enable_data(s.device.board, SensorFusionData.EULER_ANGLE);
        libmetawear.mbl_mw_sensor_fusion_enable_data(s.device.board, SensorFusionData.LINEAR_ACC);
        libmetawear.mbl_mw_sensor_fusion_start(s.device.board);

        s.last_posting_time_EULER_ANGLE = time();
        s.last_posting_time_LINEAR_ACC = time();

    sleep(30000.0)

    disconnect()