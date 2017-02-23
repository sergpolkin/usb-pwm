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
		CMD_PWM_SET  = 2,
		CMD_PWM_EEPROM_READ = 3,
		CMD_PWM_EEPROM_SET  = 4
	} cmd;

	std::array<int, 2> getValues();
	void setValues(std::array<int, 2>);

	std::array<int, 2> getEEPROM();
	void setEEPROM(std::array<int, 2>);
};
