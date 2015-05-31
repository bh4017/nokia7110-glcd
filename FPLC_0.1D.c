
// Program:			FPLC Program
// Version:			V0.1 Development
// Author:			Brian J Hoskins
// Date:			08 November 2009

// Program Notes:	The purpose of Version V0.1 will be to successfully communicate with a Nokia 7110 Display and send data to it that can be
//					verifed on the screen.  This will confirm that communication on the SPI bus is a success, and that the 7110 display is being
//					initialised properly.
// Compiler:		BOOSTC Pro V6.96 (for PIC16 Architecture) Licenced to Brian Hoskins
// Device:			PIC16F677

#include <system.h>

#pragma DATA _CONFIG, _FCMEN_OFF & _IESO_OFF & _BOR_OFF & _CPD_OFF & _CP_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTOSCIO

// BEGIN Definitions 	*******************************************************************************************************
#define LCDCS	porta.0													// LCD Chip-Select pin
#define SDI		portb.4													// SPI Serial Data In pin
#define SDO		portc.7													// SPI Serial Data Out pin
#define SCLK	portb.6													// SPI Serial Clock pin
#define LCDDC	portb.5													// LCD Data & Command pin
#define RST		portb.7													// LCD Reset Pin
// END Definitions  	 ******************************************************************************************************

// BEGIN Delcarations	*******************************************************************************************************
rom char *Message1 = "BRIAN" ;
// END Declarations

// BEGIN Fonts			*******************************************************************************************************
rom char *Upper_Font_A =	
						{
						0x40, 0x38, 0x16, 0x11, 0x16, 0x38, 0x40, 0x00,  // Code for char A
						0x7F, 0x49, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00,  // Code for char B
						0x3E, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00,  // Code for char C
						0x7F, 0x41, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x00,  // Code for char D
						0x7F, 0x49, 0x49, 0x49, 0x49, 0x41, 0x41, 0x00,  // Code for char E
						0x7F, 0x09, 0x09, 0x09, 0x09, 0x01, 0x01, 0x00,  // Code for char F
						0x7F, 0x41, 0x41, 0x41, 0x49, 0x49, 0x79, 0x00,  // Code for char G
						0x7F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x7F, 0x00,  // Code for char H
						0x41, 0x41, 0x41, 0x7F, 0x41, 0x41, 0x41, 0x00,  // Code for char I
						0x41, 0x41, 0x41, 0x3F, 0x01, 0x01, 0x01, 0x00,  // Code for char J
						0x7F, 0x08, 0x08, 0x08, 0x14, 0x22, 0x41, 0x00,  // Code for char K
						0x7F, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00,  // Code for char L
						0x7F, 0x02, 0x04, 0x08, 0x04, 0x02, 0x7F, 0x00,  // Code for char M
						0x7F, 0x02, 0x04, 0x08, 0x10, 0x20, 0x7F, 0x00,  // Code for char N
						0x3E, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3E, 0x00,  // Code for char O
						0x7F, 0x09, 0x09, 0x09, 0x09, 0x09, 0x06, 0x00,  // Code for char P
						0x1E, 0x21, 0x21, 0x29, 0x31, 0x3E, 0x40, 0x00,  // Code for char Q
						0x7F, 0x09, 0x09, 0x09, 0x19, 0x29, 0x46, 0x00,  // Code for char R
						0x46, 0x49, 0x49, 0x49, 0x49, 0x49, 0x31, 0x00,  // Code for char S
						0x01, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x01, 0x00,  // Code for char T
						0x3F, 0x40, 0x40, 0x40, 0x40, 0x40, 0x3F, 0x00,  // Code for char U
						0x03, 0x0C, 0x30, 0x40, 0x30, 0x0C, 0x03, 0x00,  // Code for char V
						0x7F, 0x20, 0x10, 0x08, 0x10, 0x20, 0x7F, 0x00,  // Code for char W
						0x41, 0x22, 0x14, 0x08, 0x14, 0x22, 0x41, 0x00,  // Code for char X
						0x01, 0x02, 0x04, 0x78, 0x04, 0x02, 0x01, 0x00,  // Code for char Y
						0x41, 0x61, 0x51, 0x49, 0x45, 0x43, 0x41, 0x00   // Code for char Z
						} ;										
// END Fonts			*******************************************************************************************************

// BEGIN Program     	*******************************************************************************************************		
void delay (unsigned int size, unsigned char multiplier)				// Total Delay (8Mhz clock) = (multiplier x (size x 6uS))
{
	unsigned int i ;													// This is our loop control variable																		
	do																	// Outer loop will complete "multiplier" times
	{					
    	for ( i=0; i<size; i++ ) ;										// Inner loop will complete "size" times
		multiplier -- ;
	}
	while (multiplier >0) ;
}

void lcd_send (unsigned char data, unsigned char type)					// data = the 8-bit data to be sent.  type = 1 for display data, 0 for command data.
{
	unsigned char Rx_Data = 0x00 ;										// This will be the receive data
	LCDDC = type ;														// Set Data / Command mode as appropriate
	LCDCS = 0 ;															// Enable LCD Chip-Select pin (active low)
	sspbuf = data ;														// Put required data into the SSP Buffer.  The PIC will do the rest.
	while (!sspstat.BF) ;												// Do nothing until BF == 1
	Rx_Data = sspbuf ;													// Collect receive data.  This data, though irrelevant, must be collected otherwise the SSP Module complains about write collisions in the SSPBUF register.
	LCDCS = 1 ;															// Disable LCD Chip-Select pin	
}

