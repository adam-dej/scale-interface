#define UBRR (F_CPU/16/BAUD-1)

void usart_init() {

    // Set the baud rate
    UBRRH = (uint8_t)(UBRR>>8);
    UBRRL = (uint8_t)UBRR;

    // Enable the receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN);

    // Set the frame format: 8 data bits, 1 stop bit
    UCSRC = (1<<URSEL)|(3<<UCSZ0);

}

static int __usart_putchar(char data, FILE *stream) {

    // Wait for empty transmit buffer
    while ( !(UCSRA & (_BV(UDRE))) );

    // Start transmission
    UDR = data;

    while ( !(UCSRA & (_BV(UDRE))) );
    // Wait for the transmission completion

    return 0;
    
}

//Setup custom streams for stdio
FILE usart_out = FDEV_SETUP_STREAM(__usart_putchar, NULL, _FDEV_SETUP_WRITE);
