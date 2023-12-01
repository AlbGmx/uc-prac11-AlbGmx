// Prototypes
// Initialization
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);

// Send
void UART_puts(char *str);
void UART_putchar(char data);

// Received
char UART_getchar();

// Escape sequences
void UART_clrscr();
void UART_gotoxy(uint8_t x, uint8_t y);
void UART_forwardsX(uint8_t x);
void UART_backwardsX(uint8_t x);
void UART_color(uint8_t color);
void UART_resetColor();


// Utils
void itoa(uint32_t number, char* str); 
uint16_t atoi(char *str);
