#include <iostream>
#include <libusb-1.0/libusb.h>
#include "pg-all_include.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/ioctl.h>
int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif
/*
#include <iostream>
*/
using namespace std;



ssize_t pg_nb_USBs = 0;
libusb_device** pg_USB_devices = NULL; //pointer to pointer of device, used to retrieve a list of devices
libusb_device** pg_USB_pedals_devices = NULL; //pointer to pointer of device, used to retrieve a list of devices
libusb_context* pg_USB_context = NULL; //a libusb session
libusb_device_handle** pg_USB_pedals_handles;
int pg_nb_USB_pedals = 0;
unsigned char* pg_USB_receiveBuffer = new unsigned char[65535];

void printdev(libusb_device* dev); //prototype of the function

// prints the detail of a USB device
void printdev(libusb_device* dev) {
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		cout << "failed to get device descriptor" << endl;
		return;
	}
	cout << "Number of possible configurations: " << (int)desc.bNumConfigurations << "  ";
	cout << "Device Class: " << (int)desc.bDeviceClass << "  ";
	cout << "VendorID: " << desc.idVendor << "  ";
	cout << "ProductID: " << desc.idProduct << endl;
	libusb_config_descriptor* config;
	libusb_get_config_descriptor(dev, 0, &config);
	cout << "Interfaces: " << (int)config->bNumInterfaces << " ||| ";
	const libusb_interface* inter;
	const libusb_interface_descriptor* interdesc;
	const libusb_endpoint_descriptor* epdesc;
	for (int i = 0; i < (int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		cout << "Number of alternate settings: " << inter->num_altsetting << " | ";
		for (int j = 0; j < inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			cout << "Interface Number: " << (int)interdesc->bInterfaceNumber << " | ";
			cout << "Number of endpoints: " << (int)interdesc->bNumEndpoints << " | ";
			for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				cout << "Descriptor Type: " << (int)epdesc->bDescriptorType << " | ";
				cout << "EP Address: " << (int)epdesc->bEndpointAddress << " | ";
			}
		}
	}
	cout << endl << endl << endl;
	libusb_free_config_descriptor(config);
}

// USB context initialization and list of USB devices retrieval
int pg_init_USB(void) {
	int r; //for return values
	r = libusb_init(&pg_USB_context); //initialize a library session
	if (r < 0) {
		cout << "Init Error " << r << endl; //there was an error
		return 1;
	}
//	libusb_set_debug(pg_USB_context, 3); //set verbosity level to 3, as suggested in the documentation
	pg_nb_USBs = libusb_get_device_list(pg_USB_context, &pg_USB_devices); //get the list of devices
	if (pg_nb_USBs < 0) {
		cout << "Get Device Error" << endl; //there was an error
	}
	cout << pg_nb_USBs << " Devices in list." << endl; //print total number of usb devices
	return 0;
}

// relases the list of USB devices and the handles to the pedals and the context
int pg_release_USB(void) {
	libusb_free_device_list(pg_USB_devices, 1); //free the list, unref the devices in it

	ssize_t i; //for iterating through the list
	for (i = 0; i < pg_nb_USB_pedals; i++) {
		int r = libusb_release_interface(pg_USB_pedals_handles[i], 0); //release the claimed interface
		if (r != 0) {
			cout << "Cannot Release Pedal Interface" << i << endl;
			return 1;
		}
		cout << "Released Pedal Interface" << i << endl;
		libusb_close(pg_USB_pedals_handles[i]); //close the device we opened
	}

	libusb_exit(pg_USB_context); //close the session
	free(pg_USB_receiveBuffer);
	return 0;
}

// lists details about the USB devices which are currently active
int pg_list_USB(void) {
	ssize_t i; //for iterating through the list
	for (i = 0; i < pg_nb_USBs; i++) {
		libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(pg_USB_devices[i], &desc);
		if (r < 0) {
			cout << "failed to get device descriptor" << endl;
			return 1;
		}
		// if (desc.idProduct == 57382) {
			printdev(pg_USB_devices[i]); //print specs of this device
		// }
	}
	return 0;
}

