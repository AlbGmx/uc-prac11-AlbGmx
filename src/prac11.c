#include <avr/io.h>
#include "UART.h"

#define TERMINAL_OFFSET 14
#define SAMPLE_OFFSET 5
#define VOLT_STAR_OFFSET 1
#define VOLT_OFFSET 50
extern void Delay(uint8_t); /* Prototipo de la función Delay */

void adc_init();
void printSample(char*);
void justifyRight(char str[], uint8_t);
void printVoltInsideColumn(uint16_t);
void printVoltValue(uint16_t);
uint16_t adc_read();

int main(void) {
	UART_Ini(0, 9600, 8, 0, 1);
	adc_init();
	char temp[10];
	uint16_t adc		 = 0;
	uint16_t sample	 = 0;
	uint16_t miliVolts = 0;
	UART_clrscr();

	while (1) {
		adc		 = adc_read();
		miliVolts = (adc * (uint32_t)5000) / 1023;

		UART_putchar('\n');
		itoa(sample++, temp);
		printSample(temp);

		printVoltInsideColumn(miliVolts);

		printVoltValue(miliVolts);

		Delay(20);
	}
}

void adc_init() {
	ADMUX	 = (1 << REFS0) | (5 << MUX0);
	ADCSRB = 0;
	ADCSRA = (1 << ADEN) | (7 << ADPS2);
	DIDR0	 = (1 << ADC5D);
}

uint16_t adc_read() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return (ADCL | ADCH << 8);
}

void justifyRight(char str[], uint8_t size) {
   uint8_t digits = 0;
	for (uint8_t i = 0; i < size; i++)
		if (str[i] >= '0' && str[i] <= '9') digits++;

	for (uint8_t i = 0; i < size - digits; i++)
		UART_forwardsX(1);
}

void printSample(char sample[]) {
   justifyRight(sample, 4);

	UART_puts(sample);
	UART_putchar('|');
}

void printVoltInsideColumn(uint16_t miliVolts) {
	UART_color((miliVolts < 2500) ? 31 : 32);
	UART_backwardsX(1);
	UART_forwardsX(miliVolts / 100);
	UART_putchar('*');
   UART_resetColor();
}

void printVoltValue(uint16_t miliVolts) {
	char temp[10];
	// Reset X cursor position
	UART_backwardsX(255);
	// As my terminal prints time in the format "HH:MM:SS:MMS >" before any new line, I need to add 14 places to the
	// cursor
	UART_forwardsX(SAMPLE_OFFSET + VOLT_OFFSET + VOLT_STAR_OFFSET);
	UART_putchar('|');
	itoa(miliVolts, temp);

   justifyRight(temp, 4);

   if (miliVolts > 999) {
      temp[2] = temp[1];
      temp[1] = '.';
   }
   else {
      UART_backwardsX(1);
   }

	UART_puts(temp);
	UART_puts((miliVolts > 999) ? "V" : "mV");
}