void lcd_print (const char *m)
{
	unsigned char x = 0 ;												// x will hold our horizontal position in the font map
	unsigned char y = 0 ;												// y will hold our vertical position in the font map
	unsigned char z = 0 ;												// z will be a temp variable to hold the character I'm working on
											
	while (m[z] != 0)													// Do this loop until we reach the null terminator
	{
		y = ((m[z] - 0x41) * 8) ;
		for (x=0 ; x<8 ; x++)											// Do this loop 8 times, incrementing the horizintal position in the font map each time
		{
			lcd_send (Upper_Font_A[(y)], 1) ;
			y++ ;
		}
	
		z++ ;															// When we've printed the entire character, go to the next character in the message
	}
}

void lcd_clear (void)
{
	unsigned char Page = 0xB0 ;
	do
	{
		lcd_send (Page, 0) ;											// Set page address to 0
		lcd_send (0x11, 0) ;													
		lcd_send (0x02, 0) ;											// Set column address to 18 (first displayable area on 7110 LCD).  See SED1565 manual page 51!
		for ( unsigned char i=0; i<0x5F; i++)
			{
				lcd_send (0x00, 1) ;									// Clear pixels at column 18.  The column is automatically incremented by the SED1565.  Do this 95 times to complete the Page!
			}
		Page++ ;														// Next Page
	} while (Page <=0xB8) ;												// When Page gets to 0xB8, we're done.
}

void lcd_init (void)
{
	LCDCS = 1 ;
	delay (14000, 1) ;
	RST = 0 ;															// Reset the LCD
	delay (14000, 1) ;													// Delay 85mS.
	RST = 1 ;															// Return RST high
	lcd_send (0xA6, 0) ;												// Set Display to Normal
	lcd_send (0xA3, 0) ;												// LCD Bias to 1/7
	lcd_send (0xA1, 0) ;												// ADC Selection to Reverse
	lcd_send (0xC0, 0) ;												// Common Output set to Normal Direction
	lcd_send (0x22, 0) ;												// V5 Output Voltage Setting (WHAT IS THIS - find out)
	lcd_send (0x81, 0) ;												// Electronic Volume Register Setting (WHAT IS THIS - find out)
	lcd_send (0x2E, 0) ;												// Power Controller Booster Circuit set to ON, Voltage Regulator ON, Voltage Follower OFF
	lcd_send (0x2F, 0) ;												// Power Controller Set, Voltage Follower Circuit ON
	lcd_send (0xE3, 0) ;												// LCD NOP
	lcd_send (0x40, 0) ;												// Set Start Line
	lcd_send (0xAF, 0) ;												// LCD ON
	lcd_send (0xA5, 0) ;												// Display All Points ON - this should set the entire display black?
	delay (16666, 10) ;													// Approximately 1 second delay
}

void setup_hardware (void)
{
	adcon0.ADON = 0 ;													// Disable ADC
	ansel = 0x00 ;														// Set PORTA to Digital I/O (turn analog input off)
	anselh = 0x00 ;														// Set PORTB to Digital I/O (turn analog input off)
	trisa = 0x00 ;														// Set PORTA all bits to Outputs
	trisb = 0x10 ;														// Set PORTB bit 4 to input, and the rest to outputs
	trisc = 0x00 ;														// Set PORTC all bits to Outputs
	osccon.IRCF0 = 1 ;
	osccon.IRCF1 = 1 ;
	osccon.IRCF2 = 1 ;													// Set internal osc to 8MHZ
	sspstat.SMP = 1 ;													// Set SSP to SPI Master Mode
	sspstat.CKE = 0 ;													// Set SPI to transmit on rising edge of clock (see page 27 of SED1566 datasheet for info on serial mode of LCD controller)
	sspcon.SSPEN = 1 ;													// Enable Serial Port and set SCK, SDO, SDI as serial port pins
	sspcon.CKP = 1 ;													// Idle state of SCK = high
	sspcon.SSPM3 = 0 ;
	sspcon.SSPM2 = 0 ;
	sspcon.SSPM1 = 0 ;
	sspcon.SSPM0 = 0 ;													// Set serial port to SPI Master Mode, Fosc/4
}

void main (void)
{
	setup_hardware() ;													// Setup the PIC16F677 peripherals
	while (!osccon.HTS) ;												// Wait for internal oscillator to stabalise.
	lcd_init () ;														// Initialise the LCD
	lcd_clear () ;														// Clear the LCD screen
	lcd_send (0xA4, 0) ;												// Display All Points - OFF
	lcd_send (0xB0, 0) ;												// Set page address to 0
	lcd_send (0x11, 0) ;																
	lcd_send (0x02, 0) ;												// Set column to 18
	lcd_print ("COOL") ;
	lcd_send (0xA4, 0) ;												// Display All Points - OFF
	lcd_send (0xB2, 0) ;												// Set page address to 0
	lcd_send (0x11, 0) ;																
	lcd_print ("IS") ;
	lcd_send (0xA4, 0) ;												// Display All Points - OFF
	lcd_send (0xB4, 0) ;												// Set page address to 0
	lcd_send (0x11, 0) ;
	lcd_print ("A") ;
	lcd_send (0xA4, 0) ;												// Display All Points - OFF
	lcd_send (0xB6, 0) ;												// Set page address to 0
	lcd_send (0x11, 0) ;
	lcd_print ("RULE") ;																																

	while (1) ;															// Do nothing for eternity
}
// END Program       ********************************************************************************************************