// gets the handles to the USB pedals through vendor and product IDs (which can be printed through pg_list_USB
// USB PEDALS
// Number of possible configurations : 1  Device Class : 0  VendorID : 6790  ProductID : 57382
// Interfaces : 2 ||| 
// Number of alternate settings : 1 | Interface Number : 0 | Number of endpoints : 1 | Descriptor Type : 5 | EP Address : 129 | 
// Number of alternate settings : 1 | Interface Number : 1 | Number of endpoints : 2 | Descriptor Type : 5 | EP Address : 130 | Descriptor Type : 5 | EP Address : 2 |
int pg_find_USB_pedals(void) {
	ssize_t i; //for iterating through the list
	pg_nb_USB_pedals = 0;
	for (i = 0; i < pg_nb_USBs; i++) {
		libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(pg_USB_devices[i], &desc);
		if (r < 0) {
			cout << "failed to get device descriptor" << endl;
			return 1;
		}
		if (desc.idProduct == 57382 && desc.idVendor == 6790) {
			printdev(pg_USB_devices[i]); //print specs of this device
			pg_nb_USB_pedals++;
		}
	}
	pg_USB_pedals_devices = new libusb_device*[pg_nb_USB_pedals];
	pg_USB_pedals_handles = new libusb_device_handle * [pg_nb_USB_pedals];
	int ind_USB_pedals = 0;
	for (i = 0; i < pg_nb_USBs; i++) {
		libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(pg_USB_devices[i], &desc);
		if (r < 0) {
			cout << "failed to get device descriptor" << endl;
			return 1;
		}
		if (desc.idProduct == 57382 && desc.idVendor == 6790) {
			//printdev(pg_USB_devices[i]); //print specs of this device
			if (ind_USB_pedals < pg_nb_USB_pedals) {
				pg_USB_pedals_devices[ind_USB_pedals] = pg_USB_devices[i];
				libusb_open(pg_USB_pedals_devices[ind_USB_pedals], &(pg_USB_pedals_handles[ind_USB_pedals]));
				if (pg_USB_pedals_handles[ind_USB_pedals] == NULL)
					cout << "Cannot open pedal device" << ind_USB_pedals << endl;
				else
					cout << "Pedal Device Opened" << ind_USB_pedals << endl;
				ind_USB_pedals++;
			}
		}
	}
	return 0;
}

// reads the data of the selected pedal
// USB PEDALS
// Number of possible configurations : 1  Device Class : 0  VendorID : 6790  ProductID : 57382
// Interfaces : 2 ||| 
// Number of alternate settings : 1 | Interface Number : 0 | Number of endpoints : 1 | Descriptor Type : 5 | EP Address : 129 | 
// Number of alternate settings : 1 | Interface Number : 1 | Number of endpoints : 2 | Descriptor Type : 5 | EP Address : 130 | Descriptor Type : 5 | EP Address : 2 |
#ifdef _WIN32
int getch_noblock() {
	if (_kbhit())
		return _getch();
	else
		return -1;
}
#endif

int read_USB_pedal(int indPedal) {
	libusb_device_handle* dev_handle = NULL;
	if (indPedal < pg_nb_USB_pedals) {
		dev_handle = pg_USB_pedals_handles[indPedal];
	}
	else {
		cout << "Access Pedal Interface" << indPedal << "only" << pg_nb_USB_pedals << "are available" << endl;
		return 1;
	}

	int actual_length; //used to find out how many bytes were read	
	
	if (libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		cout << "Kernel Driver Active" << endl;
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout << "Kernel Driver Detached!" << endl;
	}
	int r = libusb_claim_interface(dev_handle, 0); //claim interface 0 -> Interfaces - 1
	if (r < 0) {
		cout << "Cannot Claim Interface" << endl;
		return 1;
	}
	cout << "Claimed Interface" << endl;

	cout << "Reading Data..." << endl;
	r = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), pg_USB_receiveBuffer, 65535, &actual_length, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
	if (r == 0) //we read successfully
		cout << "Reading Successful on pedal" << indPedal << actual_length << endl;
	else
		cout << "Read Error on Pedal" << indPedal << endl;

	int ich;
#ifdef _WIN32
	ich = getch_noblock();
#else
	ich = _kbhit();
#endif
	if (ich == -1) {
		cout << "no char input" << endl;
	}
	else {
		cout << "read char" << char(ich) << endl;
	}

	return 0;
}

