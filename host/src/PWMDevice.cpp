#include "PWMDevice.h"

#include <iostream>
#include <cstring>
#include <stdexcept>

USBDevice::Config PWMDevice::conf = {
	0x16C0, 0x05DC, "serg", "pwm-fan"
};

//---------------------------------------------------------------------------
std::array<int, 2> PWMDevice::getValues() {
	uint8_t rType = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
	uint16_t buf[2] = {0, 0};
	int r = libusb_control_transfer(m_handle, rType,
		CMD_PWM_READ, 0, 0, (uint8_t*)buf, 4, 3000);
	if(r < 4) {
		std::cerr << "PWMDevice::getValues: error" << std::endl;
	}
	return {buf[0], buf[1]};
}
//---------------------------------------------------------------------------
void PWMDevice::setValues(std::array<int, 2> values) {
	uint8_t rType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
	int r = libusb_control_transfer(m_handle, rType,
		CMD_PWM_SET, values[0], values[1], nullptr, 0, 3000);
	if(r < 0) {
		std::cerr << "PWMDevice::getValues: error" << std::endl;
	}
}
