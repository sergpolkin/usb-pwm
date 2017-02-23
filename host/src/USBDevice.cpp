#include "USBDevice.h"

#include <iostream>
#include <cstring>
#include <stdexcept>

//---------------------------------------------------------------------------
USBDevice::USBDevice(const USBDevice::Config& conf) : m_handle(nullptr) {
	int r;
	r = libusb_init(&m_ctx);
	if (r != 0) {
		std::cerr << "USBDevice: libusb init error: " << r << std::endl;
		throw std::exception();
	}
	libusb_set_debug(m_ctx, 3); //set verbosity level to 3

	if(!find_device(conf)) {
		std::cerr << "USBDevice: can't find driver" << std::endl;
		throw std::exception();
	}

	#if linux
	//find out if kernel driver is attached
	if ((r = libusb_kernel_driver_active(m_handle, 0)) == 1) {
		// try detach it
		if ((r = libusb_detach_kernel_driver(m_handle, 0)) != 0) {
			std::cerr << "USBDevice: can't detach kernel driver: " << r << std::endl;
			throw std::exception();
		}
	}
	#endif
	//claim interface 0 of device
	r = libusb_claim_interface(m_handle, 0);
	if (r != 0) {
		std::cerr << "USBDevice: can't claim interface: " << r << std::endl;
		throw std::exception();
	}
}
USBDevice::~USBDevice() {
	int r = libusb_release_interface(m_handle, 0);
	if (r != 0) {
		std::cerr << "USBDevice: can't release interface: " << r << std::endl;
	}
}
//---------------------------------------------------------------------------
static bool is_fine(const char* manufacturer, const char* product,
	const USBDevice::Config& conf)
{
	bool r1 = std::strncmp(manufacturer, conf.manufacturer,
		std::strlen(conf.manufacturer)) == 0;
	bool r2 = std::strncmp(product, conf.product,
		std::strlen(conf.product)) == 0;
	return r1 && r2;
}
//---------------------------------------------------------------------------
bool USBDevice::find_device(const USBDevice::Config& conf) {
	libusb_device **list;
	int cnt = libusb_get_device_list(m_ctx, &list);
	bool find = false;
	if(cnt < 0) {
		std::cerr << "find_device: libusb_get_device_list error" << std::endl;
		throw std::exception();
	}
	for (int i = 0; i < cnt; i++) {
		libusb_device *dev = list[i];
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(dev, &desc);
		if (rc != 0) {
			std::cerr << "find_device: libusb_get_device_descriptor error" << std::endl;
			throw std::exception();
		}
		if(desc.idVendor == conf.vid && desc.idProduct == conf.pid) {
			libusb_device_handle *handle;
			char manufacturer[256], product[256];
			rc = libusb_open(dev, &handle);
			if (rc != 0) {
				std::cerr << "find_device: libusb_open error" << std::endl;
				throw std::exception();
			}
			rc = libusb_reset_device(handle);

			manufacturer[0] = 0;
			rc = libusb_get_string_descriptor_ascii(handle,
				desc.iManufacturer, (unsigned char*)manufacturer, 256);
			if (rc < 0) {
				std::cerr << "find_device: get manufacturer error" << std::endl;
			}

			product[0] = 0;
			rc = libusb_get_string_descriptor_ascii(handle,
				desc.iProduct, (unsigned char*)product, 256);
			if (rc < 0) {
				std::cerr << "find_device: get product error" << std::endl;
			}

			if(is_fine(manufacturer, product, conf)) {
				find = true;
				m_handle = handle;
				break;
			}

			libusb_close(handle);
		}
	}
	libusb_free_device_list(list, 1);
	return find;
}
