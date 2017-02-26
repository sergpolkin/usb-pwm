#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "usbdrv/usbdrv.h"

//---------------------------------------------------------------------------
#define CMD_ECHO     0
#define CMD_PWM_READ 1
#define CMD_PWM_SET  2
#define CMD_PWM_EEPROM_READ 3
#define CMD_PWM_EEPROM_SET  4
#define CMD_IO8_READ  5
#define CMD_IO8_WRITE 6

uint16_t EEMEM pwm_eeprom[2] = { 127, 127 };

#define ledOn()  PORTD |=  (1<<PD5)
#define ledOff() PORTD &= ~(1<<PD5)
//---------------------------------------------------------------------------
static void gpio_init(void);
static void pwm_init(void);
usbMsgLen_t usbFunctionSetup(uchar[8]);
//---------------------------------------------------------------------------
int main(void) {
	gpio_init();
	/* From the avr usb doku wiki: Call usbDeviceDisconnect(), wait several
	* 100 milliseconds and then call usbDeviceConnect(). This enforces
	* (re-)enumeration of the device. In theory, you don't need this, but
	* it prevents inconsistencies between host and device after hardware
	* or watchdog resets. */

	usbDeviceDisconnect();
	_delay_ms(500.0);
	usbDeviceConnect();

	/* This function must be called before interrupts are enabled and the main
	* loop is entered. */
	usbInit();

	pwm_init();

	/* All set up, enable interrupts and go. */
	sei();

	while (1) { /* We should never exit, or should we? */
	/* This function must be called at regular intervals from the main loop.
	 * Maximum delay between calls is somewhat less than 50ms (USB timeout for
	 * accepting a Setup message). Otherwise the device will not be recognized.
	 * If other functions are called from the USB code, they will be called from
	 * usbPoll(). */
	usbPoll();
	ledOff();
	}
	/* Never reached */
	return 0;
}
//---------------------------------------------------------------------------
void gpio_init(void) {
	// Red LED
	DDRD |= (1<<PD5);
	ledOn();
	// PWM
	DDRB |= (1<<PB4) | (1<<PB3);
	PORTB |= (1<<PB4) | (1<<PB3);
}
//---------------------------------------------------------------------------
void pwm_init(void) {
	// Fast PWM, 8-bit
	TCCR1A  = (0<<WGM11) | (1<<WGM10);
	TCCR1B  = (0<<WGM13) | (1<<WGM12);
	TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
	TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
	TCCR1B |= (0<<CS12) | (0<<CS11) | (1<<CS10); // No prescaling

	OCR1A = eeprom_read_word(&pwm_eeprom[0]);
	OCR1B = eeprom_read_word(&pwm_eeprom[1]);
}

//---------------------------------------------------------------------------
/* This function is called when the driver receives a SETUP transaction from
 * the host which is not answered by the driver itself (in practice: class and
 * vendor requests). All control transfers start with a SETUP transaction where
 * the host communicates the parameters of the following (optional) data
 * transfer. The SETUP data is available in the 'data' parameter which can
 * (and should) be casted to 'usbRequest_t *' for a more user-friendly access
 * to parameters. [...]
 * in short: Set usbMsgPtr to your answer, and return number of bytes.
 * return USB_NO_MSG to supply the long answer packets with the usbFunctionRead. */
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void *)data;
	static uchar replyBuf[8];
	usbMsgPtr = replyBuf;
	uchar len = 0;
	ledOn();
	if (rq->bRequest == CMD_ECHO) {
		replyBuf[0] = rq->wValue.bytes[0];
		replyBuf[1] = rq->wValue.bytes[1];
		len = 2;
	}
	else if (rq->bRequest == CMD_PWM_READ) {
		uint16_t *pwmReply = (uint16_t*)replyBuf;
		pwmReply[0] = OCR1A;
		pwmReply[1] = OCR1B;
		len = 4;
	}
	else if (rq->bRequest == CMD_PWM_SET) {
		OCR1A = rq->wValue.word;
		OCR1B = rq->wIndex.word;
	}
	else if (rq->bRequest == CMD_PWM_EEPROM_READ) {
		uint16_t *eepromReply = (uint16_t*)replyBuf;
		eepromReply[0] = eeprom_read_word(&pwm_eeprom[0]);
		eepromReply[1] = eeprom_read_word(&pwm_eeprom[1]);
		len = 4;
	}
	else if (rq->bRequest == CMD_PWM_EEPROM_SET) {
		eeprom_write_word(&pwm_eeprom[0], rq->wValue.word);
		eeprom_write_word(&pwm_eeprom[1], rq->wIndex.word);
	}
	else if (rq->bRequest == CMD_IO8_READ) {
		replyBuf[0] = _SFR_IO8(rq->wValue.bytes[0]);
		len = 1;
	}
	else if (rq->bRequest == CMD_IO8_WRITE) {
		_SFR_IO8(rq->wValue.bytes[0]) = rq->wValue.bytes[1];
	}
	return len;
}

/* This function is called by the driver to ask the application for a control
 * transfer's payload data (control-in). It is called in chunks of up to 8
 * bytes each. You should copy the data to the location given by 'data' and
 * return the actual number of bytes copied. If you return less than requested,
 * the control-in transfer is terminated. If you return 0xff, the driver aborts
 * the transfer with a STALL token. */
// uchar usbFunctionRead(uchar *data, uchar len) {
//   return len;
// }
