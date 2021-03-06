#include "PWMDevice.h"
#include <cstdio>
#include <cstring>

//---------------------------------------------------------------------------
static void cmd_show(PWMDevice&);
static void cmd_set(PWMDevice&, int, int);
static void cmd_eeprom_show(PWMDevice&);
static void cmd_eeprom_set(PWMDevice&, int, int);
static void cmd_io8_read(PWMDevice&, uint8_t addr);
static void cmd_io8_write(PWMDevice&, uint8_t addr, uint8_t value);
static PWMDevice::cmd get_cmd(const char* str);
static void show_help(void);

//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
	PWMDevice pwm;
	if (argc == 1) {
		cmd_show(pwm);
	}
	else if (argc > 1) {
		switch(get_cmd(argv[1])) {
			case PWMDevice::CMD_PWM_READ:
			cmd_show(pwm);
			break;
			case PWMDevice::CMD_PWM_SET:
			if(argc < 4) {
				printf("use: set <PWM0_VALUE> <PWM1_VALUE>\n");
			}
			else {
				int v0, v1;
				sscanf(argv[2], "%d", &v0);
				sscanf(argv[3], "%d", &v1);
				cmd_set(pwm, v0, v1);
			}
			break;
			case PWMDevice::CMD_PWM_EEPROM_READ:
			cmd_eeprom_show(pwm);
			break;
			case PWMDevice::CMD_PWM_EEPROM_SET:
			if(argc < 4) {
				printf("use: eeprom.set <PWM0_EEPROM_VALUE> <PWM1_EEPROM_VALUE>\n");
			}
			else {
				int v0, v1;
				sscanf(argv[2], "%d", &v0);
				sscanf(argv[3], "%d", &v1);
				cmd_eeprom_set(pwm, v0, v1);
			}
			break;
			case PWMDevice::CMD_IO8_READ:
			if(argc < 3) {
				printf("use: io8.read <ADDR>\n");
			}
			else {
				int addr;
				if(argv[2][0] == 'x')
					sscanf(argv[2]+1, "%x", &addr);
				else
					sscanf(argv[2], "%d", &addr);
				cmd_io8_read(pwm, addr);
			}
			break;
			case PWMDevice::CMD_IO8_WRITE:
			if(argc < 4) {
				printf("use: io8.write <ADDR> <VALUE>\n");
			}
			else {
				int addr;
				if(argv[2][0] == 'x')
					sscanf(argv[2]+1, "%x", &addr);
				else
					sscanf(argv[2], "%d", &addr);
				int value;
				if(argv[3][0] == 'x')
					sscanf(argv[3]+1, "%x", &value);
				else
					sscanf(argv[3], "%d", &value);
				cmd_io8_write(pwm, addr, value);
			}
			break;
			default:
			show_help();
			break;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
void cmd_show(PWMDevice& pwm) {
	auto v = pwm.getValues();
	printf("PWM0: %d; PWM1: %d\n", v[0], v[1]);
}
void cmd_set(PWMDevice& pwm, int v0, int v1) {
	pwm.setValues({v0, v1});
	printf("Set values: PWM0: %d; PWM1: %d\n", v0, v1);
}
void cmd_eeprom_show(PWMDevice& pwm) {
	auto v = pwm.getEEPROM();
	printf("EEPROM values: PWM0: %d; PWM1: %d\n", v[0], v[1]);
}
void cmd_eeprom_set(PWMDevice& pwm, int v0, int v1) {
	pwm.setEEPROM({v0, v1});
	printf("Set EEPROM values: PWM0: %d; PWM1: %d\n", v0, v1);
}
void cmd_io8_read(PWMDevice& pwm, uint8_t addr) {
	uint8_t r = pwm.io8(addr);
	printf("IO8_READ(0x%02X): %d [0x%02X]\n", addr, r, r);
}
void cmd_io8_write(PWMDevice& pwm, uint8_t addr, uint8_t value) {
	pwm.io8(addr, value);
	printf("IO8_WRITE(0x%02X): %d [0x%02X]\n", addr, value, value);
}
//---------------------------------------------------------------------------
PWMDevice::cmd get_cmd(const char* str) {
	if(std::strncmp(str, "show", 4) == 0) {
		return PWMDevice::CMD_PWM_READ;
	}
	else if(std::strncmp(str, "set", 3) == 0) {
		return PWMDevice::CMD_PWM_SET;
	}
	else if(std::strncmp(str, "eeprom.show", 11) == 0) {
		return PWMDevice::CMD_PWM_EEPROM_READ;
	}
	else if(std::strncmp(str, "eeprom.set", 10) == 0) {
		return PWMDevice::CMD_PWM_EEPROM_SET;
	}
	else if(std::strncmp(str, "io8.read", 8) == 0) {
		return PWMDevice::CMD_IO8_READ;
	}
	else if(std::strncmp(str, "io8.write", 9) == 0) {
		return PWMDevice::CMD_IO8_WRITE;
	}
	return PWMDevice::CMD_ECHO;
}
//---------------------------------------------------------------------------
void show_help(void) {
	printf("usage:\n"
		"\tshow\n"
		"\tset <PWM0_VALUE> <PWM1_VALUE>\n"
		"\teeprom.show\n"
		"\teeprom.set <PWM0_EEPROM_VALUE> <PWM1_EEPROM_VALUE>\n"
		"\tio8.read  <ADDR>\n"
		"\tio8.write <ADDR> <VALUE>\n"
	);
}
