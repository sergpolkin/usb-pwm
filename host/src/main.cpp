#include "PWMDevice.h"
#include <cstdio>
#include <cstring>

static void cmd_show(PWMDevice&);
static void cmd_set(PWMDevice&, int, int);
static PWMDevice::cmd get_cmd(const char* str);

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
		}
	}
	return 0;
}

void cmd_show(PWMDevice& pwm) {
	auto v = pwm.getValues();
	printf("PWM0: %d; PWM1: %d\n", v[0], v[1]);
}
void cmd_set(PWMDevice& pwm, int v0, int v1) {
	pwm.setValues({v0, v1});
	printf("Set values: PWM0: %d; PWM1: %d\n", v0, v1);
}
PWMDevice::cmd get_cmd(const char* str) {
	if(std::strncmp(str, "show", 4) == 0) {
		return PWMDevice::CMD_PWM_READ;
	}
	else if(std::strncmp(str, "set", 3) == 0) {
		return PWMDevice::CMD_PWM_SET;
	}
	return PWMDevice::CMD_ECHO;
}