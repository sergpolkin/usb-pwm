#pragma once

#include <libusb.h>

class USBDevice {
public:
	typedef struct {
		uint16_t vid, pid;
		char manufacturer[256], product[256];
	} Config;

public:
	USBDevice(const Config&);
	virtual ~USBDevice();
protected:
	libusb_device_handle *m_handle;
	libusb_context *m_ctx;

	bool find_device(const Config&);
};
