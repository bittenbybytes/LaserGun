//============================================================================
// Name        : midas_pi.cpp
// Author      : i
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int file = 0;

int initI2C1(unsigned char slaveAddress) {

	int file = 0;
	char filename[40];

	sprintf(filename, "/dev/i2c-1");
	if ((file = open(filename, O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, slaveAddress) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	return file;
}

size_t writeI2C(int file, unsigned char* buf, size_t len) {

	size_t txLen = write(file, buf, len);
			if (txLen != len) {
		// ERROR HANDLING: i2c transaction failed
		printf("Failed to write to the i2c bus.\n");

	}

	return txLen;
}

#define LCD_ADDR	(0x3E << 1)
#define LCD_COMMAND		0x00UL
#define LCD_DATA	0x40UL

#define LCD_CLEAR	0x01
#define LCD_RETURN_HOME		0x02
#define LCD_ENTRY_MODE(inc, shift)	( (0x04 & ((inc != 0) << 1) | (shift != 0))))
#define LCD_DISPLAY_ON_OFF(display, cursor, cursor_blink)	( (0x08 | ((display != 0) << 2) | \
														(cursor != 0) << 1) | (cursor_blink != 0))))

#define LCD_FUNCTION_SET(if_8b, lcd_2line, double_height, instr_table1)\
				(0x20 | ((if_8b != 0) << 4) | ((lcd_2line != 0) << 3)\
				| ((double_height != 0) << 2) | (instr_table1 != 0))

#define LCD_SET_DDRAM_ADDRESS(address)	(0x80 | (address & 0x7F))

void write_cmd(unsigned char ucCtrl, unsigned char ucData)
{
	unsigned char buf[2] = {ucCtrl, ucData};

	writeI2C(file, buf, 2);
}

#define INIT_SEQ_LENGTH		8UL

static unsigned char uc3V3SupplyInitSequence[] = {0x38,
													0x39,
													0x14,
													0x74,
													0x54,
													0x6f,
													0x0c,
													0x01
												};

void initLCD(int file)
{
	for (int i = 0; i < INIT_SEQ_LENGTH; i++)
		write_cmd(LCD_COMMAND, uc3V3SupplyInitSequence[i]);
}

void clearLcd()
{
	write_cmd(LCD_COMMAND, LCD_CLEAR);
}

void printStringLcd(const char* ucStr)
{
	// TODO: implement line feed at '\n'
	while(*ucStr != '\0')
	{
		if(*ucStr == '\n')
		{
			// set cursor to line 2
			write_cmd(LCD_COMMAND, LCD_SET_DDRAM_ADDRESS(0x40));
		}
		else
		{
			// print character
			write_cmd(LCD_DATA, *ucStr);
		}
		ucStr++;
	}
}


int main(int argc, char* argv[]) {

    const char *buffer;
    int addr = 0x3e;        // The I2C address

    file = initI2C1(addr);

    initLCD(file);

    sleep(1);


    if(argc > 1)
    {
            std::string str;
            str = argv[0];
            str += "\n";
            printStringLcd(str.c_str());
            printStringLcd(argv[1]);
    }
    else
    {
            std::cout << "Type display Text: ";
            std::string input;
            std::cin >> input;
            printStringLcd(input.c_str());
    }


}
