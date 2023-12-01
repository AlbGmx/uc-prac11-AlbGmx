#include <avr/io.h>

#include "UART.h"
#define F_OSC (uint32_t)16000000
#define UCSZ_OFFSET 5

volatile uint8_t *regUBRRH[4] = {&UBRR0H, &UBRR1H, &UBRR2H, &UBRR3H};
volatile uint8_t *regUBRRL[4] = {&UBRR0L, &UBRR1L, &UBRR2L, &UBRR3L};
volatile uint8_t *regUSCRA[4] = {&UCSR0A, &UCSR1A, &UCSR2A, &UCSR3A};
volatile uint8_t *regUCSRB[4] = {&UCSR0B, &UCSR1B, &UCSR2B, &UCSR3B};
volatile uint8_t *regUCSRC[4] = {&UCSR0C, &UCSR1C, &UCSR2C, &UCSR3C};
volatile uint8_t *regUDR[4]	= {&UDR0, &UDR1, &UDR2, &UDR3};

// Prototypes
// Initialization

// Consider:
// 1. The COM port (0 to 3)
// 2. The baudrate, may be non standard
// 3. The size of the data (5 to 8 bits)
// 4. The parity (0: Disabled, 1: Odd, 2: Even)
// 5. The number of stop bits (1 or 2)
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop) {
	if (com <= 3) {
		// UBRR Formula => UBRRH:UBRRL = (F_OSC / (16 * baudrate)) - 1
		uint32_t ubrrValue	 = F_OSC / ((uint32_t)16 * baudrate) - 1;
		uint8_t paritySize[3] = {0, (1 << UPM01) | (1 << UPM00), (1 << UPM01)};

		*regUBRRH[com] = (uint8_t)(ubrrValue >> 8);
		*regUBRRL[com] = (uint8_t)ubrrValue;
		*regUCSRC[com] &= ~(3 << UCSZ00);
		*regUCSRC[com] |= (size - UCSZ_OFFSET)
								<< UCSZ00; // Using UCSZ_OFFSET to map the size (5 - 8) to the UCSZ0 bits (0 - 3)
		*regUCSRC[com] &= ~(3 << UPM00);
		*regUCSRC[com] |= paritySize[parity];
		*regUCSRC[com] = (stop == 1) ? (*regUCSRC[com] & ~(1 << USBS0)) : (*regUCSRC[com] | (1 << USBS0));
		*regUCSRB[com] |= (1 << TXEN0) | (1 << RXEN0);
	}
}

// Send
void UART_puts(char *str) {
	while (*str)
		UART_putchar(*str++);
}

void UART_putchar(char data) {
	while (!(*regUSCRA[0] & (1 << UDRE0)))
		;
	*regUDR[0] = data;
}

// Received
char UART_getchar() {
	while (!(*regUSCRA[0] & (1 << RXC0)))
		;
	return *regUDR[0];
}

// Escape sequences
void UART_clrscr() { UART_puts("\033[2J"); }

void UART_gotoxy(uint8_t x, uint8_t y) {
	char str[4];
	itoa(y, str);
	UART_putchar('\033');
	UART_putchar('[');
	UART_puts(str);
	UART_putchar(';');
	itoa(x, str);
	UART_puts(str);
	UART_putchar('H');
}
void UART_forwardsX(uint8_t x) {
	char str[4];
	UART_putchar('\033');
	UART_putchar('[');
	itoa(x, str);
	UART_puts(str);
	UART_putchar('C');
}

void UART_backwardsX(uint8_t x) {
	char str[4];
	UART_putchar('\033');
	UART_putchar('[');
	itoa(x, str);
	UART_puts(str);
	UART_putchar('D');
}

void UART_color(uint8_t color) {
	char temp[4];
	UART_putchar('\033');
	UART_putchar('[');
	itoa(color, temp);
	UART_puts(temp);
	UART_putchar('m');
}

void UART_resetColor() { UART_puts("\033[39m\033[49m"); }

// Utils
void itoa(uint32_t number, char *str) {
	uint8_t index		 = 0;
	uint32_t remainder = 0;
	if (number != 0) {
		while (number != 0) {
			remainder	 = number % 10;
			str[index++] = (remainder > 9) ? (remainder - 10) + 'a' : remainder + '0';
			number /= 10;
		}
		str[index] = '\0';
		uint8_t i  = 0;
		index--;
		while (i < index) {
			char temp	 = str[i];
			str[i++]		 = str[index];
			str[index--] = temp;
		}
	} else {
		str[0] = '0';
		str[1] = '\0';
	}
}

uint16_t atoi(char *str) {
	uint16_t number = 0;
	while (*str) {
		number = (number * 10) + (*str++ - '0');
	}
	return number;
}