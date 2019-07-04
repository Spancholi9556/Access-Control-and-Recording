/*
 * HD44780.h
 *
 *  Created on: Jun 11, 2019
 *      Author: sanke
 */

#ifndef HD44780_H_
#define HD44780_H_

void hd44780_Delay(unsigned long del);
void hd44780_wr_hi_nibble(unsigned char data);
void hd44780_wr_lo_nibble(unsigned char data);
void hd44780_write(unsigned char data);
void hd44780_wr_cmd(unsigned char cmd);
void hd44780_wr_data(unsigned char data);

void HD44780_Init(void);
void HD44780_PutChar(unsigned char c);
void HD44780_GotoXY(unsigned char x, unsigned char y);
void HD44780_PutStr(char *str);
void HD44780_ClrScr(void);

#endif /* HD4470_H_ */



