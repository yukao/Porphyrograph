# from socket import socket, AF_INET, SOCK_DGRAM
# s = socket(AF_INET, SOCK_DGRAM)
# msg = ("/Hello").encode('utf-8')  # socket.sendto() takes bytes as input, hence we must encode the string first.
# s.sendto(msg, ('localhost', 9000)) 

# Import needed modules from osc4py3
from pythonosc.udp_client import SimpleUDPClient
import time

ip = "127.0.0.1"
port = 1337

client = SimpleUDPClient("127.0.0.1", 9000)  # Create client

# Periodically call osc4py3 processing method in your event loop.
n = 0
while n < 10:
    # You can send OSC messages from your event loop too
    print(n)

    client.send_message("/pen_radius", n)   # Send float message
    client.send_message("/pen_grey", n*0.1)  

    time. sleep(1) 

    n += 1

# #server
# import math

# from pythonosc.dispatcher import Dispatcher
# from pythonosc import osc_server

# def print_volume_handler(unused_addr, args, volume):
#   print("[{0}] ~ {1}".format(args[0], volume))

# def print_compute_handler(unused_addr, args, volume):
#   try:
#     print("[{0}] ~ {1}".format(args[0], args[1](volume)))
#   except ValueError: pass

# if __name__ == "__main__":
#   dispatcher = Dispatcher()
#   dispatcher.map("/pen_radius", print)
#   dispatcher.map("/pen_color", print_volume_handler, "Volume")
#   dispatcher.map("/pen_grey", print_compute_handler, "Log volume", math.log)

#   server = osc_server.ThreadingOSCUDPServer(
#       ("127.0.0.1", 9000), dispatcher)
#   print("Serving on {}".format(server.server_address))
#   server.serve_forever()