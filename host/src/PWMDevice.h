#pragma once

#include "USBDevice.h"
#include <array>

class PWMDevice : public USBDevice {
	static USBDevice::Config conf;
public:
	PWMDevice() : USBDevice(conf) {}
	
	typedef enum {
		CMD_ECHO = 0,
		CMD_PWM_READ = 1,
		CMD_PWM_SET = 2
	} cmd;
	std::array<int, 2> getValues();
	void setValues(std::array<int, 2>);
};
